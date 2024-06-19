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

TARGET_TR = 10.0
TARGET_TD = 5.0
TARGET_OS = 1.0
TARGET_TS = 20.0
TARGET_HEIGHT = 10.0
TARGET_CV = 0.01


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
    #print(f"rise_time_start: {rise_time_start}")
    #print(f"rise_time_end  : {rise_time_end}")

    T_r = rise_time_end - rise_time_start
    
    # Delay time T_d
    delay_time = data[data['Z'] >= c * DELAY_TIME_PERCENT]['timestamp'].min()
    T_d = delay_time
    
    # Maximum overshoot O_s
    max_value = data['Z'].max()
    #print(f"max_value     : {max_value}")
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
    
    # Determine results
    cs_result = "OK" if np.abs(c - TARGET_HEIGHT) <= TARGET_HEIGHT * TARGET_CV else "NG"
    tr_result = "OK" if T_r <= TARGET_TR else "NG"
    td_result = "OK" if T_d <= TARGET_TD else "NG"
    os_result = "OK" if O_s <= TARGET_OS else "NG"
    ts_result = "OK" if T_s is not None and T_s <= TARGET_TS else "NG"
    if T_s is None:
        T_s = 10000.0

    # Output results
    print(f"{cs_result} c(Steady state value)  : {c:.3f} m (Target: {TARGET_HEIGHT}±{TARGET_HEIGHT * TARGET_CV:.3f} m)")
    print(f"{tr_result} T_r(Rise time)         : {T_r:.3f} s (Target: ≤ {TARGET_TR:.3f} s)")
    print(f"{td_result} T_d(Delay time)        : {T_d:.3f} s (Target: ≤ {TARGET_TD:.3f} s)")
    print(f"{os_result} O_s(Maximum overshoot) : {O_s:.3f} m (Target: ≤ {TARGET_OS:.3f} m)")
    print(f"{ts_result} T_s(5% settling time)  : {T_s:.3f} s (Target: ≤ {TARGET_TS:.3f} s)")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <input_file>")
    else:
        input_file = sys.argv[1]
        main(input_file)
