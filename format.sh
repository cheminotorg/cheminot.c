#!/bin/bash$

astyle --style=allman -N -n src/cheminotc.cpp
astyle --style=allman -N -n src/cheminotc.h
astyle --style=allman -N -n src/fastmktime/fastmktime.cpp
astyle --style=allman -N -n src/fastmktime/fastmktime.h
astyle --style=allman -N -n tests/main.cpp
astyle --style=allman -N -n tests/unittests.cpp
