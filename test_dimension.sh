source ./test_setup.sh

dimensions=(1000 2000 3000 4000 5000 6000 7000 8000 9000 10000)
type=dimension

pre_test
for dimension in ${dimensions[@]}; do
  iteration=100
  test | tee -a output.dimension.csv
done
