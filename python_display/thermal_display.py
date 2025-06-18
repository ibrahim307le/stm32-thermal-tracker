import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation

ser = serial.Serial('COM3', 115200, timeout=1)  # Change COM port

fig, ax = plt.subplots()
data = np.zeros((8, 8))
im = ax.imshow(data, cmap='inferno', interpolation='nearest')

def update(frame):
    line = ser.readline().decode().strip()
    try:
        temp_values = list(map(float, line.split(',')))
        if len(temp_values) == 64:
            im.set_data(np.array(temp_values).reshape(8, 8))
    except:
        pass
    return [im]

ani = animation.FuncAnimation(fig, update, interval=200, blit=True)
plt.title("Live AMG8833 Thermal View")
plt.colorbar(im)
plt.show()
