source ./test_setup.sh

iterations=(1000 2000 3000 4000 5000 6000 7000 8000 9000 10000)
type=iteration

pre_test
for iteration in ${iterations[@]}; do
  dimension=1000
  test | tee -a output.iteration.csv
done
