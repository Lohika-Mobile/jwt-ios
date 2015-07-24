#!/bin/bash

if [ "$COLLECT_COVERAGE_DATA" == "YES" ]; then
    echo Running CodeCoverage ... "${CONFIGURATION_TEMP_DIR}"
    if [ "$BUILD_TARGET_PLATFORM" == "iphoneos" ]; then
        echo "Copying CodeCoverage from device ..."
        "${SRCROOT}/Scripts/DeviceAccessTool" -o coverage -app com.lohika.TestHost -target "${PROJECT_NAME}" -to "${OBJROOT}"
    fi

    GCOV_EXCLUDE='(.*./Developer/SDKs/.*)|(.*./Developer/Toolchains/.*)|(.*Tests\.m)|(.*Tests/.*)'
    "${SRCROOT}/Scripts/gcovr" -p -e '.*h$'  -e '.*DD.*' -e '.*GCD.*' -e "${GCOV_EXCLUDE}" "${CONFIGURATION_TEMP_DIR}/Library.build" --html --html-details -o "${TEST_COVERAGE_DIR_NAME}"/coverage.html
    REPORT_NAME=`echo "Code Coverage for " "${BUILD_TARGET_PLATFORM}"`
    sed -i "s/GCC Code Coverage Report/$REPORT_NAME/g" "${TEST_COVERAGE_DIR_NAME}"/coverage.html

    BUILD_DIR="${OBJROOT}"
    LCOV_INFO=lcov.info
    LCOV_EXCLUDE="${LCOV_INFO} '*/Xcode.app/Contents/Developer/*' '*Tests.m' '${BUILD_DIR}/*'"
    LCOV_REPORTS_DIR="${TEST_COVERAGE_DIR_NAME}"/lcov-reports

    lcov --capture --directory "${BUILD_DIR}" --output-file ${LCOV_INFO}
    lcov --remove ${LCOV_EXCLUDE} --output-file ${LCOV_INFO}
    genhtml ${LCOV_INFO} --output-directory ${LCOV_REPORTS_DIR}

fi
