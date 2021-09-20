#!/bin/sh

FILE=./crimvm
if [ ! -f "$FILE" ]; then
	make clean debug 
fi

tests/test.py $@

