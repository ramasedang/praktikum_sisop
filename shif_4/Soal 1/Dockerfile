# Use Alpine as a parent image
FROM alpine:latest

# Set the working directory in the container to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

# Install any needed packages
RUN apk update && apk add unzip python3 py3-pip gcc musl-dev
RUN pip3 install --upgrade pip && pip3 install kaggle

# Copy the Kaggle API token
COPY kaggle.json /root/.kaggle/

RUN chmod 600 /root/.kaggle/kaggle.json

# Compile the C program
RUN gcc -o storage storage.c

# Run the program when the container launches
CMD ["./storage"]
