
English ｜ [日本語](README-ja.md)

# What is This?

This provides an API for controlling the Hakoniwa's plant model with Python programs.

The Hakoniwa API (`hakosim`) essentially follows the signature of the [AirSim](https://microsoft.github.io/AirSim/) API.

For an overview of the Hakoniwa Python API, please refer to [this link](https://github.com/toppers/hakoniwa-px4sim/tree/main/drone_api/libs).

# Example Program

We've prepared an [example program](https://github.com/toppers/hakoniwa-px4sim/blob/main/drone_api/sample/sample.py) to help you understand how to use the Python API.

The sample program performs the following functions:

* Taking off the drone
* Moving the drone
* Displaying the drone's position information
* Transporting cargo
* Capturing and saving images from the drone's front-facing camera
* Retrieving and debugging 3D LiDAR data
* Landing the drone

# Demo

https://www.youtube.com/watch?v=ZaJunG7pxi8

# Installation

Please execute the following command.

## For Windows

```
cd drone_api
mkdir install_dir
<path/to>/install.bat 
```

## For not Windows


```bash
cd drone_control
bash build.bash
cd ..
```

```bash
bash drone_api/install.bash
```

Additionally, please append the following path to the PYTHONPATH environment variable:

```
export PYTHONPATH=/usr/local/lib/hakoniwa/py:${PYTHONPATH}
```

# Unity Settings

Please use Scenes/ApiDemo.

After selecting the scene, please click `Generate`.

# How to Run the Simulation

The method for running the simulation is as follows.

```bash
cd hakoniwa
```

## For Windows

```
./drone-app.bat ../../hakoniwa-unity-drone-model ./config/win/api_sample
```

## For not Windows

```bash
bash drone-app.bash ../../hakoniwa-unity-drone-model ./config/api_sample
```

# How to Run the Python Program

After the simulation has been executed, you can run the Python program at any time to call the Hakoniwa API.


```bash
cd drone_api/sample
```

## For Windows

```
python sample.py ../../../hakoniwa-unity-drone-model/custom.json
```

## For not Windows

```bash
python3.12 sample.py ../../../hakoniwa-unity-drone-model/custom.json
```

Here's the English translation of the provided text:

---

# Hakoniwa Drone Configuration File

Various sensors/actuators are implemented on the Hakoniwa drone in Unity. These features have parameters, which can be set by placing a parameter definition file (drone_config.json) directly under hakoniwa-unity-drone-model/plugin/plugin-srcs to reflect these parameters.

Example configuration:
```json
{
    "drones": {
        "DroneTransporter": {
            "audio_rotor_path": "file:///Users/tmori/Downloads/spo_ge_doron_tobi_r01.mp3",
            "LiDARs": {
                "LiDAR": {
                    "Enabled": false,
                    "NumberOfChannels": 16,
                    "RotationsPerSecond": 10,
                    "PointsPerSecond": 10000,
                    "MaxDistance": 10,
                    "X": 0.45, "Y": 0, "Z": -0.3,
                    "Roll": 0, "Pitch": 0, "Yaw" : 0,
                    "VerticalFOVUpper": 5,
                    "VerticalFOVLower": -5,
                    "HorizontalFOVStart": -5,
                    "HorizontalFOVEnd": 5,
                    "DrawDebugPoints": true
                }
            }
        }
    }
}
```

* audio_rotor_path: File path for the drone rotor sound. If the file exists, rotor sounds can be played.
* LiDARs: Configuration settings for the LiDAR mounted on the drone.
* Enabled: Whether the LiDAR is enabled or disabled. 'true' is enabled, 'false' is disabled.
* NumberOfChannels: Number of channels for the LiDAR.
* RotationsPerSecond: Rotational speed of the LiDAR (rotations per second).
* PointsPerSecond: Total number of points per second for the LiDAR.
* MaxDistance: Maximum measurable distance by the LiDAR (in meters).
* X, Y, Z: Position of the LiDAR relative to the drone's coordinates (in meters).
* Roll, Pitch, Yaw: Orientation of the LiDAR (roll, pitch, yaw angles).
* VerticalFOVUpper: Upper vertical Field of View (FOV) for the LiDAR.
* VerticalFOVLower: Lower vertical Field of View (FOV) for the LiDAR.
* HorizontalFOVStart: Starting angle of the horizontal FOV for the LiDAR.
* HorizontalFOVEnd: Ending angle of the horizontal FOV for the LiDAR.
* DrawDebugPoints: Whether to draw debug points for the LiDAR (only if using the Unity editor).

The basic specs for the Hakoniwa LiDAR are as follows:

```
Frequency: 5Hz
Horizontal: -90° to 90°
Vertical: -30° to 30°
Resolution: 1°
```

The PDU data size calculated above is used as the upper limit size, so the parameter definitions have the following constraints:

* NumberOfChannels: 61
* HorizontalPointsPerRotation = 181
  * PointsPerRotation = PointsPerSecond / RotationsPerSecond
  * HorizontalPointsPerRotation = PointsPerRotation / NumberOfChannels

The resolutions in horizontal and vertical directions are calculated using the following formulas:

* Horizontal direction: HorizontalRanges / HorizontalPointsPerRotation
  * HorizontalRanges = HorizontalFOVEnd - HorizontalFOVStart
* Vertical direction: VerticalRanges / param.NumberOfChannels
  * VerticalRanges = VerticalFOVUpper - VerticalFOVLower