/**
 * Frontier Detector (Refactored)
 *
 * Key improvements:
 *  1) Build a frontier mask once (single pass).
 *  2) BFS expands ONLY on frontier cells -> correctness + speed.
 *  3) PCA computed once per final cluster (and also within split recursion) and carried.
 *  4) Cleaner structure: separate small helpers + split returns richer data.
 */

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>

#include "frontier_exploration/msg/frontier_array.hpp"
#include "frontier_exploration/msg/frontier_cluster.hpp"
#include "frontier_exploration/common.hpp"

#include <queue>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

using namespace frontier_exploration;

class FrontierDetectorNode : public rclcpp::Node
{
public:
    FrontierDetectorNode() : Node("frontier_detector")
    {
        // Parameters
        declare_parameter("map_topic", "/map");
        declare_parameter("min_frontier_size", 5);
        declare_parameter("max_cluster_size", 50);
        declare_parameter("free_threshold", 25);
        declare_parameter("occupied_threshold", 65);
        declare_parameter("pca_split_threshold", 2.0);

        // Optional: reduces false-frontiers (near open unknown areas)
        declare_parameter("require_occupied_neighbor", false);

        map_topic_ = get_parameter("map_topic").as_string();
        min_frontier_size_ = get_parameter("min_frontier_size").as_int();
        max_cluster_size_ = get_parameter("max_cluster_size").as_int();
        free_threshold_ = static_cast<int8_t>(get_parameter("free_threshold").as_int());
        occupied_threshold_ = static_cast<int8_t>(get_parameter("occupied_threshold").as_int());
        pca_split_threshold_ = get_parameter("pca_split_threshold").as_double();
        require_occupied_neighbor_ = get_parameter("require_occupied_neighbor").as_bool();

        map_sub_ = create_subscription<nav_msgs::msg::OccupancyGrid>(
            map_topic_, rclcpp::QoS(10),
            std::bind(&FrontierDetectorNode::mapCallback, this, std::placeholders::_1));

        clusters_pub_ = create_publisher<frontier_exploration::msg::FrontierArray>(
            "frontier_clusters", rclcpp::QoS(10));

        color_palette_ = getColorPalette();

        RCLCPP_INFO(get_logger(), "Frontier Detector (refactored) initialized");
        RCLCPP_INFO(get_logger(), "  Input:  %s", map_topic_.c_str());
        RCLCPP_INFO(get_logger(), "  Output: frontier_clusters");
    }

private:
    // ---------------------------
    // Small utility helpers
    // ---------------------------
    inline bool inBounds(int x, int y, int w, int h) const
    {
        return (x >= 0 && x < w && y >= 0 && y < h);
    }

    inline bool isOccupiedVal(int8_t v) const
    {
        // OccupancyGrid: -1 unknown, 0 free, 100 occupied (usually)
        // We treat >= occupied_threshold_ as occupied.
        return (v >= occupied_threshold_);
    }

