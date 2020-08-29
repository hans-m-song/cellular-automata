docker build -t cellular_automata . && \
docker run --rm -it --name cellular_automata cellular_automata \
./cellular_automata $@
