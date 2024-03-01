#ifndef _PID_PARAMS_H_
#define _PID_PARAMS_H_

#define GRAVITY 9.81
/*
 * takeoff and landing
 */
// position
#define VERTICAL_POS_Kp 0.5
#define VERTICAL_POS_Ki 0.0
#define VERTICAL_POS_Kd 0.1

// velocity
#define VERTICAL_VEL_Kp 0.5
#define VERTICAL_VEL_Ki 0.0
#define VERTICAL_VEL_Kd 0.1

/*
 * Yaw
 */
// angle
#define YAW_ANGLE_Kp 0.5
#define YAW_ANGLE_Ki 0.0001
#define YAW_ANGLE_Kd 0.5

// angular velocity
#define YAW_VEL_Kp 0.02
#define YAW_VEL_Ki 0.0001
#define YAW_VEL_Kd 0.02

#endif /* _PID_PARAMS_H_ */