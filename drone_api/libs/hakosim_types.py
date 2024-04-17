import math

class Vector3r:
    def __init__(self, x_val=0.0, y_val=0.0, z_val=0.0):
        self.x_val = x_val
        self.y_val = y_val
        self.z_val = z_val

    def __repr__(self):
        return f"Vector3r(x_val={self.x_val}, y_val={self.y_val}, z_val={self.z_val})"

class Quaternionr:
    def __init__(self, w_val=1.0, x_val=0.0, y_val=0.0, z_val=0.0):
        self.w_val = w_val
        self.x_val = x_val
        self.y_val = y_val
        self.z_val = z_val

    @staticmethod
    def euler_to_quaternion(roll_rad, pitch_rad, yaw_rad):
        cy = math.cos(yaw_rad * 0.5)
        sy = math.sin(yaw_rad * 0.5)
        cp = math.cos(pitch_rad * 0.5)
        sp = math.sin(pitch_rad * 0.5)
        cr = math.cos(roll_rad * 0.5)
        sr = math.sin(roll_rad * 0.5)

        w = cr * cp * cy + sr * sp * sy
        x = sr * cp * cy - cr * sp * sy
        y = cr * sp * cy + sr * cp * sy
        z = cr * cp * sy - sr * sp * cy

        return Quaternionr(w, x, y, z)

    @staticmethod
    def quaternion_to_euler(quaternion):
        w, x, y, z = quaternion.w_val, quaternion.x_val, quaternion.y_val, quaternion.z_val
        t0 = +2.0 * (w * x + y * z)
        t1 = +1.0 - 2.0 * (x * x + y * y)
        roll_x = math.atan2(t0, t1)

        t2 = +2.0 * (w * y - z * x)
        t2 = +1.0 if t2 > +1.0 else t2
        t2 = -1.0 if t2 < -1.0 else t2
        pitch_y = math.asin(t2)

        t3 = +2.0 * (w * z + x * y)
        t4 = +1.0 - 2.0 * (y * y + z * z)
        yaw_z = math.atan2(t3, t4)

        return roll_x, pitch_y, yaw_z

    def __repr__(self):
        return f"Quaternionr(w_val={self.w_val}, x_val={self.x_val}, y_val={self.y_val}, z_val={self.z_val})"

class Pose:
    def __init__(self, position=None, orientation=None):
        if position is None:
            position = Vector3r()
        if orientation is None:
            orientation = Quaternionr()
            
        self.position = position
        self.orientation = orientation

    def __repr__(self):
        return f"Pose(position={self.position}, orientation={self.orientation})"