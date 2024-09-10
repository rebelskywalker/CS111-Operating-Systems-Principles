#!/usr/local/cs/bin/gnuplot
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

# lab2b_1.png
#set title changes the title
set title "G1: Number of Ops/s vs Threads"

#sets the x axis and y axis label
set xlabel "Number of threads"
set ylabel "Throughput (Ops/s)"

#sets the scale of graph in terms of log size
set logscale x 2
set logscale y 10
set xrange [0.75:]

set output 'lab2b_1.png'

# single threaded, unprotected, no yield
plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'spinlock list' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'mutex list' with linespoints lc rgb 'red'


#lab2b_2.png
set title "G2: Threads vs Avg Time/Op Given Protection"
set xlabel "Threads"
set ylabel "Average Time/Op (ns)"

set logscale x 2
set logscale y 10

set xrange [0.75:]
set output 'lab2b_2.png'

# single threaded, unprotected, no yield
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'Time to finish op' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'Lock wait time' with linespoints lc rgb 'red'

	

#lab2b_3.png
set title "G3: Reliably Failing Threads vs Proper Protected Threads"
set xlabel "Threads"
set ylabel "Successful runs"

set logscale x 2
set logscale y 10

set xrange [0.75:]
set yrange [0.75:]
# set xtics("" 0, "yield=i" 1, "yield=d" 2, "yield=il" 3, "yield=dl" 4, "" 5)
# set logscale y 10

set output 'lab2b_3.png'
plot \
    "< grep 'list-id-none,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'Unprotected'with points lc rgb 'blue', \
    "< grep 'list-id-s,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'Spin-Lock' with points lc rgb 'green', \
    "< grep 'list-id-m,[0-9]*,[0-9]*,4,' lab2b_list.csv" using ($2):($3) \
	title 'Mutex' with points lc rgb 'red' 


#
# "no valid points" is possible if even a single iteration can't run
#

# unset the kinky x axis
# lol kinky
#unset xtics
#set xtics


#lab2b_4.png
set title "G4: Mutex Scalability"
set xlabel "Threads"
set ylabel "Throughput"

set logscale y 10
set autoscale y
set autoscale x
#unset xrange
set xrange [0.75:]

#set logscale x 2
#set xrange [0.75:]
#set logscale y 10

set output 'lab2b_4.png'

# single threaded, unprotected, no yield
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=1' with linespoints lc rgb 'blue', \
	 "< grep 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=4' with linespoints lc rgb 'green', \
	 "< grep 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=8' with linespoints lc rgb 'red', \
     "< grep 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=16' with linespoints lc rgb 'yellow'



#lab2b_5.png
set title "G5: Spin Lock Scalability"
set xlabel "Threads"
set ylabel "Throughput"


set logscale x 2
set logscale y 10

#unset xrange
set xrange [0.75:]

set output 'lab2b_5.png'
#set key left top
# single threaded, unprotected, no yield
plot \
     "< grep 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=1' with linespoints lc rgb 'blue', \
	 "< grep 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=4' with linespoints lc rgb 'green', \
	 "< grep 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=8' with linespoints lc rgb 'red', \
     "< grep 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'lists=16' with linespoints lc rgb 'yellow'