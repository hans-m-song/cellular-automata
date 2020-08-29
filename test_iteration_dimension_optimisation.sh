#/bin/bash

count=(
  10 20 30 40 50 60 70 80 90 100 
  200 300 400 500 600 700 800 900 1000 10000
)

function test() {
  for iteration in ${count[@]}; do
    ./cellular_automata 100 100 0.5 $iteration | tee -a output.$type.csv
  done

  for dimension in ${count[@]}; do 
    ./cellular_automata $dimension $dimension 0.5 100 | tee -a output.$type.csv
  done
}

make DEFINE=-O0 new
type=o0
test

make DEFINE=-O1 new
type=o1
test

make DEFINE=-O2 new
type=o2
test

make DEFINE=-O3 new
type=o3
test