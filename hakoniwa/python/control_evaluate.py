import pandas as pd
import numpy as np
import sys

# Parameter settings
NUM_LAST_POINTS = 1000
VARIANCE_THRESHOLD = 1.0  # Variance threshold
RISE_TIME_10_PERCENT = 0.1
RISE_TIME_90_PERCENT = 0.9
DELAY_TIME_PERCENT = 0.5
SETTLING_TIME_PERCENT = 0.05

def main(input_file):
    # Read data file
    data = pd.read_csv(input_file)
    # Remove the last row if it contains garbage data
    data = data[:-1]

    # Convert timestamps to seconds
    data['timestamp'] = data['timestamp'] / 1e6  # Convert from usec to sec
    data['Z'] = -1.0 * data['Z']
    
    # Calculate steady-state value (average of the last NUM_LAST_POINTS Z values)
    if len(data) < NUM_LAST_POINTS:
        print("Insufficient data points.")
        return
    
    steady_state_data = data['Z'][-NUM_LAST_POINTS:]
    c = steady_state_data.mean()
    variance = steady_state_data.var()
    
    # Exit if variance is too high
    if variance > VARIANCE_THRESHOLD:
        print("Variance is too high. Steady state value is unstable.")
        return
    
    # Rise time T_r
    rise_time_start = data[data['Z'] >= c * RISE_TIME_10_PERCENT]['timestamp'].min()
    rise_time_end = data[data['Z'] >= c * RISE_TIME_90_PERCENT]['timestamp'].min()
    print(f"rise_time_start: {rise_time_start}")
    print(f"rise_time_end  : {rise_time_end}")

    T_r = rise_time_end - rise_time_start
    
    # Delay time T_d
    delay_time = data[data['Z'] >= c * DELAY_TIME_PERCENT]['timestamp'].min()
    T_d = delay_time
    
    # Maximum overshoot O_s
    max_value = data['Z'].max()
    print(f"max_value     : {max_value}")
    O_s = max_value - c
    
    # Settling time T_s
    settled = np.abs(data['Z'] - c) <= np.abs(c * SETTLING_TIME_PERCENT)
    T_s = None
    for i in range(len(settled)):
        if settled[i]:
            others_settled = np.all(settled[i:])
            if others_settled:
                T_s = data['timestamp'].iloc[i]
                break
    
    # Output results
    print(f"Steady state value c: {c:.3f} m")
    print(f"Rise time T_r: {T_r:.3f} seconds")
    print(f"Delay time T_d: {T_d:.3f} seconds")
    print(f"Maximum overshoot O_s: {O_s:.3f} m")
    print(f"5% settling time T_s: {T_s:.3f} seconds")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <input_file>")
    else:
        input_file = sys.argv[1]
        main(input_file)
