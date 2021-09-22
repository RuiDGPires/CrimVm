#!/bin/sh

FILE=crimvm
if [ ! -f "$FILE" ]; then
	make clean release
fi

tests/test.py $@

