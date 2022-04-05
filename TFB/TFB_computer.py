import serial
from serial import Serial
ser = serial.Serial("/dev/ttyACM0")  # open serial port
print(ser.name)         # check which port was really used
stringy = input("Enter a string: ")
print(stringy)

try:
    ser.write(stringy.encode())
except:
    ser.close()             # close port
