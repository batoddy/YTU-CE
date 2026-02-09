/**
 * Greedy Frontier Selector (Top-K Targets + Scaled Score + Time Lower Bound + Optional Nav2 Goal)
 *
 * Input:
 *   /frontier_clusters_complete (frontier_exploration/FrontierArray)
 *   /odom OR /mavros/local_position/pose
 *   /global_costmap/costmap (optional)  -> filter risky targets
 *
 * Output:
 *   /exploration/global_tour (exploration_planner/ExplorationStatus)
 *   /frontier_clusters_scored (frontier_exploration/FrontierArray)  [debug/inspection]
 *
 * Optional:
 *   If nav2_enable=true -> send best target as Nav2 NavigateToPose goal.
 *
 * Key idea:
 *   - No max-normalization. Use fixed "scale" parameters to balance units.
 *   - Compute score for every viewpoint and store it in vp.score.
 *   - Keep top K best viewpoints globally as "target waypoints".
 *
 * Score (bigger is better):
 *   time_cost = max(dist / v_max, yaw_diff / yaw_rate_max)
 *   score =  + w_cov  * (coverage / coverage_scale)
 *            + w_size * (cluster_size / size_scale)
 *            - w_time * (time_cost / time_scale)
 *            - w_costmap * (costmap_cost / costmap_scale)   [optional]
 *
 * Notes:
 *  - Requires Viewpoint.msg to include: float64 score
 */

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <nav_msgs/msg/occupancy_grid.hpp>
#include <std_srvs/srv/trigger.hpp>

#include "frontier_exploration/msg/frontier_array.hpp"
#include "frontier_exploration/msg/frontier_cluster.hpp"
#include "frontier_exploration/msg/viewpoint.hpp"

#include "exploration_planner/msg/exploration_status.hpp"
#include "exploration_planner/common.hpp"

#include <rclcpp_action/rclcpp_action.hpp>
#include <nav2_msgs/action/navigate_to_pose.hpp>

#include <algorithm>
#include <limits>
#include <vector>
#include <tuple>
#include <cmath>

using namespace exploration_planner;

