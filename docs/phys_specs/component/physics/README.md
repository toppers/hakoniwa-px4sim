# Drone Dynamics

The following equations describe the motion of a drone, considering a system with six degrees of freedom. These equations account for the drone's position in 3D space (x, y, z) and its orientation (roll (φ), pitch (θ), and yaw (ψ)). The motion is influenced by the drone's thrust, mass, attitude, and air resistance. The equations are:

<img width="464" alt="スクリーンショット 2023-12-01 7 19 07" src="https://github.com/toppers/hakoniwa-px4sim/assets/164193/3f101a80-df0a-4122-b6ed-e8ba8a927a59">


## Explanation of the Drone Equations

1. **X-Axis Acceleration**:
   - Represents acceleration along the drone's x-axis.
   - Dependent on the thrust ( u ), drone's mass ( m ), its attitude (roll (φ) and pitch (θ)), and air resistance coefficient ( c ).

2. **Y-Axis Acceleration**:
   - Represents acceleration along the y-axis.
   - Influenced by similar factors as the x-axis acceleration.

3. **Z-Axis Acceleration**:
   - Represents vertical (z-axis) acceleration.
   - Includes the effect of gravity ( g ) along with thrust and attitude, and air resistance coefficient ( c ).

4. **Roll Angle Acceleration**:
   - Describes the acceleration of the roll angle (φ).
   - Controlled by the torque ( τφ ).

5. **Pitch Angle Acceleration**:
   - Describes the acceleration of the pitch angle (θ).
   - Dependent on the torque ( τθ ).

6. **Yaw Angle Acceleration**:
   - Describes the acceleration of the yaw angle (ψ).
   - Influenced by the torque ( τψ ).

These equations are fundamental for understanding and controlling the motion of a drone, particularly in tasks involving attitude control and trajectory planning.
