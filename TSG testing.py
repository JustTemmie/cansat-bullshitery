import serial
from serial import Serial

import time
from datetime import datetime

device = "/dev/ttyACM1"
ser_rate = 9600

n = 0


if __name__ == '__main__': 
    
    #initialize serial
    ser = serial.Serial(device, ser_rate)
    
    while True:
        var = datetime.now().strftime("%H:%M.%S.%f")
        ser.write(f"TSB, 9, {var}".encode())
        print(var)
        
        time.sleep(0.3)
        