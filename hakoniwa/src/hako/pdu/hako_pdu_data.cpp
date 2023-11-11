#include "hako_pdu_data.hpp"
#include <atomic>
#include <unistd.h>

typedef struct {
    std::atomic<bool>  is_busy;
    std::atomic<bool>  hil_sensor_is_dirty;
    std::atomic<bool>  hil_gps_is_dirty;
    std::atomic<bool>  hil_state_quaternion_is_dirty;
    Hako_HakoHilSensor          hil_sensor;
    Hako_HakoHilGps             hil_gps;
    Hako_HakoHilStateQuaternion hil_state_quaternion;
} HakoPduSensorDataType;

typedef struct {
    std::atomic<bool>  is_busy;
    std::atomic<bool>  hil_actuator_controls_is_dirty;
    Hako_HakoHilActuatorControls hil_actuator_controls;
} HakoPduActuatorDataType;

static HakoPduSensorDataType hako_pdu_sensor_data;
static HakoPduActuatorDataType hako_pdu_actuator_data;


static void set_busy(std::atomic<bool> &busy_flag) {
    while(busy_flag.exchange(true)) {  // exchangeは指定された新しい値と現在の値をアトミックに交換します。
        usleep(100); //100use
    }
}

static void unset_busy(std::atomic<bool> &busy_flag) {
    busy_flag.store(false);  // ビジーフラグをリセットします。
}
template<typename T>
bool hako_read_data(std::atomic<bool> &is_busy, std::atomic<bool> &is_dirty, T &data, T &output) {
    if (!is_dirty.load()) {
        return false;
    }
    set_busy(is_busy);
    output = data;
    is_dirty.store(false);
    unset_busy(is_busy);
    return true;
}

template<typename T>
void hako_write_data(std::atomic<bool> &is_busy, std::atomic<bool> &is_dirty, T &data, const T &input) {
    set_busy(is_busy);
    data = input;
    is_dirty.store(true);
    unset_busy(is_busy);
}
bool hako_read_hil_sensor(Hako_HakoHilSensor &hil_sensor) {
    return hako_read_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_sensor_is_dirty, 
        hako_pdu_sensor_data.hil_sensor, hil_sensor);
}

void hako_write_hil_sensor(const Hako_HakoHilSensor &hil_sensor) {
    hako_write_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_sensor_is_dirty, 
        hako_pdu_sensor_data.hil_sensor, 
        hil_sensor);
}
bool hako_read_hil_gps(Hako_HakoHilGps &hil_gps) {
    return hako_read_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_gps_is_dirty, 
        hako_pdu_sensor_data.hil_gps, 
        hil_gps);
}

void hako_write_hil_gps(const Hako_HakoHilGps &hil_gps) {
    hako_write_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_gps_is_dirty, 
        hako_pdu_sensor_data.hil_gps, 
        hil_gps);
}

bool hako_read_hil_state_quaternion(Hako_HakoHilStateQuaternion &hil_state_quaternion) {
    return hako_read_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_state_quaternion_is_dirty, 
        hako_pdu_sensor_data.hil_state_quaternion, 
        hil_state_quaternion);
}

void hako_write_hil_state_quaternion(const Hako_HakoHilStateQuaternion &hil_state_quaternion) {
    hako_write_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_state_quaternion_is_dirty, 
        hako_pdu_sensor_data.hil_state_quaternion, 
        hil_state_quaternion);
}

bool hako_read_hil_actuator_controls(Hako_HakoHilActuatorControls &hil_actuator_controls) {
    return hako_read_data(
        hako_pdu_actuator_data.is_busy, 
        hako_pdu_actuator_data.hil_actuator_controls_is_dirty, 
        hako_pdu_actuator_data.hil_actuator_controls, 
        hil_actuator_controls);
}

void hako_write_hil_actuator_controls(const Hako_HakoHilActuatorControls &hil_actuator_controls) {
    hako_write_data(
        hako_pdu_actuator_data.is_busy, 
        hako_pdu_actuator_data.hil_actuator_controls_is_dirty, 
        hako_pdu_actuator_data.hil_actuator_controls, 
        hil_actuator_controls);
}
