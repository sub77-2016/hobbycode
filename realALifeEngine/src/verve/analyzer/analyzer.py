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

#!/usr/bin/python

from PythonCard import dialog, model
from pyverve.pyverve import InputDescriptor, Agent


class Point:
    def __init__(self):
        self.x = 0
        self.y = 0

    def __str__(self):
        return '(' + str(self.x) + ', ' + str(self.y) + ')'

class Color:
    def __init__(self, red, green, blue):
        self.r = red
        self.g = green
        self.b = blue

# y values are handled within Plot as if y = 0 is at the bottom of the canvas.
class Plot:
    def __init__(self, canvas, yBase, yRange, color, incrementWidth):
        self.canvas = canvas
        self.points = []
        self.color = [0, 0, 0]
        self.yBase = yBase
        self.yMin = yBase - 0.5 * yRange
        self.yMax = yBase + 0.5 * yRange
        self.range = yRange
        self.xIncrement = incrementWidth
        self.color = color

    def shiftLeft(self):
        # Don't do anything if there are no points.
        if len(self.points) > 0:
            del self.points[0]
            for p in self.points:
                p.x -= self.xIncrement

    # All values are assumed to be within the range [-1, 1].
    def addPoint(self, value):
        p = Point()
        xValue = 0
        if len(self.points) > 0:
            xValue = self.points[-1].x + self.xIncrement
        p.x = xValue
        #value += 1.0 # Now value is within [0, 2].
        #value *= 0.5 # Now value is within [0, 1].
        #value *= self.range # Now value is within [0, range].
        #value += self.yMin # Now value is within [yMin, yMax].
        #p.y = value
        p.y = value * 0.5 * self.range + self.yBase
        self.points.append(p)

    def draw(self):        
        # Draw horizontal lines.
        self.canvas.foregroundColor = (0, 0, 0)
        self.canvas.drawLine((0, self.canvas.size[1] - self.yBase), 
            (self.canvas.size[0], self.canvas.size[1] - self.yBase))
##        self.canvas.foregroundColor = (200, 200, 200)
##        self.canvas.drawLine((0, self.canvas.size[1] - self.yMin), 
##            (self.canvas.size[0], self.canvas.size[1] - self.yMin))
##        self.canvas.drawLine((0, self.canvas.size[1] - self.yMax), 
##            (self.canvas.size[0], self.canvas.size[1] - self.yMax))

        # Draw the points.  Here we flip y values since the canvas' y = 0 is 
        # at the top.
        self.canvas.foregroundColor = (self.color.r, self.color.g, self.color.b)
        for i in range(len(self.points)):
            if i > 1:
                p1 = self.points[i - 1]
                p2 = self.points[i]
                self.canvas.drawLine((p1.x, self.canvas.size[1] - p1.y), 
                    (p2.x, self.canvas.size[1] - p2.y))

