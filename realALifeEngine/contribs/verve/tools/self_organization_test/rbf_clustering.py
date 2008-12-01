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
from pygame.locals import *
from RBF import RBF

pygame.init()

RBFs = []
for x in range(0, 30):
    for y in range(0, 30):
        RBFs.append(RBF((20 * x, 20 * y)))

screen = pygame.display.set_mode([600, 600])
mouse = pygame.mouse

while pygame.event.poll().type != KEYDOWN:
    screen.fill([0, 0, 0]) # blank the screen.

    # Save time by only calling this once
    time = pygame.time.get_ticks() 
    for e in RBFs:
      e.update(time, pygame.mouse.get_pos())
      screen.blit(e.image, e.rect)

    pygame.display.update()
