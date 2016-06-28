#!/usr/bin/bash

DLLNAME="winopenvxi"
EXANAME="example"

rm -f "$DLLNAME".dll
gcc -static-libgcc -include ../"$EXANAME".h -I../include -I../ ../*.c -shared -o ./"$DLLNAME".dll -lws2_32

