#include <chrono>
#include <cmath>
#include <functional>
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy_feedback.hpp>

#include "gamepad_io/msg/rumble_command.hpp"

using namespace std::chrono_literals;

// 接收振动命令，不订阅 /joy，也不判断任何按键。
class GamepadRumble : public rclcpp::Node
{
public:
    GamepadRumble() : Node("gamepad_rumble")
    {
        feedback_pub_ = create_publisher<sensor_msgs::msg::JoyFeedback>(
            "/joy/set_feedback", 10);
        command_sub_ = create_subscription<gamepad_io::msg::RumbleCommand>(
            "/rumble_cmd", rclcpp::QoS(1),
            std::bind(&GamepadRumble::on_command, this, std::placeholders::_1));

        // 用真实经过的时间计时，不受 ROS 仿真时间暂停或跳变影响。
        timer_ = create_wall_timer(10ms, std::bind(&GamepadRumble::on_timer, this));
        RCLCPP_INFO(get_logger(),
            "等待 /rumble_cmd：intensity=0~1，duration_ms=毫秒；任一为 0 则停止");
    }

private:
    using Clock = std::chrono::steady_clock;

    void send_feedback(float intensity)
    {
        sensor_msgs::msg::JoyFeedback feedback;
        feedback.type = sensor_msgs::msg::JoyFeedback::TYPE_RUMBLE;
        feedback.id = 0;
        feedback.intensity = intensity;
        feedback_pub_->publish(feedback);
    }

    void stop()
    {
        active_ = false;
        send_feedback(0.0f);
    }

    void on_command(gamepad_io::msg::RumbleCommand::ConstSharedPtr command)
    {
        if (!std::isfinite(command->intensity) ||
            command->intensity < 0.0f || command->intensity > 1.0f) {
            RCLCPP_WARN(get_logger(), "拒绝命令：intensity 必须是 0~1 的有限数值");
            return;
        }

        if (command->intensity == 0.0f || command->duration_ms == 0) {
            stop();
            RCLCPP_INFO(get_logger(), "收到停止振动命令");
            return;
        }

        if (feedback_pub_->get_subscription_count() == 0) {
            RCLCPP_WARN(get_logger(),
                "没有 /joy/set_feedback 订阅者，请先启动 joy_node");
            return;
        }

        // 新命令立即覆盖旧命令，并从接收时刻重新计时。
        intensity_ = command->intensity;
        deadline_ = Clock::now() + std::chrono::milliseconds(command->duration_ms);
        active_ = true;
        send_feedback(intensity_);
        next_refresh_ = Clock::now() + 200ms;
        RCLCPP_INFO(get_logger(), "开始振动：强度 %.3f，持续 %u 毫秒",
            static_cast<double>(intensity_), command->duration_ms);
    }

    void on_timer()
    {
        if (!active_) {
            return;
        }

        const auto now = Clock::now();
        if (now >= deadline_) {
            stop();
            RCLCPP_INFO(get_logger(), "振动时间到，已发送停止指令");
        } else if (now >= next_refresh_) {
            // joy_node 每条反馈执行 1 秒；定期刷新以支持更长时间。
            send_feedback(intensity_);
            next_refresh_ = now + 200ms;
        }
    }

    rclcpp::Publisher<sensor_msgs::msg::JoyFeedback>::SharedPtr feedback_pub_;
    rclcpp::Subscription<gamepad_io::msg::RumbleCommand>::SharedPtr command_sub_;
    rclcpp::TimerBase::SharedPtr timer_;
    bool active_ = false;
    float intensity_ = 0.0f;
    Clock::time_point deadline_;
    Clock::time_point next_refresh_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GamepadRumble>());
    rclcpp::shutdown();
    return 0;
}
