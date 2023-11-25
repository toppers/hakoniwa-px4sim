# Accelerometer Sensor Component Specification

## Input 

* [Velocity](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#velocity).
* [Angle](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#angle)

## Output

The output is [[xyz]acc](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_SENSOR/README.md#xyzacc) of HIL_SENSOR.

## External Parameters

### Observation Interval (Δt)
- **Description**: Δt represents the time interval between each measurement taken by the accelerometer. This parameter determines the frequency at which the sensor data is sampled and updated.
- **Impact**: The choice of Δt affects the resolution and responsiveness of the accelerometer readings. Shorter intervals lead to more frequent updates but can increase noise, while longer intervals can smooth out data but may reduce sensitivity to quick changes.

### Moving Average Window (N)
- **Description**: N is the number of samples used to calculate the moving average of the accelerometer data. This parameter defines the window size over which the data is averaged.
- **Impact**: Setting N involves a trade-off between data smoothness and latency. A larger N provides smoother data by averaging over more samples, which can reduce the effect of transient noise. However, it also introduces a delay in the sensor's response to changes.

### Noise Characteristics
- **Average (Mean) of Noise**: The average value of the noise added to the accelerometer data is set to zero. This means that over time, the noise will not have a net positive or negative effect on the acceleration readings.
- **Variance of Noise**: The variance parameter specifies the spread of noise around the average. Even with an average of zero, the noise can vary, representing the randomness or fluctuation level in the sensor readings.

The integration of zero-mean noise ensures that any bias is minimized, allowing the simulation to accurately reflect only the random variations typical of real-world conditions.

## Operational Details

![スクリーンショット 2023-11-25 16 38 37](https://github.com/toppers/hakoniwa-px4sim/assets/164193/4ed00e11-c35f-45df-899c-91c3f872c37c)

- **Data Transformation and Coordinate System**: The velocity data in the NED coordinate system is transformed into acceleration data in the drone's body coordinate system, accounting for the drone's orientation.

- **Acceleration Calculation Methodology**: Acceleration is calculated as the rate of change of velocity over each interval Δt. To reduce transient fluctuations and improve data stability, a moving average is applied. This involves averaging the acceleration values over multiple samples (spanning N times Δt) to provide a more consistent and reliable measurement.

- **Observation Frequency**: Measurements are conducted at regular intervals defined by Δt. The moving average calculation uses N samples within each period of N times Δt to ensure smoother data.

- **Noise Integration**: Noise with a zero mean and a specified variance is integrated into the acceleration data. This approach emulates the random inaccuracies and variations found in real-world sensor data, without introducing a bias in either direction.

The updated noise characteristics ensure that the simulated accelerometer data is realistic and provides a valuable tool for testing the drone's response to actual flight conditions.
