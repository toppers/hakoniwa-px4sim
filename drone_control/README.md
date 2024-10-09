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
