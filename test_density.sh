source ./test_setup.sh

densities=(0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9)
iterations=(1000 2000 3000 4000 5000 6000 7000 8000 9000 10000)
type=density
dimension=100

pre_test
for density in ${densities[@]}; do
  for iteration in ${iterations[@]}; do
    test | tee -a output.density.csv
  done
done