class GreedyFrontierSelectorNode : public rclcpp::Node
{
public:
    GreedyFrontierSelectorNode() : Node("greedy_frontier_selector")
    {
        // -----------------------------
        // Weights (influence)
        // -----------------------------
        declare_parameter("w_time", 1.0);
        declare_parameter("w_coverage", 1.0);
        declare_parameter("w_size", 0.2);
        declare_parameter("w_costmap", 0.0); // 0 = ignore costmap in score (filtering still possible)

        // -----------------------------
        // Scales (unit balancing)
        // -----------------------------
        declare_parameter("coverage_scale", 50.0); // "50 cells worth of coverage" ~ 1 unit
        declare_parameter("size_scale", 200.0);    // "200 cells cluster size" ~ 1 unit
        declare_parameter("time_scale", 5.0);      // "5 seconds" ~ 1 unit

        // -----------------------------
        // Viewpoint evaluation
        // -----------------------------
        declare_parameter("max_viewpoints_to_evaluate", 5); // per cluster
        declare_parameter("top_k_targets", 3);              // global best K targets to publish
        declare_parameter("use_vp_yaw", true);              // use vp.yaw from node-2 or compute centroid-facing

        // -----------------------------
        // Motion limits (used in time lower bound)
        // -----------------------------
        declare_parameter("v_max", 4.0);
        declare_parameter("yaw_rate_max", 2.5);

        // -----------------------------
        // Optional stabilization (target jitter reduction)
        // -----------------------------
        declare_parameter("hysteresis_enabled", true);
        declare_parameter("switch_margin", 0.15); // new_best must be 15% better to switch (score-based)

        // -----------------------------
        // Costmap usage (optional)
        // -----------------------------
        declare_parameter("costmap_filter_enabled", false);
        declare_parameter("costmap_lethal_threshold", 65); // OccupancyGrid style threshold
        declare_parameter("costmap_scale", 100.0);         // if w_costmap > 0

        // -----------------------------
        // Nav2 integration (optional)
        // -----------------------------
        declare_parameter("nav2_enable", false);
        declare_parameter("nav2_action_name", "navigate_to_pose");
        declare_parameter("nav2_min_send_period", 1.0); // seconds
        declare_parameter("nav2_goal_pos_tol", 0.5);    // meters
        declare_parameter("nav2_goal_yaw_tol", 0.35);   // radians

        // Read params
        w_time_ = get_parameter("w_time").as_double();
        w_coverage_ = get_parameter("w_coverage").as_double();
        w_size_ = get_parameter("w_size").as_double();
        w_costmap_ = get_parameter("w_costmap").as_double();

        coverage_scale_ = std::max(1e-6, get_parameter("coverage_scale").as_double());
        size_scale_ = std::max(1e-6, get_parameter("size_scale").as_double());
        time_scale_ = std::max(1e-6, get_parameter("time_scale").as_double());

        max_viewpoints_to_evaluate_ = get_parameter("max_viewpoints_to_evaluate").as_int();
        top_k_targets_ = std::max(1, get_parameter("top_k_targets").as_int());
        use_vp_yaw_ = get_parameter("use_vp_yaw").as_bool();

        v_max_ = std::max(1e-6, get_parameter("v_max").as_double());
        yaw_rate_max_ = std::max(1e-6, get_parameter("yaw_rate_max").as_double());

        hysteresis_enabled_ = get_parameter("hysteresis_enabled").as_bool();
        switch_margin_ = std::max(0.0, get_parameter("switch_margin").as_double());

        costmap_filter_enabled_ = get_parameter("costmap_filter_enabled").as_bool();
        costmap_lethal_threshold_ = get_parameter("costmap_lethal_threshold").as_int();
        costmap_scale_ = std::max(1e-6, get_parameter("costmap_scale").as_double());

        // Nav2 params
        nav2_enable_ = get_parameter("nav2_enable").as_bool();
        nav2_action_name_ = get_parameter("nav2_action_name").as_string();
        nav2_min_send_period_ = std::max(0.0, get_parameter("nav2_min_send_period").as_double());
        nav2_goal_pos_tol_ = std::max(0.0, get_parameter("nav2_goal_pos_tol").as_double());
        nav2_goal_yaw_tol_ = std::max(0.0, get_parameter("nav2_goal_yaw_tol").as_double());

        // Subscribers
        clusters_sub_ = create_subscription<frontier_exploration::msg::FrontierArray>(
            "/frontier_clusters_complete", 10,
            std::bind(&GreedyFrontierSelectorNode::clustersCallback, this, std::placeholders::_1));

        odom_sub_ = create_subscription<nav_msgs::msg::Odometry>(
            "/odom", 10,
            std::bind(&GreedyFrontierSelectorNode::odomCallback, this, std::placeholders::_1));

        pose_sub_ = create_subscription<geometry_msgs::msg::PoseStamped>(
            "/mavros/local_position/pose", 10,
            std::bind(&GreedyFrontierSelectorNode::poseCallback, this, std::placeholders::_1));

        costmap_sub_ = this->create_subscription<nav_msgs::msg::OccupancyGrid>(
            "/global_costmap/costmap",
            rclcpp::QoS(1).transient_local(),
            [this](const nav_msgs::msg::OccupancyGrid::SharedPtr msg)
            { costmap_ = msg; });

        // Publishers
        tour_pub_ = create_publisher<exploration_planner::msg::ExplorationStatus>(
            "/exploration/global_tour", 10);

        // Debug publisher: scored frontier array
        scored_clusters_pub_ = create_publisher<frontier_exploration::msg::FrontierArray>(
            "/frontier_clusters_scored", 10);

        // Services
        start_srv_ = create_service<std_srvs::srv::Trigger>(
            "/exploration/start",
            std::bind(&GreedyFrontierSelectorNode::startCallback, this,
                      std::placeholders::_1, std::placeholders::_2));

        stop_srv_ = create_service<std_srvs::srv::Trigger>(
            "/exploration/stop",
            std::bind(&GreedyFrontierSelectorNode::stopCallback, this,
                      std::placeholders::_1, std::placeholders::_2));

        // Nav2 action client (created even if disabled; cheap)
        nav_client_ = rclcpp_action::create_client<NavigateToPose>(this, nav2_action_name_);

        RCLCPP_INFO(get_logger(), "Greedy Selector initialized");
        RCLCPP_INFO(get_logger(), "Weights: w_time=%.2f w_cov=%.2f w_size=%.2f w_costmap=%.2f",
                    w_time_, w_coverage_, w_size_, w_costmap_);
        RCLCPP_INFO(get_logger(), "Scales:  cov=%.1f size=%.1f time=%.1f",
                    coverage_scale_, size_scale_, time_scale_);
        RCLCPP_INFO(get_logger(), "Top-K targets: %d, per-cluster eval: %d, time model: v_max=%.2f yaw_rate_max=%.2f",
                    top_k_targets_, max_viewpoints_to_evaluate_, v_max_, yaw_rate_max_);
        RCLCPP_INFO(get_logger(), "Nav2: %s (action=%s, period=%.2fs tol=%.2fm %.2frad)",
                    nav2_enable_ ? "ENABLED" : "DISABLED",
                    nav2_action_name_.c_str(),
                    nav2_min_send_period_, nav2_goal_pos_tol_, nav2_goal_yaw_tol_);
    }

private:
    // -----------------------------
    // Nav2 types/state
    // -----------------------------
    using NavigateToPose = nav2_msgs::action::NavigateToPose;
    using GoalHandleNav = rclcpp_action::ClientGoalHandle<NavigateToPose>;
    rclcpp_action::Client<NavigateToPose>::SharedPtr nav_client_;
    GoalHandleNav::SharedPtr nav_goal_handle_;

