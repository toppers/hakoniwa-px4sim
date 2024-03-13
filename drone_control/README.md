English ｜ [日本語](README-ja.md)


# Hakoniwa Drone Control

## What is this?

Here, we create an exercise program to control the [drone physics model](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_physics/README.md) in the Hakoniwa using PID control.

## Challenge

Please make the drone hover at an altitude of 10 meters.

The evaluation points are as follows:

* 50 points: Being in a hovering state after 100 seconds
* 30 points: Time to hover is within 10 seconds
* 20 points: Overshoot is within 1 meter

## Exercise Content
### Assumed Information

* Mass: 0.1 [kg]
* Air friction constant: 0.0001 [m/s·kg]
* Simulation time step: 3 msec

### Programming Language

The program will be created in the C/C++ language.

### File Structure

Implement `hako_module_drone_controller_impl.cpp`.

```
drone_control
├── CMakeLists.txt
├── README.md
├── cmake-build
├── include
│   └── hako_module_drone_controller.h
└── workspace
    ├── DroneAvator
    │   ├── CMakeLists.txt
    │   └── hako_module_drone_controller_impl.cpp
    ├── hako_module_drone_controller.c
    └── hako_module_drone_controller_impl.h
```

### Program Structure

Please implement the following two functions:

* `hako_module_drone_controller_impl_init`
  * This function is called only once at program startup. Implement the necessary initialization for control here.
  * Return 0 if the initialization succeeds. If it fails, return a non-zero value.
* `hako_module_drone_controller_impl_run`
  * This function is called at each control timing. Implement the necessary processes for hovering control.
  * The input argument `in` will provide the state of the drone (position and attitude angle).
  * The return value should be the control instructions for the drone (thrust and torque).

**Functions to Implement:**
Please replace the `TODO` parts.

```
int hako_module_drone_controller_impl_init(void* context)
{
    //TODO
}

mi_drone_control_out_t hako_module_drone_controller_impl_run(mi_drone_control_in_t *in)
{
    //TODO
}
```
### Input/Output Data Information

#### mi_drone_control_in_t

This represents the current position and attitude information of the aircraft.

**データ型:**
```
typedef struct {
    double pos_x;
    double pos_y;
    double pos_z;
    double euler_x;
    double euler_y;
    double euler_z;
} mi_drone_control_in_t;
```
* Coordinate System: NED
* pos_x, pos_y, pos_z
  * Unit: Meters
  * pos_x: North
  * pos_y: East
  * pos_z: Down
  * Initial Value: All 0
* euler_x, euler_y, euler_z
  * Unit: Radians
  * euler_x: Angle in the direction of the x-axis (roll)
  * euler_y: Angle in the direction of the y-axis (pitch)
  * euler_z: Angle in the direction of the z-axis (yaw)
  * Initial Value: All 0

#### mi_drone_control_out_t

This structure represents the control output information to the aircraft.

**Data Type:**
```
typedef struct {
    double thrust;
    double torque_x;
    double torque_y;
    double torque_z;
} mi_drone_control_out_t;
```
* thrust
  * Unit: Newton (N)
  * Vertical thrust of the aircraft
* torque_x, torque_y, torque_z
  * Unit: Newton-meter (Nm)
  * torque_x: Torque in the roll direction
  * torque_y: Torque in the pitch direction
  * torque_z: Torque in the yaw direction

# Environment Setup

