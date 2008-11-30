set terminal svg size 720 540 fixed fsize 24

set xlabel "Trial"

set xtics 50
set ylabel "Average Reward Per Time Step"
set output "pendulum-performance.svg"
#set title "Pendulum Performance"
plot [][] "pendulum-performance.dat" using 1:2 title "" with lines linewidth 3

set ylabel "Steps to Goal"
set output "maze-performance.svg"
set title "Maze Performance"
##plot [][] "mazeDisc-performance.dat" using 1:2 title "Discrete Inputs" with lines linewidth 1, \
##	"mazeCont-performance.dat" using 1:2 title "Continuous Inputs" with lines linewidth 4
plot [][0:2000] "mazeDiscRL-performance.dat" using 1:2 title "RL" with lines linewidth 1, \
	"mazeDiscModelRL-performance.dat" using 1:2 title "RL + Planning" with lines linewidth 4, \
	"mazeDiscModelRLNew-performance.dat" using 1:2 title "RL + Planning New" with lines linewidth 4

set ylabel "Reward Sum"
set output "curiosity-performance.svg"
plot [][:120] "curiosityRL-performance.dat" using 1:2 title "RL" with lines linewidth 1, \
	"curiosityModelRL-performance.dat" using 1:2 title "RL + Planning" with lines linewidth 3, \
	"curiosityCuriousModelRL-performance.dat" using 1:2 title "RL + Planning + Curiosity" with lines linewidth 5

set ylabel "Steps to Goal"
set output "maze-performance.svg"
set title "Maze Performance"
plot [][0:800] "mazeDisc-performance.dat" using 1:2 title "Discrete Inputs" with lines linewidth 1
#	"mazeCont-performance.dat" using 1:2 title "Continuous Inputs" with lines linewidth 4

set ylabel "Average MSE"
set output "maze-modelMSE.svg"
#set title "Average MSE"
#plot [][] "mazeDisc-performance.dat" using 1:3 title "Discrete Inputs" with lines linewidth 1, \
#	"mazeCont-performance.dat" using 1:3 title "Continuous Inputs" with lines linewidth 4
plot [][] "mazeDisc-performance.dat" using 1:3 title "" with lines linewidth 3

set ylabel "Number of Planning Steps"
set output "maze-planLength.svg"
#set title "Maze Average Planning Sequence Length"
plot [][] "mazeDisc-performance.dat" using 1:4 title "" with lines linewidth 3

set xtics 100
set ylabel "Steps to Goal"
set output "2DHotPlate-performance.svg"
#set title "2D Hot Plate Performance (Optimal ~5)"
plot [][0:] "2DHotPlateDisc-performance.dat" using 1:2 title "Discrete Inputs" with lines linewidth 1, \
	"2DHotPlateCont-performance.dat" using 1:2 title "Continuous Inputs" with lines linewidth 4

set xtics 10
set ylabel "Steps to Goal"
set output "hotPlate-performance.svg"
#set title "Hot Plate Performance (Optimal ~5)"
plot [][0:] "hotPlateDisc-performance.dat" using 1:2 title "Discrete Inputs" with lines linewidth 1, \
	"hotPlateCont-performance.dat" using 1:2 title "Continuous Inputs" with lines linewidth 4

set xtics 20
set ylabel "Steps to Goal"
set output "signaledHotPlate-performance.svg"
#set title "Signaled Hot Plate Performance (Optimal ~10)"
plot [][0:] "signaledHotPlateDisc-performance.dat" using 1:2 title "Discrete Inputs" with lines linewidth 1, \
	"signaledHotPlateCont-performance.dat" using 1:2 title "Continuous Inputs" with lines linewidth 4

set xtics 200
set ylabel "Steps to Goal"
set output "2DSignaledHotPlate-performance.svg"
#set title "2D Signaled Hot Plate Performance (Optimal ~10)"
plot [][0:] "2DSignaledHotPlateDisc-performance.dat" using 1:2 title "Discrete Inputs" with lines linewidth 1, \
	"2DSignaledHotPlateCont-performance.dat" using 1:2 title "Continuous Inputs" with lines linewidth 4

set xtics 100
set ylabel "Percent Correct"
set output "sequentialActions-performance.svg"
#set title "Sequential Actions Performance"
plot [][0:105] "sequentialActions-performance.dat" using 1:2 title "" with lines linewidth 3

set xtics 100
set ylabel "Reward Received"
set output "10-ArmedBandit-performance.svg"
#set title "10-Armed Bandit Performance (Optimal ~1.55)"
#plot [][0:] "10-ArmedBandit-performance.dat" using 1:2:(100.0) smooth acsplines title "" with lines
plot [][0:] "10-ArmedBandit-performance.dat" using 1:2 title "" with lines linewidth 3

set xtics 100
set ylabel "Trial Length (seconds)"
set output "cartpole-performance.svg"
#set title "Cart-Pole Performance"
plot [][] "cartpole-performance.dat" using 1:2 title "" with lines linewidth 3