class AnalyzerApp(model.Background):

    def on_initialize(self, event):
        i = InputDescriptor()
        i.addContinuousInput(200, False)
        self.agent = Agent(i, 1)
        self.agent.applyIdealInitialNoiseToTrainableWeights()
        learningRate = 0.01
        eTraceTimeConstant = 0.1
        discountTimeConstant = 0.1
        self.agent.setLearningRate(learningRate)
        self.agent.setETraceTimeConstant(eTraceTimeConstant)
        self.agent.setTDDiscountTimeConstant(discountTimeConstant)
        self.stepSize = 0.1
        self.components.updateTimeTextField.text = str(self.stepSize)
        self.elapsedSeconds = 0.0
        self.timeBuffer = 0.0
        
        # Assume one tick mark per second.
        self.tickWidth = 200
        self.tickXBase = 0
        self.ticksPerScreen = self.components.canvas.size[0] / self.tickWidth
        self.incrementWidth = self.tickWidth / (1 / self.stepSize)

        comp = self.components
        comp.canvas.foregroundColor = (0, 0, 0)
        comp.canvas.backgroundColor = (200, 200, 200)

        # y = 0 at the bottom of the canvas.
        self.inputPlot = Plot(comp.canvas, 700, 160, Color(128, 128, 128), self.incrementWidth)
        self.rewardPlot = Plot(comp.canvas, 500, 160, Color(0, 0, 255), self.incrementWidth)
        self.valEstPlot = Plot(comp.canvas, 300, 160, Color(0, 255, 0), self.incrementWidth)
        self.tdErrorPlot = Plot(comp.canvas, 100, 160, Color(255, 0, 0), self.incrementWidth)
        
        # Initialize parts of the GUI that need special starting values.
        comp.learningRateTextField.text = str(learningRate)
        comp.learningRateSlider.value = 100 * learningRate
        comp.eTraceTCTextField.text = str(eTraceTimeConstant)
        comp.eTraceTCSlider.value = 100 * eTraceTimeConstant
        comp.discountTCTextField.text = str(discountTimeConstant)
        comp.discountTCSlider.value = 100 * discountTimeConstant
        
        # Draw the initial border and axes.
        self.redraw()

    def drawBorder(self):
        canvas = self.components.canvas
        canvas.drawLine((0, 0), (canvas.size[0]-1, 0))
        canvas.drawLine((canvas.size[0]-1, 0), (canvas.size[0]-1, canvas.size[1]-1))
        canvas.drawLine((canvas.size[0]-1, canvas.size[1]-1), (0, canvas.size[1]-1))
        canvas.drawLine((0, canvas.size[1]-1), (0, 0))

    def redraw(self):
        self.components.canvas.clear()
        self.components.canvas.foregroundColor = (0, 0, 0)
        self.drawBorder()
        self.drawTickMarks()
        self.inputPlot.draw()
        self.rewardPlot.draw()
        self.valEstPlot.draw()
        self.tdErrorPlot.draw()

    def shiftLeft(self):
        self.tickXBase -= self.incrementWidth
        if self.tickXBase < -self.tickWidth:
            self.tickXBase = 0
        self.inputPlot.shiftLeft()
        self.rewardPlot.shiftLeft()
        self.valEstPlot.shiftLeft()
        self.tdErrorPlot.shiftLeft()
        
    def drawTickMarks(self):
        canvas = self.components.canvas
        x = self.tickXBase
        while x < canvas.size[0]:
            canvas.drawLine((x, canvas.size[1] - 1), (x, canvas.size[1] - 10))
            x += self.tickWidth
        
    def on_learningRateSlider_select(self, event):
        comp = self.components
        newValue = 0.01 * float(comp.learningRateSlider.value)
        self.agent.setLearningRate(newValue)
        comp.learningRateTextField.text = str(newValue)

    def on_eTraceTCSlider_select(self, event):
        comp = self.components
        newValue = 0.01 * float(comp.eTraceTCSlider.value)
        self.agent.setETraceTimeConstant(newValue)
        comp.eTraceTCTextField.text = str(newValue)

    def on_discountTCSlider_select(self, event):
        comp = self.components
        newValue = 0.01 * float(comp.discountTCSlider.value)
        self.agent.setTDDiscountTimeConstant(newValue)
        comp.discountTCTextField.text = str(newValue)
        
    def on_inputSlider_select(self, event):
        self.components.inputTextField.text = str(0.01 * float(self.components.inputSlider.value))

    def on_rewardSlider_select(self, event):
        self.components.rewardTextField.text = str(0.01 * float(self.components.rewardSlider.value))

##    def on_inputTextField_textUpdate(self, event):
##        pass

##    def on_incrBtn_mouseClick(self, event):
##        pass
##        #startValue = int(self.components.field1.text)
##        #endValue = startValue + 1
##        #self.components.field1.text = str(endValue)
##
##    def on_decrBtn_mouseClick(self, event):
##        pass
##        #startValue = int(self.components.field1.text)
##        #endValue = startValue - 1
##        #self.components.field1.text = str(endValue)

    def on_updateButton_mouseClick(self, event):
        input = float(self.components.inputTextField.text)
        reward = float(self.components.rewardTextField.text)
        
        self.agent.setContinuousInput(0, input)

        valueEst = 0
        tdError = 0
        self.timeBuffer += float(self.components.updateTimeTextField.text)
        while self.timeBuffer >= self.stepSize:
            self.agent.update(self.stepSize, reward)
            valueEst = self.agent.getCurrentValueEstimation()
            tdError = self.agent.getTDError()
        
            self.inputPlot.addPoint(input)
            self.rewardPlot.addPoint(reward)
            self.valEstPlot.addPoint(valueEst)
            self.tdErrorPlot.addPoint(tdError)
            
            self.timeBuffer -= self.stepSize
            self.elapsedSeconds += self.stepSize
        
            # Shift the graph left if we get close to the right side.
            if self.elapsedSeconds > 0.9 * self.ticksPerScreen:
                self.shiftLeft()
            
        self.components.valEstTextField.text = '%.2f' % valueEst
        self.components.tdErrorTextField.text = '%.2f' % tdError
        
        self.redraw()

if __name__ == '__main__':
    app = model.Application(AnalyzerApp)
    app.MainLoop()

