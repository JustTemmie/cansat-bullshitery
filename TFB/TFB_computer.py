import serial
from serial import Serial

def main(): # declares a function with the name "main" this is because main is a really cool function name and it's objetively good to have a main function because it's the first thing that gets run when you run your program, but that's not what i'm gonna do so eh
    pass # pass is a keyword that tells python to ignore the rest of the code in the function, so it doesn't run it

def print_game(input): #the print_game function is a function that takes in a string and prints the game state to the screen in a pretty way so that the user(s) can see what's going on
    print(input) # this is a print statement that prints the input string to the screen
    


if __name__ == '__main__':
    #setup serial port for communication with the arduino
    ser = serial.Serial("/dev/ttyACM0")  # open serial port
    print(ser.name) # double check to make sure that the code is using the correct port or not

    while True:
        data = input("Enter a string: ") #input is a function that takes in a string and returns a string that you can use to get input from the user and it's a good way to get input from the user
        print(data)

        try: # try is a keyword that tells python to try to run the code in the try block, if it fails it will run the code in the except block instead
            ser.write(data.encode())
        except: # this is the except block, it will run if the code in the try block fails to run correctly - it's a good way to handle errors in your code
            ser.close() # close port