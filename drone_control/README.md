English ｜ [日本語](README-ja.md)

# Hakoniwa Drone Control

## What is this?

This is a program that uses PID control for the [Hakoniwa Drone Physics Model](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_physics/README-ja.md).

Two programs are provided for PID control:

- FlightController
  - A control program that allows you to operate the Hakoniwa drone using a Python API.
- RadioController
  - A control program that allows you to operate the Hakoniwa drone using a PS4 controller.

These programs, once built, are created as loadable modules and are automatically loaded by the Hakoniwa Drone Simulator.


# Build Instructions

## For Windows

The build will be performed automatically during the [hakoniwa](https://github.com/toppers/hakoniwa-px4sim/tree/main/hakoniwa) build process.

## For non-Windows (WSL2/Mac)

Move to the `cmake-build` directory and run the following commands:

```
cmake ..
make
```

Upon success, the loadable module will be created.

Example: The file `lib<directory_name>.so` will be generated
```
cmake-build/workspace/FlightController/libFlightController.so 
cmake-build/workspace/RadioController/libRadioController.so 
```

# Control Parameter File

The movement of the drone is configured through various control parameters. These parameters are defined in a text file, which you can customize as needed.

If you notice any issues with the drone's movement, you can fine-tune its behavior by adjusting the values in the parameter file.

The current default settings can be found here:
[Drone Control Parameter File](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_control/config/param-api.txt)

Additionally, to ensure the parameter file is loaded correctly, make sure to specify the file path in the environment variable `HAKO_CONTROLLER_PARAM_FILE`.