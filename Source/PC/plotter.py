import serial
import re
import time
import matplotlib.pyplot as plt
from collections import deque

# -----------------------------
# Serial configuration
# -----------------------------
PORT = "/dev/ttyACM0"
BAUD = 19200

ser = serial.Serial(PORT, BAUD, timeout=0.1)

# Give Arduino time to reset after opening serial
time.sleep(2)

# -----------------------------
# Sensor state
# -----------------------------
sensor_values = {
    1: 0,
    2: 0,
    3: 0
}

# Store history for plotting
history_length = 100

times = deque(maxlen=history_length)
sensor_history = {
    1: deque(maxlen=history_length),
    2: deque(maxlen=history_length),
    3: deque(maxlen=history_length)
}

start_time = time.time()

# -----------------------------
# Plot setup
# -----------------------------
plt.ion()

fig, ax = plt.subplots()

lines = {}

for sensor in sensor_values:
    lines[sensor], = ax.plot([], [], label=f"Sensor {sensor}")

ax.set_xlabel("Time (s)")
ax.set_ylabel("Sensor value")
ax.set_title("Live CAN Sensor Data")
ax.legend()
ax.grid(True)

# -----------------------------
# Main loop
# -----------------------------
try:

    while True:

        # Read any serial messages currently available
        while ser.in_waiting:

            line = ser.readline().decode("utf-8", errors="ignore").strip()

            if not line:
                continue

            print(line)

            # Look for:
            # Sensor: 1
            sensor_match = re.search(r"Sensor:\s*(\d+)", line)

            if sensor_match:
                current_sensor = int(sensor_match.group(1))

                # Read the next line, which should be:
                # Value: 123
                value_line = ser.readline().decode(
                    "utf-8", errors="ignore"
                ).strip()

                print(value_line)

                value_match = re.search(r"Value:\s*(\d+)", value_line)

                if value_match:
                    value = int(value_match.group(1))

                    if current_sensor in sensor_values:
                        sensor_values[current_sensor] = value

        # Current elapsed time
        current_time = time.time() - start_time

        # IMPORTANT:
        # Every plotting interval gets the CURRENT value.
        #
        # If no new CAN message arrived, sensor_values[]
        # still contains the previous value.
        times.append(current_time)

        for sensor in sensor_values:
            sensor_history[sensor].append(sensor_values[sensor])

            lines[sensor].set_data(
                times,
                sensor_history[sensor]
            )

        # Keep x-axis moving
        if len(times) > 1:
            ax.set_xlim(
                max(0, current_time - 30),
                current_time
            )

        ax.set_ylim(0, 1023)

        fig.canvas.draw()
        fig.canvas.flush_events()

        # Plot refresh rate
        time.sleep(0.1)

except KeyboardInterrupt:
    print("\nStopping...")

finally:
    ser.close()
    plt.ioff()
    plt.show()