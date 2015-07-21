#!/bin/bash

if [ "$COLLECT_COVERAGE_DATA" == "YES" ]; then
    echo Running Clean ... "${CONFIGURATION_TEMP_DIR}"
    find "${CONFIGURATION_TEMP_DIR}/Library.build" -name "*.gcda" -print0 | xargs -0 rm

    # re-create output folder
    rm -rf "${TEST_RESULTS_DIR_NAME}"
    mkdir -p "${TEST_COVERAGE_DIR_NAME}"
fi

