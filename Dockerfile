########################################################################################################################
# ECU build stage
########################################################################################################################

FROM ubuntu:22.04 AS build


RUN apt-get update && \
    apt-get install -y \
    software-properties-common && \
    add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install -y \
    cmake \
    make \
    wget \
    libssl-dev

# Set gcc and g++ 13 as the default compilers
RUN add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get install gcc-13 g++-13 -y && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 --slave /usr/bin/g++ g++ /usr/bin/g++-13


# Install CMake 3.23 or higher
#RUN wget https://github.com/Kitware/CMake/releases/download/v3.23.4/cmake-3.23.4-linux-x86_64.sh -O /tmp/cmake-install.sh && \
#    chmod +x /tmp/cmake-install.sh && \
#    /tmp/cmake-install.sh --skip-license --prefix=/usr/local && \
#    rm /tmp/cmake-install.sh

WORKDIR /ECU_ZUR

COPY source/ ./source/
COPY include/ ./include/
COPY .config/ ./.config/
COPY Libraries/ ./Libraries/
COPY CMakeLists.txt ./

WORKDIR /ECU_ZUR/build

RUN cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . --parallel 8

########################################################################################################################
# ECU image
########################################################################################################################

FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y 

WORKDIR /ECU_ZUR

COPY .config/ ./.config/

WORKDIR /ECU_ZUR/build

COPY --from=build \
    /ECU_ZUR/build/ECU_ZUR \
    .

ENTRYPOINT ["./ECU_ZUR"]
