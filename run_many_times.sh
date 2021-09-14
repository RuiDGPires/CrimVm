#!/bin/bash

FILE=./a.out
test=./tests/inputs/basic/test1.cas
TIMES=300
for ((n=0;n<$TIMES;n++)); do
	${FILE} -ar ${test}
	echo finished...
	echo 
done

