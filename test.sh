#!/bin/sh

FILE=./a.out
if [ ! -f "$FILE" ]; then
	make clean debug 
fi

tests/test.py $@

