#!/bin/bash

clang-format --verbose --style=file -i ./src/**/*.cpp ./include/**/*.hpp ./tests/*.cpp

