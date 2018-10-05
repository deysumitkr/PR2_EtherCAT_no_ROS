"""
From 2D matrix to matlab file or CSV
"""

import numpy as np

def generate_Mfile(matrix, withLineBreaks=True):
    matrix = list(np.array(matrix))

    # opening bracket
    out = "[ "
    out += "\\\n" if withLineBreaks else ""

    # each row can be used as is with the [] brackets
    for row in matrix:
        out += str(row) + "; "
        out += "\\\n" if withLineBreaks else ""

    # remove trailing ; (optional but makes it neat)
    if(out[-2:] == "; "):
        out = out[:-2]

    out += "" if withLineBreaks else " "

    # closing bracket
    out += "];"

    return out


def generate_CSV(matrix, firstLineLabels=""):
    matrix = list(np.array(matrix))
    out = ""
    for row in matrix:
        # start new line
        if(len(out)>0):
            out += "\n"

        # insert comma separated values
        for value in row:
            out += str(value) + ","

        # remove trailing comma
        if(out[-1] == ","):
            out = out[:-1]

    if len(firstLineLabels) == 0:
        return out # return if no labels are provided
    else:
        return firstLineLabels + "\n" + out # add labels to the first line


def create_Mfile(matrix, fileName, variableName="Motion_Mat", lineBreaks=True):
    out = variableName + " = " + generate_Mfile(matrix, lineBreaks)
    f = open(fileName, "w")
    f.write(out)
    f.close()

def create_CSVfile(matrix, fileName, labels=""):
    out = generate_CSV(matrix, labels)
    f = open(fileName, "w")
    f.write(out)
    f.close()


if __name__ == '__main__':
    a = [[1,2,3], [4,5,6], [7,8,9]]
    # a = []
    print generate_Mfile(a, True)
    # print generate_CSV(a, "x,y,theta")