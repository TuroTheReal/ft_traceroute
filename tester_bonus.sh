#!/bin/bash

FT_TRACEROUTE="./ft_traceroute"
SYS_PING=$(command -v traceroute)
TIMEOUT_DURATION=1

test_cases=(
	"-i eth0 8.8.8.8"
	"-i wlan0 google.com"
	"-i not_an_iface 127.0.0.1"
	"-q 3 8.8.8.8"
	"-q 1 google.com"
	"-q 0 localhost"
	"-q notnumber 192.0.2.123"
	"-m 30 8.8.8.8"
	"-m 5 google.com"
	"-m 0 localhost"
	"-m -1 127.0.0.1"
	"-p 33434 8.8.8.8"
	"-p 53 google.com"
	"-p 999999 notarealhost.tld"
	"-p notaport 192.0.2.123"
	"-i eth0 -q 2 -m 20 -p 33434 google.com"
	"-i wlan0 -q 5 -m 64 -p 33434 8.8.8.8"
)

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
GRAY='\033[0;90m'
NC='\033[0m'

run_test() {
	local args="$1"

	echo -e "${YELLOW}=== Testing: ${NC}$args ${YELLOW}===${NC}"

	echo -e "${GREEN}--- ft_traceroute ---${NC}"
	sudo timeout -s SIGINT $TIMEOUT_DURATION $FT_TRACEROUTE $args 2>&1
	local ft_exit=$?
	echo -e "${GRAY}[Exit code: $ft_exit]${NC}"

	echo -e "${GREEN}--- system ping ---${NC}"
	timeout -s SIGINT $TIMEOUT_DURATION $SYS_PING $args 2>&1
	local sys_exit=$?
	echo -e "${GRAY}[Exit code: $sys_exit]${NC}"

	echo -e "${BLUE}==============================${NC}"
	echo
}


for case in "${test_cases[@]}"; do
	run_test "$case"
	sleep 2
done