#! /bin/sh

#Begin Script
path=$1
chmod 777 ./histogram
./histogram $path > int.txt
gnuplot << EOF
set terminal jpeg
set style data histogram 
set style histogram clustered gap 1
set title "Filetypes vs. Number of Files" font ",20" offset character 2, 0, 0
set ylabel "Number of Files" font ",18" offset character 2,0,0
set xlabel "Filetypes" font ",20" offset character 0, -4, 0
set grid y
set grid x
set xtics border in scale 0,0 nomirror rotate by 90 offset character 0, -4, 0
set boxwidth 0.9 absolute
#set xr [1:6]
set style fill transparent solid 0.8 noborder
set key off
plot 'int.txt' using 2:xticlabels(1)
EOF
rm int.txt





