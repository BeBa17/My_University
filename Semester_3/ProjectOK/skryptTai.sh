#!/bin/bash

for f in tailardTesty/*
do
    head -n 1 $f | awk '{print $2;}' >> maszyny.txt
done
