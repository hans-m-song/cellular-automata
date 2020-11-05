# cellular-automata

## compiling

### serial

- `make` to compile
- `make visual` enable visualization of grid each iteration

### parallel

- `make parallel` to compile
- `make visualparallel` enable visualization of grid each iteration

### other

- `make clean` to remove compiled

## running

### arguments

- `width` number indicating grid width, e.g. 100
- `height` number indicating grid height, e.g. 100
- `initial_density` decimal number indicating percentage of board to start with cells, e.g. 0.4
- `ticks` number of iterations, e.g. 20

- `./cellular_automata width height initial_density ticks` to run normally
- `./parallel_cellular_automata width height initial_density ticks` to run using MPI and OpenMP

example: `./cellular_automata 100 100 0.6 100`

## propogation rules

based on conways game of life:

1. Any live cell with two or three live neighbours survives.
2. Any dead cell with three live neighbours becomes a live cell.
3. All other live cells die in the next generation. Similarly, all other dead cells stay dead.
