import serial
from serial import Serial

import time

def main(): 
    pass 

def print_game(input): 
    print(input)
    

if __name__ == '__main__': 
    n = 0
    funny = "1"
    
    input = serial.Serial("/dev/ttyUSB1", 9600)
    output = serial.Serial("/dev/ttyACM0", 9600)
    time.sleep(0.4)
    print(input.name)
    print(output.name)
        
    input.flushInput()
    output.flushOutput

    while True:
        time.sleep(0.2)
        ser_bytes = input.readline()
        #decoded_bytes = (ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
        print(ser_bytes)
        
        #n += 1
        #time.sleep(0.05)
        
        #output.write(funny.encode())
        #print("sent " + str(funny))