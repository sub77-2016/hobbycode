import pylab, numpy

def fileExists(filename):
    """Returns whether the file exists."""
    
    try:
        f = open(filename)
        f.close()
        return True
    except IOError:
        return False

def plotLineData(dataFilenameList, outputFilename, title=None, xRange=None, yRange=None, 
    xLabel=None, yLabel=None, legend=None):
    """Generates a line plot image from the data files."""
    
    plotData('line', dataFilenameList, outputFilename, title, xRange, yRange, xLabel, 
        yLabel, legend)

def plotRBFData(dataFilenameList, outputFilename, title=None, xRange=None, yRange=None, 
    xLabel=None, yLabel=None, legend=None):
    """Generates a plot of circles representing RBFs defined by the data files."""
    
    plotData('rbf', dataFilenameList, outputFilename, title, xRange, yRange, xLabel, 
        yLabel, legend)

def plotData(plotType, dataFilenameList, outputFilename, title, xRange, yRange, xLabel, 
    yLabel, legend):
    """Generates a line plot image from the data files."""
    
    for i in dataFilenameList:
        if not fileExists(i):
            print 'Warning: %s does not exist. Ignoring...' % i
            return
    
    # Clear everything before plotting.
    pylab.cla()
    pylab.clf()
    
    if title:
        pylab.title(title)
    if xLabel:
        pylab.xlabel(xLabel)
    if title:
        pylab.ylabel(yLabel)
    
    for i in range(len(dataFilenameList)):
        data = pylab.load(dataFilenameList[i], comments='#')
        x = data[:,0]
        y = data[:,1]
        if plotType == 'line':
            if legend:
                pylab.plot(x, y, antialiased=True, linewidth=2, label=legend[i])
            else:
                pylab.plot(x, y, antialiased=True, linewidth=2)
        elif plotType == 'rbf':
            pylab.plot(x, y, 'ro')
    
    # This must come after plotting since the axis isn't created until then.
    if xRange:
        pylab.axis(xmin = xRange[0], xmax = xRange[1])
    if yRange:
        pylab.axis(ymin = yRange[0], ymax = yRange[1])
    if legend:
        pylab.legend()
    
    # This will make an 8" x 6" figure using the given dpi.
    pylab.savefig(outputFilename, dpi=200, orientation='landscape')
