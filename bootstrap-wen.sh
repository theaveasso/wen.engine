#!/bin/bash

echo "Configuring preset $1 ..."
cmake --preset $1

echo "Building preset $1 ..."
cmake --build --preset $1
