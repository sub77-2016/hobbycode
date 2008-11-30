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

import pygame
import math
from math import exp, pow, sqrt

class RBF(pygame.sprite.Sprite):
    def __init__(self, initialPos):
        pygame.sprite.Sprite.__init__(self)
        self.image = pygame.Surface([10, 10])
        self.image.fill([0, 255, 0])
        self.rect = self.image.get_rect()
        self.rect.topleft = initialPos
        self.pos = (0.0, 0.0)
        self.pos = (initialPos[0], initialPos[1])
        self.next_update_time = 0

    def update(self, current_time, mousePos):
        if self.next_update_time < current_time:
            ##self.linear(mousePos)
            self.gaussian(mousePos)
            ##self.hybridLinearGaussian(mousePos)
            ##self.shiftedGaussian(mousePos)
            
            self.rect.topleft = self.pos

            # Update every 100 ms (10 hz)
            self.next_update_time = current_time + 10

    # Move in proportion to the linear distance.
    def linear(self, mousePos):
        learningRate = 0.01
        
        distanceX = mousePos[0] - self.pos[0]
        distanceY = mousePos[1] - self.pos[1]
        
        if (math.sqrt(distanceX * distanceX + distanceY * distanceY) < 80):
            self.pos = (self.pos[0] + learningRate * distanceX, 
                self.pos[1] + learningRate * distanceY)

    # Move based on a Gaussian activation function.
    def gaussian(self, mousePos):
        learningRate = 1.0
        stdDev = 50
        
        distanceX = mousePos[0] - self.pos[0]
        distanceY = mousePos[1] - self.pos[1]
        distanceSquared = distanceX * distanceX + distanceY * distanceY
        gaussianActivation = math.exp(-distanceSquared / (2 * stdDev * stdDev))
        
        # Normalize the direction vector.
        directionX = 0.0
        directionY = 0.0
        length = math.sqrt(distanceSquared)
        if (length > 0):
            directionX = distanceX / length
            directionY = distanceY / length

        influenceX = gaussianActivation * directionX
        influenceY = gaussianActivation * directionY

        self.pos = (self.pos[0] + learningRate * influenceX, 
            self.pos[1] + learningRate * influenceY)

    # Move based on a Gaussian activation function and linear distance.
    def hybridLinearGaussian(self, mousePos):
        learningRate = 0.05
        stdDev = 50
        
##        # Alternate method
##        distanceX = mousePos[0] - self.pos[0]
##        distanceY = mousePos[1] - self.pos[1]
##        distanceSquared = distanceX * distanceX + distanceY * distanceY
##        gaussianActivation = math.exp(-distanceSquared / (2 * stdDev * stdDev))
##        influenceX = gaussianActivation * distanceX
##        influenceY = gaussianActivation * distanceY
##
##        self.pos = (self.pos[0] + learningRate * influenceX, 
##            self.pos[1] + learningRate * influenceY)

        directionX = mousePos[0] - self.pos[0]
        directionY = mousePos[1] - self.pos[1]
        distanceSquared = directionX * directionX + directionY * directionY
        #distanceSquared -= 500
        
        # Normalize the direction vector.
        distance = 0.0
        if (distanceSquared > 0):
            distance = math.sqrt(distanceSquared)
            directionX = directionX / distance
            directionY = directionY / distance

        gaussianActivation = math.exp(-distanceSquared / (2 * stdDev * stdDev))
        
        influenceX = gaussianActivation * distance * directionX
        influenceY = gaussianActivation * distance * directionY
        
        self.pos = (self.pos[0] + learningRate * influenceX, 
            self.pos[1] + learningRate * influenceY)

    # Move based on a Gaussian activation function.
    def shiftedGaussian(self, mousePos):
        learningRate = 1.0
        stdDev = 20

        directionX = mousePos[0] - self.pos[0]
        directionY = mousePos[1] - self.pos[1]
        distanceSquared = directionX * directionX + directionY * directionY
        #distanceSquared -= 500
        
        # Normalize the direction vector.
        distance = 0.0
        if (distanceSquared > 0):
            distance = math.sqrt(distanceSquared)
            directionX = directionX / distance
            directionY = directionY / distance

        gaussianActivation = math.exp(-(distance - 5 * stdDev)**2 / (2 * stdDev**2))
        
        influenceX = gaussianActivation * directionX
        influenceY = gaussianActivation * directionY
        
        self.pos = (self.pos[0] + learningRate * influenceX, 
            self.pos[1] + learningRate * influenceY)
            