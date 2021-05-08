#!/bin/bash

echo "";

if [ -f "./a.out" ]; then
  echo -e "Removing \"./a.out\"\n";
  rm "./a.out";
fi

if [ -f "./output.txt" ]; then
  echo -e "Removing \"./output.txt\"\n";
  rm "./output.txt";
fi

if [ -f "./cuda_error.txt" ]; then
  echo -e "Removing \"./cuda_error.txt\"\n";
  rm "./cuda_error.txt";
fi

# I know this is ugly (sadface)
echo -e "Removing \"./Job.sh.*\"\n";
rm -f ./Job.sh.*;
