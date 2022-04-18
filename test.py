def write(array, x, y, character):
    array[y][x] = character
    return array


if __name__ == '__main__': 
    rows = 15
    cols = 15

    arr = [[0 for i in range(rows)] for j in range(cols)]
    
    for y in range(0, rows):
        for x in range(0, cols):
            if (y == 0 or y == rows - 1) and (x == 0 or x == cols - 1):
                write(arr, x, y, "x")
            elif y == 0 or y == rows - 1:
                write(arr, x, y, "--")
            elif x == 0 or x == cols - 1:
                write(arr, x, y, "|")
            else:
                write(arr, x, y, " ")
                
    
    for a in range(0, rows):
        row = ""
        for i in arr[a]:
            row += str(i)
            if i == " ": 
                row += " "
        print(row)