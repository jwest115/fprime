#!/bin/bash
####
# 30-ints.bash:
#
# Integration tests for CI
####
export CTEST_OUTPUT_ON_FAILURE=1

export SCRIPT_DIR="$(dirname ${BASH_SOURCE})"
. "${SCRIPT_DIR}/../helpers.bash"
. ${SCRIPT_DIR}/fputil.bash

#### NEEDED ENVIRONMENT ####
export FPRIME_DIR="$(pwd)"
export LOG_DIR="${FPRIME_DIR}/ci-30-ints-logs-$(date +"%Y-%m-%dT%H%M%S")"
mkdir -p "${LOG_DIR}"

# Directory to be used for Integration CI test
export FPUTIL_DEPLOYS="${FPRIME_DIR}/RPI"

echo -e "${BLUE}Starting CI test ${FPUTIL_DEPLOYS} RPI${NOCOLOR}"

# Run integration tests
echo -e "${BLUE}Testing ${INT_DEPLOYS} against integration tests${NOCOLOR}"
integration_test_run "${FPUTIL_DEPLOYS}" "raspberrypi"

# Test Completed
echo -e "${GREEN}CI test ${FPUTIL_DEPLOYS} Integration SUCCESSFUL${NOCOLOR}"

archive_logs
