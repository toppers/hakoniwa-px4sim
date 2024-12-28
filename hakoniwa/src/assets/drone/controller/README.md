English ｜ [日本語](README-ja.md)

# PID Control

Drones within Hakoniwa can be controlled using PID control without using PX4's control program.

The control program can be found in the following code:

https://github.com/toppers/hakoniwa-px4sim/blob/main/hakoniwa/src/assets/drone/controller/drone_pid_control.cpp

This code controls the altitude of the aircraft using PID control.

## Installation Instructions

First, please perform the following installation to install the Hakoniwa command:

[Hakoniwa Command and Library Installation Instructions](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa#hakoniwa-command-and-library-installation-instructions)

Next, install the following Python libraries:

```sh
pip3 install pandas matplotlib
```

## Execution Instructions

**Navigate to the directory:**

```
cd hakoniwa
```

**Execute the program:**

Executing the following script will run a 3-second program and display the results for 50 seconds of simulation time in a graph.

```
bash do_pid_test.bash 3 50
```

![Screenshot 2024-01-26 9 05 50](https://github.com/toppers/hakoniwa-px4sim/assets/164193/95ebacd7-d9b4-4b56-b0f3-e39c77f6c1bd)

The specifications for this script are as follows:

```
Usage: do_pid_test.bash <run-time-sec> <duration>
```

- `<run-time-sec>`: Simulation execution time (in real time, in seconds)
- `<duration>`: Simulation time to display in the graph (in seconds)

## Parameters for PID Control

PID control parameters can be modified in the aircraft's parameters (hakoniwa/config/drone_config_pid.json).

- **controller**: Container for the control program
  - **pid**: Container for defining PID control parameters
    - **position**: Container for defining parameters for position control
      - **x**: Container for defining parameters for position x
      - **y**: Container for defining parameters for position y
      - **z**: Container for defining parameters for position z
    - **angle**: Container for defining parameters for attitude control
      - **phi**: Container for defining parameters for attitude angle φ
      - **theta**: Container for defining parameters for attitude angle θ
      - **psi**: Container for defining parameters for attitude angle ψ

Parameters within the `position` and `angle` containers specify PID control parameters (all have the same parameter names).

- **setpoint**: The target value
  - For position, the unit is meters.
  - For angle, the unit is degrees.
- **Kp**: Proportional gain
- **Ki**: Integral gain
- **Kd**: Derivative gain
