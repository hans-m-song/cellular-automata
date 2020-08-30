docker build -t cellular_automata . > /dev/null && \
docker run --rm -it --name cellular_automata cellular_automata \
./cellular_automata $@
