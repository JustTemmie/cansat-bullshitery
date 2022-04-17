import serial
from serial import Serial

import time
import random

print_axis = True
debug_info = False
chonk_mode = True

rows = 15
cols = 15

score_TFB = 0
score_TSB = 0

arr = [[" " for i in range(rows)] for j in range(cols)]

def main(): 
    pass 

def write(x, y, character):
    arr[round(y)][round(x)] = character


def clear_board():
    for y in range(0, rows):
        for x in range(0, cols):
            if (y == 0 or y == rows - 1) and (x == 0 or x == cols - 1):
                write(x, y, "x")
            elif y == 0 or y == rows - 1:
                write(x, y, "--")
            elif x == 0 or x == cols - 1:
                write(x, y, "|")
            else:
                write(x, y, " ")


def write_paddles(TFB, TSB):
    for y in range(0, rows):
        if y == TFB:
            for u in range(y - 2, y + 3):
                if u != 0 and u != rows - 1:
                    write(3, u, "I")
            
        if y == TSB:
            for u in range(y - 2, y + 3):
                if u != 0 and u != rows - 1:
                    write(11, u, "I")
            

def add_visual_data(row, a):
    for i in arr[a]:
        row += str(i)
        if chonk_mode: row += str(i)
        if i != "x" and i != "--" and i != "|": 
            row += " "
            if chonk_mode: row += " "
    return row
    
def print_game():
    for a in range(0, rows):
        row = ""
        row += add_visual_data(row, a)

                             
        if print_axis: print(row, a)
        else: print(row)
        
        if chonk_mode:
            if print_axis: print(row, a)
            else: print(row)

    if print_axis:
        if chonk_mode:
            print("0   1  2   3  4   5   6   7   8   9   10  11  12  13  14")
        else:
            print("0  2,  4  6,  8  10 , 12  14")
        
        print("(the x axis is only semi-accurate)")
    

if __name__ == '__main__': 
    clear_board()

    n = 0
    pos = 7
    funny = "1"
    
    ser = serial.Serial("/dev/ttyACM0", 9600)
    time.sleep(0.4)
    print(ser.name)
    
    ser.flushInput()
    

    
    while True:
        clear_board()
        time.sleep(0.1)
        ser_bytes = ser.readline()
        decoded_bytes = (ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
        array = decoded_bytes.split(",")
        
        
    
        try:
            write(float(array[1]), float(array[2]), "O") # ball x and y
            write_paddles(round(float(array[4])), round(float(array[5]))) # TFB and TSB
        except:
            pass
            #print("error in trying to update the board")
        
        try:
            if round(float(array[2])) < pos and pos > 3: # if y is less than the paddle position
                pos -= 1
            elif round(float(array[2])) > pos and pos < rows - 4: # if y is greater than the paddle position
                pos += 1
            else:
                pos += 0
        except:
            print("could not understand the input")
        

        try:
            if round(float(array[1])) == 1:
                score_TSB += 1
            elif round(float(array[1])) == cols-2:
                score_TFB += 1
        
        except:
            pass

       
        #if pos < 13:
        #    pos += 1
        #else:
        #    pos = 2
        
        if debug_info:
            print(array)
            try:
                print(f"desired position = {pos}, current position = {array[5]}")
                #ser.write(str(pos).encode())
                pass
            except:
                pass
        
        try:
            ser.write(str(pos).encode())
        except:
            print("failed to write to TFB")
        
        #time.sleep(0.5)
        print(f"TFB score = {score_TFB} and TSB score = {score_TSB}")
        print_game()
        #print("sent " + str(funny))
        #n = 0
        
        
        #try:   
        #    ser.write(data.encode()) # encode is a function that takes in a string and returns a byte string that you can use to send data to the arduino
        #except:
        #    ser.close() 
        #    print("Error") 