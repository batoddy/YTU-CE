/**
 * Node 2: Viewpoint Generator (Feature-rich + Occlusion-aware + Stabilized)
 *
 * Input:
 *   /frontier_clusters (frontier_exploration/FrontierArray)
 *   /octomap_binary    (octomap_msgs/Octomap)   [optional 3D checking]
 *   /map               (nav_msgs/OccupancyGrid) [2D fallback + LOS]
 *
 * Output:
 *   /frontier_clusters_with_viewpoints (frontier_exploration/FrontierArray)
 *
 * What it does:
 *  - For each frontier cluster, samples candidate viewpoints on a ring (cylindrical sampling).
 *  - Checks 2D footprint clearance on OccupancyGrid.
 *  - Checks 3D footprint clearance on OctoMap (if available).
 *  - Finds best yaw per viewpoint by maximizing frontier coverage.
 *  - Coverage is occlusion-aware:
 *      If there is an occupied cell along the ray from viewpoint to a frontier cell,
 *      that frontier cell is not counted, and farther cells along that direction are not counted.
 *  - Temporal stabilization (hysteresis) keeps old best viewpoint if new one is not significantly better.
 *
 * Added (Feature-rich):
 *  - Writes into each Viewpoint:
 *      coverage, distance_to_centroid,
 *      cluster_id, cluster_size, cluster_centroid
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

        // Viewpoint sampling parameters
        declare_parameter("min_dist", 1.5);
        declare_parameter("max_dist", 4.0);
        declare_parameter("num_dist_samples", 3);
        declare_parameter("num_angle_samples", 12);
        declare_parameter("min_coverage", 3);
        declare_parameter("max_viewpoints", 5);

        // Map thresholds
        declare_parameter("free_threshold", 25);
        declare_parameter("occupied_threshold", 65);

        // Robot footprint parameters
        declare_parameter("robot_width", 0.5);
        declare_parameter("robot_length", 0.5);
        declare_parameter("robot_height", 0.3);
        declare_parameter("safety_margin", 0.3);

        // Flight height for 3D checking
        declare_parameter("flight_height", 1.5);
        declare_parameter("height_tolerance", 0.2);

        // Viewpoint stabilization parameters
        declare_parameter("vp_hysteresis_distance", 1.0); // [m]
        declare_parameter("vp_hysteresis_coverage", 1.3); // [x] new_cov must be > old_cov * this
        declare_parameter("vp_tracking_timeout", 5.0);    // [s]
        declare_parameter("vp_stabilization_enabled", true);

        // Occlusion-aware coverage parameters
        declare_parameter("occlusion_enabled", true);
        declare_parameter("occlusion_angle_bin_rad", 0.02); // angular bin size [rad], ~1.15 deg
        declare_parameter("yaw_samples", 36);               // yaw optimization samples

        // Read parameters
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

        vp_hysteresis_distance_ = get_parameter("vp_hysteresis_distance").as_double();
        vp_hysteresis_coverage_ = get_parameter("vp_hysteresis_coverage").as_double();
        vp_tracking_timeout_ = get_parameter("vp_tracking_timeout").as_double();
        vp_stabilization_enabled_ = get_parameter("vp_stabilization_enabled").as_bool();

        occlusion_enabled_ = get_parameter("occlusion_enabled").as_bool();
        occlusion_angle_bin_rad_ = get_parameter("occlusion_angle_bin_rad").as_double();
        yaw_samples_ = get_parameter("yaw_samples").as_int();

        // Clearance radius (2D)
        clearance_radius_ =
            std::sqrt(robot_width_ * robot_width_ + robot_length_ * robot_length_) / 2.0 + safety_margin_;

        // Subscribers
        clusters_sub_ = create_subscription<frontier_exploration::msg::FrontierArray>(
            "frontier_clusters", 10,
            std::bind(&ViewpointGeneratorNode::clustersCallback, this, std::placeholders::_1));

        map_sub_ = create_subscription<nav_msgs::msg::OccupancyGrid>(
            "/map", 10,
            std::bind(&ViewpointGeneratorNode::mapCallback, this, std::placeholders::_1));

        octomap_sub_ = create_subscription<octomap_msgs::msg::Octomap>(
            "/octomap_binary", 10,
            std::bind(&ViewpointGeneratorNode::octomapCallback, this, std::placeholders::_1));

        // Publisher
        clusters_pub_ = create_publisher<frontier_exploration::msg::FrontierArray>(
            "frontier_clusters_with_viewpoints", 10);

        RCLCPP_INFO(get_logger(), "Viewpoint Generator initialized");
        RCLCPP_INFO(get_logger(), "  Output: frontier_clusters_with_viewpoints");
        RCLCPP_INFO(get_logger(), "  Clearance radius: %.2f m (safety=%.2f)", clearance_radius_, safety_margin_);
        RCLCPP_INFO(get_logger(), "  Stabilization: %s", vp_stabilization_enabled_ ? "ENABLED" : "DISABLED");
        RCLCPP_INFO(get_logger(), "  Occlusion-aware coverage: %s", occlusion_enabled_ ? "ENABLED" : "DISABLED");
    }

private:
    // ----------------------------
    // Tracking structure (stabilization)
    // ----------------------------
    struct TrackedViewpoint
    {
        frontier_exploration::msg::Viewpoint viewpoint; // best viewpoint (with features)
        rclcpp::Time last_seen;
        geometry_msgs::msg::Point cluster_centroid;
    };

    std::unordered_map<uint32_t, TrackedViewpoint> tracked_viewpoints_;

    // ----------------------------
    // Utility helpers
    // ----------------------------
    static inline double dist2(const geometry_msgs::msg::Point &a, const geometry_msgs::msg::Point &b)
    {
        const double dx = a.x - b.x;
        const double dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    static inline double dist(const geometry_msgs::msg::Point &a, const geometry_msgs::msg::Point &b)
    {
        return std::sqrt(dist2(a, b));
    }

    // ----------------------------
    // Callbacks
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
            delete abstract_tree; // dynamic_cast failed
        }
    }

    void clustersCallback(const frontier_exploration::msg::FrontierArray::SharedPtr msg)
    {
        if (!current_map_)
        {
            RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 5000, "No map received yet");
            return;
        }

        frontier_exploration::msg::FrontierArray output;
        output.header = msg->header;
        output.clusters = msg->clusters;

        std::unordered_set<uint32_t> seen_clusters;
        seen_clusters.reserve(output.clusters.size());

        for (auto &cluster : output.clusters)
        {
            seen_clusters.insert(cluster.id);

            // 1) Generate candidates + compute yaw + compute coverage
            generateViewpoints(cluster, *current_map_);

            // 2) Stabilize best viewpoint (index 0)
            if (vp_stabilization_enabled_ && !cluster.viewpoints.empty())
            {
                stabilizeBestViewpoint(cluster);
            }

            // 3) Make sure cluster context fields are present (in case stabilization swapped an old VP)
            //    (old vp already has these, but this is safe)
            for (auto &vp : cluster.viewpoints)
            {
                fillClusterContextIntoViewpoint(vp, cluster);
            }
        }

        cleanupTrackedViewpoints(seen_clusters);

        clusters_pub_->publish(output);
    }

    // ----------------------------
    // Feature injection (cluster context -> viewpoint)
    // ----------------------------
    void fillClusterContextIntoViewpoint(frontier_exploration::msg::Viewpoint &vp,
                                         const frontier_exploration::msg::FrontierCluster &cluster)
    {
        // These fields require you to extend Viewpoint.msg accordingly.
        vp.cluster_id = cluster.id;
        vp.cluster_size = static_cast<int32_t>(cluster.cells.size());
        vp.cluster_centroid = cluster.centroid;
    }

    // ----------------------------
    // Viewpoint generation
    // ----------------------------
    void generateViewpoints(frontier_exploration::msg::FrontierCluster &cluster,
                            const nav_msgs::msg::OccupancyGrid &map)
    {
        cluster.viewpoints.clear();

        if (num_angle_samples_ <= 0 || num_dist_samples_ <= 0)
            return;

        const double dist_step =
            (num_dist_samples_ > 1) ? (max_dist_ - min_dist_) / (num_dist_samples_ - 1) : 0.0;
        const double angle_step = 2.0 * M_PI / static_cast<double>(num_angle_samples_);

        std::vector<frontier_exploration::msg::Viewpoint> candidates;
        candidates.reserve(static_cast<size_t>(num_angle_samples_ * num_dist_samples_));

        for (int di = 0; di < num_dist_samples_; ++di)
        {
            const double dist_to_centroid = min_dist_ + di * dist_step;

            for (int ai = 0; ai < num_angle_samples_; ++ai)
            {
                const double angle = ai * angle_step;

                geometry_msgs::msg::Point vp_pos;
                vp_pos.x = cluster.centroid.x + dist_to_centroid * std::cos(angle);
                vp_pos.y = cluster.centroid.y + dist_to_centroid * std::sin(angle);
                vp_pos.z = flight_height_;

                // 1) 2D footprint clearance
                if (!hasFootprintClearance2D(vp_pos.x, vp_pos.y, map))
                {
                    continue;
                }

                // 2) 3D footprint clearance (if available)
                if (current_octree_ && !hasFootprintClearance3D(vp_pos.x, vp_pos.y, vp_pos.z))
                {
                    continue;
                }

                // 3) Optimize yaw by coverage
                const auto [best_yaw, best_cov] = optimizeYaw(vp_pos, cluster, map);
                if (best_cov < min_coverage_)
                {
                    continue;
                }

                // Build viewpoint (with features)
                frontier_exploration::msg::Viewpoint vp;
                vp.position = vp_pos;
                vp.yaw = best_yaw;
                vp.coverage = best_cov;
                vp.distance_to_centroid = dist_to_centroid;

                // Put cluster context (fiş) directly here
                fillClusterContextIntoViewpoint(vp, cluster);

                candidates.push_back(vp);
            }
        }

        // Sort by coverage desc (stable)
        std::sort(candidates.begin(), candidates.end(),
                  [](const auto &a, const auto &b)
                  { return a.coverage > b.coverage; });

        const int keep_n = std::min(static_cast<int>(candidates.size()), max_viewpoints_);
        cluster.viewpoints.assign(candidates.begin(), candidates.begin() + keep_n);
    }

    // ----------------------------
    // Stabilization (keep old best VP if still good)
    // ----------------------------
    void stabilizeBestViewpoint(frontier_exploration::msg::FrontierCluster &cluster)
    {
        if (cluster.viewpoints.empty())
            return;

        auto &new_best = cluster.viewpoints[0];
        auto it = tracked_viewpoints_.find(cluster.id);

        // First time: track it
        if (it == tracked_viewpoints_.end())
        {
            updateTrackedViewpoint(cluster.id, new_best, cluster.centroid);
            return;
        }

        auto &tracked = it->second;

        // Timeout check
        const double elapsed = (now() - tracked.last_seen).seconds();
        if (elapsed > vp_tracking_timeout_)
        {
            updateTrackedViewpoint(cluster.id, new_best, cluster.centroid);
            return;
        }

        // Cluster moved too much -> reset
        const double centroid_move = dist(cluster.centroid, tracked.cluster_centroid);
        if (centroid_move > vp_hysteresis_distance_ * 2.0)
        {
            updateTrackedViewpoint(cluster.id, new_best, cluster.centroid);
            return;
        }

        const auto &old_vp = tracked.viewpoint;
        const double vp_move = dist(new_best.position, old_vp.position);

        const bool within_hys = (vp_move < vp_hysteresis_distance_);
        const bool new_not_much_better = (new_best.coverage < static_cast<int>(old_vp.coverage * vp_hysteresis_coverage_));

        if (within_hys && new_not_much_better)
        {
            // Keep old if still valid
            if (isViewpointStillValid(old_vp, *current_map_))
            {
                cluster.viewpoints[0] = old_vp;
                tracked.last_seen = now();
                return;
            }
            // old invalid -> accept new
        }

        updateTrackedViewpoint(cluster.id, new_best, cluster.centroid);
    }

    void updateTrackedViewpoint(uint32_t cluster_id,
                                const frontier_exploration::msg::Viewpoint &vp,
                                const geometry_msgs::msg::Point &centroid)
    {
        TrackedViewpoint t;
        t.viewpoint = vp;
        t.last_seen = now();
        t.cluster_centroid = centroid;
        tracked_viewpoints_[cluster_id] = t;
    }

    bool isViewpointStillValid(const frontier_exploration::msg::Viewpoint &vp,
                               const nav_msgs::msg::OccupancyGrid &map)
    {
        if (!hasFootprintClearance2D(vp.position.x, vp.position.y, map))
            return false;
        if (current_octree_ && !hasFootprintClearance3D(vp.position.x, vp.position.y, vp.position.z))
            return false;
        return true;
    }

    void cleanupTrackedViewpoints(const std::unordered_set<uint32_t> &seen)
    {
        const auto tnow = now();

        for (auto it = tracked_viewpoints_.begin(); it != tracked_viewpoints_.end();)
        {
            const bool seen_now = (seen.find(it->first) != seen.end());
            const double elapsed = (tnow - it->second.last_seen).seconds();

            if (!seen_now && elapsed > vp_tracking_timeout_)
            {
                it = tracked_viewpoints_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // ----------------------------
    // 2D footprint clearance
    // ----------------------------
    bool hasFootprintClearance2D(double wx, double wy, const nav_msgs::msg::OccupancyGrid &map)
    {
        auto [cgx, cgy] = worldToGrid(wx, wy, map);

        const int width = static_cast<int>(map.info.width);
        const int height = static_cast<int>(map.info.height);
        const double res = map.info.resolution;

        // center must be in map
        if (cgx < 0 || cgx >= width || cgy < 0 || cgy >= height)
            return false;

        const int cell_radius = static_cast<int>(std::ceil(clearance_radius_ / res));
        const double r2 = clearance_radius_ * clearance_radius_;

        for (int dy = -cell_radius; dy <= cell_radius; ++dy)
        {
            for (int dx = -cell_radius; dx <= cell_radius; ++dx)
            {
                const int gx = cgx + dx;
                const int gy = cgy + dy;

                // Out of bounds -> invalid
                if (gx < 0 || gx >= width || gy < 0 || gy >= height)
                    return false;

                // Circle mask using squared distance
                const double wxr = static_cast<double>(dx) * res;
                const double wyr = static_cast<double>(dy) * res;
                const double d2 = wxr * wxr + wyr * wyr;
                if (d2 > r2)
                    continue;

                const int idx = getIndex(gx, gy, width);
                const int8_t v = map.data[idx];

                if (isOccupied(v, occupied_threshold_))
                    return false;

                // Unknown near center is risky: reject
                if (isUnknown(v) && d2 < (0.5 * clearance_radius_) * (0.5 * clearance_radius_))
                    return false;
            }
        }

        // ensure center is free
        const int cidx = getIndex(cgx, cgy, width);
        return isFree(map.data[cidx], free_threshold_);
    }

    // ----------------------------
    // 3D footprint clearance (OctoMap)
    // ----------------------------
    bool hasFootprintClearance3D(double wx, double wy, double wz)
    {
        if (!current_octree_)
            return true;

        const double resolution = current_octree_->getResolution();

        const double half_w = (robot_width_ / 2.0) + safety_margin_;
        const double half_l = (robot_length_ / 2.0) + safety_margin_;
        const double half_h = (robot_height_ / 2.0) + height_tolerance_;

        const double step = resolution;

        for (double dz = -half_h; dz <= half_h; dz += step)
        {
            for (double dy = -half_l; dy <= half_l; dy += step)
            {
                for (double dx = -half_w; dx <= half_w; dx += step)
                {
                    const double px = wx + dx;
                    const double py = wy + dy;
                    const double pz = wz + dz;

                    octomap::OcTreeNode *node = current_octree_->search(px, py, pz);
                    if (node && current_octree_->isNodeOccupied(node))
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    // ----------------------------
    // Yaw optimization by (occlusion-aware) coverage
    // ----------------------------
    std::pair<double, int> optimizeYaw(const geometry_msgs::msg::Point &vp_pos,
                                       const frontier_exploration::msg::FrontierCluster &cluster,
                                       const nav_msgs::msg::OccupancyGrid &map)
    {
        const int samples = std::max(4, yaw_samples_);
        double best_yaw = 0.0;
        int best_cov = -1;

        for (int i = 0; i < samples; ++i)
        {
            const double yaw = (2.0 * M_PI * i) / static_cast<double>(samples) - M_PI;
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

    // Simple coverage: count LOS-visible frontier cells (no occlusion bin blocking)
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

            const double a = std::atan2(dy, dx);
            if (angleDiff(a, yaw) > half_fov)
                continue;

            if (hasLineOfSight(vp_pos.x, vp_pos.y, cell.x, cell.y, map))
            {
                count++;
            }
        }
        return count;
    }

    // Occlusion-aware coverage:
    // - Bucket frontier cells by angular bins (ray_id)
    // - Sort by angle then distance
    // - For each ray bin, once LOS fails, mark ray blocked: farther cells in same bin are not counted
    struct PolarCell
    {
        double ang;
        double dist;
        geometry_msgs::msg::Point p;
    };

    int computeCoverageOcclusionAware(const geometry_msgs::msg::Point &vp_pos, double yaw,
                                      const frontier_exploration::msg::FrontierCluster &cluster,
                                      const nav_msgs::msg::OccupancyGrid &map)
    {
        const double range2 = sensor_range_ * sensor_range_;
        const double half_fov = sensor_fov_h_ * 0.5;

        std::vector<PolarCell> cells;
        cells.reserve(cluster.cells.size());

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

            cells.push_back(PolarCell{ang, std::sqrt(d2), cell});
        }

        if (cells.empty())
            return 0;

        std::sort(cells.begin(), cells.end(),
                  [](const PolarCell &a, const PolarCell &b)
                  {
                      if (std::abs(a.ang - b.ang) < 1e-9)
                          return a.dist < b.dist;
                      return a.ang < b.ang;
                  });

        // blocked rays by ray_id
        std::unordered_set<int> blocked;
        blocked.reserve(256);

        const double inv_bin = 1.0 / std::max(1e-6, occlusion_angle_bin_rad_);

        int count = 0;
        for (const auto &c : cells)
        {
            // Normalize angle to [-pi, pi) so binning is stable
            const double a = normalizeAngle(c.ang);
            const int ray_id = static_cast<int>(std::floor((a + M_PI) * inv_bin)); // shift to [0, 2pi)

            if (blocked.find(ray_id) != blocked.end())
            {
                continue;
            }

            if (hasLineOfSight(vp_pos.x, vp_pos.y, c.p.x, c.p.y, map))
            {
                count++;
            }
            else
            {
                // This direction is occluded -> block farther cells in same direction
                blocked.insert(ray_id);
            }
        }

        return count;
    }

    // Bresenham LOS on occupancy grid
    bool hasLineOfSight(double x1, double y1, double x2, double y2,
                        const nav_msgs::msg::OccupancyGrid &map)
    {
        auto [gx1, gy1] = worldToGrid(x1, y1, map);
        auto [gx2, gy2] = worldToGrid(x2, y2, map);

        const int width = static_cast<int>(map.info.width);
        const int height = static_cast<int>(map.info.height);

        int dx = std::abs(gx2 - gx1);
        int dy = std::abs(gy2 - gy1);
        int sx = (gx1 < gx2) ? 1 : -1;
        int sy = (gy1 < gy2) ? 1 : -1;
        int err = dx - dy;

        int x = gx1;
        int y = gy1;

        while (true)
        {
            // Out of bounds -> treat as blocked
            if (x < 0 || x >= width || y < 0 || y >= height)
                return false;

            const int idx = getIndex(x, y, width);
            if (isOccupied(map.data[idx], occupied_threshold_))
                return false;

            if (x == gx2 && y == gy2)
                break;

            int e2 = 2 * err;
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

private:
    // Parameters
    double sensor_range_{5.0};
    double sensor_fov_h_{1.57};

    double min_dist_{1.5}, max_dist_{4.0};
    int num_dist_samples_{3}, num_angle_samples_{12};
    int min_coverage_{3}, max_viewpoints_{5};

    int8_t free_threshold_{25}, occupied_threshold_{65};

    // Robot footprint
    double robot_width_{0.5}, robot_length_{0.5}, robot_height_{0.3};
    double safety_margin_{0.3};
    double clearance_radius_{0.0};
    double flight_height_{1.5}, height_tolerance_{0.2};

    // Stabilization
    double vp_hysteresis_distance_{1.0};
    double vp_hysteresis_coverage_{1.3};
    double vp_tracking_timeout_{5.0};
    bool vp_stabilization_enabled_{true};

    // Occlusion-aware coverage
    bool occlusion_enabled_{true};
    double occlusion_angle_bin_rad_{0.02};
    int yaw_samples_{36};

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
