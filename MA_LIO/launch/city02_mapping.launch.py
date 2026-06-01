from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    # 1. Khai báo cấu hình các tham số truyền vào từ dòng lệnh (Arguments)
    city02_root = LaunchConfiguration('city02_root')
    rate = LaunchConfiguration('rate')
    rviz_enable = LaunchConfiguration('rviz')

    # Đường dẫn nạp file cấu hình City.yaml mặc định
    city_yaml = PathJoinSubstitution([
        FindPackageShare('ma_lio'),
        'config',
        'City.yaml',
    ])

    # Đường dẫn nạp file cấu hình giao diện đồ họa Rviz
    rviz_config_dir = PathJoinSubstitution([
        FindPackageShare('ma_lio'),
        'rviz_cfg',
        'ma_lio.rviz'
    ])

    return LaunchDescription([
        # Khai báo các đối số để người dùng tùy biến khi gõ lệnh
        DeclareLaunchArgument('city02_root', description='Path to City02/sensor_data'),
        DeclareLaunchArgument('rate', default_value='1.0', description='Playback rate for player'),
        DeclareLaunchArgument('rviz', default_value='true', description='Whether to start RViz or not'),

        # Node 1: Bộ phát dữ liệu City02 Player
        Node(
            package='city02_player_py',
            executable='city02_player_node',
            name='city02_player_node',
            output='screen',
            parameters=[{
                'root': city02_root,
                'rate': rate,
            }],
        ),

        # Node 2: Lõi thuật toán SLAM ma_lio_mapping (Đã đồng bộ đầy đủ tham số ROS1 gốc)
        Node(
            package='ma_lio',
            executable='malio_mapping',
            name='ma_lio_mapping',
            output='screen',
            # Gộp chung file YAML cấu hình và các tham số ép cứng từ file ROS1 cũ
            parameters=[
                city_yaml, 
                {
                    'feature_extract_enable': False, # Ép kiểu bool trong Python thay cho 0/1 của ROS1
                    'max_iteration': 3,
                    'filter_size_surf': 0.5,
                    'filter_size_map': 0.5,
                    'cube_side_length': 1000.0,
                    'plane_th': 0.4,
                    'range_min': 0.0,
                    'range_max': 1.0,
                    'runtime_pos_log_enable': False
                }
            ],
        ),

        # Node 3: Khởi động giao diện 3D trực quan RViz2 (Tự động kích hoạt dựa trên đối số rviz)
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz2',
            arguments=['-d', rviz_config_dir],
            output='screen',
            condition=lambda context: rviz_enable.perform(context).lower() == 'true'
        )
    ])