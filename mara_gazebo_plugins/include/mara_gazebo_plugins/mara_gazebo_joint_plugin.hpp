// Copyright (c) 2010, Daniel Hewlett, Antons Rebguns
// All rights reserved.
//
// Software License Agreement (BSD License 2.0)
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//  * Neither the name of the company nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef GAZEBO_PLUGINS__MARA_GAZEBO_PLUGINS_HPP_
#define GAZEBO_PLUGINS__MARA_GAZEBO_PLUGINSE_HPP_

#include <gazebo/common/Plugin.hh>
#include <gazebo/common/Time.hh>
#include <gazebo/physics/Joint.hh>
#include <gazebo/physics/Link.hh>
#include <gazebo/physics/Model.hh>
#include <gazebo/physics/World.hh>

#include <gazebo_ros/conversions/builtin_interfaces.hpp>
#include <gazebo_ros/conversions/geometry_msgs.hpp>
#include <gazebo_ros/node.hpp>

#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "hrim_actuator_rotaryservo_msgs/msg/state_rotary_servo.hpp"
#include "hrim_actuator_rotaryservo_msgs/msg/goal_rotary_servo.hpp"
#include "hrim_actuator_rotaryservo_msgs/msg/specs_rotary_servo.hpp"

#include "hrim_generic_msgs/msg/id.hpp"
#include "hrim_generic_msgs/msg/status.hpp"
#include "hrim_generic_msgs/msg/power.hpp"
#include "hrim_generic_msgs/msg/simulation3_d.hpp"
#include "hrim_generic_msgs/msg/simulation_urdf.hpp"
#include "hrim_generic_msgs/msg/specs_communication.hpp"
#include "hrim_generic_msgs/msg/state_communication.hpp"

#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include <ament_index_cpp/get_package_share_directory.hpp>

#include <mara_gazebo_plugins/spline.hpp>

static const double NSEC_PER_SECOND = 1e+9;

using namespace std::chrono_literals;

namespace gazebo_plugins
{
  class MARAGazeboPluginRosPrivate
  {
  public:

    /// Indicates which wheel
    enum
    {
      AXIS1 = 0,

      AXIS2 = 1,
    };

    /// Callback to be called at every simulation iteration.
    /// \param[in] _info Updated simulation info.
    void OnUpdate(const gazebo::common::UpdateInfo & _info);

    void timer_motor_state_msgs();
    std::shared_ptr<rclcpp::TimerBase> timer_motor_state_;

    void publish3DModels();
    void readfullFile(std::string file_to_read, hrim_generic_msgs::msg::Simulation3D& msg_sim_3d);

    /// A pointer to the GazeboROS node.
    gazebo_ros::Node::SharedPtr ros_node_;

    /// Connection to event called at every world iteration.
    gazebo::event::ConnectionPtr update_connection_;

    /// Pointers to wheel joints.
    std::vector<gazebo::physics::JointPtr> joints_;

    /// Pointer to model.
    gazebo::physics::ModelPtr model_;

    /// Protect variables accessed on callbacks.
    std::mutex lock_;

    /// Update period in seconds.
    double update_period_;

    /// Last update time.
    gazebo::common::Time last_update_time_;

    /// Last time the encoder was updated
    gazebo::common::Time last_encoder_update_;

    /// Robot base frame ID
    std::string robot_base_frame_;


    std::shared_ptr<rclcpp::Publisher<hrim_actuator_rotaryservo_msgs::msg::StateRotaryServo>> motor_state_axis1_pub;
    std::shared_ptr<rclcpp::Publisher<hrim_actuator_rotaryservo_msgs::msg::StateRotaryServo>> motor_state_axis2_pub;
    std::shared_ptr<rclcpp::Publisher<hrim_actuator_rotaryservo_msgs::msg::SpecsRotaryServo>> specs_pub;

    std::shared_ptr<rclcpp::Subscription<hrim_actuator_rotaryservo_msgs::msg::GoalRotaryServo>> command_sub_axis1_;
    std::shared_ptr<rclcpp::Subscription<hrim_actuator_rotaryservo_msgs::msg::GoalRotaryServo>> command_sub_axis2_;

    std::shared_ptr<rclcpp::Subscription<trajectory_msgs::msg::JointTrajectory>> trajectory_sub_;
    std::shared_ptr<rclcpp::Subscription<trajectory_msgs::msg::JointTrajectory>> trajectory2_sub_;

    void commandCallback_axis1(const hrim_actuator_rotaryservo_msgs::msg::GoalRotaryServo::SharedPtr msg);
    void commandCallback_axis2(const hrim_actuator_rotaryservo_msgs::msg::GoalRotaryServo::SharedPtr msg);
    void trajectoryAxis1Callback(const trajectory_msgs::msg::JointTrajectory::SharedPtr msg);
    void trajectoryAxis2Callback(const trajectory_msgs::msg::JointTrajectory::SharedPtr msg);

    std::shared_ptr<rclcpp::TimerBase> timer_info_;
    std::shared_ptr<rclcpp::TimerBase> timer_status_;
    std::shared_ptr<rclcpp::TimerBase> timer_power_;
    std::shared_ptr<rclcpp::TimerBase> timer_specs_;
    std::shared_ptr<rclcpp::TimerBase> timer_comm_;

    std::shared_ptr<rclcpp::Publisher<hrim_generic_msgs::msg::ID>> info_pub;
    std::shared_ptr<rclcpp::Publisher<hrim_generic_msgs::msg::Status>> status_pub;
    std::shared_ptr<rclcpp::Publisher<hrim_generic_msgs::msg::Power>> power_pub;
    std::shared_ptr<rclcpp::Publisher<hrim_generic_msgs::msg::Simulation3D>> sim3d_pub;
    std::shared_ptr<rclcpp::Publisher<hrim_generic_msgs::msg::SimulationURDF>> sim_urdf_pub;
    std::shared_ptr<rclcpp::Publisher<hrim_generic_msgs::msg::StateCommunication>> state_comm_pub;
    std::shared_ptr<rclcpp::Publisher<hrim_generic_msgs::msg::SpecsCommunication>> specs_comm_pub;

    void timer_info_msgs();
    void timer_power_msgs();
    void timer_status_msgs();
    void timer_specs_msgs();
    void timer_comm_msgs();

    // rclcpp::Clock clock_ros;

    std::vector<float> trajectories_position_axis1;
    std::vector<float> trajectories_velocities_axis1;
    std::vector<float> trajectories_position_axis2;
    std::vector<float> trajectories_velocities_axis2;
    bool executing_axis1;
    bool executing_axis2;
    unsigned int index_trajectory_axis1;
    unsigned int index_trajectory_axis2;
    float goal_position_axis1_rad;
    float goal_position_axis2_rad;

    std::string type_motor;

  };

/// A plugin for gazebo.
/*
 *
 * \author  Alejandro Hernandez (alex <at> erlerobotics.com)
 *
 */

class MARAGazeboPluginRos : public gazebo::ModelPlugin
{
public:
  /// Constructor
  MARAGazeboPluginRos();

  /// Destructor
  ~MARAGazeboPluginRos();

  void createGenericTopics(std::string node_name);

protected:
  // Documentation inherited
  void Load(gazebo::physics::ModelPtr _model, sdf::ElementPtr _sdf) override;

  // Documentation inherited
  void Reset() override;

private:
  /// Private data pointer
  std::unique_ptr<MARAGazeboPluginRosPrivate> impl_;
};
}  // namespace gazebo_plugins

#endif  // GAZEBO_PLUGINS__MARA_GAZEBO_PLUGINSE_HPP_
