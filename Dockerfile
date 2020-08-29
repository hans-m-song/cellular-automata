FROM debian:stretch-slim

RUN apt update && apt install -y build-essential valgrind

COPY *.cpp *.hpp makefile ./

RUN make

ENTRYPOINT [ "valgrind", "--leak-check=full"]
