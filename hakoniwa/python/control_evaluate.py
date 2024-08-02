import pandas as pd
import numpy as np
import sys
import json

# Default parameter settings
default_params = {
    "NUM_LAST_POINTS": 1000,
    "VARIANCE_THRESHOLD": 1.0,
    "RISE_TIME_10_PERCENT": 0.1,
    "RISE_TIME_90_PERCENT": 0.9,
    "DELAY_TIME_PERCENT": 0.5,
    "SETTLING_TIME_PERCENT": 0.05,
    "TARGET_TR": 10.0,
    "TARGET_TD": 5.0,
    "TARGET_OS": 1.0,
    "TARGET_TS": 20.0,
    "TARGET_VALUE": 10.0,
    "TARGET_CV": 0.01,
    "AXIS": "Z",
    "INVERT_AXIS": True,
    "EVALUATION_START_TIME": 0.0,
    "CONVERT_TO_DEGREE": False
}

def load_parameters(param_file):
    with open(param_file, 'r') as file:
        params = json.load(file)
    return params

def main(input_file, param_file=None):
    # Load parameters
    params = default_params.copy()
    if param_file:
        params.update(load_parameters(param_file))
    
    # Read data file
    data = pd.read_csv(input_file)
    # Remove the last row if it contains garbage data
    data = data[:-1]

    # Convert timestamps to seconds
    data['timestamp'] = data['timestamp'] / 1e6  # Convert from usec to sec
    
    # Select the axis and apply inversion if specified
    axis = params['AXIS']
    if params['INVERT_AXIS']:
        data[axis] = -1.0 * data[axis]

    # Convert radians to degrees if specified
    if params.get('CONVERT_TO_DEGREE', False):
        data[axis] = np.degrees(data[axis])

    # Filter data starting from the evaluation start time
    evaluation_start_time = params['EVALUATION_START_TIME']
    data = data[data['timestamp'] >= evaluation_start_time].copy()
    # Adjust timestamps to start from the evaluation start time
    data['timestamp'] -= evaluation_start_time
    
    # Check if sufficient data points are available
    if len(data) < params['NUM_LAST_POINTS']:
        print("Insufficient data points.")
        return

    # Calculate steady-state value (average of the last NUM_LAST_POINTS values)
    steady_state_data = data[axis][-params['NUM_LAST_POINTS']:]
    c = steady_state_data.mean()
    variance = steady_state_data.var()
    
    # Exit if variance is too high
    if variance > params['VARIANCE_THRESHOLD']:
        print("Variance is too high. Steady state value is unstable.")
        return
    
    # Check if the value is increasing or decreasing
    is_increase = True
    steady_value = c
    target_value = params['TARGET_VALUE']
    if data[axis].iloc[0] > c:
        # Decreasing case
        is_increase = False
        initial_value = data[axis].iloc[0]
        c = initial_value - steady_value
        data[axis] = initial_value - data[axis]
        target_value = 0
    
    # Now process as increasing case
    rise_time_start = data[data[axis] >= c * params['RISE_TIME_10_PERCENT']]['timestamp'].min()
    rise_time_end = data[data[axis] >= c * params['RISE_TIME_90_PERCENT']]['timestamp'].min()
    delay_time = data[data[axis] >= c * params['DELAY_TIME_PERCENT']]['timestamp'].min()
    max_value = data[axis].max()
    
    T_r = rise_time_end - rise_time_start
    T_d = delay_time
    O_s = max_value - c
    
    # Settling time T_s
    settled = np.abs(data[axis] - c) <= np.abs(c * params['SETTLING_TIME_PERCENT'])
    T_s = None
    for i in range(len(settled)):
        if settled.iloc[i]:
            others_settled = np.all(settled.iloc[i:])
            if others_settled:
                T_s = data['timestamp'].iloc[i]
                break

    if T_s is None:
        T_s = 10000.0
    
    # Determine results
    cs_result = "OK" if np.abs(c - params['TARGET_VALUE']) <= params['TARGET_VALUE'] * params['TARGET_CV'] else "NG"
    tr_result = "OK" if T_r <= params['TARGET_TR'] else "NG"
    td_result = "OK" if T_d <= params['TARGET_TD'] else "NG"
    os_result = "OK" if O_s <= params['TARGET_OS'] else "NG"
    ts_result = "OK" if T_s is not None and T_s <= params['TARGET_TS'] else "NG"

    # Output results
    print(f"{cs_result} c(Steady state value)  : {steady_value:.3f}   (Target: {target_value}±{params['TARGET_VALUE'] * params['TARGET_CV']:.3f} m)")
    print(f"{tr_result} T_r(Rise time)         : {T_r:.3f} s (Target: ≤ {params['TARGET_TR']:.3f} s)")
    print(f"{td_result} T_d(Delay time)        : {T_d:.3f} s (Target: ≤ {params['TARGET_TD']:.3f} s)")
    print(f"{os_result} O_s(Maximum overshoot) : {O_s:.3f}   (Target: ≤ {params['TARGET_OS']:.3f} m)")
    print(f"{ts_result} T_s(5% settling time)  : {T_s:.3f} s (Target: ≤ {params['TARGET_TS']:.3f} s)")

if __name__ == "__main__":
    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print("Usage: python script.py <input_file> [param_file]")
    else:
        input_file = sys.argv[1]
        param_file = sys.argv[2] if len(sys.argv) == 3 else None
        main(input_file, param_file)
