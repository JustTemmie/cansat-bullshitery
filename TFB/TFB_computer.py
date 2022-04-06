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
    
    ser = serial.Serial("/dev/ttyUSB0", 9600)
    time.sleep(0.4)
    print(ser.name)
    
    ser.flushInput()

    while True:
        time.sleep(0.2)
        ser_bytes = ser.readline()
        decoded_bytes = (ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
        print(decoded_bytes)
        
        #n += 1
        #time.sleep(0.05)
        
        ser.write(funny.encode())
        #print("sent " + str(funny))
        #n = 0
        
        
        #try:   
        #    ser.write(data.encode()) # encode is a function that takes in a string and returns a byte string that you can use to send data to the arduino
        #except:
        #    ser.close() 
        #    print("Error") 