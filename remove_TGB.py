import os


# open input.txt and remove any lines including "TGB"
# and remove spaces from every lines including "TFB"
# this is done so that the software we use to analyze data gets really fucky whenever a space appears
with open("input.txt", "r") as input:
    with open("temp.txt", "w") as output:
        # iterate all lines from file
        for line in input:
            #if "TGB" not in line.strip("\n"):
                if "TGB" in line.strip("\n"):
                    write_string = ""
                    for i, v in enumerate(line):
                        if v != " ":
                            write_string += v
                            
                    output.write(write_string)
                
                #else:
                #    output.write(line)

# replace file with original name
os.replace('temp.txt', 'output.txt')