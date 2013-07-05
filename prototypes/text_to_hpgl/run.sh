#!/bin/bash
OUT=hello.hpgl
SCRIPT=./to-hpgl.py
touch $OUT
echo "" > $OUT
$SCRIPT H 0 0  >> $OUT
$SCRIPT e 300 0 >> $OUT
$SCRIPT l 600 0 >> $OUT
$SCRIPT l 900 0 >> $OUT
$SCRIPT o 1200 0 >> $OUT

