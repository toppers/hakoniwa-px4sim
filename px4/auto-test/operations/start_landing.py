from operations.ioperation import IOperation
import utils.mavlink_msg as mav_msg

class StartLandingOperation(IOperation):
    def __init__(self, connection, lat, lon, alt, duration_sec):
        self.connection = connection
        self.lat = lat
        self.lon = lon
        self.alt = alt
        self.is_operation_done = False
        self.command_sent = False
        self.command_ack_recv = False
        self.duration_sec = duration_sec
        self.command_start = 0

    def start(self, start_time):
        print("INFO: StartLanding Operation started.")
        self.start_time = start_time

    def is_op_done(self):
        return self.is_operation_done

    def event_msg(self, msg):
        message_type = msg.get_type()
        if message_type == "COMMAND_ACK":
            self.command_ack_recv = True

    def do_operation(self, current_time):
        if self.is_operation_done:
            return
        if (current_time - self.start_time) > 3 and not self.command_sent:
            command_msg = mav_msg.start_landing_sequence(self.lat, self.lon, self.alt)
            mav_msg.dump_command_int(command_msg)
            # コマンドを送信
            self.connection.write(command_msg.get_msgbuf())
            self.command_sent = True  # コマンド送信済みフラグをセット
            self.command_start = current_time
            print("INFO: landing event start_landing")

        if self.command_sent and self.command_ack_recv:
            if (current_time - self.command_start) > self.duration_sec:
                self.is_operation_done = True
                print("INFO: StartLanding Operation done.")

