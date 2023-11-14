#!/usr/bin/env sh

set -e

mkdir -p server/log
cd server

# Execute passed arguments
"$@"
