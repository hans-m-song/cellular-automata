# cellular-automata

## commands

- `make` to compile
- `make clean` to remove compiled
- `make new` to remove and compile
- `make visual` enable visualization of grid each iteration
- `./debug.sh <args>` to run valgrind in docker
- `./cellular_automata <args>` to run normally

## arguments

- `width` number indicating grid width, e.g. 100
- `height` number indicating grid height, e.g. 100
- `initial density` decimal number indicating percentage of board to start with cells, e.g. 0.4
- `ticks` number of iterations, e.g. 20

## propogation rules

based on conways game of life:

1. Any live cell with two or three live neighbours survives.
2. Any dead cell with three live neighbours becomes a live cell.
3. All other live cells die in the next generation. Similarly, all other dead cells stay dead.
