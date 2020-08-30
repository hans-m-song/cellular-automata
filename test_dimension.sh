source ./test_setup.sh

dimensions=(100 200 300 400 500 600 700 800 900 1000)
type=dimension

pre_test
for dimension in ${dimensions[@]}; do
  iteration=100
  test | tee -a output.dimension.csv
done
