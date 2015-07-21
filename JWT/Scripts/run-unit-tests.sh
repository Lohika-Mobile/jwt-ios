#!/bin/bash

BASE_DIR=`dirname $0`
pushd "$BASE_DIR"/.. >/dev/null

XCODE_PROJECT=JWT.xcodeproj

if [ "$1" == "debug" ]; then
    BUILD_SCHEME="JWT-Debug"
elif [ "$1" == "release" ]; then
    BUILD_SCHEME="JWT-Release"
else
    echo "ERROR: incorrect parameter - $1"
    echo "Valid values: debug, release"
    exit 1
fi

BUILD_ARCHITECTURE="$2"

function main()
{
    checkTools

    TEST_START_TIMESTAMP=`date +%s`

    build "$XCODE_PROJECT" $BUILD_ARCHITECTURE $BUILD_SCHEME

    TEST_STOP_TIMESTAMP=`date +%s`
    echo "Tests run in $(( ($TEST_STOP_TIMESTAMP-$TEST_START_TIMESTAMP) )) seconds"
}

function checkTools()
{
    if ! type git &>/dev/null; then
        echo "You must have git installed to run this script"
        popd >/dev/null
        exit 1
    fi
}

function build()
{
    BUILD_OUTPUT_DIR="Autobuild"
    TEST_RESULTS_DIR="$BUILD_OUTPUT_DIR"/unit-tests

    PROJECT=$1
    ARCHITECTURE=$2
    SCHEME=$3

    rm -rf "${BUILD_OUTPUT_DIR}"
    mkdir -p "${TEST_RESULTS_DIR}"

    case "$ARCHITECTURE" in
        armv7) BUILD_TARGET=iphoneos
            ;;
        armv7s) BUILD_TARGET=iphoneos
            ;;
        arm64) BUILD_TARGET=iphoneos
            ;;
        i386) BUILD_TARGET=iphonesimulator
            ;;
        x86_64) BUILD_TARGET=iphonesimulator
            ;;
    esac

    if [ "$BUILD_TARGET" == "iphoneos" ]; then

        message=`./Scripts/DeviceAccessTool -o udid 2>&1`
        DEVICE_UUID=`echo "${message##* }"`
        echo "${DEVICE_UUID}"

        xcodebuild \
            -scheme $SCHEME \
            -project $PROJECT \
            -sdk $BUILD_TARGET \
            -destination id="${DEVICE_UUID}" \
            COLLECT_COVERAGE_DATA=YES \
            BUILD_TARGET_PLATFORM="$BUILD_TARGET" \
            clean \
            test | xcpretty --report junit --color --output "${TEST_RESULTS_DIR}"/junit.xml && exit ${PIPESTATUS[0]}
            if [ "$?" -ne "0" ]; then
                echo "ERROR: Failed to test $PROJECT for $BUILD_TARGET $ARCHITECTURE architecture"
                popd >/dev/null
                exit 1
            fi
    fi

    if [ "$BUILD_TARGET" == "iphonesimulator" ]; then
        xcodebuild \
            -scheme $SCHEME \
            -project $PROJECT \
            -sdk $BUILD_TARGET \
            COLLECT_COVERAGE_DATA=YES \
            BUILD_TARGET_PLATFORM="$BUILD_TARGET" \
            clean \
            test | xcpretty --report junit --color --output "${TEST_RESULTS_DIR}"/junit.xml && exit ${PIPESTATUS[0]}
        if [ "$?" -ne "0" ]; then
            echo "ERROR: Failed to test $PROJECT for $BUILD_TARGET $ARCHITECTURE architecture"
            popd >/dev/null
            exit 1
        fi
    fi
}

echo "****** Building… *********"
main

if [ "$?" -ne "0" ]; then
    echo "****** Failed! *******"
    popd >/dev/null
    exit 1
else
    echo "****** Done! *************"
fi

popd >/dev/null