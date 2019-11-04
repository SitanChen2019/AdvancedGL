#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

if [ $1 == "cmake" ]; then
     cmake ${DIR} -B ${DIR}/cmake-build
elif [ $1 == "ide" ]; then
    open -a Xcode ${DIR}/cmake-build/AdvancedGL.xcodeproj
fi

