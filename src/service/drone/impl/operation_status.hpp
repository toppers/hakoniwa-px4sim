#ifndef _OPERATION_STATUS_HPP_
#define _OPERATION_STATUS_HPP_

namespace hako::service::impl {

typedef enum {
    OPERATION_STATUS_DONE = 0,
    OPERATION_STATUS_DOING,
    OPERATION_STATUS_CANCELING,
    OPERATION_STATUS_NUM
} OperationStatusType;

class OperationStatus {
private:
    OperationStatusType status;
public:
    OperationStatus() {
        this->status = OPERATION_STATUS_DONE;
    }
    virtual ~OperationStatus() {}
    void reset()
    {
        this->status = OPERATION_STATUS_DONE;
    }
    OperationStatusType get_status()
    {
        return this->status;
    }
    bool is_busy()
    {
        return this->status != OPERATION_STATUS_DONE;
    }
    bool event_done()
    {
        switch (this->status) {
            case OPERATION_STATUS_DONE:
                //nothing to do.
                return false;
            case OPERATION_STATUS_DOING:
                this->status = OPERATION_STATUS_DONE;
                return true;
            case OPERATION_STATUS_CANCELING:
                this->status = OPERATION_STATUS_DONE;
                return true;
            default:
                return false;
        }
    }
    bool event_do()
    {
        switch (this->status) {
            case OPERATION_STATUS_DONE:
                this->status = OPERATION_STATUS_DOING;
                return true;
            case OPERATION_STATUS_CANCELING:
                //nothing to do.
                return false;
            default:
                return false;
        }
    }    
    bool event_cancel()
    {
        switch (this->status) {
            case OPERATION_STATUS_DONE:
                //nothing to do.
                return false;
            case OPERATION_STATUS_DOING:
                this->status = OPERATION_STATUS_CANCELING;
                return true;
            case OPERATION_STATUS_CANCELING:
                //nothing to do.
                return false;
            default:
                return false;
        }
    }
};
}

#endif /* _OPERATION_STATUS_HPP_ */