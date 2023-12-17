/****************************************************************************
 *
 *   Copyright (c) 2012-2019 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file px4_csv_logger.c
 * Minimal application example for PX4 autopilot
 *
 * @author Example User <mail@example.com>
 */

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/tasks.h>
#include <px4_platform_common/posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include <math.h>

#include "px4_csv_logger.hpp"
#include <uORB/uORB.h>
#include <uORB/topics/vehicle_acceleration.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/actuator_outputs.h>

#include "csv_logger.hpp"
#include "icsv_log.hpp"
#include <cmath>

// クォータニオン構造体
struct Quaternion {
    double w, x, y, z;
};

// オイラー角構造体
struct EulerAngles {
    double roll, pitch, yaw;
};

// クォータニオンをオイラー角に変換
static EulerAngles toEulerAngles(const Quaternion& q) {
    EulerAngles angles;

    // roll (x-axis rotation)
    double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
    double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
    angles.roll = std::atan2(sinr_cosp, cosr_cosp);

    // pitch (y-axis rotation)
    double sinp = 2 * (q.w * q.y - q.z * q.x);
    if (std::abs(sinp) >= 1)
        angles.pitch = std::copysign(M_PI / 2, sinp); // use 90 degrees if out of range
    else
        angles.pitch = std::asin(sinp);

    // yaw (z-axis rotation)
    double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
    double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
    angles.yaw = std::atan2(siny_cosp, cosy_cosp);

    return angles;
}
class Px4Attitude : public ICsvLog {
private:
	uint64_t time_usec;
	vehicle_attitude_s att;
	EulerAngles angles;
public:
	Px4Attitude() {}
	~Px4Attitude() {}
	void set_data(uint64_t t, vehicle_attitude_s& data)
	{
		time_usec = t;
		att = data;
		Quaternion q;
		q.w = data.q[0];
		q.x = data.q[1];
		q.y = data.q[2];
		q.z = data.q[3];
		angles = toEulerAngles(q);
	}
    const std::vector<std::string> log_head() override
    {
        return { "TIME", "q0", "q1", "q2", "q3", "euler_x", "euler_y", "euler_z" };
    }
    const std::vector<std::string> log_data() override
    {
        return {
			std::to_string(time_usec), 
			std::to_string(att.q[0]), std::to_string(att.q[1]), std::to_string(att.q[2]),std::to_string(att.q[3]),
			std::to_string(angles.pitch), std::to_string(angles.roll), std::to_string(angles.yaw)
			};
    }
};
class Px4LocalPosition : public ICsvLog {
private:
	uint64_t time_usec;
	vehicle_local_position_s pos;
public:
	Px4LocalPosition() {}
	~Px4LocalPosition() {}
	void set_data(uint64_t t, vehicle_local_position_s& data)
	{
		time_usec = t;
		pos = data;
	}
    const std::vector<std::string> log_head() override
    {
        return { "TIME", "x", "y", "z", "vx", "vy", "vz", "ax", "ay", "az" };
    }
    const std::vector<std::string> log_data() override
    {
        return {
			std::to_string(time_usec), 
			std::to_string(pos.x), std::to_string(pos.y), std::to_string(pos.z),
			std::to_string(pos.vx), std::to_string(pos.vy), std::to_string(pos.vz),
			std::to_string(pos.ax), std::to_string(pos.ay), std::to_string(pos.az)
			};
    }
};
static CsvLogger logger_pos;
static CsvLogger logger_att;

static Px4Attitude px4_att;
static Px4LocalPosition px4_pos;

extern "C" __EXPORT int px4_csv_logger_main(int argc, char *argv[])
{
	logger_att.add_entry(px4_att, "./att.csv");
	logger_pos.add_entry(px4_pos, "./pos.csv");
	return Px4CsvLogger::main(argc, argv);
}
#define DELTA_TIME_USEC 1000
Px4CsvLogger::Px4CsvLogger() :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::hp_default)
{
}
void Px4CsvLogger::log_pos()
{
	if (_vehicle_local_position_sub.updated()) {
		_vehicle_local_position_sub.copy(&vehicle_local_position);
		hrt_abstime curr = hrt_absolute_time();
		px4_pos.set_data(curr, vehicle_local_position);
		logger_pos.run();
	}
}
void Px4CsvLogger::log_att()
{
	if (_vehicle_attitude_sub.updated()) {
		_vehicle_attitude_sub.copy(&vehicle_attitude);
		hrt_abstime curr = hrt_absolute_time();
		px4_att.set_data(curr, vehicle_attitude);
		logger_att.run();
	}
}
bool Px4CsvLogger::init()
{
	ScheduleOnInterval(DELTA_TIME_USEC); 
	return true;
}

void Px4CsvLogger::Run()
{
	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup();
		return;
	}
	this->log_pos();
	this->log_att();
}

int Px4CsvLogger::task_spawn(int argc, char *argv[])
{
	Px4CsvLogger *instance = new Px4CsvLogger();

	if (instance) {
		_object.store(instance);
		_task_id = task_id_is_work_queue;

		if (instance->init()) {
			return PX4_OK;
		}

	} else {
		PX4_ERR("alloc failed");
	}

	delete instance;
	_object.store(nullptr);
	_task_id = -1;

	return PX4_ERROR;
}


int Px4CsvLogger::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}

int Px4CsvLogger::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("px4_csv_logger", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
	return 0;
}
