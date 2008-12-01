set terminal svg size 720 665 fixed fsize 24

set pointsize 3

set xlabel "X Position"
set ylabel "Y Position"
set output "pendulum-trial0-RBF.svg"
plot [-1:1][-1:1] "pendulum-trial0-RBF.dat" using 1:2 title "" with points pt 6

set output "pendulum-trial4-RBF.svg"
plot [-1:1][-1:1] "pendulum-trial4-RBF.dat" using 1:2 title "" with points pt 6

set output "pendulum-trial19-RBF.svg"
plot [-1:1][-1:1] "pendulum-trial19-RBF.dat" using 1:2 title "" with points pt 6

set output "pendulum-trial99-RBF.svg"
plot [-1:1][-1:1] "pendulum-trial99-RBF.dat" using 1:2 title "" with points pt 6

set xlabel "X Position"
set ylabel "Y Position"
set output "2DHotPlateCont-fullRBFData.svg"
plot [-1:1][-1:1] "2DHotPlateCont-fullRBFData.dat" using 1:2 title "" with points pt 6

set output "2DHotPlateCont-trial4-dynamicRBFData.svg"
plot [-1:1][-1:1] "2DHotPlateCont-trial4-dynamicRBFData.dat" using 1:2 title "" with points pt 6

set output "2DHotPlateCont-trial9-dynamicRBFData.svg"
plot [-1:1][-1:1] "2DHotPlateCont-trial9-dynamicRBFData.dat" using 1:2 title "" with points pt 6

set output "2DHotPlateCont-trial49-dynamicRBFData.svg"
plot [-1:1][-1:1] "2DHotPlateCont-trial49-dynamicRBFData.dat" using 1:2 title "" with points pt 6

set output "2DHotPlateCont-trial299-dynamicRBFData.svg"
plot [-1:1][-1:1] "2DHotPlateCont-trial299-dynamicRBFData.dat" using 1:2 title "" with points pt 6
