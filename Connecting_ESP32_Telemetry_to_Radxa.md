# Connecting ESP32 Telemetry System to Radxa

This document describes how to connect the ESP32-based rack telemetry system to the Radxa cluster.
It reflects the current project implementation: W5500 Ethernet, UDP JSON telemetry, and heartbeat-based A/B failover.

---

## System Overview

- Radxa acts as the central telemetry sink.
- Each rack contains two ESP32 controllers:
  - Controller A (Primary): actively sends telemetry.
  - Controller B (Standby): monitors A via heartbeat and takes over telemetry transmission if A fails.
- Telemetry is transmitted using Ethernet (W5500) as UDP JSON packets.
- All ESP32 devices send telemetry to a single Radxa IP address.

ESP32 controllers do not coordinate over the network.
Failover coordination occurs only through the local heartbeat link between Controller A and B.

---

## Network Topology

```
[ESP32 + W5500] ┐
[ESP32 + W5500] ├── Ethernet Switch ── Radxa (eth0)
[ESP32 + W5500] ┘
        (14 racks, 2 ESP32 per rack)
```

---

## Step 1: Radxa Network Configuration

Assign a static IP address to the Radxa Ethernet interface.

Example:
IP Address: 192.168.1.10
Netmask:    255.255.255.0

Verify:
ip addr show eth0

---

## Step 2: ESP32 Network Configuration

All ESP32 devices use the same Radxa destination IP and UDP port.

Example:
RADXA_IP = 192.168.1.10
RADXA_UDP_PORT = 9000

Device identity is provided by the ESP32 MAC address inside the telemetry payload.

---

## Step 3: Power-Up Order

1. Power on Radxa.
2. Start the UDP listener.
3. Power on Controller A.
4. Power on Controller B.

---

## Step 4: Radxa UDP Listener

For validation:
sudo nc -klu 9000

Expected:
- One telemetry stream per rack.
- Telemetry initially from Controller A only.

---

## Step 5: Failover Verification

- Normal: A sends, B silent.
- A fails: B starts sending after ~2000 ms.
- A recovers: B stops, A resumes.

Radxa should never receive telemetry from both controllers at once.

---

## Step 6: Rack Identification

Radxa identifies racks using ESP32 MAC addresses.
Maintain a MAC → rack_id mapping.

---

## Step 7: Scaling

The system scales to 14 racks without configuration changes.

---

## Summary

- One Radxa IP
- One UDP port
- Flat Ethernet network
- MAC-based identity
- Heartbeat-based A/B failover
