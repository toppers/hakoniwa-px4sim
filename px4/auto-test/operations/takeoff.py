from operations.ioperation import IOperation
import utils.mavlink_msg as mav_msg

class TakeoffOperation(IOperation):
    def __init__(self, connection, altitude, duration_sec):
        self.connection = connection
        self.altitude = altitude
        self.is_operation_done = False
        self.takeoff_command_sent = False
        self.arm_command_sent = False
        self.command_long_ack_recv = False
        self.duration_sec = duration_sec
        self.takeoff_start = 0

    def start(self, start_time):
        print("INFO: Takeoff Operation started.")
        self.start_time = start_time

    def is_op_done(self):
        return self.is_operation_done

    def event_msg(self, msg):
        message_type = msg.get_type()
        if message_type == "COMMAND_ACK":
            self.command_long_ack_recv = True

    def do_operation(self, current_time):
        if self.is_operation_done:
            return
        if (current_time - self.start_time) > 3 and not self.takeoff_command_sent:
            command_msg = mav_msg.create_command_long_takeoff(self.altitude)
            mav_msg.dump_command_long(command_msg)
            # コマンドを送信
            self.connection.write(command_msg.get_msgbuf())
            self.takeoff_command_sent = True  # コマンド送信済みフラグをセット
            print("INFO: takeoff event start_takeoff")

        if self.command_long_ack_recv and not self.arm_command_sent:
            # ARMコマンドを作成
            command_msg = mav_msg.create_command_long_arm(True)
            mav_msg.dump_command_long(command_msg)
            # コマンドを送信
            self.connection.write(command_msg.get_msgbuf())

            self.arm_command_sent = True  # コマンド送信済みフラグをセット
            self.takeoff_start = current_time
            self.command_long_ack_recv = False
            print("INFO: takeoff event start_arm")
        if self.arm_command_sent and self.command_long_ack_recv:
            if (current_time - self.takeoff_start) > self.duration_sec:
                self.is_operation_done = True
                print("INFO: Takeoff Operation done.")

