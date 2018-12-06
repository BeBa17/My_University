#!/bin/bash

for f in output/*
do
    head -n 1 $f >> wyniki.txt
done
