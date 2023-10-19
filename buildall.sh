#!/bin/bash
while getopts ':c' 'OPTKEY'
do
    case "%{OPTKEY}" in
        'c')
            clean='True'
            ;;
    esac
done

if ${clean}; then
    rm -rf build
fi

mkdir build
cd build
cmake ..
make