FROM ubuntu:latest as executor

USER root

RUN apt-get update && \
    apt-get install -y \
      libboost-dev libboost-program-options-dev \
      libgtest-dev \
      gcc \
      g++ \
      cmake \
    && \
    cmake -DCMAKE_BUILD_TYPE=Release /usr/src/gtest && \
    cmake --build .

COPY . /app
COPY . /var/www

WORKDIR /app/build

RUN cmake .. && cmake --build .

EXPOSE 80

CMD ./WebServer