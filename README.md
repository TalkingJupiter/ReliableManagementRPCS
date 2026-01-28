# Reliable Remote Management of REPACSS Cluster

### Requirements: Heartbeat
Purpose: The heartbeat mechanism is designed to ensure reliable operation and fault tolerance by continuously monitoring the health of the primary ESP32 and enabling seamless failover to a standby ESP32 when necessary.

- The primary ESP32 shall transmit a heartbeat signal at a fixed interval not exceeding 500 ms.

- The standby ESP32 shall continuously monitor the heartbeat signal to assess the operational status of the primary ESP32.

- A heartbeat timeout or more than 3 missed heartbeat shall be interpreted as a primary ESP32 failure.

- Upon detecting a heartbeat failure, the standby ESP32 shall automatically assume responsibility for system communication without requiring manual intervention.

- A heartbeat failure event shall trigger a notification to the system administrator or maintenance team for awareness and diagnostics.

### Requirements: Relay Control
Purpose: The relay control mechanism ensures safe and deterministic switching of communication or sensor lines between the primary and standby ESP32s, enabling controlled failover and preventing bus contention during normal operation and fault conditions.

- The relay shall default to connecting the system to the primary ESP32 during normal operation.

- Relay state changes shall be controlled exclusively by validated system logic (e.g., heartbeat status or explicit failover conditions).

- Upon detection of a primary ESP32 failure, the relay shall switch control to the standby ESP32 within a bounded and predictable time window.

- The relay shall prevent simultaneous connection of both ESP32s to shared communication lines to avoid electrical conflicts.

- Relay switching shall occur only after the primary ESP32 is deemed inactive or unresponsive, as determined by the heartbeat mechanism.

- Relay state transitions shall be logged or reported to the monitoring system for diagnostics and maintenance purposes.
The relay control logic shall support recovery, allowing control to return to the primary ESP32 once it is verified to be operational and stable.

### Requirements: Sensor Bus Configuration
Purpose: The sensor bus architecture provides redundant access to environmental sensors by implementing two independent buses (front and back) while ensuring electrical isolation and controlled selection using analog SPDT switches.

- The system shall implement two sensor buses, designated as the front bus and the back bus.

- Both sensor buses shall share the same ESP32 GPIO pins, with bus selection performed through SPDT switches.

- Each sensor bus shall be connected to the ESP32 through a dedicated SPDT switch to ensure electrical isolation.

- Only one sensor bus shall be connected to the ESP32 at any time, preventing bus contention and undefined electrical states.

- SPDT switch control shall be managed by system logic and coordinated with heartbeat and failover mechanisms.

- Upon failover, the active SPDT switch shall select the appropriate sensor bus without requiring firmware pin reconfiguration.

- A failure or short on one sensor bus shall not electrically affect the other bus due to SPDT-based isolation.

- The active sensor bus state shall be observable and reportable for monitoring and diagnostics.

### Requirements: Network Communication
Purpose: The network communication subsystem ensures reliable data delivery by prioritizing wired Ethernet connectivity while providing automatic wireless failover to maintain system availability during network faults.

- The system shall use Ethernet as the primary communication protocol for all normal data transmission and control traffic.

- The system shall support Wi-Fi as a secondary communication path used only when Ethernet connectivity is unavailable or degraded.

- Ethernet link status shall be continuously monitored to detect loss of connectivity or failure conditions.

- Upon detection of an Ethernet failure, the system shall automatically switch communication to Wi-Fi without requiring manual intervention.

- Failover from Ethernet to Wi-Fi shall occur within a bounded and predictable time window to minimize data loss.

- When Ethernet connectivity is restored and verified as stable, the system shall transition back to Ethernet as the primary communication channel.

- Network failover events shall be logged and reported to the monitoring or alerting system for diagnostics and maintenance awareness.

- The failover mechanism shall operate independently of the heartbeat and relay logic but remain coordinated to preserve overall system consistency.

### Requirements: Rack Identification
Purpose: Rack identity shall be assigned and managed centrally by the Radxa cluster to support scalable deployment without requiring firmware changes. The system shall also detect and safely handle telemetry received from unregistered (unknown) ESP32 devices.

- The ESP32 firmware shall transmit telemetry containing a unique device identifier (e.g., MAC address) with every message.

- The Radxa cluster shall maintain a configuration mapping of MAC address → rack_id (and optionally role, location, and metadata).

- The Radxa cluster shall be the source of truth for rack identification and labeling.

- Upon receiving telemetry from a MAC address not present in the Radxa mapping, the system shall:

    - Mark the data as unassigned/unknown (e.g., rack_id = "unknown" or unassigned=true).

    - Store the unknown MAC address and associated metadata (first-seen time, last-seen time, message count).

    - Continue ingesting the data without dropping it, while ensuring it is clearly labeled as unassigned.

**Alerting Policy for Unknown MAC Addresses**

- Unknown MAC address detection shall generate a warning severity event (not an error).

- The system shall send a summarized notification to the administrator once every 24 hours if one or more unknown MAC addresses were observed during that period.

- The daily warning notification shall include, at minimum:

    - List of unknown MAC addresses

    - First-seen and last-seen timestamps (per MAC)

    - Count of messages received (per MAC)

    - Any available network/source details (e.g., IP, interface: Ethernet/Wi-Fi)

**Non-Functional Requirement**

- The unknown-device warning mechanism shall be rate-limited to at most one notification per 24-hour window, to prevent alert fatigue while ensuring visibility for maintenance.