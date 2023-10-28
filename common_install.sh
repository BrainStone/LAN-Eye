#!/usr/bin/env bash
# Helper script that allows running the install scripts through automation. Just include this file

if [[ "$1" == "auto" ]]; then
	# Make apt-get operate without ever asking the user
	export DEBIAN_FRONTEND=noninteractive

	# Override sudo, so we can force `apt` to be `apt-get -y`, which is better for non-interactive situations
	function sudo() {
		local exec=("$1")
		shift

		# Magic happens here
		if [[ "${exec[0]}" == "apt" ]]; then
			exec=(apt-get -y)
		fi

		command sudo "${exec[@]}" "$@"
	}

	# Update first
	sudo apt update
fi
