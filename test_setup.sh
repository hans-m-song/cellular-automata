#/bin/bash

executable=./cellular_automata
density=0.5
dimension=1000
iteration=100
optimisation=O2

function pre_test() {
  make DEFINE=-${optimisation} SUBDIR=$type new
}

function test() {
  echo "$optimisation,$type,$($executable $dimension $dimension $density $iteration)"
}
