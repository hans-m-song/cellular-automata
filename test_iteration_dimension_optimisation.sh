#/bin/bash

count=(
  10 20 30 40 50 60 70 80 90 100 
  200 300 400 500 600 700 800 900 1000 10000
)

function test() {
  for iteration in ${count[@]}; do
    echo 100 $iteration
    ./cellular_automata 100 100 0.5 $iteration >> output.$type.csv
  done

  for dimension in ${count[@]}; do 
    echo $dimension 100
    ./cellular_automata $dimension $dimension 0.5 100 >> output.$type.csv
  done
}

make DEFINE=-O0 new
filename=o0
test

make DEFINE=-O1 new
filename=o1
test

make DEFINE=-O2 new
filename=o2
test

make DEFINE=-O3 new
filename=o3
test