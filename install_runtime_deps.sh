#!/usr/bin/env bash

. common_install.sh

# This is currently only made for Debian based environments (like Debian, Ubuntu, etc).
# You need the same stuff for any other system.

# Install runtime dependencies
sudo apt install \
	nmap \
	openssl \
	libcap2 \
	libjsoncpp25 \
	libuuid1 \
	zlib1g