    // 4-connected neighbors (for frontier definition)
    static constexpr std::array<std::pair<int, int>, 4> NEIGHBORS_4{{{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};

    // ---------------------------
    // PCA
    // ---------------------------
    struct PCAResult
    {
        double mean_x{0}, mean_y{0};
        double axis_x{1}, axis_y{0};
        double eigenvalue{0};
    };

    PCAResult computePCA(const std::vector<std::pair<int, int>> &cells) const
    {
        PCAResult result;
        if (cells.size() < 2)
            return result;

        // Mean
        for (const auto &[x, y] : cells)
        {
            result.mean_x += x;
            result.mean_y += y;
        }
        result.mean_x /= static_cast<double>(cells.size());
        result.mean_y /= static_cast<double>(cells.size());

        // Covariance (2x2)
        double cov_xx = 0, cov_yy = 0, cov_xy = 0;
        for (const auto &[x, y] : cells)
        {
            double dx = x - result.mean_x;
            double dy = y - result.mean_y;
            cov_xx += dx * dx;
            cov_yy += dy * dy;
            cov_xy += dx * dy;
        }
        cov_xx /= static_cast<double>(cells.size());
        cov_yy /= static_cast<double>(cells.size());
        cov_xy /= static_cast<double>(cells.size());

        // Largest eigenvalue of 2x2
        const double trace = cov_xx + cov_yy;
        const double det = cov_xx * cov_yy - cov_xy * cov_xy;
        const double disc = std::max(0.0, trace * trace - 4.0 * det);

        result.eigenvalue = (trace + std::sqrt(disc)) / 2.0;

        // Corresponding eigenvector
        if (std::abs(cov_xy) > 1e-9)
        {
            double vx = cov_xy;
            double vy = result.eigenvalue - cov_xx;
            double norm = std::sqrt(vx * vx + vy * vy);
            if (norm > 1e-12)
            {
                result.axis_x = vx / norm;
                result.axis_y = vy / norm;
            }
        }
        else
        {
            // If cov_xy ~ 0, axis is aligned with max variance axis
            if (cov_xx >= cov_yy)
            {
                result.axis_x = 1.0;
                result.axis_y = 0.0;
            }
            else
            {
                result.axis_x = 0.0;
                result.axis_y = 1.0;
            }
        }

        return result;
    }

    std::vector<std::vector<std::pair<int, int>>> splitByPCA(
        const std::vector<std::pair<int, int>> &cells, const PCAResult &pca) const
    {
        std::vector<std::pair<int, int>> a, b;
        a.reserve(cells.size());
        b.reserve(cells.size());

        for (const auto &[x, y] : cells)
        {
            double dx = x - pca.mean_x;
            double dy = y - pca.mean_y;
            double proj = dx * pca.axis_x + dy * pca.axis_y;
            (proj <= 0.0 ? a : b).push_back({x, y});
        }

        std::vector<std::vector<std::pair<int, int>>> out;
        if (!a.empty())
            out.push_back(std::move(a));
        if (!b.empty())
            out.push_back(std::move(b));
        return out;
    }

    // ---------------------------
    // Frontier logic: build mask once
    // ---------------------------
    bool isFrontierCell(int x, int y, const nav_msgs::msg::OccupancyGrid &map) const
    {
        const int w = static_cast<int>(map.info.width);
        const int h = static_cast<int>(map.info.height);

        if (!inBounds(x, y, w, h))
            return false;

        const int idx = getIndex(x, y, w);
        const int8_t v = map.data[idx];

        // Must be free
        if (!isFree(v, free_threshold_))
            return false;

        // Must touch unknown
        bool touches_unknown = false;
        for (const auto &[dx, dy] : NEIGHBORS_4)
        {
            const int nx = x + dx, ny = y + dy;
            if (!inBounds(nx, ny, w, h))
                continue;
            const int nidx = getIndex(nx, ny, w);
            if (isUnknown(map.data[nidx]))
            {
                touches_unknown = true;
                break;
            }
        }
        if (!touches_unknown)
            return false;

        // Optional: require at least one occupied neighbor (helps reject noisy frontiers)
        if (require_occupied_neighbor_)
        {
            bool touches_occupied = false;
            for (const auto &[dx, dy] : NEIGHBORS_8)
            {
                const int nx = x + dx, ny = y + dy;
                if (!inBounds(nx, ny, w, h))
                    continue;
                const int nidx = getIndex(nx, ny, w);
                if (isOccupiedVal(map.data[nidx]))
                {
                    touches_occupied = true;
                    break;
                }
            }
            if (!touches_occupied)
                return false;
        }

        return true;
    }

    std::vector<bool> buildFrontierMask(const nav_msgs::msg::OccupancyGrid &map) const
    {
        const int w = static_cast<int>(map.info.width);
        const int h = static_cast<int>(map.info.height);

        std::vector<bool> mask(w * h, false);

        // Avoid borders (safe neighbor checks)
        for (int y = 1; y < h - 1; ++y)
        {
            for (int x = 1; x < w - 1; ++x)
            {
                const int idx = getIndex(x, y, w);
                mask[idx] = isFrontierCell(x, y, map);
            }
        }
        return mask;
    }

    // ---------------------------
    // BFS clustering ONLY on frontier cells (correctness fix)
    // ---------------------------
    std::vector<std::pair<int, int>> bfsCluster(
        int sx, int sy,
        const nav_msgs::msg::OccupancyGrid &map,
        const std::vector<bool> &frontier_mask,
        std::vector<bool> &visited) const
    {
        const int w = static_cast<int>(map.info.width);
        const int h = static_cast<int>(map.info.height);

        std::vector<std::pair<int, int>> cells;
        std::queue<std::pair<int, int>> q;

        const int sidx = getIndex(sx, sy, w);
        visited[sidx] = true;
        q.push({sx, sy});

        while (!q.empty())
        {
            auto [x, y] = q.front();
            q.pop();

            cells.push_back({x, y});

            // Expand with 8-connected adjacency
            for (const auto &[dx, dy] : NEIGHBORS_8)
            {
                const int nx = x + dx, ny = y + dy;
                if (!inBounds(nx, ny, w, h))
                    continue;

                const int nidx = getIndex(nx, ny, w);
                if (visited[nidx])
                    continue;

                // The IMPORTANT fix: only traverse frontier cells.
                if (!frontier_mask[nidx])
                    continue;

                visited[nidx] = true;
                q.push({nx, ny});
            }
        }

        return cells;
    }

    // ---------------------------
    // Split recursion returns final clusters with PCA already computed
    // ---------------------------
    struct ClusterPiece
    {
        std::vector<std::pair<int, int>> cells;
        PCAResult pca;
    };

    void recursiveSplit(
        const std::vector<std::pair<int, int>> &cells,
        std::vector<ClusterPiece> &out) const
    {
        if (static_cast<int>(cells.size()) < min_frontier_size_)
            return;

        PCAResult pca = computePCA(cells);

        // Stop conditions: small enough or not elongated enough
        if (static_cast<int>(cells.size()) <= max_cluster_size_ ||
            pca.eigenvalue < pca_split_threshold_)
        {
            out.push_back(ClusterPiece{cells, pca});
            return;
        }

        // Split and recurse
        auto parts = splitByPCA(cells, pca);
        bool pushed_any = false;

        for (const auto &part : parts)
        {
            if (static_cast<int>(part.size()) >= min_frontier_size_)
            {
                recursiveSplit(part, out);
                pushed_any = true;
            }
        }

        // Fallback: if split produced too small parts, keep original
        if (!pushed_any)
        {
            out.push_back(ClusterPiece{cells, pca});
        }
    }

    // ---------------------------
    // Convert cluster into message (PCA reused)
    // ---------------------------
    frontier_exploration::msg::FrontierCluster toMsg(
        const ClusterPiece &piece,
        const nav_msgs::msg::OccupancyGrid &map)
    {
        frontier_exploration::msg::FrontierCluster msg;
        msg.id = next_cluster_id_++;
        msg.size = piece.cells.size();

        int min_x = std::numeric_limits<int>::max();
        int min_y = std::numeric_limits<int>::max();
        int max_x = std::numeric_limits<int>::min();
        int max_y = std::numeric_limits<int>::min();

        double sum_x = 0.0, sum_y = 0.0;

        msg.cells.reserve(piece.cells.size());

        for (const auto &[x, y] : piece.cells)
        {
            min_x = std::min(min_x, x);
            min_y = std::min(min_y, y);
            max_x = std::max(max_x, x);
            max_y = std::max(max_y, y);

            sum_x += x;
            sum_y += y;

            msg.cells.push_back(gridToWorld(x, y, map));
        }

        msg.bbox_min_x = min_x;
        msg.bbox_min_y = min_y;
        msg.bbox_max_x = max_x;
        msg.bbox_max_y = max_y;

        const int cx = static_cast<int>(std::round(sum_x / piece.cells.size()));
        const int cy = static_cast<int>(std::round(sum_y / piece.cells.size()));
        msg.centroid = gridToWorld(cx, cy, map);

        // PCA (already computed)
        msg.principal_axis[0] = piece.pca.axis_x;
        msg.principal_axis[1] = piece.pca.axis_y;
        msg.principal_eigenvalue = piece.pca.eigenvalue;

        // Color
        msg.color = color_palette_[color_index_ % color_palette_.size()];
        color_index_++;

        return msg;
    }

    // ---------------------------
    // Main detection pipeline
    // ---------------------------
    std::vector<frontier_exploration::msg::FrontierCluster> detectFrontiers(
        const nav_msgs::msg::OccupancyGrid &map)
    {
        const int w = static_cast<int>(map.info.width);
        const int h = static_cast<int>(map.info.height);

        std::vector<frontier_exploration::msg::FrontierCluster> out_msgs;
        if (w <= 2 || h <= 2)
            return out_msgs;

        // 1) Build frontier mask once
        auto frontier_mask = buildFrontierMask(map);

        // 2) Visited only for frontier cells
        std::vector<bool> visited(w * h, false);

        color_index_ = 0;
        next_cluster_id_ = 0;

        // 3) BFS clustering on frontier mask
        for (int y = 1; y < h - 1; ++y)
        {
            for (int x = 1; x < w - 1; ++x)
            {
                const int idx = getIndex(x, y, w);

                if (!frontier_mask[idx] || visited[idx])
                    continue;

                auto raw_cluster = bfsCluster(x, y, map, frontier_mask, visited);

                if (static_cast<int>(raw_cluster.size()) < min_frontier_size_)
                {
                    continue;
                }

                // 4) Split large cluster recursively, PCA computed once per final piece
                std::vector<ClusterPiece> final_pieces;
                final_pieces.reserve(4);
                recursiveSplit(raw_cluster, final_pieces);

                // 5) Convert each piece to msg
                for (const auto &piece : final_pieces)
                {
                    out_msgs.push_back(toMsg(piece, map));
                }
            }
        }

        return out_msgs;
    }

    // ---------------------------
    // ROS callback
    // ---------------------------
    void mapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr map_msg)
    {
        if (!map_msg || map_msg->data.empty())
            return;

        const auto start_time = now();

        auto clusters = detectFrontiers(*map_msg);

        frontier_exploration::msg::FrontierArray out;
        out.header.stamp = now();
        out.header.frame_id = map_msg->header.frame_id;
        out.clusters = std::move(clusters);

        clusters_pub_->publish(out);

        const double ms = (now() - start_time).seconds() * 1000.0;
        RCLCPP_DEBUG(get_logger(), "Published %zu clusters in %.1f ms", out.clusters.size(), ms);
    }

private:
    // Parameters
    std::string map_topic_;
    int min_frontier_size_{5};
    int max_cluster_size_{50};
    int8_t free_threshold_{25};
    int8_t occupied_threshold_{65};
    double pca_split_threshold_{2.0};
    bool require_occupied_neighbor_{false};

    // State
    uint32_t next_cluster_id_{0};
    size_t color_index_{0};
    std::vector<std_msgs::msg::ColorRGBA> color_palette_;

    // ROS
    rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr map_sub_;
    rclcpp::Publisher<frontier_exploration::msg::FrontierArray>::SharedPtr clusters_pub_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<FrontierDetectorNode>());
    rclcpp::shutdown();
    return 0;
}
