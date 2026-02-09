/**
 * Node 2: Viewpoint Generator (Refactored + Occlusion-Aware Coverage)
 *
 * Input:
 *   - /frontier_clusters (frontier_exploration/FrontierArray)
 *   - /octomap_binary (octomap_msgs/Octomap)  [optional]
 *   - /map (nav_msgs/OccupancyGrid)
 *
 * Output:
 *   - /frontier_clusters_with_viewpoints (frontier_exploration/FrontierArray)
 *
 * Key features:
 *  - Cylindrical sampling around cluster centroid
 *  - 2D footprint clearance (OccupancyGrid)
 *  - 3D footprint clearance (OctoMap) if available
 *  - Yaw optimization by coverage
 *  - Coverage is occlusion-aware: if a ray is blocked, farther frontier points on that ray are not counted
 *  - Temporal stabilization (hysteresis) to prevent viewpoint jitter
 */

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>

#include <octomap_msgs/msg/octomap.hpp>
#include <octomap_msgs/conversions.h>
#include <octomap/octomap.h>

#include "frontier_exploration/msg/frontier_array.hpp"
#include "frontier_exploration/msg/frontier_cluster.hpp"
#include "frontier_exploration/msg/viewpoint.hpp"
#include "frontier_exploration/common.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <tuple>
#include <vector>

using namespace frontier_exploration;

