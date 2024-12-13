FROM ubuntu:22.04
ARG N_CPU=1
WORKDIR /are
# TODO set versions for each library
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
    jupyter \
    fish \
    python3-pip && pip3 install psutil dlib numpy=='1.26.4' scikit-learn=='1.5.0' pandas=='2.2.2' paretoset matplotlib=='3.9.0' seaborn=='0.13.2'

# x11-apps only to test GUI xclock within docker

ENV LD_LIBRARY_PATH=/usr/local/lib

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

# PCL
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
  libvtk9-dev \
  libflann-dev \
  libusb-1.0-0-dev \
  libpcap-dev
WORKDIR /are
RUN git clone https://github.com/PointCloudLibrary/pcl.git
WORKDIR /are/pcl/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make -j ${N_CPU} && make install

# ARE Framework
# To force rebuild try different values on command line ./build.sh --build-arg="REBUILD_ARE=1"
ARG REBUILD_ARE=0
ARG BRANCH=main
WORKDIR /are
RUN git clone -b ${BRANCH} --recurse-submodules https://github.com/AutonomousRoboticEvolution/AREFramework.git
WORKDIR /are/AREFramework/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DVREP_FOLDER=/are/AREFramework/vrep-3.6.2 -DONLY_SIMULATION=1 .. && make -j ${N_CPU} && make install
#If using Limbo
#RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DVREP_FOLDER=/are/AREFramework/vrep-3.6.2 -DLIMBO_FODLER=/are/limbo -DONLY_SIMULATION=1 .. && make -j ${N_CPU} && make install

## export LD_LIBRARY_PATH=/usr/local/lib
## example run
#python3 run_cluster.py 20 --vrep ../../vrep-3.6.2/are_sim.sh --client are-client --params /are/AREFramework/experiments/meim/parameters_docker.csv --port-start 10000 --xvfb 0

WORKDIR /are