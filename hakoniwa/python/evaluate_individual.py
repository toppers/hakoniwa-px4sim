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

def calculate_fitness(results):
    fitness_value = 0.0
    if not results:
        return fitness_value

    # Steady state
    target_steady_state = 10.0
    tolerance_steady_state = 0.1
    error_steady_state = min(abs(results['steady_state'] - target_steady_state), 1)
    fitness_value += max(0, 1.0 - (error_steady_state / tolerance_steady_state))

    # Rise time
    tolerance_rise_time = 10.0
    error_rise_time = min(abs(results['rise_time'] - 0.0), 1)  # 0に近いほど良い
    fitness_value += max(0, 1.0 - (error_rise_time / tolerance_rise_time))

    # Delay time
    tolerance_delay_time = 5.0
    error_delay_time = min(abs(results['delay_time'] - 0.0),1)  # 0に近いほど良い
    fitness_value += max(0, 1.0 - (error_delay_time / tolerance_delay_time))

    # Overshoot
    target_overshoot = 1.0
    if (results['overshoot'] > target_overshoot):
        pass
    elif (results['overshoot'] >= 0) and (results['overshoot'] < target_overshoot):
        fitness_value += 10.0 * (2.0 - results['overshoot'])

    # Settling time
    tolerance_settling_time = 20.0
    error_settling_time = min(abs(results['settling_time'] - 0.0), 1)  # 0に近いほど良い
    fitness_value += max(0, 1.0 - (error_settling_time / tolerance_settling_time))

    return fitness_value


def main(input_file, output_file):
    try:
        # Read data file
        data = pd.read_csv(input_file)
        data = data[:-1]

        data['timestamp'] = data['timestamp'] / 1e6  # Convert from usec to sec
        data['Z'] = -1.0 * data['Z']

        if len(data) < NUM_LAST_POINTS:
            fitness_value = 0.0
            with open(output_file, 'a') as f:
                f.write(f"{fitness_value}\n")
            return
        
        steady_state_data = data['Z'][-NUM_LAST_POINTS:]
        c = steady_state_data.mean()
        variance = steady_state_data.var()
        
        if variance > VARIANCE_THRESHOLD:
            fitness_value = 0.0
            with open(output_file, 'a') as f:
                f.write(f"{fitness_value}\n")
            return
        
        rise_time_start = data[data['Z'] >= c * RISE_TIME_10_PERCENT]['timestamp'].min()
        rise_time_end = data[data['Z'] >= c * RISE_TIME_90_PERCENT]['timestamp'].min()
        T_r = rise_time_end - rise_time_start
        
        delay_time = data[data['Z'] >= c * DELAY_TIME_PERCENT]['timestamp'].min()
        T_d = delay_time
        
        max_value = data['Z'].max()
        O_s = max_value - c
        
        settled = np.abs(data['Z'] - c) <= np.abs(c * SETTLING_TIME_PERCENT)
        T_s = None
        for i in range(len(settled)):
            if settled[i]:
                others_settled = np.all(settled[i:])
                if others_settled:
                    T_s = data['timestamp'].iloc[i]
                    break
        
        results = {
            'steady_state': c,
            'steady_state_ok': np.abs(c - TARGET_HEIGHT) <= TARGET_HEIGHT * TARGET_CV,
            'rise_time': T_r,
            'rise_time_ok': T_r <= TARGET_TR,
            'delay_time': T_d,
            'delay_time_ok': T_d <= TARGET_TD,
            'overshoot': O_s,
            'overshoot_ok': O_s <= TARGET_OS,
            'settling_time': T_s,
            'settling_time_ok': T_s is not None and T_s <= TARGET_TS
        }

        fitness_value = calculate_fitness(results)

        with open(output_file, 'a') as f:
            f.write(f"{fitness_value}\n")
    except Exception as e:
        # Log the exception and write 0.0 as fitness value
        print(f"Error processing {input_file}: {e}")
        with open(output_file, 'a') as f:
            f.write("0.0\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python evaluate_individual.py <input_file> <output_file>")
    else:
        input_file = sys.argv[1]
        output_file = sys.argv[2]
        main(input_file, output_file)
