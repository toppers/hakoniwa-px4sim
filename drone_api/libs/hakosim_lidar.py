import struct

class LidarData:
    def __init__(self, point_cloud, time_stamp, pose, data_frame='VehicleInertialFrame', segmentation=None):
        """
        Initializes a new instance of the LidarData class.

        :param point_cloud: A flat list of floats representing the [x, y, z] coordinates of each point.
        :param time_stamp: Timestamp of the Lidar data capture.
        :param pose: The pose of the Lidar in vehicle inertial frame (in NED, in meters).
        :param data_frame: Frame of the point cloud data. Default is 'VehicleInertialFrame'.
                           It can also be 'SensorLocalFrame' for points in Lidar local frame.
        :param segmentation: Optional; segmentation information for each point's collided object.
        """
        self.point_cloud = point_cloud
        self.time_stamp = time_stamp
        self.pose = pose
        self.data_frame = data_frame
        self.segmentation = segmentation

    def __repr__(self):
        return f"LidarData(time_stamp={self.time_stamp}, data_frame={self.data_frame}, " \
               f"pose={self.pose}, number_of_points={len(self.point_cloud) // 3})"

    @staticmethod
    def parse_point_cloud(point_cloud):
        """
        Parses the flat list of floats into a list of (x, y, z) tuples.

        :param point_cloud: A flat list of floats.
        :return: A list of (x, y, z) tuples representing the coordinates.
        """
        return [(point_cloud[i], point_cloud[i+1], point_cloud[i+2]) for i in range(0, len(point_cloud), 3)]


    @staticmethod
    def extract_xyz_from_point_cloud(point_cloud_bytes, total_data_bytes):
        # 各ポイントは16バイトで、x, y, z, intensityが含まれています。
        num_points = total_data_bytes // 16
        # 出力リストを初期化
        points = []

        for i in range(num_points):
            # 16バイトごとにデータを取り出す
            offset = i * 16
            # '<3f'はリトルエンディアンのfloat32が3つ、x, y, z座標を意味します。
            # 12バイトを読み取り、次の4バイト（intensity）は無視します。
            point = struct.unpack_from('<3f', point_cloud_bytes, offset)
            # 取り出した座標をリストに追加
            points.extend(point)

        return points