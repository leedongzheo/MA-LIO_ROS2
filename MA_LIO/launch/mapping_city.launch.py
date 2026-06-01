from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    pkg_share = get_package_share_directory('ma_lio')
    city_cfg = os.path.join(pkg_share, 'config', 'City.yaml')

    return LaunchDescription([
        Node(
            package='ma_lio',
            executable='malio_mapping',
            name='malio_mapping',
            output='screen',
            parameters=[city_cfg]
        )
    ])
