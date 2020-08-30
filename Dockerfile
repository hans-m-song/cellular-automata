FROM debian:stretch-slim

RUN apt update && apt install -y build-essential valgrind

COPY *.cpp *.hpp makefile ./

RUN make DEFINE="-O2"

ENTRYPOINT [ "valgrind", "--leak-check=full"]
