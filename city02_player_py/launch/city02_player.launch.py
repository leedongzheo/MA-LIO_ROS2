from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument('root', description='Path to City02/sensor_data'),
        DeclareLaunchArgument('rate', default_value='1.0'),
        Node(
            package='city02_player_py',
            executable='city02_player_node',
            output='screen',
            parameters=[{
                'root': LaunchConfiguration('root'),
                'rate': LaunchConfiguration('rate'),
            }],
        )
    ])
