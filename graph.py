import os
import string

with open("./data/ping.txt", "r") as input:
    with open("temp.txt", "w") as output:
        # iterate all lines from file
        for line in input:
            write_string  = ""
            #if "TGB" not in line.strip("\n"):
            array = line.split(",")
            time1 = array[0]
            time2 = array[len(array)-1]
            groundtime = array[len(array)-2]
            airtime = array[len(array)-3]
            
            
            sep = "-"
            stripped2 = time2.split(sep, 1)[0]
            
            groundtime = groundtime[4:-3]
            
            #print(stripped1[-9:], stripped2[-9:])
            if airtime != "beav":
                airtime = airtime[:-3]
                write_string = f"{float(time1[-9:]) - float(stripped2[-9:])}, {groundtime}, {airtime}"
                
            else:
                write_string = f"{float(time1[-9:]) - float(stripped2[-9:])}, {groundtime}"
            output.write(f"{write_string} \n")
               
        # Close the file
        output.close()
        
os.replace('temp.txt', 'output.txt')

while False:
    # importing the required module
    import matplotlib.pyplot as plt
    
    # x axis values
    x = []
    # corresponding y axis values
    y = [2,4,1]
    
    # plotting the points
    plt.plot(x, y)
    
    # naming the x axis
    plt.xlabel('x - axis')
    # naming the y axis
    plt.ylabel('y - axis')
    
    # giving a title to my graph
    plt.title('My first graph!')
    
    # function to show the plot
    plt.show()