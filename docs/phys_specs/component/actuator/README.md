# RotorDynamics

## Basic Dynamic Model

This fundamental equation represents the motor dynamics, using a time constant and a gain to translate PWM duty cycle to rotational speed change:
   
![スクリーンショット 2023-11-26 8 39 19](https://github.com/toppers/hakoniwa-px4sim/assets/164193/ac0612e4-9dbf-45b3-8abe-6c48c87227fb)

Where:
 - **Ω** is the rotor's angular velocity in revolutions per second (RPS).
 - **k** is the conversion factor (gain) from PWM duty cycle to angular velocity.
 - **τ** is the motor's time constant, indicating its responsiveness.

## Resistance Loss

The model that accounts for the increasing aerodynamic drag as the rotor's angular velocity rises:

![スクリーンショット 2023-11-26 8 40 08](https://github.com/toppers/hakoniwa-px4sim/assets/164193/6436c91b-316d-439a-a26d-91273da375cc)

Where:
 - **c** is the aerodynamic drag coefficient.
 - - **f(Ω)** is the function representing the resistance effect as a function of angular velocity.

## Saturation Effect

This equation models the saturation effect as the motor’s angular velocity approaches its limit:

![スクリーンショット 2023-11-26 8 41 05](https://github.com/toppers/hakoniwa-px4sim/assets/164193/d3d74b4e-3e3b-4bc2-b467-59404d123b35)

As **Ω** approaches **Ωmax**, the increase in **dΩ/dt** is moderated by substituting **Ω** with **Ωsat(Ω)** in the dynamic model, thereby incorporating the saturation effect without exceeding the motor's maximum speed.

## Overall　Model
The complete model is then expressed as:

![スクリーンショット 2023-11-26 8 43 40](https://github.com/toppers/hakoniwa-px4sim/assets/164193/327f6141-ec7e-47b7-a4f7-35ff0bfa47c0)

This model describes how the rotor responds to the PWM duty cycle and how the motor's physical limits are handled within the simulation. In practical systems, further adjustments to this model would be made to match the specific behaviors of the motor.
