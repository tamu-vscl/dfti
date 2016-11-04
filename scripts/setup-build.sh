#!/usr/bin/env bash
#
#   Creates the folders needed for an out of source build. Assumes readlink is
#   available.
#

# Get path to this script.
script=$(readlink -f "$0")
scriptpath=$(dirname "$script")

# Make folders.
mkdir "$scriptpath/../{bin,build,lib}"
