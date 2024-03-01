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

/*
 * Roll
 */
// angle
#define ROLL_ANGLE_Kp 0.5
#define ROLL_ANGLE_Ki 0.0001
#define ROLL_ANGLE_Kd 0.5

// angular velocity
#define ROLL_VEL_Kp 0.02
#define ROLL_VEL_Ki 0.0001
#define ROLL_VEL_Kd 0.02

/*
 * PITCH
 */
// angle
#define PITCH_ANGLE_Kp 0.5
#define PITCH_ANGLE_Ki 0.0001
#define PITCH_ANGLE_Kd 0.5

// angular velocity
#define PITCH_VEL_Kp 0.02
#define PITCH_VEL_Ki 0.0001
#define PITCH_VEL_Kd 0.02

#endif /* _PID_PARAMS_H_ */