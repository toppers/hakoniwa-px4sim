# Gyro Sensor Component Specification

## Input 

* [](https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/physics#angular-velocity-in-body-coordinate-system)
* [Angle](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#angle)

## Output

* [[xyz]gyro](https://github.com/toppers/hakoniwa-px4sim/tree/main/docs/phys_specs/data/mavlink/HIL_SENSOR#xyzgyro) of HIL_SENSOR.

## External Parameters

### Moving Average Window (N)
- **Description**: N is the number of samples used to calculate the moving average of the gyroscope data. This parameter defines the window size over which the data is averaged.
- **Impact**: A larger N smooths the data by averaging over more samples, which can help to reduce the effect of transient noise. Conversely, a larger N can also introduce a delay in the sensor's response to changes, affecting the real-time accuracy.

### Noise Characteristics
- **Average (Mean) of Noise**: The average noise level added to the gyroscope data is set to zero. This ensures that, over time, the noise does not introduce a net bias to the gyroscope readings.
- **Variance of Noise**: This parameter specifies the variability of noise added to the gyroscope data. With a zero average, the noise can still exhibit variability, which is indicative of the sensor's performance under typical operational conditions.

The integration of zero-mean noise is crucial for ensuring that simulated gyroscope data is representative of actual sensor behavior, free from systematic bias and reflective of the random inaccuracies encountered in real-world scenarios.

## Operational Details

<img width="760" alt="スクリーンショット 2023-12-08 6 09 39" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/352f7875-f690-4a7c-a18d-13d188002a21">

- **Noise Integration**: Noise characterized by a zero mean and defined variance is integrated into the gyroscope data to simulate the random inaccuracies and variations found in real-world sensors, without introducing directional bias.

These operational details ensure that the simulated gyroscope data is realistic and provides an essential tool for assessing the drone's orientation and rotational behavior under actual flight conditions.
