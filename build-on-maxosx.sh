#!/bin/bash
make
ranlib ./lib/libloki.a
make
ranlib ./test/Register/libfoo.a
make
