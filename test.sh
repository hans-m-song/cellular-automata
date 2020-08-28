#/bin/bash

count=(10 20 30 40 50 60 70 80 90 100 200 300 400 500 600 700 800 900 1000 10000)

for iteration in ${count[@]}; do
  echo 100 $iteration
  ./cellular_automata 100 100 0.5 $iteration >> iteration.output.csv
done

for dimension in ${count[@]}; do 
  echo $dimension 100
  ./cellular_automata $dimension $dimension 0.5 100 >> dimension.output.csv
done
