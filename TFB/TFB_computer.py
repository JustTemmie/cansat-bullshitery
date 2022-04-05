import serial
from serial import Serial

if __name__ == '__main__':
    ser = serial.Serial("/dev/ttyACM0")  # open serial port
    print(ser.name) # double check to make sure that the code is using the correct port or not

    while True:
        data = input("Enter a string: ")
        print(data)

        try:
            ser.write(data.encode())
        except:
            ser.close() # close port


def main():
    pass


def print_board(input):
    print(input)