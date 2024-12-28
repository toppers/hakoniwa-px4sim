# GPS Sensor Component Specification

## Input Specifications

* [Position](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#position).
* [Velocity](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/physics/README.md#velocity).

## Output Specifications

- [GPS Data](https://github.com/toppers/hakoniwa-px4sim/blob/main/docs/phys_specs/data/mavlink/HIL_GPS/README.md)

## External Parameters

### Latitude/Longitude/Altitude (Lat/Lon/Alt)
- **Description**: These parameters define the initial reference point for the drone's position and are used to calculate subsequent position changes.

### Moving Average Window (N)
- **Description**: The number of samples over which the moving average is calculated for GPS data to smooth out the variations and provide a more consistent reading.

### Variance of Noise
- **Description**: Specifies the level of variability in the noise added to the GPS data to simulate the inaccuracies inherent in real-world GPS measurements.

## Operational Details

![image](https://github.com/toppers/hakoniwa-px4sim/assets/164193/92253673-61b7-4182-98ec-c6981b957552)


- **Data Transformation and Processing**: 
  - **Convert LatLonAlt**: Converts the input position to the drone's body coordinate system for internal processing.
  - **Convert VelVnVeVd**: Translates the velocity components to the drone's body coordinate system.
  - **Convert Cog**: Processes the course over ground based on the velocity components.
  - **Assemble**: Compiles the transformed position and velocity data into a comprehensive GPS data structure.
  - **Add Noise**: Applies noise with defined variance to the GPS data to simulate real-world sensor behavior.
  
- **Average GPS Data**: Uses a moving average over the defined sample window (N) to smooth out the data, which is particularly useful for mitigating the effect of random noise and making the data more reliable for navigation algorithms.

By integrating these operational details, the GPS sensor component within the simulation provides a realistic representation of how a physical GPS sensor would function, including the handling of noise and data averaging to reflect actual flight conditions.
