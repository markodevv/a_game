#!/bin/bash
gcc -Wall -Wno-write-strings -g -fPIC -shared -I ../../source/ ./namespace_demo.c -o namespace_demo.so
../../build/data_desk --custom ./namespace_demo.so data_v1.ds data_v2.ds
gcc -Wall -Wno-write-strings -g -fPIC -shared -I ../../source/ ./namespace_demo.c -o namespace_demo.so
gcc -Wall -Wno-write-strings -Wno-missing-braces -g conversion.c -o conversion
./conversion

