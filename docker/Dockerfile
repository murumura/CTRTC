FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential  \
    software-properties-common \
    git \
    pkg-config \
    libmpc-dev\
    flex\
    cmake  \
    libcurl4-openssl-dev --fix-broken \
    libjsoncpp-dev \
    librlog-dev \
    libgmock-dev \
    cpputest \
    libboost-all-dev && \
    rm -rf /var/lib/apt/lists/*
RUN apt-get update && apt-get install -y g++-12 gcc-12 && \
    rm -rf /var/lib/apt/lists/*
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 --slave /usr/bin/g++ g++ /usr/bin/g++-12 --slave /usr/bin/gcov gcov /usr/bin/gcov-12
WORKDIR /home/RayTracer
