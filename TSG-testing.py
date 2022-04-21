import serial
from serial import Serial

import time
from datetime import datetime
import random

device = "/dev/ttyACM4"
ser_rate = 9600

n = 0


if __name__ == '__main__': 
    
    #initialize serial
    ser = serial.Serial(device, ser_rate)
    
    while True:
        #var = datetime.now().strftime("%H:%M.%S.%f")
        ser.write(f"TSB, {random.randint(0,13)}beaver".encode())
        
        time.sleep(1)
        