    bool nav2_enable_{false};
    std::string nav2_action_name_{"navigate_to_pose"};
    double nav2_min_send_period_{1.0};
    double nav2_goal_pos_tol_{0.5};
    double nav2_goal_yaw_tol_{0.35};

    rclcpp::Time last_nav2_send_time_{0, 0, RCL_ROS_TIME};
    bool have_last_sent_goal_{false};
    geometry_msgs::msg::PoseStamped last_sent_goal_;

    // -----------------------------
    // Services
    // -----------------------------
    void startCallback(const std_srvs::srv::Trigger::Request::SharedPtr,
                       std_srvs::srv::Trigger::Response::SharedPtr response)
    {
        exploration_active_ = true;
        response->success = true;
        response->message = "Exploration started";
        RCLCPP_INFO(get_logger(), "Exploration STARTED");
    }

    void stopCallback(const std_srvs::srv::Trigger::Request::SharedPtr,
                      std_srvs::srv::Trigger::Response::SharedPtr response)
    {
        exploration_active_ = false;

        exploration_planner::msg::ExplorationStatus status;
        status.header.stamp = now();
        status.state = exploration_planner::msg::ExplorationStatus::IDLE;
        tour_pub_->publish(status);

        response->success = true;
        response->message = "Exploration stopped";
        RCLCPP_INFO(get_logger(), "Exploration STOPPED");
    }

    // -----------------------------
    // Pose updates
    // -----------------------------
    void odomCallback(const nav_msgs::msg::Odometry::SharedPtr msg)
    {
        current_pose_.header = msg->header;
        current_pose_.pose = msg->pose.pose;
        current_velocity_ = msg->twist.twist;
        have_pose_ = true;
    }

    void poseCallback(const geometry_msgs::msg::PoseStamped::SharedPtr msg)
    {
        if (!have_pose_)
        {
            current_pose_ = *msg;
            have_pose_ = true;
        }
    }

    // -----------------------------
    // Main callback
    // -----------------------------
    void clustersCallback(const frontier_exploration::msg::FrontierArray::SharedPtr msg)
    {
        if (!exploration_active_)
            return;

        if (!have_pose_)
        {
            RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 5000, "No pose received yet");
            return;
        }

        // Copy input -> scored output (so we can write vp.score)
        frontier_exploration::msg::FrontierArray scored = *msg;

        // Collect candidates while writing vp.score into scored.clusters
        std::vector<Candidate> candidates;
        candidates.reserve(256);

        const auto &cur_pos = current_pose_.pose.position;
        const double cur_yaw = getYaw(current_pose_.pose.orientation);

