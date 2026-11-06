from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    # 找到功能包安装目录中的配置文件
    config_file = PathJoinSubstitution([
        FindPackageShare('gamepad_io'),
        'config',
        'gamepad.yaml',
    ])

    return LaunchDescription([
        DeclareLaunchArgument(
            'enable_print', default_value='true'),  # 默认开启打印

        DeclareLaunchArgument(
            'enable_rumble', default_value='true'),  # 默认开启振动接口

        Node(
            package='joy',
            executable='joy_node',
            name='joy_node',
            parameters=[config_file],  # 读取手柄配置
            output='screen',
        ),

        Node(
            package='gamepad_io',
            executable='gamepad_print',
            name='gamepad_printer',
            parameters=[config_file],  # 读取同一份设备编号
            condition=IfCondition(
                LaunchConfiguration('enable_print')),  # 打印开关
            output='screen',
        ),

        Node(
            package='gamepad_io',
            executable='gamepad_rumble',
            name='gamepad_rumble',
            condition=IfCondition(
                LaunchConfiguration('enable_rumble')),  # 振动接口开关
            output='screen',
        ),
    ])