Please place the program you have created under the [drone_control/workspace](https://github.com/toppers/hakoniwa-px4sim/tree/main/drone_control/workspace) directory with a unique directory name.

Example of placement:
```
Gemini
├── CMakeLists.txt
└── hako_module_drone_controller_impl.cpp
```
# Environment Setup

Make sure that the `project` in `CMakeLists.txt` matches your directory name.

Example: If the directory name is `Gemini`, the project name should be `Gemini`.
```
# Build settings for the loadable module
project(Gemini)
```

# How to Build

## For Windows

The [hakoniwa](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa) build process automatically builds  the applications.

## For not Windows(WSL2/Mac)

Move to `cmake-build` and execute the following commands.

```
cmake ..
make
```

Upon success, a loadable module will be created.

Example: lib<Directory Name>.so will be created
```
cmake-build/workspace/Gemini/libGemini.so 
```

# Preparing to Run the Simulation

To run the created loadable module in the simulation, the following steps are necessary.

1. Install [hakoniwa](https://github.com/toppers/hakoniwa-px4sim/blob/main/hakoniwa/README-ja.md) from hakoniwa-px4sim.
2. Install [hakoniwa-unity-drone-model](https://github.com/toppers/hakoniwa-unity-drone-model).
3. [Settings on the Hakoniwa side](#Settings-on-the-Hakoniwa-side).
4. [Settings on the Unity side](#Settings-on-the-Unity-side).

## Settings on the Hakoniwa Side

### For Simulating a Single Aircraft

Use the default config file (hakoniwa/config/drone_config_0.json) provided.

### For Simulating Multiple Aircraft Simultaneously

You will need to prepare the aircraft parameters for the Hakoniwa drone simulator for each loadable module you wish to run.

Example:
```
hakoniwa/config/sample_control
├── drone_config_0.json
├── drone_config_1.json
└── drone_config_2.json
```

The format for the aircraft parameter files should be as follows:

* `drone_config_<index>.json`
* `<index>` should be a sequential number starting from 0.

For each aircraft's parameters, please set only the following items:

* name
  * Specify the directory name of the loadable module. Example: Gemini
* components.droneDynamics.position_meter
  * Set the initial position of the aircraft. Please choose a location that does not overlap with other aircraft.
* controller.moduleDirectory
  * Specify the directory name where the loadable module is placed, relative to the `hakoniwa` directory.
  * Example: ../drone_control/cmake-build/workspace/Gemini

## Settings on the Unity Side

### For Simulating a Single Aircraft

Please use the Scenes/Hakoniwa.

After selecting the scene, please click `Generate`.

### For Simulating Multiple Aircraft Simultaneously

Please use the Scenes/MultiDrones.

Place the number of aircraft corresponding to the number of loadable modules.

The name of each aircraft should match the directory name of the loadable module.

Example:

![スクリーンショット 2024-02-19 14 45 27](https://github.com/toppers/hakoniwa-px4sim/assets/164193/f556e5f7-327c-47dd-80a6-c41a0382f619)

After placing the aircraft, please click `Generate`.

# How to Run the Simulation

Under the `hakoniwa` directory, execute the following command.

For Windows:
```
drone-control.bat <hakoniwa-unity-drone-model location> <config directory>
```

For not Windows:
```
bash drone-control.bash <hakoniwa-unity-drone-model location> <config directory>
```

For `<config directory>`, specify the directory path where the aircraft parameter files are located.

## Examples for Windows
Example: For simulating a single aircraft
```
./drone-control.bat ../../hakoniwa-unity-drone-model config/win
```

Example: For simulating multiple aircraft
```
./drone-control.bat ../../hakoniwa-unity-drone-model config/win/sample_control
```

## Examples for not Windows
Example: For simulating a single aircraft
```
bash drone-control.bash ../../hakoniwa-unity-drone-model config
```

Example: For simulating multiple aircraft
```
bash drone-control.bash ../../hakoniwa-unity-drone-model config/sample_control
```

Upon success, the following logs will be output, after which you should start the simulation on the Unity side.

```
HAKO_CAPTURE_SAVE_FILEPATH : ./capture.bin
HAKO_BYPASS_IPADDR : 127.0.0.1
HAKO_CUSTOM_JSON_PATH : ../../../release/hakoniwa-unity-drone-model/custom.json
DRONE_CONFIG_PATH : config/sample_control
HAKO_BYPASS_PORTNO : 54001
INFO: LOADED drone config file: config/sample_control/drone_config_0.json
INFO: LOADED drone config file: config/sample_control/drone_config_1.json
INFO: LOADED drone config file: config/sample_control/drone_config_2.json
INFO: hako_master_init() success
INFO: setup start
DroneDynamicType: BodyFrame
INFO: logpath: ./drone_log0/drone_dynamics.csv
Rotor vendor: None
Thruster vendor: None
param_A: 1.53281e-08
param_B: 1.3e-10
param_Jr: 1e-10
DroneDynamicType: BodyFrame
INFO: logpath: ./drone_log1/drone_dynamics.csv
Rotor vendor: None
Thruster vendor: None
param_A: 1.53281e-08
param_B: 1.3e-10
param_Jr: 1e-10
DroneDynamicType: BodyFrame
INFO: logpath: ./drone_log2/drone_dynamics.csv
Rotor vendor: None
Thruster vendor: None
param_A: 1.53281e-08
param_B: 1.3e-10
param_Jr: 1e-10
INFO: loading drone & controller: 0
SUCCESS: Loaded module name: ChatGPT3_5
INFO: loading drone & controller: 1
SUCCESS: Loaded module name: ChatGPT4
INFO: loading drone & controller: 2
SUCCESS: Loaded module name: Gemini
Robot: Gemini, PduWriter: Gemini_drone_motor
channel_id: 0 pdu_size: 88
INFO: Gemini create_lchannel: logical_id=0 real_id=0 size=88
Robot: Gemini, PduWriter: Gemini_drone_pos
channel_id: 1 pdu_size: 48
INFO: Gemini create_lchannel: logical_id=1 real_id=1 size=48
Robot: ChatGPT4, PduWriter: ChatGPT4_drone_motor
channel_id: 0 pdu_size: 88
INFO: ChatGPT4 create_lchannel: logical_id=0 real_id=2 size=88
Robot: ChatGPT4, PduWriter: ChatGPT4_drone_pos
channel_id: 1 pdu_size: 48
INFO: ChatGPT4 create_lchannel: logical_id=1 real_id=3 size=48
Robot: ChatGPT3_5, PduWriter: ChatGPT3_5_drone_motor
channel_id: 0 pdu_size: 88
INFO: ChatGPT3_5 create_lchannel: logical_id=0 real_id=4 size=88
Robot: ChatGPT3_5, PduWriter: ChatGPT3_5_drone_pos
channel_id: 1 pdu_size: 48
INFO: ChatGPT3_5 create_lchannel: logical_id=1 real_id=5 size=48
WAIT START
```

# P Control and PD Control Strategies

From the physical equations of a drone's vertical motion, examples of P control and PD control strategies are presented.

## Plant Side Physical Equations

The z-axis follows the NED coordinate system.

**Target motion equation:**

$\ddot{z} = -\frac{u(t)}{m} + g - \frac{d}{m} \dot{z}$

**Laplace Transform:**

$s^{2}Z(s) = -\frac{U(s)}{m} + \frac{g}{s} - \frac{d}{m} s Z(s)$

## Control Side Equations

R is specified as a positive value.

### P Control

**Control equation:**

$u(t) = K_p ( z(t) + R  ) + m g$

**Laplace Transform:**

$U(s) = K_p ( Z(s) + \frac{R}{s} ) + \frac{m g}{s}$

#### Substituting P Control equation into U(s) on the Plant Side

$s^{2}Z(s) = -\frac{ K_p ( Z(s) + \frac{R}{s}) + \frac{mg}{s}  }{m} + \frac{g}{s} - \frac{d}{m} s Z(s)$

When separating differential and integral terms, you get:

$s^{2}Z(s) + \frac{d}{m} s Z(s) + \frac{K_p}{m} Z(s) = ( -\frac{K_p R}{m}) \frac{1}{s}$

Solving for Z(s) leads to:

$Z(s) = \frac{( -\frac{K_p R}{m})}{s(s^{2} + \frac{d}{m} s + \frac{K_p}{m})}$

By defining:

$\omega_n^{2} = \frac{K_p}{m}$

$\zeta = \frac{d}{2\sqrt{mK_p}}$

The equation becomes:

$Z(s) = - R \frac{1}{s} \frac{\omega_n^{2}}{s^{2} + 2 \zeta \omega_n s + \omega_n^{2}}$

### PD Control

**Control equation:**

$u(t) = K_p ( z(t) + R  ) + m g + K_d ( \dot{z(t) + R} )$

Since R is a constant, it simplifies to:

$u(t) = K_p ( z(t) + R  ) + m g + K_d  \dot{z(t)}  $

**Laplace Transform:**

$U(s) = K_p ( Z(s) + \frac{R}{s} ) + \frac{m g}{s} + K_d s Z(s) $

#### Substituting PD Control equation into U(s) on the Plant Side

$s^{2}Z(s) = -\frac{ K_p ( Z(s) + \frac{R}{s}) + \frac{mg}{s} + K_d s Z(s) }{m} + \frac{g}{s} - \frac{d}{m} s Z(s)$

When differentiating and integrating:

$s^{2}Z(s) + \frac{d}{m} s Z(s) +  K_d s Z(s) + \frac{K_p}{m} Z(s) = ( -\frac{K_p R}{m}) \frac{1}{s}$

Solving for Z(s):

$Z(s) = \frac{( -\frac{K_p R}{m})}{s(s^{2} + (\frac{d}{m} + K_d ) s + \frac{K_p}{m})}$

Defining:

$\omega_n^{2} = \frac{K_p}{m}$

$\zeta = \frac{d + m K_d}{2\sqrt{mK_p}}$

Leads to:

$Z(s) = - R \frac{1}{s} \frac{\omega_n^{2}}{s^{2} + 2 \zeta \omega_n s + \omega_n^{2}}$
