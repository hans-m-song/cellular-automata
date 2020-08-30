source ./test_setup.sh

optimisations=(O0 O1 O2 O3)
type=optimiser

for optimisation in ${optimisations[@]}; do
  pre_test
  iteration=1000
  dimension=1000
  test | tee -a output.optimisation.csv
done
