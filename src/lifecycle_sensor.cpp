#include <chrono>
#include <memory>
#include <random>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "std_msgs/msg/float64.hpp"

using namespace std::chrono_literals;

class LifecycleSensor : public rclcpp_lifecycle::LifecycleNode
{
public:
    LifecycleSensor()
    : rclcpp_lifecycle::LifecycleNode("lifecycle_sensor")
    {
        // Random number generator setup
        std::random_device rd_;
        gen_ = std::mt19937(rd_());
        dist_ = std::uniform_real_distribution<>(0.0, 100.0);
    }

protected:
    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
    on_configure(const rclcpp_lifecycle::State &)
    {
        publisher_ = this->create_publisher<std_msgs::msg::Float64>(
            "/sensor_data", 10);

        RCLCPP_INFO(get_logger(), "Sensor configured");
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
    on_activate(const rclcpp_lifecycle::State &)
    {
        publisher_->on_activate();

        timer_ = this->create_wall_timer(
            500ms,
            std::bind(&LifecycleSensor::publish_data, this));

        RCLCPP_INFO(get_logger(), "Sensor activated");
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
    on_deactivate(const rclcpp_lifecycle::State &)
    {
        publisher_->on_deactivate();
        timer_.reset();

        RCLCPP_INFO(get_logger(), "Sensor deactivated");
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
    on_cleanup(const rclcpp_lifecycle::State &)
    {
        publisher_.reset();
        timer_.reset();

        RCLCPP_INFO(get_logger(), "Sensor cleaned up");
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

    rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
    on_shutdown(const rclcpp_lifecycle::State &)
    {
        RCLCPP_INFO(get_logger(), "Sensor shutting down");
        return rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn::SUCCESS;
    }

private:
    void publish_data()
    {
        auto msg = std_msgs::msg::Float64();
        msg.data = dist_(gen_);

        RCLCPP_INFO(get_logger(), "Publishing sensor data: %.2f", msg.data);
        publisher_->publish(msg);
    }

    rclcpp_lifecycle::LifecyclePublisher<std_msgs::msg::Float64>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_real_distribution<> dist_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LifecycleSensor>();
    rclcpp::spin(node->get_node_base_interface());
    rclcpp::shutdown();
    return 0;
}
