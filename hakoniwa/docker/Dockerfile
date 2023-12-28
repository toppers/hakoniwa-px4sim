FROM ubuntu:22.04 as hakoniwa-px4sim

ARG DEBIAN_FRONTEND=noninteractive
ENV CMAKE_INSTALL_DIR /usr/local/cmake

RUN apt-get update
RUN apt-get install -y \
	git	\
	build-essential	\
	wget	\
	gcc	\
	g++	\
	vim	\
	gem \
	libssl-dev libreadline-dev zlib1g-dev \
	make	\
	autoconf \
	automake \
	pkg-config \
	curl \
	net-tools \
	netcat \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/*

ENV PATH ${PATH}:${CMAKE_INSTALL_DIR}/bin
RUN apt-get update
RUN apt-get install -y cmake
RUN apt-get install -y python3-pip python3-setuptools python3-wheel ninja-build
RUN pip3 install --upgrade numpy empy toml
RUN apt-get install lsof -y

RUN mkdir /usr/local/lib/hakoniwa
RUN mkdir /usr/local/bin/hakoniwa

RUN apt-get install -y protobuf-compiler
RUN apt-get install -y build-essential

ENV RUST_HOME /usr/local/lib/rust
ENV RUSTUP_HOME ${RUST_HOME}/rustup
ENV CARGO_HOME ${RUST_HOME}/cargo
RUN mkdir /usr/local/lib/rust && \
    chmod 0755 $RUST_HOME
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs > ${RUST_HOME}/rustup.sh \
    && chmod +x ${RUST_HOME}/rustup.sh \
    && ${RUST_HOME}/rustup.sh -y --default-toolchain nightly --no-modify-path
ENV PATH $PATH:$CARGO_HOME/bin

RUN apt-get install -y libc++-dev libc++abi-dev

RUN apt install -y mosquitto mosquitto-clients

ENV PYTHONPATH="/usr/local/lib/hakoniwa:$PYTHONPATH"
WORKDIR /root
RUN git clone --recursive https://github.com/toppers/hakoniwa-core-cpp-client.git
WORKDIR /root/hakoniwa-core-cpp-client
RUN apt update
RUN apt-get install -y python3-pip  libffi-dev python3-dev
RUN pip3 install numpy
RUN git pull
RUN bash build.bash
RUN cp cmake-build/src/hakoc/libshakoc.* /usr/local/lib/hakoniwa/
RUN cp cmake-build/src/hakoc/libshakoc.so /usr/local/lib/hakoniwa/hakoc.so
RUN cp -rp py /usr/local/lib/hakoniwa/
ENV PYTHONPATH="/usr/local/lib/hakoniwa/py:$PYTHONPATH"
RUN cp cmake-build/src/proxy/hako-proxy /usr/local/bin/hakoniwa
RUN cp cmake-build/core/sample/base-procs/hako-cmd/hako-cmd /usr/local/bin/hakoniwa
RUN pip3 install pyquaternion
RUN pip3 install scipy

WORKDIR /root
RUN git clone --recursive https://github.com/toppers/hakoniwa-conductor.git
WORKDIR /root/hakoniwa-conductor/main
RUN bash install.bash

RUN mkdir -p /root/workspace
WORKDIR /root/workspace
RUN apt-get install -y libgtest-dev