        for (auto &cluster : scored.clusters)
        {
            if (cluster.viewpoints.empty())
                continue;

            const int n_eval = std::min(static_cast<int>(cluster.viewpoints.size()),
                                        std::max(1, max_viewpoints_to_evaluate_));

            for (int i = 0; i < n_eval; ++i)
            {
                auto &vp = cluster.viewpoints[i];

                // Optional costmap filter
                if (costmap_filter_enabled_ && costmap_)
                {
                    const int cm = costAt(*costmap_, vp.position.x, vp.position.y);
                    if (cm < 0)
                        continue; // out of bounds / unknown -> reject
                    if (cm >= costmap_lethal_threshold_)
                        continue;
                }

                // Compute score
                const double score = computeScore(cluster, vp, cur_pos, cur_yaw);

                // Write back into message
                vp.score = score;

                candidates.push_back(Candidate{
                    cluster.id, i, score,
                    vp.position, vp.yaw,
                    cluster.centroid, cluster.size,
                    vp.coverage});
            }
        }

        // Publish scored clusters for inspection/debug
        scored_clusters_pub_->publish(scored);

        if (candidates.empty())
        {
            exploration_planner::msg::ExplorationStatus status;
            status.header.stamp = now();
            status.header.frame_id = msg->header.frame_id;
            status.state = exploration_planner::msg::ExplorationStatus::COMPLETED;
            tour_pub_->publish(status);
            RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 5000, "No valid candidates - exploration complete!");
            return;
        }

        // Sort by score desc, keep top K
        std::sort(candidates.begin(), candidates.end(),
                  [](const Candidate &a, const Candidate &b)
                  { return a.score > b.score; });

        const int k = std::min(top_k_targets_, static_cast<int>(candidates.size()));
        std::vector<Candidate> new_targets(candidates.begin(), candidates.begin() + k);

        // Optional hysteresis: keep previous best unless new best is sufficiently better
        if (hysteresis_enabled_ && have_last_best_)
        {
            const Candidate &new_best = new_targets.front();
            const Candidate &old_best = last_best_;

            if (new_best.score <= old_best.score * (1.0 + switch_margin_))
            {
                new_targets.front() = old_best;
            }
        }

        // Save targets (internal state)
        target_viewpoints_ = new_targets;
        last_best_ = target_viewpoints_.front();
        have_last_best_ = true;

        // Publish ExplorationStatus with K waypoints
        publishTour(msg->header, target_viewpoints_, cur_pos);

        // If Nav2 enabled: send best as goal (rate-limited + tolerance)
        if (nav2_enable_ && !target_viewpoints_.empty())
        {
            geometry_msgs::msg::PoseStamped goal;
            goal.header.stamp = now();
            goal.header.frame_id = msg->header.frame_id; // should match Nav2 global frame ("map")

            const auto &best = target_viewpoints_.front();
            goal.pose.position = best.position;

            const double yaw = use_vp_yaw_
                                   ? best.vp_yaw
                                   : std::atan2(best.centroid.y - best.position.y, best.centroid.x - best.position.x);
            goal.pose.orientation = yawToQuaternion(yaw);

            maybeSendNav2Goal(goal);
        }
    }

    // -----------------------------
    // Scoring (bigger is better)
    // -----------------------------
    double computeScore(const frontier_exploration::msg::FrontierCluster &cluster,
                        const frontier_exploration::msg::Viewpoint &vp,
                        const geometry_msgs::msg::Point &cur_pos,
                        double cur_yaw)
    {
        const double dist = distance2D(cur_pos, vp.position);

        const double target_yaw = use_vp_yaw_
                                      ? vp.yaw
                                      : std::atan2(cluster.centroid.y - vp.position.y, cluster.centroid.x - vp.position.x);

        const double yaw_diff = angleDiff(cur_yaw, target_yaw);
        const double t_trans = dist / v_max_;
        const double t_yaw = yaw_diff / yaw_rate_max_;
        const double time_cost = std::max(t_trans, t_yaw);

        const double cov_term = static_cast<double>(vp.coverage) / coverage_scale_;
        const double size_term = static_cast<double>(cluster.size) / size_scale_;
        const double time_term = time_cost / time_scale_;

        double costmap_term = 0.0;
        if (w_costmap_ > 0.0 && costmap_)
        {
            const int cm = costAt(*costmap_, vp.position.x, vp.position.y);
            if (cm >= 0)
                costmap_term = static_cast<double>(cm) / costmap_scale_;
        }

        const double score =
            (w_coverage_ * cov_term) +
            (w_size_ * size_term) -
            (w_time_ * time_term) -
            (w_costmap_ * costmap_term);

        return score;
    }

    // -----------------------------
    // Publish top-K targets as "global tour"
    // -----------------------------
    void publishTour(const std_msgs::msg::Header &header,
                     const std::vector<Candidate> &targets,
                     const geometry_msgs::msg::Point &cur_pos)
    {
        exploration_planner::msg::ExplorationStatus status;
        status.header.stamp = now();
        status.header.frame_id = header.frame_id;
        status.state = exploration_planner::msg::ExplorationStatus::EXPLORING;

        status.cluster_order.clear();
        status.waypoints.clear();

        double total_dist = 0.0;
        geometry_msgs::msg::Point last = cur_pos;

        for (const auto &t : targets)
        {
            status.cluster_order.push_back(t.cluster_id);

            geometry_msgs::msg::PoseStamped wp;
            wp.header = status.header;
            wp.pose.position = t.position;

            const double yaw = use_vp_yaw_
                                   ? t.vp_yaw
                                   : std::atan2(t.centroid.y - t.position.y, t.centroid.x - t.position.x);

            wp.pose.orientation = yawToQuaternion(yaw);

            status.waypoints.push_back(wp);

            total_dist += distance2D(last, t.position);
            last = t.position;
        }

        status.total_waypoints = static_cast<int32_t>(status.waypoints.size());
        status.current_waypoint_index = 0;
        status.current_target = status.waypoints.front();

        status.total_distance_remaining = total_dist;
        status.estimated_time_remaining = total_dist / v_max_;

        tour_pub_->publish(status);

        const auto &best = targets.front();
        RCLCPP_INFO(get_logger(),
                    "BEST: cluster=%u vp=%d score=%.3f cov=%d size=%u pos=(%.2f,%.2f) topK=%zu",
                    best.cluster_id, best.vp_index, best.score, best.coverage, best.cluster_size,
                    best.position.x, best.position.y, targets.size());
    }

    // -----------------------------
    // Nav2 goal send (rate-limited + tolerance)
    // -----------------------------
    double yawFromQuat(const geometry_msgs::msg::Quaternion &q) const
    {
        return getYaw(q);
    }

    bool rateLimitOk() const
    {
        if (last_nav2_send_time_.nanoseconds() == 0)
            return true;
        const double elapsed = (now() - last_nav2_send_time_).seconds();
        return elapsed >= nav2_min_send_period_;
    }

    bool goalSignificantlyDifferent(const geometry_msgs::msg::PoseStamped &g) const
    {
        if (!have_last_sent_goal_)
            return true;

        const double d = distance2D(g.pose.position, last_sent_goal_.pose.position);
        const double dyaw = angleDiff(yawFromQuat(g.pose.orientation),
                                      yawFromQuat(last_sent_goal_.pose.orientation));

        return (d > nav2_goal_pos_tol_) || (dyaw > nav2_goal_yaw_tol_);
    }

    void maybeSendNav2Goal(const geometry_msgs::msg::PoseStamped &goal)
    {
        if (!nav_client_)
            return;

        if (!rateLimitOk())
            return;

        if (!goalSignificantlyDifferent(goal))
            return;

        if (!nav_client_->wait_for_action_server(std::chrono::milliseconds(200)))
        {
            RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 2000,
                                 "Nav2 action server '%s' not available yet", nav2_action_name_.c_str());
            return;
        }

        NavigateToPose::Goal nav_goal;
        nav_goal.pose = goal;

        auto opts = rclcpp_action::Client<NavigateToPose>::SendGoalOptions();

        opts.goal_response_callback =
            [this](const GoalHandleNav::SharedPtr &handle)
        {
            if (!handle)
            {
                RCLCPP_WARN(get_logger(), "Nav2 goal rejected");
            }
            else
            {
                nav_goal_handle_ = handle;
                RCLCPP_INFO(get_logger(), "Nav2 goal accepted");
            }
        };

        opts.result_callback =
            [this](const GoalHandleNav::WrappedResult &result)
        {
            RCLCPP_INFO(get_logger(), "Nav2 result code: %d", static_cast<int>(result.code));
        };

        nav_client_->async_send_goal(nav_goal, opts);

        last_nav2_send_time_ = now();
        last_sent_goal_ = goal;
        have_last_sent_goal_ = true;

        RCLCPP_INFO(get_logger(), "Sent Nav2 goal: (%.2f, %.2f) yaw=%.1f deg",
                    goal.pose.position.x, goal.pose.position.y,
                    yawFromQuat(goal.pose.orientation) * 180.0 / M_PI);
    }

    // -----------------------------
    // Costmap helpers (OccupancyGrid style)
    // -----------------------------
    static inline int getIndex2D(int x, int y, int w) { return y * w + x; }

    static inline std::pair<int, int> worldToGrid2D(double wx, double wy, const nav_msgs::msg::OccupancyGrid &map)
    {
        const double res = map.info.resolution;
        const double ox = map.info.origin.position.x;
        const double oy = map.info.origin.position.y;
        const int gx = static_cast<int>((wx - ox) / res);
        const int gy = static_cast<int>((wy - oy) / res);
        return {gx, gy};
    }

    int costAt(const nav_msgs::msg::OccupancyGrid &cm, double wx, double wy) const
    {
        if (cm.data.empty())
            return -1;

        auto [gx, gy] = worldToGrid2D(wx, wy, cm);
        const int w = static_cast<int>(cm.info.width);
        const int h = static_cast<int>(cm.info.height);

        if (gx < 0 || gx >= w || gy < 0 || gy >= h)
            return -1;

        const int idx = getIndex2D(gx, gy, w);
        return static_cast<int>(cm.data[idx]);
    }

    // -----------------------------
    // Candidate struct (target_viewpoint)
    // -----------------------------
    struct Candidate
    {
        uint32_t cluster_id;
        int vp_index;
        double score;

        geometry_msgs::msg::Point position;
        double vp_yaw;

        geometry_msgs::msg::Point centroid;
        uint32_t cluster_size;
        int coverage;
    };

