# Geomagnetic Sensor Component Specification

## Input 

* [Angle](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#angle)

## Output

* [[xyz]mag](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_SENSOR/README.md#xyzmag) of HIL_SENSOR.

## External Parameters

### Magnetic North Vector (MagNorthV), Declination, and Inclination

- **Description**: Based on the drone's initial geolocation (latitude and longitude), the vector representing magnetic north along with the declination and inclination angles are determined and set as external parameters. These values are crucial for calculating the magnetic field vector relative to the drone's body frame.
- **Reference**: For more details on how these are used in the simulation, refer to [[xyz]mag](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_SENSOR/README.md#xyzmag).

### Moving Average Window (N)
- **Description**: N is the number of samples over which the moving average is calculated for the geomagnetic sensor data. This parameter defines the window size for averaging and smoothing data.
- **Impact**: A larger N will provide smoother data by reducing transient noise effects, but may also increase the latency of the sensor's response to magnetic field changes.

### Noise Characteristics
- **Average (Mean) of Noise**: The mean value of noise integrated into the geomagnetic sensor data is set to zero, ensuring no net bias over time in the sensor readings.
- **Variance of Noise**: Specifies the variability of the noise added to the sensor data. Despite a zero mean, the noise will display variability, simulating the level of fluctuation that would be present under typical conditions.

The use of zero-mean noise is vital for emulating real-world sensor behavior accurately within the simulation, providing data free from systematic bias and representative of the randomness seen in actual sensor performance.

## Operational Details

![スクリーンショット 2023-11-26 7 55 57](https://github.com/toppers/hakoniwa-px4sim/assets/164193/7471c594-da27-464d-9011-0e06e28fe186)

- **Data Transformation**: The transformation process involves converting the drone's orientation angles and the magnetic north vector into the geomagnetic sensor readings ([xyz]mag). This ensures that the simulated sensor data reflects how the drone's onboard magnetometer would interpret the Earth's magnetic field in real time.
- **Noise Integration**: Random noise with a zero mean and defined variance is integrated into the geomagnetic sensor data, capturing the natural inaccuracies and variations found in actual sensors without introducing any directional bias.

By accurately simulating these aspects, the geomagnetic sensor component provides essential data for the realistic evaluation of the drone's navigation systems, particularly those relying on magnetic field measurements for orientation and course correction.
