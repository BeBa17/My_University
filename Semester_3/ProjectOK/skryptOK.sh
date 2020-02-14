#!/bin/bash

for f in beasley/*
do
    ./jobshop2 b $f
    ./jobshopRANDOM3 b $f
    ./jobshopZERO b $f
done
for y in tailard/*
do
    ./jobshop2 t $y
    ./jobshopRANDOM3 t $y
    ./jobshopZERO t $y
done
