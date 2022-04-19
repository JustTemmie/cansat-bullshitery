import os

with open("input.txt", "r") as input:
    with open("temp.txt", "w") as output:
        # iterate all lines from file
        for line in input:
            # if text matches then don't write it
            if "TGB" not in line.strip("\n"):
                output.write(line)

# replace file with original name
os.replace('temp.txt', 'output.txt')