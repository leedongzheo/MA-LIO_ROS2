#!/usr/bin/env python3
import csv
import glob
import os
import struct
from dataclasses import dataclass

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Imu, PointCloud2, PointField
from std_msgs.msg import Header


@dataclass
class Event:
    stamp: float
    kind: str
    path: str


class City02PlayerNode(Node):
    def __init__(self) -> None:
        super().__init__('city02_player_node')
        self.declare_parameter('root', '')
        self.declare_parameter('rate', 1.0)
        self.declare_parameter('frame_ouster', 'ouster')
        self.declare_parameter('frame_livox_avia', 'livox_avia')
        self.declare_parameter('frame_livox_tele', 'livox_tele')
        self.declare_parameter('frame_imu', 'imu_link')

        self.root = self.get_parameter('root').get_parameter_value().string_value
        self.rate = self.get_parameter('rate').get_parameter_value().double_value
        self.frame_ouster = self.get_parameter('frame_ouster').value
        self.frame_livox_avia = self.get_parameter('frame_livox_avia').value
        self.frame_livox_tele = self.get_parameter('frame_livox_tele').value
        self.frame_imu = self.get_parameter('frame_imu').value

        if not self.root:
            raise RuntimeError('Parameter root must point to City02/sensor_data')

        self.pub_ouster = self.create_publisher(PointCloud2, '/ouster_points', 10)
        self.pub_avia = self.create_publisher(PointCloud2, '/livox_avia_points', 10)
        self.pub_tele = self.create_publisher(PointCloud2, '/livox_tele_points', 10)
        self.pub_imu = self.create_publisher(Imu, '/imu/data', 100)

        self.events = self._build_events()
        self.idx = 0
        self.t0_data = self.events[0].stamp if self.events else 0.0
        self.t0_wall = self.get_clock().now().nanoseconds / 1e9

        self.timer = self.create_timer(0.002, self._tick)
        self.get_logger().info(f'Loaded {len(self.events)} events at rate={self.rate}x')

    def _build_events(self):
        events = []
        events.extend(self._lidar_events('ouster', 'ouster_stamp.csv'))
        events.extend(self._lidar_events('Livox_avia', 'data_stamp.csv'))
        events.extend(self._lidar_events('Livox_tele', 'data_stamp.csv'))
        events.extend(self._imu_events())
        events.sort(key=lambda e: e.stamp)
        return events

    def _lidar_events(self, folder: str, stamp_csv: str):
        csv_path = os.path.join(self.root, stamp_csv)
        bin_dir = os.path.join(self.root, folder)
        bins = sorted(glob.glob(os.path.join(bin_dir, '*.bin')))
        stamps = self._read_first_float_col(csv_path)
        n = min(len(stamps), len(bins))
        kind = folder.lower()
        return [Event(stamps[i], kind, bins[i]) for i in range(n)]

    def _imu_events(self):
        imu_csv = os.path.join(self.root, 'xsens_imu.csv')
        events = []
        with open(imu_csv, 'r', encoding='utf-8') as f:
            reader = csv.reader(f)
            header = next(reader, None)
            for row in reader:
                if not row:
                    continue
                stamp = float(row[0])
                events.append(Event(stamp, 'imu', ','.join(row)))
        return events

    @staticmethod
    def _read_first_float_col(csv_path: str):
        out = []
        with open(csv_path, 'r', encoding='utf-8') as f:
            reader = csv.reader(f)
            for row in reader:
                if not row:
                    continue
                try:
                    out.append(float(row[0]))
                except ValueError:
                    continue
        return out

    def _tick(self):
        if self.idx >= len(self.events):
            self.get_logger().info('Playback finished')
            rclpy.shutdown()
            return

        now_wall = self.get_clock().now().nanoseconds / 1e9
        data_time_target = self.t0_data + (now_wall - self.t0_wall) * self.rate

        while self.idx < len(self.events) and self.events[self.idx].stamp <= data_time_target:
            event = self.events[self.idx]
            self._publish_event(event)
            self.idx += 1

    def _publish_event(self, event: Event):
        if event.kind == 'imu':
            self.pub_imu.publish(self._imu_from_row(event.path, event.stamp))
            return

        cloud = self._pointcloud_from_bin(event.path, event.kind, event.stamp)
        if event.kind == 'ouster':
            self.pub_ouster.publish(cloud)
        elif event.kind == 'livox_avia':
            self.pub_avia.publish(cloud)
        elif event.kind == 'livox_tele':
            self.pub_tele.publish(cloud)

    def _imu_from_row(self, row_string: str, stamp_sec: float) -> Imu:
        cols = row_string.split(',')
        msg = Imu()
        msg.header = self._header(stamp_sec, self.frame_imu)
        if len(cols) >= 13:
            msg.angular_velocity.x = float(cols[4])
            msg.angular_velocity.y = float(cols[5])
            msg.angular_velocity.z = float(cols[6])
            msg.linear_acceleration.x = float(cols[7])
            msg.linear_acceleration.y = float(cols[8])
            msg.linear_acceleration.z = float(cols[9])
            msg.orientation.w = float(cols[10])
            msg.orientation.x = float(cols[11])
            msg.orientation.y = float(cols[12])
            msg.orientation.z = float(cols[13]) if len(cols) > 13 else 0.0
        return msg

    def _pointcloud_from_bin(self, bin_path: str, kind: str, stamp_sec: float) -> PointCloud2:
        with open(bin_path, 'rb') as f:
            blob = f.read()

        point_step = 16
        num_points = len(blob) // point_step

        msg = PointCloud2()
        frame_id = self.frame_ouster if kind == 'ouster' else (self.frame_livox_avia if kind == 'livox_avia' else self.frame_livox_tele)
        msg.header = self._header(stamp_sec, frame_id)
        msg.height = 1
        msg.width = num_points
        msg.fields = [
            PointField(name='x', offset=0, datatype=PointField.FLOAT32, count=1),
            PointField(name='y', offset=4, datatype=PointField.FLOAT32, count=1),
            PointField(name='z', offset=8, datatype=PointField.FLOAT32, count=1),
            PointField(name='intensity', offset=12, datatype=PointField.FLOAT32, count=1),
        ]
        msg.is_bigendian = False
        msg.point_step = point_step
        msg.row_step = msg.point_step * num_points
        msg.is_dense = True
        msg.data = blob[: num_points * point_step]
        return msg

    def _header(self, stamp_sec: float, frame_id: str) -> Header:
        sec = int(stamp_sec)
        nsec = int((stamp_sec - sec) * 1e9)
        h = Header()
        h.stamp.sec = sec
        h.stamp.nanosec = nsec
        h.frame_id = frame_id
        return h


def main():
    rclpy.init()
    node = City02PlayerNode()
    rclpy.spin(node)


if __name__ == '__main__':
    main()
