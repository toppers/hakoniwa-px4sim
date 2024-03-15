
English ｜ [日本語](README-ja.md)

# What is This?

This provides an API for controlling the Hakoniwa's plant model with Python programs.

The Hakoniwa API (`hakosim`) essentially follows the signature of the AirSim API.

# Example Program

After performing the initialization process, the following operations are executed:

* Takes off to 3 meters.
  * client.takeoff(3)
* Moves to position (0, 3, 3) at 5m/sec.
  * client.moveToPosition(0, 3, 3, 5)
* Lands.
  * client.land()

```python
# connect to the HakoSim simulator
client = hakosim.MultirotorClient(<custom.json>)
client.confirmConnection()
client.enableApiControl(True)
client.armDisarm(True)

client.takeoff(3)
client.moveToPosition(0, 3, 3, 5)
client.land()
```

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
