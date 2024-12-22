#include "hako_pdu_data.hpp"
#include <vector>
#include <atomic>
#include "utils/hako_osdep.h"

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

class HakoPduBuffer {
public:
    std::vector<std::unique_ptr<HakoPduSensorDataType>> sensor_buffers;
    std::vector<std::unique_ptr<HakoPduActuatorDataType>> actuator_buffers;
};

static HakoPduBuffer hako_pdu_buffer;

bool hako_pdu_data_init(DroneConfigManager& mgr) {
    size_t configCount = mgr.getConfigCount();
    for (size_t i = 0; i < configCount; ++i) {
        auto* sensorBuffer = new HakoPduSensorDataType();
        if (sensorBuffer == nullptr) {
            return false;
        }
        sensorBuffer->is_busy = false;
        sensorBuffer->hil_sensor_is_dirty = false;
        sensorBuffer->hil_gps_is_dirty = false;
        sensorBuffer->hil_state_quaternion_is_dirty = false;
        hako_pdu_buffer.sensor_buffers.push_back(std::unique_ptr<HakoPduSensorDataType>(sensorBuffer));

        auto* actuatorBuffer = new HakoPduActuatorDataType();
        if (actuatorBuffer == nullptr) {
            return false;
        }
        actuatorBuffer->is_busy = false;
        actuatorBuffer->hil_actuator_controls_is_dirty = false;
        hako_pdu_buffer.actuator_buffers.push_back(std::unique_ptr<HakoPduActuatorDataType>(actuatorBuffer));
    }
    return true;
}


static void set_busy(std::atomic<bool> &busy_flag) {
    while(busy_flag.exchange(true)) {
        usleep(100); //100use
    }
}

static void unset_busy(std::atomic<bool> &busy_flag) {
    busy_flag.store(false);
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
bool hako_read_hil_sensor(int index, Hako_HakoHilSensor &hil_sensor) {
    auto& hako_pdu_sensor_data = *hako_pdu_buffer.sensor_buffers[index];
    return hako_read_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_sensor_is_dirty, 
        hako_pdu_sensor_data.hil_sensor, hil_sensor);
}

void hako_write_hil_sensor(int index, const Hako_HakoHilSensor &hil_sensor) {
    auto& hako_pdu_sensor_data = *hako_pdu_buffer.sensor_buffers[index];
    hako_write_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_sensor_is_dirty, 
        hako_pdu_sensor_data.hil_sensor, 
        hil_sensor);
}
bool hako_read_hil_gps(int index, Hako_HakoHilGps &hil_gps) {
    auto& hako_pdu_sensor_data = *hako_pdu_buffer.sensor_buffers[index];
    return hako_read_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_gps_is_dirty, 
        hako_pdu_sensor_data.hil_gps, 
        hil_gps);
}

void hako_write_hil_gps(int index, const Hako_HakoHilGps &hil_gps) {
    auto& hako_pdu_sensor_data = *hako_pdu_buffer.sensor_buffers[index];
    hako_write_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_gps_is_dirty, 
        hako_pdu_sensor_data.hil_gps, 
        hil_gps);
}

bool hako_read_hil_state_quaternion(int index, Hako_HakoHilStateQuaternion &hil_state_quaternion) {
    auto& hako_pdu_sensor_data = *hako_pdu_buffer.sensor_buffers[index];
    return hako_read_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_state_quaternion_is_dirty, 
        hako_pdu_sensor_data.hil_state_quaternion, 
        hil_state_quaternion);
}

void hako_write_hil_state_quaternion(int index, const Hako_HakoHilStateQuaternion &hil_state_quaternion) {
    auto& hako_pdu_sensor_data = *hako_pdu_buffer.sensor_buffers[index];
    hako_write_data(
        hako_pdu_sensor_data.is_busy, 
        hako_pdu_sensor_data.hil_state_quaternion_is_dirty, 
        hako_pdu_sensor_data.hil_state_quaternion, 
        hil_state_quaternion);
}

bool hako_read_hil_actuator_controls(int index, Hako_HakoHilActuatorControls &hil_actuator_controls) {
    auto& hako_pdu_actuator_data = *hako_pdu_buffer.actuator_buffers[index];
    return hako_read_data(
        hako_pdu_actuator_data.is_busy, 
        hako_pdu_actuator_data.hil_actuator_controls_is_dirty, 
        hako_pdu_actuator_data.hil_actuator_controls, 
        hil_actuator_controls);
}

void hako_write_hil_actuator_controls(int index, const Hako_HakoHilActuatorControls &hil_actuator_controls) {
    auto& hako_pdu_actuator_data = *hako_pdu_buffer.actuator_buffers[index];
    hako_write_data(
        hako_pdu_actuator_data.is_busy, 
        hako_pdu_actuator_data.hil_actuator_controls_is_dirty, 
        hako_pdu_actuator_data.hil_actuator_controls, 
        hil_actuator_controls);
}
