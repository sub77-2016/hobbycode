from plottingTools import plotLineData, plotRBFData

######### 10-Armed Bandit

plotLineData(dataFilenameList=['10-ArmedBandit-performance.dat'], 
    outputFilename='10-ArmedBandit-performance.png', 
    title='10-Armed Bandit Performance', 
    xLabel='Trial', 
    yLabel='Rewards Received')

######### Maze

plotLineData(dataFilenameList=['mazeDisc-performance.dat', 'mazeCont-performance.dat'], 
    outputFilename='maze-performance.png', 
    title='Maze Performance', 
    xLabel='Trial', 
    yLabel='Steps to Goal', 
    legend=('Discrete', 'Continuous'))

plotLineData(dataFilenameList=['mazeDisc-MSE.dat', 'mazeCont-MSE.dat'], 
    outputFilename='maze-MSE.png', 
    title='Maze Model Average MSE', 
    xLabel='Trial', 
    yLabel='MSE', 
    legend=('Discrete', 'Continuous'))

plotLineData(dataFilenameList=['mazeDisc-planLength.dat', 'mazeCont-planLength.dat'], 
    outputFilename='maze-planLength.png', 
    title='Maze Plan Length', 
    xLabel='Trial', 
    yLabel='Planning Steps', 
    legend=('Discrete', 'Continuous'))

######### Pendulum

plotLineData(dataFilenameList=['pendulum-performance.dat'], 
    outputFilename='pendulum-performance.png', 
    title='Pendulum Performance', 
    xLabel='Trial', 
    yLabel='Average Reward Per Time Step')

######### Cart-Pole

plotLineData(dataFilenameList=['cartpole-performance.dat'], 
    outputFilename='cartpole-performance.png', 
    title='Cart-Pole Performance', 
    xLabel='Trial', 
    yLabel='Trial Length (seconds)')

######### Curiosity Maze Test

plotLineData(dataFilenameList=['curiosity-performance.dat'], 
    outputFilename='curiosity-performance.png', 
    title='Curiosity Performance Comparison', 
    xLabel='Trial', 
    yLabel='Reward Sum', 
    legend=('RL', 'RL + Planning', 'RL + Planning + Curiosity'))

######### 1D Hot Plate

plotLineData(dataFilenameList=['hotPlate-performance.dat'], 
    outputFilename='hotPlate-performance.png', 
    title='Hot Plate Performance (Optimal ~5)', 
    xLabel='Trial', 
    yLabel='Steps to Goal')

######### Signaled 1D Hot Plate

plotLineData(dataFilenameList=['signaledHotPlate-performance.dat'], 
    outputFilename='signaledHotPlate-performance.png', 
    title='Signaled Hot Plate Performance (Optimal ~10)', 
    xLabel='Trial', 
    yLabel='Steps to Goal')

######### 2D Hot Plate

plotLineData(dataFilenameList=['2DHotPlate-performance.dat'], 
    outputFilename='2DHotPlate-performance.png', 
    title='2D Hot Plate Performance (Optimal ~5)', 
    xLabel='Trial', 
    yLabel='Steps to Goal')

######### 2D Signaled Hot Plate

plotLineData(dataFilenameList=['2DSignaledHotPlate-performance.dat'], 
    outputFilename='2DSignaledHotPlate-performance.png', 
    title='2D Signaled Hot Plate Performance (Optimal ~10)', 
    xLabel='Trial', 
    yLabel='Steps to Goal')

######### Sequential Action Learning

plotLineData(dataFilenameList=['sequentialActions-performance.dat'], 
    outputFilename='sequentialActions-performance.png', 
    title='Sequential Actions Performance', 
    xLabel='Trial', 
    yLabel='Percent Correct')
