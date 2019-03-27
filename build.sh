#!/bin/sh

# Create build dir if it doesn't already exist.
if [ ! -d build/ ]; then
    mkdir build/
fi

# Default build type is debug.
build_type=$1
if [ -z $build_type ]; then
    build_type="debug"
fi

case $build_type in
    "debug")
        cmd="cmake -DCMAKE_BUILD_TYPE=Debug ../.. && make"
        ;;
    "release")
        cmd="cmake -DCMAKE_BUILD_TYPE=Release ../.. && make"
        ;;
    "asan")
        cmd="cmake -DCMAKE_BUILD_TYPE=Debug -DSANITIZER_TYPE=ASan ../.. && make"
        ;;
    "ubsan")
        cmd="cmake -DCMAKE_BUILD_TYPE=Debug -DSANITIZER_TYPE=UBSan ../.. && make"
        ;;
    "scan")
        # Need to do a full rebuild for scan-build to work.
        if [ -d build/scan/ ]; then
            rm -rf build/scan/
        fi
        cmd="scan-build cmake ../.. && scan-build -o . make"
        ;;
    *)
        echo "unrecognised build type $build_type"
        exit 1
esac

cd build/
if [ ! -d $build_type ]; then
    mkdir $build_type
fi

cd $build_type

# Execute build cmd.
eval $cmd
