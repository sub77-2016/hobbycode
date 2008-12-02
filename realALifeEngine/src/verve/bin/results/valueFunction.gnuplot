set terminal svg dynamic
set zlabel "Value Estimation"
set nokey

# The following enables non-grid to grid data mapping.  The parameters 
# are: num x points, num y points, weight norm
set dgrid3d 25, 25, 8

# Only show one side of the surface.
set hidden3d

set output "pendulum-valueFunction.svg"
set title "Pendulum Value Estimation after 99 Trials"
set xlabel "Angle"
set ylabel "Angular Velocity"
splot "pendulum-trial99-value.dat" with lines

set output "2DHotPlateDisc-valueEstimation.svg"
set title "2D Hot Plate Value Estimation with Discrete Inputs"
set xlabel "X Position"
set ylabel "Y Position"
splot "2DHotPlateDisc-valueData.dat" with lines

set output "2DHotPlateCont-valueEstimation.svg"
set title "2D Hot Plate Value Estimation with Continuous Inputs"
splot "2DHotPlateCont-valueData.dat" with lines
