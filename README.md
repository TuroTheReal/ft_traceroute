# FT_TRACEROUTE

## Table of Contents

* [About](#about)
* [Installation](#installation)
* [Usage](#usage)
* [Key Concepts Learned](#key-concepts-learned)
* [Skills Developed](#skills-developed)
* [Project Overview](#project-overview)
* [Features Implemented](#features-implemented)
* [Program Architecture](#program-architecture)
* [Testing](#testing)
* [42 School Standards](#42-school-standards)

## About

This repository contains my implementation of the **ft_traceroute** project at 42 School.  
ft_traceroute is a recreation of the famous **traceroute** network diagnostic tool, used worldwide since 1987 to map network paths and diagnose routing issues.  
Built entirely in **C**, this program traces the route packets take across an IP network to reach a destination host using **UDP datagrams** and **ICMP Time Exceeded messages**, revealing every router hop along the path with precise timing measurements.

The project demonstrates mastery of network protocols, raw socket programming, and advanced packet manipulation while implementing the core functionality of the original traceroute utility.

## Installation

### Prerequisites

* **C Compiler** (gcc/clang)
* **Make** utility
* **UNIX/Linux environment** (raw socket programming)
* **Root/sudo privileges** (required for raw socket creation)

### Compilation

```bash
# Clone the repository
git clone https://github.com/TuroTheReal/ft_traceroute.git
cd ft_traceroute

# Compile the program
make

# Compile with bonus features
make bonus

# Clean object files
make clean

# Clean everything
make fclean

# Recompile
make re
```

## Usage

### Basic Usage

```bash
# Trace route to a host (requires sudo for raw sockets)
sudo ./ft_traceroute <hostname or IP>

# Example - Trace route to Google DNS
sudo ./ft_traceroute 8.8.8.8
sudo ./ft_traceroute google.com
```

### Command-Line Options

```bash
# Set maximum hops (default: 30)
sudo ./ft_traceroute -m 15 google.com

# Set number of probes per hop (default: 3)
sudo ./ft_traceroute -q 5 8.8.8.8

# Set UDP port (default: 33434)
sudo ./ft_traceroute -p 44000 8.8.8.8

# Set interval between probes in seconds (default: 0)
sudo ./ft_traceroute -i 1 google.com

# Set wait timeout for responses in seconds (default: 5)
sudo ./ft_traceroute -w 3 8.8.8.8

# Disable DNS resolution (numeric output only)
sudo ./ft_traceroute -n google.com

# Display version
./ft_traceroute -V

# Display help
./ft_traceroute -h
./ft_traceroute -?
```

### Example Output

```bash
$ sudo ./ft_traceroute google.com
traceroute to google.com (142.250.185.46), 30 hops max, 60 byte packets
 1  192.168.1.1 (192.168.1.1)  1.234 ms  1.156 ms  1.089 ms
 2  10.0.0.1 (10.0.0.1)  8.456 ms  8.234 ms  8.123 ms
 3  172.16.1.1 (172.16.1.1)  12.345 ms  12.234 ms  12.156 ms
 4  * * *
 5  142.250.185.46 (142.250.185.46)  15.678 ms  15.567 ms  15.456 ms

$ sudo ./ft_traceroute 8.8.8.8
traceroute to 8.8.8.8 (8.8.8.8), 30 hops max, 60 byte packets
 1  gateway (192.168.1.1)  0.523 ms  0.487 ms  0.445 ms
 2  10.20.30.1 (10.20.30.1)  5.234 ms  5.123 ms  5.089 ms
 3  8.8.8.8 (8.8.8.8)  12.345 ms  12.234 ms  12.156 ms
```

## Key Concepts Learned

### Network Protocol Fundamentals

* **TTL (Time To Live)**: Understanding IP packet hop limits and TTL expiration mechanism
* **ICMP Time Exceeded**: Processing ICMP Type 11 messages from intermediate routers
* **UDP Protocol**: Using UDP datagrams as traceroute probes with incrementing ports
* **Port Unreachable**: Detecting destination with ICMP Type 3 Code 3 messages

### Advanced Socket Programming

* **Raw Socket Creation**: Building SOCK_DGRAM and SOCK_RAW sockets for UDP and ICMP
* **Socket Options**: Setting IP_TTL, SO_RCVTIMEO, and other socket-level options
* **Packet Construction**: Manually crafting UDP probe packets with custom payload
* **Multi-Socket I/O**: Managing separate sockets for sending (UDP) and receiving (ICMP)

### Routing and Network Topology

* **Route Discovery**: Mapping network paths hop-by-hop across internet infrastructure
* **Router Identification**: Extracting router IP addresses from ICMP responses
* **Path Asymmetry**: Understanding that forward and reverse paths may differ
* **Network Latency**: Measuring round-trip time to each hop for performance analysis

### Low-Level C Programming

* **Packet Parsing**: Dissecting IP headers, ICMP headers, and embedded original packets
* **Binary Protocol Handling**: Working with network byte order and protocol structures
* **Pointer Arithmetic**: Navigating nested packet headers (IP → ICMP → IP → UDP)
* **Timeout Management**: Implementing probe timeouts using select() with microsecond precision

## Skills Developed

* **Network Diagnostic Expertise**: Building professional network troubleshooting tools
* **Protocol Stack Understanding**: Deep knowledge of IP, UDP, ICMP interaction
* **Packet-Level Programming**: Mastering raw socket operations and packet crafting
* **Route Analysis Skills**: Identifying network topology and routing behavior
* **Performance Measurement**: Accurate timing with gettimeofday() and usleep()
* **Multi-Probe Coordination**: Managing multiple simultaneous network probes
* **Cross-Platform Networking**: Writing portable network code for Unix-like systems

## Project Overview

ft_traceroute implements the core functionality of the traceroute utility, enabling network administrators and developers to visualize network paths, diagnose routing problems, identify network bottlenecks, and measure hop-by-hop latency.
The program sends UDP probe packets with incrementing TTL values, listening for ICMP Time Exceeded responses from intermediate routers and ICMP Port Unreachable from the final destination.

### Core Components

**TTL-Based Probe System**: Sends UDP probes with TTL starting at 1 and incrementing up to max_hops (default 30), causing packets to expire at each successive router, triggering ICMP Time Exceeded responses that reveal hop addresses.

**UDP Packet Generator**: Constructs UDP datagrams with destination ports starting at 33434 and incrementing for each probe, includes timing payload for RTT calculation, uses SOCK_DGRAM socket with IP_TTL socket option.

**ICMP Response Listener**: Creates raw SOCK_RAW socket to receive ICMP messages, filters for Time Exceeded (Type 11) and Destination Unreachable (Type 3 Code 3), extracts router IP from ICMP source address, validates responses by examining embedded original packet.

**Packet Validator**: Verifies ICMP responses match sent probes by comparing embedded UDP packet headers, checks source/destination IPs and ports, discards spurious ICMP messages from unrelated network traffic.

**RTT Calculator**: Records probe send time with microsecond precision using gettimeofday(), calculates round-trip time when response received, handles timeout detection using select() with configurable timeout (typically 5 seconds).

**Hop Formatter**: Displays each hop with sequential number, performs reverse DNS lookup for router hostnames, shows IP address in parentheses, displays RTT for each probe, marks timeouts with asterisks (*), groups responses from same router.

**Destination Detector**: Recognizes arrival at destination through ICMP Port Unreachable message, stops tracing when destination reached, handles cases where destination doesn't send Port Unreachable (silent hosts).

## Features Implemented

### Core Traceroute Functionality

* **Hop-by-Hop Route Discovery**: Trace network path from source to destination
* **TTL Increment Strategy**: Start at TTL 1, increment until destination reached
* **UDP Probe Mechanism**: Send UDP datagrams as traceroute probes
* **ICMP Response Processing**: Capture and analyze Time Exceeded and Port Unreachable

### Command-Line Options

* **-m (max_hops)**: Set maximum number of hops (default: 30)
* **-q (nqueries)**: Set number of probes per hop (default: 3)
* **-p (port)**: Set base destination UDP port (default: 33434)
* **-i (interval)**: Set interval between probes in seconds (default: 0)
* **-w (wait)**: Set timeout for responses in seconds (default: 5)
* **-n**: Disable DNS resolution, show IPs only
* **-V**: Display version information
* **-h/-?**: Display usage information and available options

### Network Analysis

* **Round-Trip Time Measurement**: Microsecond-precision RTT for each probe
* **Multiple Probes per Hop**: Send multiple probes for reliability and variance analysis
* **Timeout Detection**: Identify unresponsive hops with asterisk (*) markers
* **Router Identification**: Display IP addresses and hostnames of intermediate routers

### DNS Resolution

* **Reverse DNS Lookup**: Convert router IPs to hostnames when available
* **Forward DNS Resolution**: Convert destination hostname to IP address
* **Numeric Mode (-n)**: Skip DNS resolution for faster output
* **Fallback to IP Display**: Show IP addresses when DNS resolution fails
* **Non-Blocking DNS**: Avoid delays from slow DNS queries

### Packet Validation

* **Response Filtering**: Match ICMP responses to sent UDP probes
* **Port Verification**: Ensure ICMP refers to correct UDP destination port
* **Sequence Tracking**: Associate responses with specific probe attempts
* **Duplicate Detection**: Ignore duplicate ICMP responses from same router

### Error Handling

* **Socket Creation Errors**: Handle raw socket privilege requirements
* **Network Unreachable**: Detect and report network connectivity issues
* **Host Unreachable**: Identify unreachable destinations
* **Timeout Management**: Handle probes that receive no response (-w option)
* **Probe Interval Control**: Configure delay between probes to avoid network flooding (-i option)
* **Invalid Hop Count**: Validate user-specified parameters

## Program Architecture

### Multi-Socket Design

Uses dual-socket architecture: SOCK_DGRAM socket for sending UDP probes with IP_TTL option, SOCK_RAW socket for receiving ICMP replies with IPPROTO_ICMP protocol, separates concerns between probe transmission and response reception.

### TTL Progression Algorithm

Outer loop iterates through TTL values (1 to max_hops), inner loop sends multiple probes per TTL (default 3), increments UDP destination port for each probe to uniquely identify responses, terminates when destination reached or max_hops exceeded.

### ICMP Message Validation

Parses IP header to locate ICMP payload, examines ICMP Type and Code fields, extracts embedded original IP and UDP headers from ICMP payload, compares embedded packet against sent probe to confirm match, rejects unrelated ICMP traffic.

### Timeout Strategy

Uses select() with timeout for each probe, typically 5 seconds per probe, allows multiple concurrent probes in flight, tracks which probes are awaiting responses, marks timed-out probes with asterisks.

### Port Increment Technique

Starts with base port (default 33434), increments port for each probe sent, allows packet identification by destination port number, creates unique signature for each probe attempt, destination port range: 33434-33534+ for typical traceroute.

### Response Grouping

Collects all probe responses for each hop before displaying, identifies responses from same IP address, groups RTT measurements for readability, advances to next TTL after completing all probes for current hop.

## Testing

### Basic Route Tracing

```bash
# Test local network paths
sudo ./ft_traceroute 192.168.1.1
sudo ./ft_traceroute localhost
sudo ./ft_traceroute 127.0.0.1

# Test public DNS servers
sudo ./ft_traceroute 8.8.8.8
sudo ./ft_traceroute 1.1.1.1
sudo ./ft_traceroute 9.9.9.9

# Test with domain names
sudo ./ft_traceroute google.com
sudo ./ft_traceroute github.com
sudo ./ft_traceroute cloudflare.com
```

### Option Testing

```bash
# Test maximum hops
sudo ./ft_traceroute -m 10 google.com
sudo ./ft_traceroute -m 5 8.8.8.8

# Test probe count
sudo ./ft_traceroute -q 5 google.com
sudo ./ft_traceroute -q 1 8.8.8.8

# Test custom port
sudo ./ft_traceroute -p 40000 google.com

# Test probe interval
sudo ./ft_traceroute -i 2 google.com
sudo ./ft_traceroute -i 0.5 8.8.8.8

# Test wait timeout
sudo ./ft_traceroute -w 2 google.com
sudo ./ft_traceroute -w 10 8.8.8.8

# Test numeric output (no DNS)
sudo ./ft_traceroute -n google.com
sudo ./ft_traceroute -n 8.8.8.8

# Combined options
sudo ./ft_traceroute -m 20 -q 5 -i 1 -w 3 8.8.8.8
sudo ./ft_traceroute -n -m 15 -q 3 google.com
```

### Network Condition Tests

```bash
# Test with unreachable hosts
sudo ./ft_traceroute 192.168.255.255
sudo ./ft_traceroute 10.0.0.1

# Test with filtered routes (may timeout)
sudo ./ft_traceroute 8.8.8.8 -m 5

# Test with different network paths
sudo ./ft_traceroute asia.example.com
sudo ./ft_traceroute europe.example.com
```

### Comparison with Original Traceroute

```bash
# Compare route discovery
traceroute google.com
sudo ./ft_traceroute google.com

# Compare with numeric mode (no DNS)
traceroute -n 8.8.8.8
sudo ./ft_traceroute -n 8.8.8.8

# Compare timing measurements
traceroute -w 3 8.8.8.8
sudo ./ft_traceroute -w 3 8.8.8.8

# Compare with custom options
traceroute -m 15 -q 5 google.com
sudo ./ft_traceroute -m 15 -q 5 google.com
```

### Edge Cases

```bash
# Test with immediate destination (1 hop)
sudo ./ft_traceroute 192.168.1.1

# Test with many hops
sudo ./ft_traceroute -m 30 distant-server.com

# Test with all timeouts
sudo ./ft_traceroute 192.168.255.255

# Test with Ctrl+C interruption
sudo ./ft_traceroute google.com  # Press Ctrl+C during trace
```

## 42 School Standards

### Project Standards

* ✅ No memory leaks (validated with valgrind)
* ✅ Raw socket implementation for ICMP reception
* ✅ UDP socket for probe transmission
* ✅ Proper TTL-based route discovery algorithm
* ✅ Accurate RTT calculation and display
* ✅ Norm compliance (42 coding standards)

### Network Requirements

* ✅ ICMP Time Exceeded message handling
* ✅ ICMP Destination Unreachable detection
* ✅ UDP probe packet construction
* ✅ Support for hostnames and IP addresses
* ✅ Configurable max hops and probe count
* ✅ Timeout handling for lost probes

### Technical Requirements

* ✅ TTL manipulation with IP_TTL socket option
* ✅ Port increment strategy for probe identification
* ✅ ICMP packet validation and filtering
* ✅ Microsecond-precision timing with gettimeofday()
* ✅ Non-blocking I/O with select() for timeouts
* ✅ Proper cleanup of network resources

### Output Requirements

* ✅ Hop number and router IP/hostname display
* ✅ RTT measurements in milliseconds
* ✅ Asterisk (*) for timed-out probes
* ✅ Destination reached indicator
* ✅ Compatible output format with original traceroute

### Bonus Features (Optional)

* ✅ Probe interval configuration (-i flag)
* ✅ Configurable response timeout (-w flag)
* ✅ Numeric mode without DNS resolution (-n flag)
* ✅ Custom UDP port (-p flag)
* ✅ Maximum hop limit configuration (-m flag)
* ✅ Configurable probe count per hop (-q flag)
* ✅ Version display (-V flag)
* ✅ Multiple help options (-h and -? flags)

## Contact

* **GitHub**: [@TuroTheReal](https://github.com/TuroTheReal)
* **Email**: [arthurbernard.dev@gmail.com](mailto:arthurbernard.dev@gmail.com)
* **LinkedIn**: [Arthur Bernard](https://www.linkedin.com/in/arthurbernard92/)

---

[![Made with C](https://img.shields.io/badge/Made%20with-C-blue.svg)](https://img.shields.io/badge/Made%20with-C-blue.svg)
[![Network Diagnostic](https://img.shields.io/badge/Tool-traceroute-green.svg)](https://img.shields.io/badge/Tool-traceroute-green.svg)
[![Protocol Stack](https://img.shields.io/badge/Protocols-UDP%2FICMP-red.svg)](https://img.shields.io/badge/Protocols-UDP%2FICMP-red.svg)
