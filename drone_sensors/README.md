English ｜ [日本語](README-ja.md)

# What is This?

Here, we manage the code for creating sensor models for each vendor.

# Target Sensors

The currently supported sensors are as follows:

- [ ] Accelerometer
- [X] Gyroscope
- [ ] GPS
- [ ] Magnetic sensor
- [ ] Barometric pressure sensor

The sensor modules created here are built as dynamic link libraries. The libraries created can be dynamically loaded by specifying the path in the airframe characteristic parameter file of the sandbox.

# Gyroscope

## How to Create

Creating a gyroscope is achieved by implementing the following interface functions.

Definition file: hako_module_drone_sensor_gyro.h
```c
typedef struct {
    int (*init) (mi_drone_sensor_gyro_context_t* context);
    mi_drone_sensor_gyro_out_t (*run) (mi_drone_sensor_gyro_in_t *in);
} HakoModuleDroneSensorGyroType;
```

The implementation should be placed directly under `sensors/gyro/vendor`. Create a directory name for each vendor and place the necessary files under it.

Example: Sandbox's sample gyroscope
```
./sensors/gyro/vendor
└── hakogyro
    ├── CMakeLists.txt
    ├── config_parser.hpp
    └── hako_module_drone_sensor_gyro_impl.cpp
```

Example of CMakeLists.txt settings:

```cmake
# Build settings for the loadable module
project(hakogyro)

# Source files
set(SOURCE_FILES hako_module_drone_sensor_gyro_impl.cpp)

# Generate dynamic library as a loadable module
add_library(
    ${PROJECT_NAME} MODULE 
    ${SOURCE_FILES}
    ../hako_module_drone_sensor_gyro.c
)

target_include_directories(
    ${PROJECT_NAME} 
    PRIVATE ../../include
    PRIVATE ..
)
```

## How to Build

The sensor module can be automatically built by moving to `drone_sensors` and executing the following command:

```bash
bash build.bash
```

Execution log:
```
-- The C compiler identification is AppleClang 15.0.0.15000100
-- The CXX compiler identification is AppleClang 15.0.0.15000100
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (0.6s)
-- Generating done (0.0s)
-- Build files have been written to: /Users/tmori/project/release/hakoniwa-px4sim/drone_sensors/cmake-build
[ 33%] Building CXX object sensors/gyro/vendor/hakogyro/CMakeFiles/hakogyro.dir/hako_module_drone_sensor_gyro_impl.cpp.o
[ 66%] Building C object sensors/gyro/vendor/hakogyro/CMakeFiles/hakogyro.dir/__/hako_module_drone_sensor_gyro.c.o
[100%] Linking CXX shared module libhakogyro.so
[100%] Built target hakogyro
```

## Sample Implementation Description

As an example of a gyroscope sample implementation, `hakogyro` is provided.

The implementation of the interface functions is as follows:

* Overview
  * This sensor experiences bias in sensor values due to temperature drift.
  * The parameters for temperature drift are as follows:
    * TEMP_MIN: The lower limit of acceptable temperature
    * TEMP_MAX: The upper limit of acceptable temperature
    * TEMP_DRIFT: The rate of drift when outside the acceptable range
  * The parameters for temperature drift are defined in an external file.
* hako_module_drone_sensor_gyro_impl_init
  * Retrieves the temperature drift parameter file and saves each parameter as context.
* hako_module_drone_sensor_gyro_impl_run
  * From the sandbox, each acceleration (in the aircraft coordinate system) and aircraft temperature are specified as input data, based on which, the sensor value with drift is returned.

## Gyroscope Settings in the Airframe Parameters

The settings for the gyroscope in the airframe parameters consist of the following two items:

* gyro
  * vendor: Specify the directory path of the created sensor.
  * context
    * moduleName: Specify the module name of the created sensor.
    * file: Specify the sensor parameter file, such as temperature drift parameters.

Example of settings:
```json
"gyro": {
    "vendor": "../drone_sensors/cmake-build/sensors/gyro/vendor/hakogyro",
    "context": {
        "moduleName": "hakogyro",
        "file": "./config/hard-maker-demo/sensor0.txt"
    },
    "sampleCount": 1,
    "noise": 0.0
}
```