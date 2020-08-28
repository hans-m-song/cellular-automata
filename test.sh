#/bin/bash

dimensions=(10 20 30 40 50 60 70 80 90 100 200 300 400 500 600 700 800 900 1000)

for dimension in ${dimensions[@]}; do 
  echo $dimension 100
  ./cellular_automata $dimension $dimension 0.5 100 >> dimension.output.csv
done

for iteration in ${dimensions[@]}; do
  echo 100 $iteration
  ./cellular_automata 100 100 0.5 $iteration >> iteration.output.csv
done