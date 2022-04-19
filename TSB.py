import serial
from serial import Serial

import time
from datetime import datetime

print_axis = False
debug_info = False
chonk_mode = False #chonk mode is made mostly as a joke, it just doubles the side of everything in both dimensions
print_map = True

device = "/dev/ttyACM1"
input_device = "/dev/ttyUSB0"
use_device_as_input = True # must be set to false for wireless
ser_rate = 9600

ball = "😳"
topbottom = "--"
sides = "|"
corner = "x"
paddles = "I"
skip_ball_offset = True # some balls like the 😳 emoji for example, are actually twice the width of a normal character, if the ball you're using is two wide turn this to True

rows = 15
cols = 15
paddle_chonk = 2 # having a paddle chonk of 2 would mean the paddle is 5 tall, the main point with two over and two under

score_TFB = 0
score_TSB = 0

#declare a 2D array, this is used instead of a normal 1D array as the game is 2D and it's waaay easier
arr = [[" " for i in range(rows)] for j in range(cols)]

def main(): 
    pass 


#the write function only exists because the code "arr[2][6]" would write to x 6 and y 2, this function just flips them around
def write(x, y, character):
    arr[round(y)][round(x)] = character


def clear_board():
    #loops thru the array and sets up the board
    for y in range(0, rows):
        for x in range(0, cols):
            
            #checks where the array index is on the board and writes to it accordingly
            if (y == 0 or y == rows - 1) and (x == 0 or x == cols - 1):
                write(x, y, corner)
            elif y == 0 or y == rows - 1:
                write(x, y, topbottom)
            elif x == 0 or x == cols - 1:
                write(x, y, sides)
            else:
                write(x, y, " ")


def write_ball(x, y):
    write(x, y, ball)

def write_paddles(TFB, TSB):
    #write the paddles to the board using the balls Y coordinate
    for y in range(0, rows):
        if y == TFB:
            for u in range(y - paddle_chonk, y + paddle_chonk+1):
                if u != 0 and u != rows - 1:
                    write(3, u, paddles)
            
        if y == TSB:
            for u in range(y - paddle_chonk, y + paddle_chonk+1):
                if u != 0 and u != rows - 1:
                    write(cols-4, u, paddles)
            

def add_visual_data(row, a):
    for i in arr[a]:
        row += str(i)
        if chonk_mode: row += str(i) # doubles the width if chonk mode is on
        
        #adds a space to the end of I, such that the board is almost rectangular, as opposed to a tall and slim square
        if i != corner and i != sides and i != topbottom:
            if skip_ball_offset:
                if i != ball:
                    row += " "
                    if chonk_mode: row += " "
            
            else:
                row += " "
                if chonk_mode: row += " "
            
    return row
    
def print_game():
    #loops thru every row and prints the content to the terminal
    for a in range(0, rows):
        row = ""
        row += add_visual_data(row, a)

                             
        if print_axis: print(row, a)
        else: print(row)
        
        if chonk_mode:
            if print_axis: print(row, a)
            else: print(row)


    #displays the axis after the board is finished printing
    if print_axis:
        if chonk_mode:
            print("0   1  2   3  4   5   6   7   8   9   10  11  12  13  14")
        else:
            print("0  2,  4  6,  8  10 , 12  14")
        
        print("(the x axis is only semi-accurate)")
    

def get_input(device):
    ser_bytes = device.readline()
    
    if debug_info:
        print(ser_bytes)
    
    #decodes the input into a string and removes some garbage
    try:
        decoded_bytes = (ser_bytes[0:len(ser_bytes)-2].decode("utf-8"))
        
        try:
            # Open a file with access mode 'a'
            file_object = open('cansat_output.txt', 'a')
            # Append 'hello' at the end of file
            file_object.write(f"{decoded_bytes} \n")
            # Close the file
            file_object.close()

        except Exception as e:
            print(f"error in writing to output.txt : {e}")

    except:
        return get_input(device)


    #splits the input at every point where it sees a comma
    #this is done so that the script can very easily use the incoming data
    #for example i can just ask what array[1] is and it will return the X position for the ball
    array = decoded_bytes.split(",")
    
    if array[0] == "TGB": 
        if array != None:
            if debug_info:
                print(f"sucess!, {array}")
            return array

    return get_input(device)
    
    
    
if __name__ == '__main__': 
    clear_board()

    #declare variables that will be used in the future
    n = 0
    pos = int((rows-1)/2)
    funny = "1"
    
    #initialize serial
    ser = serial.Serial(device, ser_rate)
    if use_device_as_input:
        ser_input = serial.Serial(device, ser_rate)
    else:
        ser_input = serial.Serial(input_device, ser_rate)
    time.sleep(0.4)
    print(ser.name)
    
    # Open a file with access mode 'a'
    file_object = open('cansat_output.txt', 'a')
    # Append 'hello' at the end of file
    file_object.write(f"\n\n\n ----------------------------------- \n python script started at \n {datetime.now()} \n\n\n")
    # Close the file
    file_object.close()
            
    
    #flush the serial port to clear any garbage
    ser.flushInput()
    

    
    while True:        
        
        clear_board()
        time.sleep(0.06)
        
        
        if debug_info:
            print("trying to get input")
        array = get_input(ser_input)
        
        if debug_info:
            print(array)
        
        time.sleep(0.04)
        
        
        try:
            #print(array[2])
            write_ball(float(array[1]), float(array[2]))
            write_paddles(round(float(array[5])), round(float(array[4]))) # TSB and TFB
        except:
            print("error in trying to update the board")
            #pass
        
        try:
            #checks where the ball is relative to the player's paddle
            #also double checks that it's not moving the paddle out of bounds
            if round(float(array[2])) < pos and pos > paddle_chonk+1:
                pos -= 1
                #pos = round(float(array[2]))
            elif round(float(array[2])) > pos and pos < rows - paddle_chonk - 2:
                pos += 1
                #pos = round(float(array[2]))
            else:
                pos += 0
            
        except:
            print(f"could not understand the input, array = {array}")
        

        try:
            if round(float(array[1])) == 1:
                score_TSB += 1
            elif round(float(array[1])) == cols-2:
                score_TFB += 1
        
        except:
            pass


        
        #prints debuf info meaning the array, and the position of the paddle
        if debug_info:
            try:
                print(f"desired position = {pos}, current position = {array[4]}")
                #ser.write(str(pos).encode())
                pass
            except:
                pass
        
        output = f"TSB, {pos}"
        print(output)
        try:
            ser.write(output.encode())
        except:
            print("failed whilst writing to TDB")
        


        #prints the score and the game board
        
        if print_map:
            print(f"TFB score = {score_TFB} and TSB score = {score_TSB}")
            print_game()
        else:
            print("-------------------------------------------------------------------------")
