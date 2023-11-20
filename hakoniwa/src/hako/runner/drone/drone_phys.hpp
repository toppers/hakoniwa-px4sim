#ifndef _DRONE_PHYS_HPP_
#define _DRONE_PHYS_HPP_

#include "../../pdu/hako_pdu_data.hpp"
#include "../common/drone_types.hpp"

typedef struct {
    uint64 up_cycle_hil_sensor;
    uint64 up_cycle_hil_gps;
    uint64 up_cycle_hil_state_quaternion;
    Hako_HakoHilSensor hil_sensor;
    Hako_HakoHilGps hil_gps;
    Hako_HakoHilStateQuaternion hil_state_quaternion;
} DroneSensorType;

typedef struct {
    Hako_HakoHilActuatorControls hil_actuator_controls;
} DroneActuatorType;

typedef struct {
    double w[DRONE_PROPELLER_NUM];
} DronePropellerRotationRateType;


typedef struct {
    /*
     * ドローンの質量： 1 kg
     */
    double m;
    /*
     * 重力加速度：9.81 m/s^2
     */
    double gravity;
    /*
     * プロペラ回転中心から機体重心までの距離：0.3m
     */
    double l;
    /*
     * ρ と κ はプロペラの形状等で決まる空力特性に 依存するパラメータ
     */
    double p;
    double k;
} DronePhysParamType;

typedef struct {
    Vector3Type pos;
    Vector3Type vec;
    Vector3Type rot;
    Vector3Type rot_vec;
} DronePhysStateType;

#define SENSOR_MAX_COUNT_NUM    128
typedef struct {
    int inx;
    int max_count;
    int data_num;
    Vector3Type average_value;
    Vector3Type values[SENSOR_MAX_COUNT_NUM];
} DroneSensorAverageDataType;
static inline bool initAverageData(DroneSensorAverageDataType& data, int max_count)
{
    if (max_count > SENSOR_MAX_COUNT_NUM || max_count <= 0) {
        return false;  // 入力されたmax_countが範囲外であれば、失敗を返す
    }

    data.inx = 0;
    data.max_count = max_count;
    data.data_num = 0;
    for (int i = 0; i < SENSOR_MAX_COUNT_NUM; ++i) {
        data.values[i].x = 0.0;
        data.values[i].y = 0.0;
        data.values[i].z = 0.0;
    }
    data.average_value.x = 0.0;
    data.average_value.y = 0.0;
    data.average_value.z = 0.0;

    return true;  // 初期化成功
}
static inline void addAverageData(DroneSensorAverageDataType& data, const Vector3Type value)
{
    // Add the new value to the values array at the current index
    data.values[data.inx] = value;

    // Update the index for the next data point
    data.inx = (data.inx + 1) % data.max_count;

    // Update the data count (but don't let it exceed SENSOR_AVERAGE_COUNT)
    if (data.data_num < data.max_count)
    {
        data.data_num++;
    }
}

static inline void calcAverage(DroneSensorAverageDataType& data, Vector3Type& result)
{
    result.x = 0;
    result.y = 0;
    result.z = 0;

    // Sum up all the values in the array
    for (int i = 0; i < data.data_num; i++)
    {
        result.x += data.values[i].x;
        result.y += data.values[i].y;
        result.z += data.values[i].z;
    }

    // Calculate the average
    if (data.data_num > 0)
    {
        result.x /= data.data_num;
        result.y /= data.data_num;
        result.z /= data.data_num;

        data.average_value.x = result.x;
        data.average_value.y = result.y;
        data.average_value.z = result.z;
    }
}

typedef struct {
    /*
     * constant values
     */
    double delta_t;
    DronePhysParamType param;
    DronePhysStateType initial_value;
    /*
     * update values
     */
    double current_time;
    DronePhysStateType current;
    DronePhysStateType next;


    /*
     * for other module communication data:
     */
    /*
     * Actuators
     */
    DroneActuatorType actuator;
    /*
     * Sensors
     */
    DroneSensorType sensor;
    DroneSensorAverageDataType sensor_rot;
    DroneSensorAverageDataType sensor_rot_vec;
    DroneSensorAverageDataType sensor_pos;
    DroneSensorAverageDataType sensor_vec;
    DroneSensorAverageDataType sensor_acc;

    DroneSensorAverageDataType sensor_rot_rvec;
    DroneSensorAverageDataType sensor_racc;

    Vector3Type prev_vec;
    Vector3Type prev_rot;
    Vector3Type prev_rvec;
    Vector3Type rot_rvec;
} DronePhysType;


extern void drone_init(double delta_t, const DronePhysParamType&param, const DronePhysStateType& initial_value, DronePhysType& phys);
extern void drone_run(const DronePropellerRotationRateType& propeller, DronePhysType& phys);

#endif /* _DRONE_PHYS_HPP_ */