FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive


RUN apt-get update && apt-get install -y \
    kmod\
    apt-utils\
    iproute2\
    can-utils \
    nano\
    busybox \
    wget \
    software-properties-common \
    cmake

RUN add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install gcc-13 g++-13 -y && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 --slave /usr/bin/g++ g++ /usr/bin/g++-13

# wait for answer from marco rau if specific version of cmake is neccessary or not
# this wget link does not work
#RUN apt-get remove cmake && \
#    wget https://github.com/Kitware/CMake/releases/download/v3.29.3/cmake-3.29.3/linux-aarch64.sh && \
#    chmod +x ./cmake-3.29.0-rc3-linux-aarch64.sh && \
#    bash ./cmake-3.29.0-rc3-linux-aarch64.sh


COPY . /opt/

RUN cd /opt/ && ls && cmake .


CMD [ "/opt/build/ECU_UASRacing" ]
