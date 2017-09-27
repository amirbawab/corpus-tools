# Docker

## Step by step


### Clone repository
```
git clone https://github.com/amirbawab/corpus-tools
```

### Build docker image
```
sudo docker build -t reddit:latest .
```

### Start instance
```
sudo docker start --name reddit-container -it reddit:latest
```

### Start pipeline
*From container*
```
./reddit.sh
# Use the file: french_comments_part.json
```
*Note: On the google cloud, you can use the large file*
