#!/bin/bash -e
PROG_TYPE="$1"

CURR_DIR=$(cd $(dirname $0) && pwd )
#echo "Build path: "$CURR_DIR
cd $CURR_DIR

if [ "clean" = "$PROG_TYPE" ]; then
	echo -e "Start to clean all:"
	make clean
else
	cmake ../code/
	make
fi
