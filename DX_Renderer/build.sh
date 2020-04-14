#!/bin/bash

code="$PWD"
opts=-g
cd ../debug > /dev/null
g++ $opts $code/windows -o ../release
cd $code > /dev/null
