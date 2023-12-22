
class IOperation:
    def start(self, start_time):
        raise NotImplementedError("Start method must be implemented.")

    def is_op_done(self):
        raise NotImplementedError("is_op_done method must be implemented.")

    def event_msg(self, msg):
        raise NotImplementedError("event_msg method must be implemented.")

    def do_operation(self, current_time):
        raise NotImplementedError("do_operation method must be implemented.")
