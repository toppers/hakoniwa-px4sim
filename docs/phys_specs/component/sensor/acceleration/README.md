# Accelerometer Sensor Component Specification

## Input 

Drone's [Velocity](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#velocity).

## Output Specifications

The output is [[xyz]acc](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_SENSOR/README.md#xyzacc) of HIL_SENSOR.

## External Parameters

### Observation Interval (Δt)
- **Description**: Δt represents the time interval between each measurement taken by the accelerometer. This parameter determines the frequency at which the sensor data is sampled and updated.
- **Impact**: The choice of Δt affects the resolution and responsiveness of the accelerometer readings. Shorter intervals lead to more frequent updates but can increase noise, while longer intervals can smooth out data but may reduce sensitivity to quick changes.

### Moving Average Window (N)
- **Description**: N is the number of samples used to calculate the moving average of the accelerometer data. This parameter defines the window size over which the data is averaged.
- **Impact**: Setting N involves a trade-off between data smoothness and latency. A larger N provides smoother data by averaging over more samples, which can reduce the effect of transient noise. However, it also introduces a delay in the sensor's response to changes.

### Noise Characteristics
- **Average (Mean) of Noise**: This parameter defines the average value of the noise added to the accelerometer data. It represents the baseline level of noise.
- **Variance of Noise**: This parameter specifies the variance in the noise added to the accelerometer data. It indicates how much the noise can deviate from the average value, representing the unpredictability or randomness of the noise.

The inclusion of noise characteristics as external parameters allows for the simulation to be tuned to different levels of sensor accuracy and environmental conditions. Adjusting these parameters can help in creating scenarios ranging from ideal (low noise) to challenging (high noise) sensor conditions.

## Operational Details

- **Data Transformation and Coordinate System**: The velocity data in the NED coordinate system is transformed into acceleration data in the drone's body coordinate system, accounting for the drone's orientation.

- **Acceleration Calculation Methodology**: Acceleration is calculated as the rate of change of velocity over each interval Δt. To reduce transient fluctuations and improve data stability, a moving average is applied. This involves averaging the acceleration values over multiple samples (spanning N times Δt) to provide a more consistent and reliable measurement.

- **Observation Frequency**: Measurements are conducted at regular intervals defined by Δt. The moving average calculation uses N samples within each period of N times Δt to ensure smoother data.

- **Noise Integration**: Realistic noise, characterized by its average and variance, is integrated into the acceleration data to emulate inaccuracies and environmental variations typical in real-world sensor data.

By including these external parameters, the accelerometer sensor component within the simulation can provide adaptable data, suitable for a variety of testing needs. This flexibility is crucial for developing robust drone navigation systems capable of operating under diverse conditions.
