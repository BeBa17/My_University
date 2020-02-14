#!/bin/bash

for f in input/beasley/*
do
    ./jobshop.o b $f
done
for y in input/tailard/*
do
    ./jobshop.o t $y
done
