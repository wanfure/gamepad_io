#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>

#include <SDL.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("gamepad_printer");

    const int device_id = node->declare_parameter<int>("device_id", 0);
    std::string device_name = "未获取";

    if (SDL_Init(SDL_INIT_JOYSTICK) != 0) {
        RCLCPP_ERROR(
            node->get_logger(),
            "SDL 初始化失败：%s", SDL_GetError());
    } else {
        RCLCPP_INFO(
            node->get_logger(),
            "SDL 发现手柄数量：%d", SDL_NumJoysticks());

        const char *name = SDL_JoystickNameForIndex(device_id);

        if (name != nullptr) {
            device_name = name;
        } else {
            RCLCPP_ERROR(
                node->get_logger(),
                "手柄名称查询失败：%s", SDL_GetError());
        }

        SDL_Quit();
    }

    std::string last_output;

    auto subscription =
        node->create_subscription<sensor_msgs::msg::Joy>(
            "/joy",
            rclcpp::SensorDataQoS(),
            [&last_output, device_id, device_name](sensor_msgs::msg::Joy::ConstSharedPtr msg)
            {
                const char *axis_names[] = {
                    "左摇杆左右",
                    "左摇杆上下",
                    "左扳机 LT",
                    "右摇杆左右",
                    "右摇杆上下",
                    "右扳机 RT",
                    "方向键左右",
                    "方向键上下"
                };

                const char *button_names[] = {
                    "A", "B", "X", "Y",
                    "LB", "RB", "SELECT", "START",
                    "开机键",
                    "左摇杆按下 L3",
                    "右摇杆按下 R3"
                };

                std::ostringstream out;
                out << std::fixed << std::setprecision(6);
                out << "\n========== 手柄状态 ==========\n";
                out << "轴总数：" << msg->axes.size() << '\n';
                out << "按键总数：" << msg->buttons.size() << '\n';
                out << "手柄名称：" << device_name << '\n';
                out << "设备编号（SDL）：" << device_id << '\n';
                out << "电量：当前接口不可获取\n";
                out << "充电状态：当前接口不可获取\n";
                out << "无线信号强度：当前接口不可获取\n";

                for (size_t i = 0; i < msg->axes.size(); ++i) {
                    const char *name =
                        i < 8 ? axis_names[i] : "其他轴";

                    out << name << "  axes[" << i << "] = "
                        << msg->axes[i] << '\n';
                }

                for (size_t i = 0; i < msg->buttons.size(); ++i) {
                    const char *name =
                        i < 11 ? button_names[i] : "其他按键";

                    out << name << "  buttons[" << i << "] = "
                        << msg->buttons[i] << '\n';
                }

                const std::string output = out.str();

                if (output != last_output) {
                    std::cout << output << std::flush;
                    last_output = output;
                }
            });

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
