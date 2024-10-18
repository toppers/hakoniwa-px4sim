
import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
omega_times_series = pd.read_csv('omega_times_series.csv')

fig, ax1 = plt.subplots()

# Create a line plot for ACC
ax1.set_xlabel('TIME')
ax1.set_ylabel('ACC', color='tab:blue')
ax1.plot(omega_times_series['TIME'], omega_times_series['ACC(a)'], color='tab:blue', label='acc(a)')
ax1.plot(omega_times_series['TIME'], omega_times_series['ACC(b)'], color='tab:green', label='acc(b)')
ax1.legend()
ax1.tick_params(axis='y', labelcolor='tab:blue')

# Create a second y-axis for OMEGA
ax2 = ax1.twinx()
ax2.set_ylabel('OMEGA', color='tab:red')
ax2.plot(omega_times_series['TIME'], omega_times_series['OMEGA(a)'], color='tab:red', label='omega(a)')
ax2.plot(omega_times_series['TIME'], omega_times_series['OMEGA(b)'], color='tab:pink', label='omega(b)')
ax2.legend()
ax2.tick_params(axis='y', labelcolor='tab:red')

fig.tight_layout()
plt.show()
