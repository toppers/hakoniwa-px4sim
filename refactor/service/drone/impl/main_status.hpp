#ifndef _MAIN_STATUS_HPP_
#define _MAIN_STATUS_HPP_

#include "service/drone/impl/operation_status.hpp"
#include <iostream>

typedef enum {
    MAIN_STATUS_LANDED = 0,
    MAIN_STATUS_TAKINGOFF,
    MAIN_STATUS_LANDING,
    MAIN_STATUS_HOVERING,
    MAIN_STATUS_MOVING,
    MAIN_STATUS_CANCELING,
    MAIN_STATUS_NUM
} MainStatusType;


class MainStatus {
private:
    MainStatusType status;
    MainStatusType cancel_move_status;
    OperationStatus operation;
    std::string get_status_str() {
        switch (status) {
            case MAIN_STATUS_LANDED:
                return "Landed";
            case MAIN_STATUS_TAKINGOFF:
                return "Taking off";
            case MAIN_STATUS_LANDING:
                return "Landing";
            case MAIN_STATUS_HOVERING:
                return "Hovering";
            case MAIN_STATUS_MOVING:
                return "Moving";
            case MAIN_STATUS_CANCELING:
                return "Canceling";
            default:
                return "Unknown";
        }
    }
public:
    MainStatus() : status(MAIN_STATUS_LANDED), cancel_move_status(MAIN_STATUS_LANDED) {
        std::cout << "Initial state: Landed" << std::endl;
    }
    MainStatusType get_status() {
        return this->status;
    }
    void reset()
    {
        this->status = MAIN_STATUS_LANDED;
        this->cancel_move_status = MAIN_STATUS_LANDED;
        this->operation.reset();
    }
    bool is_canceling() {
        return this->status == MAIN_STATUS_CANCELING;
    }
    MainStatusType get_cancel_move_status() {
        return this->cancel_move_status;
    }
    bool is_busy() {
        return operation.is_busy();
    }
    bool takeoff() {
        if (operation.is_busy()) {
            std::cout << "Takeoff attempt ignored: Operation is busy." << std::endl;
            return false;
        }
        if (status == MAIN_STATUS_LANDED) {
            std::cout << "Taking off..." << std::endl;
            this->cancel_move_status = MAIN_STATUS_LANDED;
            status = MAIN_STATUS_TAKINGOFF;
            operation.event_do();
            return true;
        }
        return false;
    }
    bool land() {
        if (operation.is_busy()) {
            std::cout << "Landing attempt ignored: Operation is busy." << std::endl;
            return false;
        }
        if (status == MAIN_STATUS_HOVERING) {
            std::cout << "Landing..." << std::endl;
            this->cancel_move_status = MAIN_STATUS_HOVERING;
            status = MAIN_STATUS_LANDING;
            operation.event_do();
            return true;
        }
        return false;
    }
    bool move() {
        if (operation.is_busy()) {
            std::cout << "Move attempt ignored: Operation is busy." << std::endl;
            return false;
        }
        if (status == MAIN_STATUS_HOVERING) {
            std::cout << "Moving..." << std::endl;
            this->cancel_move_status = MAIN_STATUS_HOVERING;
            status = MAIN_STATUS_MOVING;
            operation.event_do();
            return true;
        }
        return false;
    }
    bool cancel() {
        if (operation.is_busy()) {
            std::cout << "Cancelling current operation..." << std::endl;
            operation.event_cancel();
            this->status = MAIN_STATUS_CANCELING;
            return true;
        }
        std::cout << "Cancel attempt ignored: No operation to cancel." << std::endl;
        return false;
    }
    bool done() {
        if (operation.event_done()) {
            std::cout << "Operation completed. Updating status..." << std::endl;
            switch (status) {
                case MAIN_STATUS_TAKINGOFF:
                    status = MAIN_STATUS_HOVERING;
                    break;
                case MAIN_STATUS_LANDING:
                    status = MAIN_STATUS_LANDED;
                    break;
                case MAIN_STATUS_MOVING:
                    status = MAIN_STATUS_HOVERING;
                    break;
                case MAIN_STATUS_CANCELING:
                    status = cancel_move_status;
                    break;
                default:
                    std::cout << "Unexpected state." << std::endl;
                    return false;
            }
            std::cout << "New status( " << get_status_str() << " ) set." << std::endl;
            return true;
        }
        return false;
    }
};

#endif /* _MAIN_STATUS_HPP_ */