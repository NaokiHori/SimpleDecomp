#!/bin/bash

# https://github.com/NaokiHori/SimpleDecomp

# Description
#
# This script installs SimpleDecomp library to your machine,
#   so that you can use them without including
#   the sources src/sdecomp/* and the header include/sdecomp.h
#   to your project.
#
# Usage (install)
#
#   $ bash install.sh install
#
#   This will create
#     a dynamic library libsdecomp.so
#     a header file     sdecomp.h
#   to a specified place (default: ${HOME}/.local)
#
# Usage (uninstall)
#
#   $ bash install.sh uninstall
#
#   This will remove
#     a dynamic library libsdecomp.so
#     a header file     sdecomp.h
#   from a specified place (default: ${HOME}/.local)
#
# Confirm that you have a proper permission.

# installation parameters
prefix=${HOME}/.local
dynlib=${prefix}/lib/libsdecomp.so
header=${prefix}/include/sdecomp.h
cflags="-std=c99 -O3 -Wall -Wextra"

# check number of arguments is 1
if [ "$#" -ne 1 ]; then
  echo "Give one of \"install\" or \"uninstall\""
  exit 1
fi
arg="${1}"

# check the argument is "install" or "uninstall", and process
if [ "${arg}" = "install" ]; then
  echo "Install to ${prefix}"
  # check place to install
  dname=$(dirname ${dynlib})
	if [ ! -e "${dname}" ]; then
    echo "Create directory: ${dname}"
		mkdir -p "${dname}"
	fi
  dname=$(dirname ${header})
	if [ ! -e "${dname}" ]; then
    echo "Create directory: ${dname}"
		mkdir -p "${dname}"
	fi
  # create object files
  # NOTE: not good to iterate over find...
  srcs=$(find src/sdecomp -type f -name "*.c")
  for src in ${srcs[@]}; do
    obj=${src/src/obj}
    obj=${obj/.c/.o}
    if [ ! -e $(dirname ${obj}) ]; then
      mkdir -p $(dirname ${obj})
    fi
    mpicc -Iinclude ${cflags} -fPIC -c ${src} -o ${obj}
  done
  # create dynamic library
  echo "Dynamic library ${dynlib} is created"
  mpicc -Iinclude ${cflags} -fPIC -shared $(find obj/sdecomp -type f -name "*.o") -o ${dynlib}
  # copy header
  echo "Header ${header} is created"
  cp include/sdecomp.h ${header}
elif [ "${arg}" = "uninstall" ]; then
  echo "Uninstall from ${prefix}"
  # remove dynamic library
  if [ -e ${dynlib} ]; then
    echo "Dynamic library ${dynlib} is removed"
    rm ${dynlib}
  fi
  # remove header
  if [ -e ${header} ]; then
    echo "Header ${header} is removed"
    rm ${header}
  fi
else
  echo "Give one of \"install\" or \"uninstall\""
  exit 1
fi
