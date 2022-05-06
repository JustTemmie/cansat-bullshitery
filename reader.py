import serial
from serial import Serial
import time

device = "/dev/ttyUSB1"

ser = serial.Serial(device, 9600)

while True:
    strin = ser.readline()
    print(strin)
    
    
    
    time.sleep(0.2)