class ViewpointGeneratorNode : public rclcpp::Node
{
public:
    ViewpointGeneratorNode() : Node("viewpoint_generator")
    {
        // Sensor parameters
        declare_parameter("sensor_range", 5.0);
        declare_parameter("sensor_fov_h", 1.57);

        // Sampling parameters
        declare_parameter("min_dist", 1.5);
        declare_parameter("max_dist", 4.0);
        declare_parameter("num_dist_samples", 3);
        declare_parameter("num_angle_samples", 12);
        declare_parameter("min_coverage", 3);
        declare_parameter("max_viewpoints", 5);

        // Map thresholds
        declare_parameter("free_threshold", 25);
        declare_parameter("occupied_threshold", 65);

        // Robot footprint
        declare_parameter("robot_width", 0.5);
        declare_parameter("robot_length", 0.5);
        declare_parameter("robot_height", 0.3);
        declare_parameter("safety_margin", 0.3);

        // Flight height for 3D checking
        declare_parameter("flight_height", 1.5);
        declare_parameter("height_tolerance", 0.2);

        // Occlusion-aware coverage
        declare_parameter("occlusion_enabled", true);
        declare_parameter("occlusion_bin_angle", 0.02); // rad (~1.15 deg). Smaller => more accurate, slower.

        // Viewpoint stabilization
        declare_parameter("vp_hysteresis_distance", 1.0);    // [m]
        declare_parameter("vp_hysteresis_coverage", 1.3);    // multiplier
        declare_parameter("vp_tracking_timeout", 5.0);       // [s]
        declare_parameter("vp_stabilization_enabled", true); // on/off

        // Read params
        sensor_range_ = get_parameter("sensor_range").as_double();
        sensor_fov_h_ = get_parameter("sensor_fov_h").as_double();
        min_dist_ = get_parameter("min_dist").as_double();
        max_dist_ = get_parameter("max_dist").as_double();
        num_dist_samples_ = get_parameter("num_dist_samples").as_int();
        num_angle_samples_ = get_parameter("num_angle_samples").as_int();
        min_coverage_ = get_parameter("min_coverage").as_int();
        max_viewpoints_ = get_parameter("max_viewpoints").as_int();
        free_threshold_ = static_cast<int8_t>(get_parameter("free_threshold").as_int());
        occupied_threshold_ = static_cast<int8_t>(get_parameter("occupied_threshold").as_int());

        robot_width_ = get_parameter("robot_width").as_double();
        robot_length_ = get_parameter("robot_length").as_double();
        robot_height_ = get_parameter("robot_height").as_double();
        safety_margin_ = get_parameter("safety_margin").as_double();

        flight_height_ = get_parameter("flight_height").as_double();
        height_tolerance_ = get_parameter("height_tolerance").as_double();

        occlusion_enabled_ = get_parameter("occlusion_enabled").as_bool();
        occlusion_bin_angle_ = get_parameter("occlusion_bin_angle").as_double();

        vp_hysteresis_distance_ = get_parameter("vp_hysteresis_distance").as_double();
        vp_hysteresis_coverage_ = get_parameter("vp_hysteresis_coverage").as_double();
        vp_tracking_timeout_ = get_parameter("vp_tracking_timeout").as_double();
        vp_stabilization_enabled_ = get_parameter("vp_stabilization_enabled").as_bool();

        // Precompute clearance radius (2D circle)
        clearance_radius_ = std::sqrt(robot_width_ * robot_width_ + robot_length_ * robot_length_) / 2.0 + safety_margin_;

        // Subscribers
        clusters_sub_ = create_subscription<frontier_exploration::msg::FrontierArray>(
            "frontier_clusters", rclcpp::QoS(10),
            std::bind(&ViewpointGeneratorNode::clustersCallback, this, std::placeholders::_1));

        map_sub_ = create_subscription<nav_msgs::msg::OccupancyGrid>(
            "/map", rclcpp::QoS(10),
            std::bind(&ViewpointGeneratorNode::mapCallback, this, std::placeholders::_1));

        octomap_sub_ = create_subscription<octomap_msgs::msg::Octomap>(
            "/octomap_binary", rclcpp::QoS(10),
            std::bind(&ViewpointGeneratorNode::octomapCallback, this, std::placeholders::_1));

        // Publisher
        clusters_pub_ = create_publisher<frontier_exploration::msg::FrontierArray>(
            "frontier_clusters_with_viewpoints", rclcpp::QoS(10));

        RCLCPP_INFO(get_logger(), "Viewpoint Generator (refactored) initialized");
        RCLCPP_INFO(get_logger(), "  Robot: %.2f x %.2f x %.2f m", robot_width_, robot_length_, robot_height_);
        RCLCPP_INFO(get_logger(), "  Clearance radius: %.2f m (+ safety %.2f)", clearance_radius_, safety_margin_);
        RCLCPP_INFO(get_logger(), "  Flight height: %.2f m (±%.2f)", flight_height_, height_tolerance_);
        RCLCPP_INFO(get_logger(), "  Occlusion-aware coverage: %s (bin=%.3f rad)",
                    occlusion_enabled_ ? "ENABLED" : "DISABLED", occlusion_bin_angle_);
        RCLCPP_INFO(get_logger(), "  Stabilization: %s (hysteresis: %.2fm, coverage: %.1fx, timeout: %.1fs)",
                    vp_stabilization_enabled_ ? "ENABLED" : "DISABLED",
                    vp_hysteresis_distance_, vp_hysteresis_coverage_, vp_tracking_timeout_);
    }

private:
    // ----------------------------
    // Tracking (stabilization)
    // ----------------------------
    struct TrackedViewpoint
    {
        frontier_exploration::msg::Viewpoint viewpoint;
        rclcpp::Time last_seen;
        geometry_msgs::msg::Point cluster_centroid;
    };

    std::unordered_map<uint32_t, TrackedViewpoint> tracked_viewpoints_;

