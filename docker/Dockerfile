FROM ubuntu:22.04 as hakoniwa-px4sim

ARG DEBIAN_FRONTEND=noninteractive

ENV PATH /usr/local/bin/hakoniwa:${PATH}
ENV LD_LIBRARY_PATH /usr/local/lib/hakoniwa:${LD_LIBRARY_PATH}

RUN apt-get update
RUN apt-get install -y \
	build-essential	\
	wget	\
	vim	\
	libssl-dev libreadline-dev zlib1g-dev \
	autoconf \
	automake \
	pkg-config \
	curl \
	net-tools \
	netcat \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y gcc g++ make cmake
RUN apt-get install -y git jq libgtest-dev net-tools python3-dev

WORKDIR /root


