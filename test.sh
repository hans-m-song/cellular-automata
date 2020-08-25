#/bin/bash

dimensions=(10 100 1000)

for dimension in ${dimensions[@]}; 
  do ./cellular_automata $dimension $dimension 0.5 100 >> output.csv;
done;