
import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
omega_times_series = pd.read_csv('omega_times_series.csv')

fig, ax1 = plt.subplots()

# Create a line plot for ACC
color = 'tab:blue'
ax1.set_xlabel('TIME')
ax1.set_ylabel('ACC', color=color)
ax1.plot(omega_times_series['TIME'], omega_times_series['ACC'], color=color)
ax1.tick_params(axis='y', labelcolor=color)

# Create a second y-axis for OMEGA
ax2 = ax1.twinx()
color = 'tab:red'
ax2.set_ylabel('OMEGA', color=color)
ax2.plot(omega_times_series['TIME'], omega_times_series['OMEGA'], color=color)
ax2.tick_params(axis='y', labelcolor=color)

fig.tight_layout()
plt.show()
