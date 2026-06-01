# File player for complex urban data set

Maintainer: Jinyong Jeong (jjy0923@kaist.ac.kr)

This program is a file player for the complex urban data set. If a user installs the ROS 2 Jazzy (Desktop), there is only one additional dependent package, except for the ROS default package. First, clone this package into the src folder of your desired ROS workspace.

## 1. Obtain dependent package (defined msg)

```
$mkdir -p ~/ros2_ws/src
$cd ~/ros2_ws/src
$git clone https://github.com/minwoo0611/MA-LIO.git
```

## 2. Build workspace

```
$cd ~/ros2_ws
$colcon build --symlink-install
```

## 3. Run file player

```
$source install/setup.bash
$ros2 launch file_player file_player.launch.py
```

## 4. Load data files and play

1. Click 'Load' button.
2. Choose data set folder including sensor_data folder and calibration folder.
3. The player button starts publishing data in ROS 2 messages.
4. The Stop skip button skips data while the vehicle is stationary for convenience.
5. The loop button resumes when playback is finished.
