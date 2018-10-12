#!/bin/bash

export ARNOLD_PATH=/opt/solidangle/arnold/5.2.0.1
c++ ar_polyplane.cpp -o ar_polyplane.so -std=c++11 -Wall -O2 -shared -fPIC -I$ARNOLD_PATH/include -L$ARNOLD_PATH/bin -lai