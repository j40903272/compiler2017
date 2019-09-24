#!/bin/bash

echo "[assign]"
./run.sh ./parser ~/Desktop/compiler-hw/hw5/pattern_extra/assign.c
./a.out > tmp
diff -y tmp ~/Desktop/compiler-hw/hw5/pattern_extra/answer/assign

echo
echo "[control]"
./run.sh ./parser ~/Desktop/compiler-hw/hw5/pattern_extra/control.c
./a.out > tmp
diff -y tmp ~/Desktop/compiler-hw/hw5/pattern_extra/answer/control

echo
echo "[expr]"
./run.sh ./parser ~/Desktop/compiler-hw/hw5/pattern_extra/expr.c
./a.out > tmp
diff -y tmp ~/Desktop/compiler-hw/hw5/pattern_extra/answer/expr

echo
echo "[func]"
./run.sh ./parser ~/Desktop/compiler-hw/hw5/pattern_extra/func.c
./a.out > tmp
diff -y tmp ~/Desktop/compiler-hw/hw5/pattern_extra/answer/func

echo
echo "[while]"
./run.sh ./parser ~/Desktop/compiler-hw/hw5/pattern_extra/while.c
./a.out > tmp
diff -y tmp ~/Desktop/compiler-hw/hw5/pattern_extra/answer/while