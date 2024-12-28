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
 * @file px4_hakoniwa.c
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

#include "px4_hakoniwa.hpp"
#include <uORB/uORB.h>
#include <uORB/topics/vehicle_acceleration.h>
#include <uORB/topics/vehicle_attitude.h>
#include <uORB/topics/actuator_outputs.h>

#define ENABLE_CONTROL
//#define ENABLE_DEBUG_POS
#define ENABLE_DEBUG_ROT

extern "C" __EXPORT int px4_hakoniwa_main(int argc, char *argv[])
{
	return Px4Hakoniwa::main(argc, argv);
}
#define DRONE_PHYS_DELTA_TIME	0.001 /* 4msec */
#define DRONE_PHYS_DELTA_TIME_USC	1000

Px4Hakoniwa::Px4Hakoniwa() :
	ScheduledWorkItem(MODULE_NAME, px4::wq_configurations::hp_default)
{
	_param.m = DRONE_PARAM_M;
    _param.l = DRONE_PARAM_L;
    _param.gravity = DRONE_PARAM_GRAVITY;
    _param.k = DRONE_PARAM_K;
    _param.p = DRONE_PARAM_P;
	
	memset(&_drone_propeller, 0, sizeof(_drone_propeller));
    drone_control_init(_drone_ctrl, DRONE_PHYS_DELTA_TIME);

	memset(&act_outs, 0, sizeof(act_outs));
	act_pub = orb_advertise(ORB_ID(actuator_outputs_sim), &act_outs);

}

bool Px4Hakoniwa::init()
{
	ScheduleOnInterval(DRONE_PHYS_DELTA_TIME_USC); 
	return true;
}

static void debug_info(const char* name, double true_value, double value)
{
	hrt_abstime curr = hrt_absolute_time();
	PX4_INFO("%ld %s t_value=%f value=%f diff=%f", curr, name, true_value, value, true_value - value);
	//PX4_INFO("t_value=%f value=%f", true_value, value);
}
static inline void calc_rot(const vehicle_attitude_s& true_rot, const vehicle_attitude_s& rot, Vector3Type& true_rot_value, Vector3Type& rot_value)
{
	QuaternionType local_q;
	local_q.w = rot.q[0];
	local_q.x = rot.q[1];
	local_q.y = rot.q[2];
	local_q.z = rot.q[3];
	quaternion2Euler(local_q, rot_value);

	QuaternionType truth_q;
	truth_q.w = true_rot.q[0];
	truth_q.x = true_rot.q[1];
	truth_q.y = true_rot.q[2];
	truth_q.z = true_rot.q[3];
	quaternion2Euler(truth_q, true_rot_value);	
}
void Px4Hakoniwa::debug_log_rot()
{
	QuaternionType local_q;
	local_q.w = vehicle_attitude.q[0];
	local_q.x = vehicle_attitude.q[1];
	local_q.y = vehicle_attitude.q[2];
	local_q.z = vehicle_attitude.q[3];
	Vector3Type local_rot;
	quaternion2Euler(local_q, local_rot);

	QuaternionType truth_q;
	truth_q.w = vehicle_attitude_grounadtrue.q[0];
	truth_q.x = vehicle_attitude_grounadtrue.q[1];
	truth_q.y = vehicle_attitude_grounadtrue.q[2];
	truth_q.z = vehicle_attitude_grounadtrue.q[3];
	Vector3Type truth_rot;
	quaternion2Euler(truth_q, truth_rot);

	debug_info("rot_x", truth_rot.x, local_rot.x);
	debug_info("rot_y", truth_rot.y, local_rot.y);
	debug_info("rot_z", truth_rot.z, local_rot.z);
}

void Px4Hakoniwa::debug_log_pos()
{
	if (_vehicle_global_position_sub.updated()) {
		_vehicle_global_position_sub.copy(&vehicle_global_position);
		if (_vehicle_local_position_sub.updated()) {
			_vehicle_local_position_sub.copy(&vehicle_local_position);
			//Px4Hakoniwa::do_control();
			debug_info("pos_z", vehicle_global_position.alt, vehicle_local_position.z);
		}
	}

}
void Px4Hakoniwa::do_control()
{
	hrt_abstime curr = hrt_absolute_time();
	hrt_abstime prev = 0;
	double delta = _drone_ctrl.delta_t;
	if (prev != 0) {
		hrt_abstime diff = curr - prev; /* usec */
		delta = ((double)diff) / 1000000.0; /* sec */
	}
	prev = curr;
	//PX4_INFO("time: %ld alt: %f", curr, (double)vehicle_local_position.z);
	Vector3Type current_pos;
	current_pos.x = vehicle_local_position.x;
	current_pos.y = vehicle_local_position.y;
	current_pos.z = -vehicle_local_position.z;
	Vector3Type rot_value;
	Vector3Type true_rot_value;
	calc_rot(vehicle_attitude_grounadtrue, vehicle_attitude, true_rot_value, rot_value);

	drone_control_run(_drone_ctrl, current_pos, rot_value, delta);
	convert2RotationRate(_drone_ctrl.signal, _param, _drone_propeller);
	act_outs.timestamp = 1024;
#define MY_CONST 1.0
	act_outs.output[0] = (double)_drone_propeller.w[0] * MY_CONST;
	act_outs.output[1] = (double)_drone_propeller.w[1] * MY_CONST;
	act_outs.output[2] = (double)_drone_propeller.w[2] * MY_CONST;
	act_outs.output[3] = (double)_drone_propeller.w[3] * MY_CONST;
	orb_publish(ORB_ID(actuator_outputs_sim), act_pub, &act_outs);
}
void Px4Hakoniwa::Run()
{
	if (should_exit()) {
		ScheduleClear();
		exit_and_cleanup();
		return;
	}
#ifdef ENABLE_DEBUG_POS
	//debug_log_pos();
#endif
#ifdef ENABLE_DEBUG_ROT
	//debug_log_rot();
#endif
#ifdef ENABLE_CONTROL	
	if (_vehicle_attitude_sub.updated()) {
		_vehicle_attitude_sub.copy(&vehicle_attitude);
	}
	if (_vehicle_attitude_groundtruth_sub.updated()) {
		_vehicle_attitude_groundtruth_sub.copy(&vehicle_attitude_grounadtrue);
	}
	if (_vehicle_global_position_sub.updated()) {
		_vehicle_global_position_sub.copy(&vehicle_global_position);
	}
	if (_vehicle_local_position_sub.updated()) {
		_vehicle_local_position_sub.copy(&vehicle_local_position);
		//do_control();
		//debug_info("pos_x", (vehicle_global_position.lat - 35.6895)*111000, vehicle_local_position.x);
		//debug_info("pos_y", (vehicle_global_position.lon - 139.6917)*111000, vehicle_local_position.y);
		//debug_info("pos_z", vehicle_global_position.alt, vehicle_local_position.z);
		//debug_log_pos();
		debug_log_rot();
	}
#endif

}

int Px4Hakoniwa::task_spawn(int argc, char *argv[])
{
	Px4Hakoniwa *instance = new Px4Hakoniwa();

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


int Px4Hakoniwa::custom_command(int argc, char *argv[])
{
	return print_usage("unknown command");
}

int Px4Hakoniwa::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("px4_hakoniwa", "driver");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
	return 0;
}
