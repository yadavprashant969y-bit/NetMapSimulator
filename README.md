# NetMap Simulator

## 2.1 Project Title
**NetMap Simulator** - A comprehensive network operations simulation tool.

## 2.2 Problem Statement
Modern computer networks are complex systems that require efficient management of routers, firewalls, traffic routing, bandwidth allocation, and device tracking. The problem is to design a software simulator that can efficiently model these various network components and their operations, ensuring fast lookups, optimal pathfinding, and reliable task execution, thereby demonstrating the practical application of core data structures and algorithms.

## 2.3 Objectives
- To simulate the configuration and retrieval of router data efficiently.
- To implement a robust mechanism for applying and rolling back firewall rules.
- To manage and process network traffic packets in a sequential order.
- To facilitate rapid device lookup by IP address.
- To analyze network link utilization and identify potential bottlenecks.
- To compute the fastest routing path between network nodes based on latency.
- To allocate bandwidth to network tasks based on priority levels.

## 2.4 System Overview / Architecture
The **NetMap Simulator** is a console-based C++ application that provides a unified menu-driven interface to manage simulated network entities. The system architecture is built around a central `NetMapSimulator` class that encapsulates the state of the network (routers, devices, links, connections) and provides methods to manipulate and query this state. The simulation is broken down into seven distinct functional modules, each leveraging specific data structures tailored for optimal performance.

## 2.5 Data Structures and Algorithms Used
1. **Hash Map (`std::unordered_map`)**: Used for the Router Database to enable $O(1)$ average-time complexity for adding and retrieving router configurations by Router ID.
2. **Stack (LIFO via `std::vector`)**: Used for the Firewall Undo mechanism, allowing the most recently applied rule to be rolled back efficiently ($O(1)$).
3. **Queue (FIFO via `std::queue`)**: Used for the Traffic Line to process data packets in the exact order they arrive ($O(1)$ enqueue and dequeue).
4. **Sorted Array + Binary Search**: Used for Address Lookup. Devices are stored in an array and sorted by IP. Binary search provides $O(\log N)$ lookup time.
5. **Array Sorting (`std::sort`)**: Used for the Usage Sorter to order network links by utilization in descending order ($O(N \log N)$), helping to identify bottlenecks.
6. **Graph (Adjacency List) & Dijkstra's Algorithm**: The Network Map is represented as a graph. Dijkstra's Algorithm (with a Min-Priority Queue) is used to find the shortest path (Fastest Route) based on latency.
7. **Max-Heap (Priority Queue)**: Used for the Bandwidth Allocator to ensure that tasks with the highest priority level are allocated bandwidth first ($O(\log N)$ insertion and removal).

## 2.6 Implementation Approach
The project follows an Object-Oriented Programming (OOP) approach. All network entities (e.g., `RouterConfig`, `FirewallRule`, `DataPacket`, `Device`, `NetworkLink`, `Task`) are modeled as `struct`s. The core logic is encapsulated within the `NetMapSimulator` class, which maintains the corresponding data structures as private members and exposes public methods for each network operation. The `main()` function drives the program using an interactive CLI menu, allowing the user to select specific operations or run an automated test suite.

## 2.7 Time and Space Complexity Analysis

| Module | Operation | Time Complexity | Space Complexity |
| :--- | :--- | :--- | :--- |
| **Router Database** | Add / Get | $O(1)$ average | $O(N)$ |
| **Firewall Undo** | Apply / Undo | $O(1)$ | $O(N)$ |
| **Traffic Line** | Enqueue / Process | $O(1)$ | $O(N)$ |
| **Address Lookup** | Add / Find | Add: $O(1)$, Find: $O(\log N)$* | $O(N)$ |
| **Usage Sorter** | Add / Bottlenecks | Add: $O(1)$, Sort: $O(N \log N)$ | $O(N)$ |
| **Network Map** | Add / Route (Dijkstra) | Add: $O(1)$, Route: $O(E \log V)$ | $O(V + E)$ |
| **Bandwidth Allocator**| Request / Allocate | Request: $O(\log N)$, Alloc: $O(\log N)$| $O(N)$ |

*\* Note: Address lookup requires sorting the list once ($O(N \log N)$) if modifications were made before performing the binary search.*

## 2.8 Execution Steps
To compile and run the project locally on your machine:
1. Ensure you have a C++ compiler installed (e.g., `g++` or `clang++`).
2. Open a terminal and navigate to the `src` directory of the project.
3. Compile the source code using the following command:
   ```bash
   g++ -std=c++11 NetMapSimulator.cpp -o NetMapSimulator
   ```
4. Execute the compiled program:
   ```bash
   ./NetMapSimulator
   ```
5. Follow the on-screen menu to interact with the simulator, or select option `8` to run the automated test sequence.

## 2.9 Sample Inputs and Outputs

**Sample Input** (Choosing the Automated Test and Exiting):
```text
8
0
```

**Sample Output**:
```text
=== NetMap Simulator Menu ===
1. Router DB (Add/Get)
2. Firewall (Apply/Undo)
3. Traffic Line (Enqueue/Process)
4. Address Lookup (Add/Find)
5. Usage Sorter (Add Link/Bottlenecks)
6. Network Map (Add Conn/Route)
7. Bandwidth Allocator (Request/Alloc)
8. Run Automated Test
0. Exit
Choice: 8

--- Running Full Automated Test ---
[Router Database] Added config for Router ID: RTR-Core
[Router Database] Found config for Router ID: RTR-Core (IP: 10.0.0.1)
[Firewall] Applied rule: R1 (DENY 1.1.1.1 -> 2.2.2.2)
[Firewall] Undid rule: R1
[Traffic Line] Enqueued packet: P1
[Traffic Line] Processed packet: P1 from 10.0.0.5 to 8.8.8.8
[Address Lookup] Added device: 192.168.1.5
[Address Lookup] Device list sorted for fast lookup.
[Address Lookup] Found device: 192.168.1.5 (MAC: MAC-AB)
[Usage Sorter] Added link: L1 (Util: 90%)

[Usage Sorter] Network Links Sorted by Utilization:
  - Link L1: 90% [BOTTLENECK WARNING]
[Network Map] Added connection: HQ <--> Branch (Latency: 35ms)
[Fastest Route] Shortest path from HQ to Branch: HQ -> Branch (Total Latency: 35ms)
[Bandwidth Allocator] Task T1 requested 500Mbps (Priority: 10)
[Bandwidth Allocator] Allocated 500Mbps to Task: T1 - Vid (Priority: 10)
```

## 2.10 Screenshots
*Please add your execution screenshots in the `screenshots/` directory.*
- `screenshots/menu_view.png`
- `screenshots/automated_test.png`

## 2.11 Results and Observations
The simulator correctly models fundamental network operations. The use of specialized data structures directly impacts performance:
- Using a Hash Map for router configurations drastically reduces lookup times compared to linear searches.
- Representing the network topology as an Adjacency List and applying Dijkstra's Algorithm successfully identified the shortest path with optimal time complexity.
- The Max-Heap priority queue ensured that critical bandwidth tasks were consistently serviced before lower-priority tasks, demonstrating efficient resource management.

## 2.12 Conclusion
The NetMap Simulator project successfully demonstrates how foundational data structures and algorithms are pivotal in solving real-world domain-specific problems like network management. The careful selection of Arrays, Stacks, Queues, Hash Maps, Trees (Heaps), and Graphs allowed the system to remain highly performant and scalable for simulations.