private:
    // Weights
    double w_time_{1.0};
    double w_coverage_{1.0};
    double w_size_{0.2};
    double w_costmap_{0.0};

    // Scales
    double coverage_scale_{50.0};
    double size_scale_{200.0};
    double time_scale_{5.0};
    double costmap_scale_{100.0};

    // Viewpoint selection
    int max_viewpoints_to_evaluate_{5};
    int top_k_targets_{3};
    bool use_vp_yaw_{true};

    // Motion limits
    double v_max_{4.0};
    double yaw_rate_max_{2.5};

    // Stabilization
    bool hysteresis_enabled_{true};
    double switch_margin_{0.15};
    bool have_last_best_{false};
    Candidate last_best_;

    // Costmap
    bool costmap_filter_enabled_{false};
    int costmap_lethal_threshold_{65};
    nav_msgs::msg::OccupancyGrid::SharedPtr costmap_;

    // State
    geometry_msgs::msg::PoseStamped current_pose_;
    geometry_msgs::msg::Twist current_velocity_;
    bool have_pose_{false};
    bool exploration_active_{false};

    // Target viewpoint list (top-K)
    std::vector<Candidate> target_viewpoints_;

    // ROS
    rclcpp::Subscription<frontier_exploration::msg::FrontierArray>::SharedPtr clusters_sub_;
    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
    rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr pose_sub_;
    rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr costmap_sub_;

    rclcpp::Publisher<exploration_planner::msg::ExplorationStatus>::SharedPtr tour_pub_;
    rclcpp::Publisher<frontier_exploration::msg::FrontierArray>::SharedPtr scored_clusters_pub_;

    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr start_srv_;
    rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr stop_srv_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GreedyFrontierSelectorNode>());
    rclcpp::shutdown();
    return 0;
}
