#!/usr/bin/env sh

# This is currently only made for Debian based environments (like Debian, Ubuntu, etc).
# You need the same stuff for any other system. If the package `libXXX-dev` isn't available it's typically `XXX-devel`
# instead

# Install build dependencies
sudo apt install \
	cmake \
	build-essential \
	git \
	libjsoncpp-dev \
	uuid-dev  \
	zlib1g-dev \
	openssl libssl-dev
