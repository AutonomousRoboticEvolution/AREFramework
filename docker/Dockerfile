FROM ubuntu:22.04
ARG N_CPU=1
WORKDIR /are
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    libeigen3-dev \
    git \
    cmake \
    build-essential \
    wget \
    libopencv-dev \
    libtbb-dev \
    liblua5.1-0-dev \
    qtbase5-dev \
    x11-apps \
    xvfb \
    psmisc \
    vim \
    libzmq5 \
    libzmq3-dev \
    python3-pip && pip3 install psutil dlib

# x11-apps only to test GUI xclock within docker

# Boost 1.77
WORKDIR /are
RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.77.0/source/boost_1_77_0.tar.gz \
    && tar -xf boost_1_77_0.tar.gz && rm boost_1_77_0.tar.gz
WORKDIR /are/boost_1_77_0
RUN ./bootstrap.sh && ./b2 install --prefix=/usr/local

## Polyvox
WORKDIR /are
RUN git clone https://github.com/portaloffreedom/polyvox.git
WORKDIR /are/polyvox/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make -j ${N_CPU} && make install

# Libdlibxx
WORKDIR /are
RUN git clone https://github.com/m-renaud/libdlibxx.git
WORKDIR /are/libdlibxx/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make && make install

# LibCMAES
WORKDIR /are
RUN git clone https://github.com/LeniLeGoff/libcmaes
WORKDIR /are/libcmaes/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make -j ${N_CPU} && make install

# Limbo headers
#WORKDIR /are
#RUN git clone https://github.com/resibots/limbo.git

# ARE Framework
WORKDIR /are
RUN git clone -b melai --recurse-submodules https://github.com/AutonomousRoboticEvolution/AREFramework.git
WORKDIR /are/AREFramework/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DVREP_FOLDER=/are/AREFramework/vrep-3.6.2 -DONLY_SIMULATION=1 .. && make -j ${N_CPU} && make install
#If using Limbo
#RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DVREP_FOLDER=/are/AREFramework/vrep-3.6.2 -DLIMBO_FODLER=/are/limbo -DONLY_SIMULATION=1 .. && make -j ${N_CPU} && make install

WORKDIR /are

# Buid
# $ cd docker
# $ sudo docker build --build-arg="N_CPU=20" -t are_framework:melai .  
# Run in your host machine to allow GUI
# $ sudo xhost +local:docker
# Example to run after successfull built
# tmp is where we will store the logs
# DISPLAY, x11-unix and net are only required if using the graphic interface
# docker run --rm -it --env DISPLAY=$DISPLAY --volume /tmp/.X11-uni:/tmp/.X11-unix --volume /tmp:/tmp --volume host_path/AREFramework:/are/AREFramework --net=host are_framework:melai /bin/bash
# $ run_cluster.py --xvfb 1 --params /are/AREFramework/experiments/homeokinesis/parameters.csv --client are-client --port-start 10000 --vrep /are/AREFramework/vrep-3.6.2/are_sim.sh 48