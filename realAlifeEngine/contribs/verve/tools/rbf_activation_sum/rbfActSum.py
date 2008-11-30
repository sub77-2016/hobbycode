#########################################################################
# Verve                                                                 #
# http://verve-agents.sourceforge.net                                   #
# Copyright (C) 2004-2008                                               #
# Tyler Streeter (http://www.tylerstreeter.net)                         #
#                                                                       #
# This library is free software; you can redistribute it and/or         #
# modify it under the terms of EITHER:                                  #
#   (1) The GNU Lesser General Public License as published by the Free  #
#       Software Foundation; either version 2.1 of the License, or (at  #
#       your option) any later version. The text of the GNU Lesser      #
#       General Public License is included with this library in the     #
#       file license-LGPL.txt.                                          #
#   (2) The BSD-style license that is included with this library in     #
#       the file license-BSD.txt.                                       #
#                                                                       #
# This library is distributed in the hope that it will be useful,       #
# but WITHOUT ANY WARRANTY; without even the implied warranty of        #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    #
# license-LGPL.txt and license-BSD.txt for more details.                #
#########################################################################

import math
from math import exp, sqrt

def gauss(stdDevWidth, x):
    return exp(-pow(abs(x), 2) / (2 * stdDevWidth * stdDevWidth))

def computeActSum1D(stdDevWidth, res):
    xIncrement = 2.0 / res
    xPos = xIncrement
    sum = 1 # Start 1 for center point.
    actWidth = 4 * stdDevWidth
    if actWidth > 1:
        actWidth = 1 # Stay within [-1, 1].
    while xPos <= actWidth:
        sum += 2 * gauss(stdDevWidth, xPos)
        xPos += xIncrement
    return sum

def computeActSum2D(stdDevWidth, res):
    increment = 2.0 / (res - 1)
    xPos = -1
    yPos = -1
    sum = 0
    while xPos <= 1:
        yPos = -1
        while yPos <= 1:
            distance = sqrt(xPos * xPos + yPos * yPos)
            sum += gauss(stdDevWidth, distance)
            yPos += increment
        xPos += increment
    return sum

def computeActSum3D(stdDevWidth, res):
    increment = 2.0 / (res - 1)
    xPos = -1
    yPos = -1
    zPos = -1
    sum = 0
    while xPos <= 1:
        yPos = -1
        while yPos <= 1:
            zPos = -1
            while zPos <= 1:
                distance = sqrt(xPos * xPos + yPos * yPos + zPos * zPos)
                sum += gauss(stdDevWidth, distance)
                zPos += increment
            yPos += increment
        xPos += increment
    return sum

def computeActSum4D(stdDevWidth, res):
    increment = 2.0 / (res - 1)
    xPos = -1
    yPos = -1
    zPos = -1
    wPos = -1
    sum = 0
    while xPos <= 1:
        yPos = -1
        while yPos <= 1:
            zPos = -1
            while zPos <= 1:
                wPos = -1
                while wPos <= 1:
                    distance = sqrt(xPos * xPos + yPos * yPos + zPos * zPos + wPos * wPos)
                    sum += gauss(stdDevWidth, distance)
                    wPos += increment
                zPos += increment
            yPos += increment
        xPos += increment
    return sum

def printActSumData(maxRes):
    file = open('actSumData.dat', 'w')
    res = 2
    while res <= maxRes:
        file.write(str(res))
        file.write(' ')
        stdDevWidth = 2.0 / res
        file.write(str(computeActSum4D(stdDevWidth, res)))
        file.write('\n')
        res += 1
    file.close()
    