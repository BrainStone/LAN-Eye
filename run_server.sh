#!/usr/bin/env sh

set -e

mkdir -p server/log
cd server

# Pass path to executable as argument
"$1"