    // ----------------------------
    // ROS callbacks
    // ----------------------------
    void mapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg)
    {
        current_map_ = msg;
    }

    void octomapCallback(const octomap_msgs::msg::Octomap::SharedPtr msg)
    {
        octomap::AbstractOcTree *abstract_tree = octomap_msgs::msgToMap(*msg);
        if (!abstract_tree)
            return;

        current_octree_.reset(dynamic_cast<octomap::OcTree *>(abstract_tree));
        if (!current_octree_)
        {
            delete abstract_tree;
        }
    }

    void clustersCallback(const frontier_exploration::msg::FrontierArray::SharedPtr msg)
    {
        if (!current_map_)
        {
            RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 5000, "No map received yet");
            return;
        }

        const auto t0 = now();

        frontier_exploration::msg::FrontierArray output;
        output.header = msg->header;
        output.clusters.reserve(msg->clusters.size());

        int total_candidates = 0, rejected_2d = 0, rejected_3d = 0, accepted = 0, stabilized = 0;

        std::unordered_set<uint32_t> seen_clusters;
        seen_clusters.reserve(msg->clusters.size());

        for (auto cluster : msg->clusters)
        {
            seen_clusters.insert(cluster.id);

            auto stats = generateViewpointsForCluster(cluster, *current_map_);
            total_candidates += std::get<0>(stats);
            rejected_2d += std::get<1>(stats);
            rejected_3d += std::get<2>(stats);
            accepted += std::get<3>(stats);

            if (vp_stabilization_enabled_ && !cluster.viewpoints.empty())
            {
                if (stabilizeBestViewpoint(cluster))
                    stabilized++;
            }

            output.clusters.push_back(std::move(cluster));
        }

        cleanupTrackedViewpoints(seen_clusters);

        clusters_pub_->publish(output);

        const double ms = (now() - t0).seconds() * 1000.0;
        RCLCPP_DEBUG(get_logger(),
                     "VP: %d cand, %d rej2D, %d rej3D, %d acc, %d stab [%.1fms]",
                     total_candidates, rejected_2d, rejected_3d, accepted, stabilized, ms);
    }

    // ----------------------------
    // Stabilization logic
    // ----------------------------
    bool stabilizeBestViewpoint(frontier_exploration::msg::FrontierCluster &cluster)
    {
        if (cluster.viewpoints.empty())
            return false;

        auto &new_best = cluster.viewpoints.front();
        auto it = tracked_viewpoints_.find(cluster.id);

        // First time seen
        if (it == tracked_viewpoints_.end())
        {
            updateTracked(cluster.id, new_best, cluster.centroid);
            return false;
        }

        auto &tracked = it->second;
        const double elapsed = (now() - tracked.last_seen).seconds();
        if (elapsed > vp_tracking_timeout_)
        {
            updateTracked(cluster.id, new_best, cluster.centroid);
            return false;
        }

        // Cluster centroid moved a lot -> reset
        const double centroid_dist = dist2D(cluster.centroid, tracked.cluster_centroid);
        if (centroid_dist > vp_hysteresis_distance_ * 2.0)
        {
            updateTracked(cluster.id, new_best, cluster.centroid);
            return false;
        }

        const auto &old_vp = tracked.viewpoint;
        const double vp_dist = dist2D(new_best.position, old_vp.position);

        const bool within_hyst = (vp_dist < vp_hysteresis_distance_);
        const bool not_much_better = (new_best.coverage < static_cast<int>(old_vp.coverage * vp_hysteresis_coverage_));

        if (within_hyst && not_much_better)
        {
            // keep old if still valid
            if (isViewpointValid(old_vp))
            {
                cluster.viewpoints[0] = old_vp;
                tracked.last_seen = now();
                return true;
            }
        }

        updateTracked(cluster.id, new_best, cluster.centroid);
        return false;
    }

    void updateTracked(uint32_t id, const frontier_exploration::msg::Viewpoint &vp,
                       const geometry_msgs::msg::Point &centroid)
    {
        tracked_viewpoints_[id] = TrackedViewpoint{vp, now(), centroid};
    }

    void cleanupTrackedViewpoints(const std::unordered_set<uint32_t> &seen)
    {
        const auto t = now();
        for (auto it = tracked_viewpoints_.begin(); it != tracked_viewpoints_.end();)
        {
            if (seen.find(it->first) == seen.end())
            {
                const double elapsed = (t - it->second.last_seen).seconds();
                if (elapsed > vp_tracking_timeout_)
                {
                    it = tracked_viewpoints_.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }

    bool isViewpointValid(const frontier_exploration::msg::Viewpoint &vp)
    {
        if (!current_map_)
            return false;
        if (!hasFootprintClearance2D(vp.position.x, vp.position.y, *current_map_))
            return false;
        if (current_octree_ && !hasFootprintClearance3D(vp.position.x, vp.position.y, vp.position.z))
            return false;
        return true;
    }

    // ----------------------------
    // Viewpoint generation pipeline
    // ----------------------------
    std::tuple<int, int, int, int> generateViewpointsForCluster(
        frontier_exploration::msg::FrontierCluster &cluster,
        const nav_msgs::msg::OccupancyGrid &map)
    {
        cluster.viewpoints.clear();

        int total_candidates = 0, rejected_2d = 0, rejected_3d = 0;
        std::vector<frontier_exploration::msg::Viewpoint> candidates;
        candidates.reserve(static_cast<size_t>(num_dist_samples_ * num_angle_samples_));

        // Sampling steps
        const double dist_step = (num_dist_samples_ > 1) ? (max_dist_ - min_dist_) / (num_dist_samples_ - 1) : 0.0;
        const double angle_step = (num_angle_samples_ > 0) ? (2.0 * M_PI / num_angle_samples_) : (2.0 * M_PI);

        for (int di = 0; di < num_dist_samples_; ++di)
        {
            const double dist = min_dist_ + di * dist_step;

            for (int ai = 0; ai < num_angle_samples_; ++ai)
            {
                const double angle = ai * angle_step;
                total_candidates++;

                geometry_msgs::msg::Point pos;
                pos.x = cluster.centroid.x + dist * std::cos(angle);
                pos.y = cluster.centroid.y + dist * std::sin(angle);
                pos.z = flight_height_;

                // 2D clearance
                if (!hasFootprintClearance2D(pos.x, pos.y, map))
                {
                    rejected_2d++;
                    continue;
                }

                // 3D clearance (if available)
                if (current_octree_ && !hasFootprintClearance3D(pos.x, pos.y, pos.z))
                {
                    rejected_3d++;
                    continue;
                }

                // Yaw optimization by (occlusion-aware) coverage
                auto [best_yaw, best_cov] = optimizeYaw(pos, cluster, map);

                if (best_cov >= min_coverage_)
                {
                    frontier_exploration::msg::Viewpoint vp;
                    vp.position = pos;
                    vp.yaw = best_yaw;
                    vp.coverage = best_cov;
                    vp.distance_to_centroid = dist;
                    candidates.push_back(vp);
                }
            }
        }

        std::sort(candidates.begin(), candidates.end(),
                  [](const auto &a, const auto &b)
                  { return a.coverage > b.coverage; });

        const int keep_n = std::min(static_cast<int>(candidates.size()), max_viewpoints_);
        cluster.viewpoints.assign(candidates.begin(), candidates.begin() + keep_n);

        return {total_candidates, rejected_2d, rejected_3d, static_cast<int>(cluster.viewpoints.size())};
    }

    // ----------------------------
    // Yaw optimization + coverage
    // ----------------------------
    std::pair<double, int> optimizeYaw(const geometry_msgs::msg::Point &vp_pos,
                                       const frontier_exploration::msg::FrontierCluster &cluster,
                                       const nav_msgs::msg::OccupancyGrid &map)
    {
        const int yaw_samples = 36;
        double best_yaw = 0.0;
        int best_cov = -1;

        for (int i = 0; i < yaw_samples; ++i)
        {
            const double yaw = (2.0 * M_PI * i) / yaw_samples - M_PI;

            const int cov = occlusion_enabled_
                                ? computeCoverageOcclusionAware(vp_pos, yaw, cluster, map)
                                : computeCoverageSimple(vp_pos, yaw, cluster, map);

            if (cov > best_cov)
            {
                best_cov = cov;
                best_yaw = yaw;
            }
        }

        return {best_yaw, best_cov};
    }

    int computeCoverageSimple(const geometry_msgs::msg::Point &vp_pos, double yaw,
                              const frontier_exploration::msg::FrontierCluster &cluster,
                              const nav_msgs::msg::OccupancyGrid &map)
    {
        const double range2 = sensor_range_ * sensor_range_;
        const double half_fov = sensor_fov_h_ * 0.5;

        int count = 0;
        for (const auto &cell : cluster.cells)
        {
            const double dx = cell.x - vp_pos.x;
            const double dy = cell.y - vp_pos.y;
            const double d2 = dx * dx + dy * dy;
            if (d2 > range2)
                continue;

            const double angle_to_cell = std::atan2(dy, dx);
            if (angleDiff(angle_to_cell, yaw) > half_fov)
                continue;

            if (hasLineOfSight(vp_pos.x, vp_pos.y, cell.x, cell.y, map))
            {
                count++;
            }
        }
        return count;
    }

    // Occlusion-aware:
    // - We bin rays by angle
    // - We process near-to-far per ray
    // - First LOS failure blocks that ray => farther points on same ray are not counted
    int computeCoverageOcclusionAware(const geometry_msgs::msg::Point &vp_pos, double yaw,
                                      const frontier_exploration::msg::FrontierCluster &cluster,
                                      const nav_msgs::msg::OccupancyGrid &map)
    {
        struct PolarCell
        {
            double angle; // [-pi, pi]
            double dist2; // squared distance
            geometry_msgs::msg::Point p;
        };

        const double range2 = sensor_range_ * sensor_range_;
        const double half_fov = sensor_fov_h_ * 0.5;

        std::vector<PolarCell> cells;
        cells.reserve(cluster.cells.size());

        // Collect only cells within range & FOV
        for (const auto &cell : cluster.cells)
        {
            const double dx = cell.x - vp_pos.x;
            const double dy = cell.y - vp_pos.y;
            const double d2 = dx * dx + dy * dy;
            if (d2 > range2)
                continue;

            const double ang = std::atan2(dy, dx);
            if (angleDiff(ang, yaw) > half_fov)
                continue;

            cells.push_back(PolarCell{ang, d2, cell});
        }

        if (cells.empty())
            return 0;

        // Sort by (angle bin, distance) => near-to-far per ray
        const double bin = std::max(1e-4, occlusion_bin_angle_);
        auto rayId = [bin](double angle) -> int
        {
            // Map [-pi, pi] into [0, 2pi) bins
            double a = angle + M_PI;
            int id = static_cast<int>(std::floor(a / bin));
            return id;
        };

        std::sort(cells.begin(), cells.end(),
                  [&](const PolarCell &a, const PolarCell &b)
                  {
                      const int ra = rayId(a.angle);
                      const int rb = rayId(b.angle);
                      if (ra != rb)
                          return ra < rb;
                      return a.dist2 < b.dist2;
                  });

        // We keep a blocked flag per ray that appears.
        // Using unordered_set for blocked rays is simple and efficient enough.
        std::unordered_set<int> blocked;
        blocked.reserve(cells.size() / 4);

        int count = 0;
        for (const auto &c : cells)
        {
            const int r = rayId(c.angle);
            if (blocked.find(r) != blocked.end())
                continue;

            if (hasLineOfSight(vp_pos.x, vp_pos.y, c.p.x, c.p.y, map))
            {
                count++;
            }
            else
            {
                // First blockage on this ray => everything behind is not visible
                blocked.insert(r);
            }
        }

        return count;
    }

    // ----------------------------
    // Clearance checks
    // ----------------------------
    bool hasFootprintClearance2D(double wx, double wy, const nav_msgs::msg::OccupancyGrid &map)
    {
        auto [cgx, cgy] = worldToGrid(wx, wy, map);

        const int w = static_cast<int>(map.info.width);
        const int h = static_cast<int>(map.info.height);
        const double res = map.info.resolution;

        const int cell_r = static_cast<int>(std::ceil(clearance_radius_ / res));
        const double r2 = clearance_radius_ * clearance_radius_;

        // bounds + occupied/unknown checks in circle
        for (int dy = -cell_r; dy <= cell_r; ++dy)
        {
            for (int dx = -cell_r; dx <= cell_r; ++dx)
            {
                const int gx = cgx + dx;
                const int gy = cgy + dy;

                if (gx < 0 || gx >= w || gy < 0 || gy >= h)
                    return false;

                const double dist2 = (dx * dx + dy * dy) * (res * res);
                if (dist2 > r2)
                    continue;

                const int8_t v = map.data[getIndex(gx, gy, w)];

                if (isOccupied(v, occupied_threshold_))
                    return false;

                // Unknown very close to center => unsafe
                if (isUnknown(v) && dist2 < (0.5 * clearance_radius_) * (0.5 * clearance_radius_))
                {
                    return false;
                }
            }
        }

        // Center must be free
        const int cidx = getIndex(cgx, cgy, w);
        return isFree(map.data[cidx], free_threshold_);
    }

    bool hasFootprintClearance3D(double wx, double wy, double wz)
    {
        if (!current_octree_)
            return true;

        const double res = current_octree_->getResolution();
        const double half_w = (robot_width_ / 2.0) + safety_margin_;
        const double half_l = (robot_length_ / 2.0) + safety_margin_;
        const double half_h = (robot_height_ / 2.0) + height_tolerance_;

        for (double dz = -half_h; dz <= half_h; dz += res)
        {
            for (double dy = -half_l; dy <= half_l; dy += res)
            {
                for (double dx = -half_w; dx <= half_w; dx += res)
                {
                    const double px = wx + dx;
                    const double py = wy + dy;
                    const double pz = wz + dz;

                    octomap::OcTreeNode *node = current_octree_->search(px, py, pz);
                    if (node && current_octree_->isNodeOccupied(node))
                        return false;
                }
            }
        }
        return true;
    }

    // ----------------------------
    // LOS (2D Bresenham)
    // ----------------------------
    bool hasLineOfSight(double x1, double y1, double x2, double y2,
                        const nav_msgs::msg::OccupancyGrid &map)
    {
        auto [gx1, gy1] = worldToGrid(x1, y1, map);
        auto [gx2, gy2] = worldToGrid(x2, y2, map);

        int dx = std::abs(gx2 - gx1);
        int dy = std::abs(gy2 - gy1);
        int sx = (gx1 < gx2) ? 1 : -1;
        int sy = (gy1 < gy2) ? 1 : -1;
        int err = dx - dy;

        int x = gx1, y = gy1;
        const int w = static_cast<int>(map.info.width);
        const int h = static_cast<int>(map.info.height);

        while (true)
        {
            if (x >= 0 && x < w && y >= 0 && y < h)
            {
                if (isOccupied(map.data[getIndex(x, y, w)], occupied_threshold_))
                {
                    return false;
                }
            }
            else
            {
                // outside map => treat as blocked
                return false;
            }

            if (x == gx2 && y == gy2)
                break;

            const int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y += sy;
            }
        }

        return true;
    }

    // ----------------------------
    // Small helpers
    // ----------------------------
    double dist2D(const geometry_msgs::msg::Point &a, const geometry_msgs::msg::Point &b) const
    {
        const double dx = a.x - b.x;
        const double dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }

private:
    // Parameters
    double sensor_range_{5.0}, sensor_fov_h_{1.57};
    double min_dist_{1.5}, max_dist_{4.0};
    int num_dist_samples_{3}, num_angle_samples_{12};
    int min_coverage_{3}, max_viewpoints_{5};
    int8_t free_threshold_{25}, occupied_threshold_{65};

    // Robot footprint
    double robot_width_{0.5}, robot_length_{0.5}, robot_height_{0.3};
    double safety_margin_{0.3};
    double clearance_radius_{0.6};
    double flight_height_{1.5}, height_tolerance_{0.2};

    // Occlusion
    bool occlusion_enabled_{true};
    double occlusion_bin_angle_{0.02};

    // Stabilization
    double vp_hysteresis_distance_{1.0};
    double vp_hysteresis_coverage_{1.3};
    double vp_tracking_timeout_{5.0};
    bool vp_stabilization_enabled_{true};

    // Maps
    nav_msgs::msg::OccupancyGrid::SharedPtr current_map_;
    std::shared_ptr<octomap::OcTree> current_octree_;

    // ROS
    rclcpp::Subscription<frontier_exploration::msg::FrontierArray>::SharedPtr clusters_sub_;
    rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr map_sub_;
    rclcpp::Subscription<octomap_msgs::msg::Octomap>::SharedPtr octomap_sub_;
    rclcpp::Publisher<frontier_exploration::msg::FrontierArray>::SharedPtr clusters_pub_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ViewpointGeneratorNode>());
    rclcpp::shutdown();
    return 0;
}
