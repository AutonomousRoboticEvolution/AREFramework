FROM ubuntu:22.04

WORKDIR /are
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    libeigen3-dev \
    git \
    cmake \
    build-essential \
    wget \
    software-properties-common \
    libopencv-dev \
    vim

# Boost 1.82.0
WORKDIR /are
RUN add-apt-repository ppa:mhier/libboost-latest -y && apt-get update && apt-get install -y boost1.81

# MultiNEAT
WORKDIR /are
RUN git clone --recurse-submodules https://github.com/ci-group/MultiNEAT.git
WORKDIR /are/MultiNEAT/cereal/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make -j 4 && make install
#TODO: Modify CMake on MultiNEAT, remove directory to cereal
#WORKDIR /are/MultiNEAT/build
#RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
# && make && make install

# Polyvox
WORKDIR /are
RUN git clone https://github.com/portaloffreedom/polyvox.git
WORKDIR /are/polyvox/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make && make install

# Libdlibxx
WORKDIR /are
RUN git clone https://github.com/m-renaud/libdlibxx.git
WORKDIR /are/libdlibxx/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make && make install

# LibCMAES
WORKDIR /are
RUN git clone https://github.com/beniz/libcmaes.git
WORKDIR /are/libcmaes/build
RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local .. && make && make install

# Limbo headers
WORKDIR /are
RUN git clone https://github.com/resibots/limbo.git

# ARE Framework
WORKDIR /are
RUN git clone -b intr_mot https://github.com/AutonomousRoboticEvolution/AREFramework.git
# NN2 headers #TODO add NN2 as submodule of ARE
WORKDIR /are/AREFramework/modules
RUN git clone https://github.com/LeniLeGoff/nn2
WORKDIR /are/AREFramework/build
#RUN cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DVREP_FOLDER=/are/AREFramework/vrep-3.6.2 -DLIMBO_FODLER=/are/limbo .. && make && make install