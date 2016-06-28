#!/usr/bin/bash

DLLNAME="winopenvxi"
EXANAME="example"

rm -f "$DLLNAME".dll "EXANAME".exe
shlc=$(ls ../*.c | grep -v "$EXANAME")
gcc -static-libgcc -include ../"$EXANAME".h -I../include -I../ $shlc -shared -o ./"$DLLNAME".dll -lws2_32
gcc -static-libgcc -I../include -I../ -o "$EXANAME".exe ../"$EXANAME".c "$DLLNAME".dll -lws2_32

