#! bin/gnuplot --persist

set terminal svg enhanced font "Helvetica, 16" size 800, 800

settile(x, y) = "set title '".x." (size:".y.")'"
setout(x) = "set output 'results/".x.".svg'"
eval(settile(ARG1, ARG2))
eval(setout(ARG1))

dat = "results/".ARG1.".dat"

set xtics 1
set xrange [0:]
set yrange [0:]
set grid y x
set key inside b r
set xlabel "Threads"
set ylabel "Speedup"


f(x)=x
plot f(x) dashtype 30 title 'Ideal', dat using 1:2 title 'Pthread' with lines, '' using 1:3 title 'OpenMP' with lines
