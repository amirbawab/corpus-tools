FROM ubuntu:16.04
RUN apt-get update
RUN apt-get install vim wget curl git cmake build-essential -y
COPY . /ML
WORKDIR /ML

# Replace this link by your Google BigQuery JSON file
RUN wget https://storage.googleapis.com/reddit_dataset_aml/reddit/french_comments_part.json

RUN git submodule update --init --recursive
RUN cd tools/cpp/ && cmake . && make
CMD ["/bin/bash"]
