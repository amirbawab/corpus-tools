FROM ubuntu:16.04
RUN apt-get update
RUN apt-get install wget curl git cmake build-essential -y
COPY . /ML
WORKDIR /ML
RUN git submodule update --init --recursive
RUN cd tools/cpp/ && cmake . && make
CMD ["/bin/bash"]
