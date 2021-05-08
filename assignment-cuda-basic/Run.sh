#!/bin/bash

if [ -f "./a.out" ]; then
  "./a.out" 1000000 1000 1024;
else
  echo -e "\nERROR: You must compile before running program!\n";
fi
