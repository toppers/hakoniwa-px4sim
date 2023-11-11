FROM ubuntu:20.04 as px4-build

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

WORKDIR /root/workspace/px4
RUN git clone -b v1.14.0-rc2 --recursive https://github.com/PX4/PX4-Autopilot.git
WORKDIR /root/workspace/px4/PX4-Autopilot
RUN apt-get update -y
RUN apt-get install -y lsb-release
RUN apt-get install -y sudo
ENV DOCKER_LOCAL_HOST_ADDR "host.docker.internal"
RUN ./Tools/setup/ubuntu.sh --no-nuttx --no-sim-tools
RUN make px4_sitl_default


#ENTRYPOINT [ "/bin/bash", "/root/workspace/sim/simstart.bash" ]
