#ifndef VIEWER_ROS_H
#define VIEWER_ROS_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QPixmap>
#include <QVector>
#include <QVector3D>
#include <QDateTime>
#include <QReadLocker>
#include <QPainter>
#include <QLabel>
#include <algorithm>
#include <rclcpp/rclcpp.hpp>
#include <builtin_interfaces/msg/time.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <image_transport/image_transport.hpp>
#include <image_transport/transport_hints.hpp>
#include <cv_bridge/cv_bridge.hpp>

#include <sensor_msgs/msg/point_cloud2.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <rosgraph_msgs/msg/clock.hpp>

#include <camera_info_manager/camera_info_manager.hpp>
#include <std_msgs/msg/string.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_srvs/srv/set_bool.hpp>
#include <std_msgs/msg/int64_multi_array.hpp>
#include <std_msgs/msg/float32.hpp>
#include <std_msgs/msg/float64.hpp>
#include <sensor_msgs/msg/nav_sat_fix.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/srv/set_camera_info.hpp>
#include <irp_sen_msgs/msg/vrs.hpp>
#include <irp_sen_msgs/msg/altimeter.hpp>
#include <irp_sen_msgs/msg/encoder.hpp>
#include <irp_sen_msgs/msg/fog.hpp>
#include <irp_sen_msgs/msg/imu.hpp>
#include <irp_sen_msgs/msg/fog3axis.hpp>
#include <irp_sen_msgs/msg/laser_scan_array.hpp>

#include <sensor_msgs/msg/imu.hpp>
#include <sensor_msgs/msg/magnetic_field.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <geometry_msgs/msg/quaternion.hpp>





#include <Eigen/Dense>
#include <thread>
#include <mutex>
#include <condition_variable>

//pcl
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include "file_player/color.h"


#include "file_player/datathread.h"
#include <sys/types.h>

#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <livox_ros_driver2/msg/custom_msg.hpp>

using namespace std;
using namespace cv;

class ROSThread : public QThread
{
    Q_OBJECT

public:
    explicit ROSThread(QObject *parent = 0, QMutex *th_mutex = 0);
    ~ROSThread();
    void ros_initialize(const rclcpp::Node::SharedPtr &node);
    void run();
    QMutex *mutex_;
    rclcpp::Node::SharedPtr node_;








    std::shared_ptr<camera_info_manager::CameraInfoManager> left_cinfo_;
    std::shared_ptr<camera_info_manager::CameraInfoManager> right_cinfo_;

    std::shared_ptr<camera_info_manager::CameraInfoManager> thermal_left_cinfo_;
    std::shared_ptr<camera_info_manager::CameraInfoManager> thermal_right_cinfo_;

    std::shared_ptr<camera_info_manager::CameraInfoManager> thermal_14bit_left_cinfo_;
    std::shared_ptr<camera_info_manager::CameraInfoManager> thermal_14bit_right_cinfo_;

    int64_t initial_data_stamp_;
    int64_t last_data_stamp_;

    bool auto_start_flag_;
    int stamp_show_count_;

    bool play_flag_;
    bool pause_flag_;
    bool loop_flag_;
    bool stop_skip_flag_;
    double play_rate_;
    string data_folder_path_;

    int imu_data_version_;

    void Ready();
    void ResetProcessStamp(int position);

signals:
    void StampShow(quint64 stamp);
    void StartSignal();

private:

    int search_bound_;
    bool omni_active_;

    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr start_sub_;
    rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr stop_sub_;

    rclcpp::Publisher<sensor_msgs::msg::NavSatFix>::SharedPtr gps_pub_;
    // TODO(ROS2): inspva publisher message type pending migration.
    // TODO(ROS2): inspvax publisher message type pending migration.
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr gps_odometry_pub_;
    rclcpp::Publisher<irp_sen_msgs::msg::Imu>::SharedPtr imu_origin_pub_;
    rclcpp::Publisher<sensor_msgs::msg::Imu>::SharedPtr imu_pub_;
    rclcpp::Publisher<sensor_msgs::msg::MagneticField>::SharedPtr magnet_pub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr velodyne_left_pub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr velodyne_right_pub_;
    rclcpp::Publisher<livox_ros_driver2::msg::CustomMsg>::SharedPtr livox_avia_pub_;
    rclcpp::Publisher<livox_ros_driver2::msg::CustomMsg>::SharedPtr livox_tele_pub_;
    rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr ouster_pub_;

    rclcpp::Publisher<rosgraph_msgs::msg::Clock>::SharedPtr clock_pub_;

    int64_t prev_clock_stamp_;

    multimap<int64_t, string>                    data_stamp_;
    map<int64_t, nav_msgs::msg::Odometry>     odometry_data_;
    map<int64_t, sensor_msgs::msg::NavSatFix>    gps_data_;
    map<int64_t, nav_msgs::msg::Odometry>     gps_odometry_data_;

    map<int64_t, irp_sen_msgs::msg::Imu>         imu_data_origin_;
    map<int64_t, sensor_msgs::msg::Imu>         imu_data_;
    map<int64_t, sensor_msgs::msg::MagneticField>         mag_data_;

    DataThread<int64_t> data_stamp_thread_;
    DataThread<int64_t> gps_thread_;
    DataThread<int64_t> inspva_thread_;
    DataThread<int64_t> inspvax_thread_;
    DataThread<int64_t> imu_thread_;
    DataThread<int64_t> velodyne_left_thread_;
    DataThread<int64_t> velodyne_right_thread_;
    DataThread<int64_t> livox_avia_thread_;
    DataThread<int64_t> livox_tele_thread_;
    DataThread<int64_t> ouster_thread_;

    map<int64_t, int64_t> stop_period_; //start and stop stamp

    void DataStampThread();
    void GpsThread();
    void InspvaThread();
    void InspvaxThread();
    void ImuThread();
    void VelodyneLeftThread();
    void VelodyneRightThread();
    void LivoxAviaThread();
    void LivoxTeleThread();
    void OusterThread();

    void FilePlayerStart(const std_msgs::msg::Bool::SharedPtr msg);
    void FilePlayerStop(const std_msgs::msg::Bool::SharedPtr msg);

    vector<string> velodyne_left_file_list_;
    vector<string> velodyne_right_file_list_;
    vector<string> livox_avia_file_list_;
    vector<string> livox_tele_file_list_;
    vector<string> ouster_file_list_;

    rclcpp::TimerBase::SharedPtr timer_;
    void TimerCallback();
    int64_t processed_stamp_;
    int64_t pre_timer_stamp_;
    bool reset_process_stamp_flag_;

    pair<string,sensor_msgs::msg::PointCloud2> ouster_next_;

    pair<string,sensor_msgs::msg::PointCloud2> velodyne_left_next_;
    pair<string,sensor_msgs::msg::PointCloud2> velodyne_right_next_;
    pair<string,livox_ros_driver2::msg::CustomMsg> livox_avia_next_;
    pair<string,livox_ros_driver2::msg::CustomMsg> livox_tele_next_;

    int GetDirList(string dir, vector<string> &files);


public slots:

};

#endif // VIEWER_LCM_H
