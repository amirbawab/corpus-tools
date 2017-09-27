# Container image: Ubuntu 16.04
FROM ubuntu:16.04

# Refresh repo and install packages
RUN apt-get update
RUN apt-get install vim wget curl git cmake build-essential -y

# Copy current git repository
COPY . /ML
WORKDIR /ML

# Download Google BigQuery JSON files.
# Small set of data (41MB)
RUN wget https://storage.googleapis.com/reddit_dataset_aml/reddit/french_comments_part.json

# Large set of data (996MB)
# This file should only be processed on super powerful machines
#RUN wget https://storage.googleapis.com/reddit_dataset_aml/reddit/french_all.json

# Make sure the submodules are updated
RUN git submodule update --init --recursive

# Build the C++ tools
RUN cd tools/cpp/ && cmake . && make

# Set the default CMD
CMD ["/bin/bash"]
