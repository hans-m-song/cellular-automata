# cellular-automata

## commands

- `make` to compile
- `make clean` to remove compiled
- `make new` to remove and compile
- `./debug.sh <args>` to run valgrind in docker
- `./cellular_automata <args>` to run normally

## arguments

- `width`
- `height`
- `ticks`

## propogation rules

1. Any live cell with two or three live neighbours survives.
2. Any dead cell with three live neighbours becomes a live cell.
3. All other live cells die in the next generation. Similarly, all other dead cells stay dead.
