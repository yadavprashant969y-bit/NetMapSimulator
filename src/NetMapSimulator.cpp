#include <algorithm>
#include <climits>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

// --- Data Structures for Network Entities ---
struct RouterConfig { string id, ipAddress, firmwareVersion; };
struct FirewallRule { string ruleId, action, sourceIp, destIp; };
struct DataPacket { string packetId, sourceIp, destIp; int payloadSize; string timestamp; };
struct Device {
  string ipAddress, macAddress, deviceType;
  // Overload < operator to sort devices by IP address for Binary Search
  bool operator<(const Device &o) const { return ipAddress < o.ipAddress; }
};
struct NetworkLink { string linkId, sourceNode, destNode; double utilization; int capacity; };
struct Task {
  string taskId; int priorityLevel, requiredBandwidthMbps; string description;
  bool operator<(const Task &o) const { return priorityLevel < o.priorityLevel; }
};
struct Edge { string dest; int latency; };
struct Node {
  string id; int dist;
  bool operator>(const Node &o) const { return dist > o.dist; }
};

class NetMapSimulator {
private:
  // 1. Router Database: Hash Map (unordered_map) -> O(1) lookups
  
  unordered_map<string, RouterConfig> routerDatabase;

  // 2. Firewall Undo: Stack (LIFO behavior using a vector)
  
  vector<FirewallRule> firewallRules;

  // 3. Traffic Line: Queue (FIFO)
 
  queue<DataPacket> trafficQueue;

  // 4. Address Lookup: Sorted Array + Binary Search -> O(log N) lookup
 
  
  vector<Device> deviceList;
  bool isDeviceListSorted = false;

  // 5. Usage Sorter: Array/Vector with Sorting Algorithm
  
  vector<NetworkLink> networkLinks;

  // 6. Network Map & Fastest Route: Graph (Adjacency List) & Dijkstra's Algorithm
 
  
  unordered_map<string, vector<Edge>> networkMap;

  // 7. Bandwidth Allocator: Max-Heap (Priority Queue)
  
  priority_queue<Task> taskQueue;

public:
  // --- 1. Router Database Methods ---
  void addRouterConfig(const string &id, const string &ip,
                       const string &firmware) {
    routerDatabase[id] = {id, ip, firmware};
    cout << "[Router Database] Added config for Router ID: " << id << "\n";
  }

  RouterConfig *getRouterConfig(const string &id) {
    auto it = routerDatabase.find(id);
    if (it != routerDatabase.end()) {
      cout << "[Router Database] Found config for Router ID: " << id
           << " (IP: " << it->second.ipAddress << ")\n";
      return &(it->second);
    }
    cout << "[Router Database] Router ID: " << id << " not found.\n";
    return nullptr;
  }

  // --- 2. Firewall Undo Methods ---
  void applyFirewallRule(const string &id, const string &act, const string &src, const string &dest) {
    firewallRules.push_back({id, act, src, dest});
    cout << "[Firewall] Applied rule: " << id << " (" << act << " " << src << " -> " << dest << ")\n";
  }

  void undoLastFirewallRule() {
    if (firewallRules.empty()) {
      cout << "[Firewall] No rules to undo.\n";
      return;
    }
    cout << "[Firewall] Undid rule: " << firewallRules.back().ruleId << "\n";
    firewallRules.pop_back();
  }

  // --- 3. Traffic Line Methods ---
  void enqueuePacket(const string &id, const string &src, const string &dest,
                     int size) {
    trafficQueue.push({id, src, dest, size, "2023-10-26 10:00:00"});
    cout << "[Traffic Line] Enqueued packet: " << id << "\n";
  }

  void processNextPacket() {
    if (trafficQueue.empty()) {
      cout << "[Traffic Line] No packets to process.\n";
      return;
    }
    DataPacket packet = trafficQueue.front();
    trafficQueue.pop();
    cout << "[Traffic Line] Processed packet: " << packet.packetId << " from "
         << packet.sourceIp << " to " << packet.destIp << "\n";
  }

  // --- 4. Address Lookup Methods ---
  void addDevice(const string &ip, const string &mac, const string &type) {
    deviceList.push_back({ip, mac, type});
    isDeviceListSorted = false;
    cout << "[Address Lookup] Added device: " << ip << "\n";
  }

  Device *findDeviceByIp(const string &ip) {
    if (!isDeviceListSorted) {
      sort(deviceList.begin(), deviceList.end());
      isDeviceListSorted = true;
      cout << "[Address Lookup] Device list sorted for fast lookup.\n";
    }

    // Binary search (O(log N)) implementation for a sorted list
    int left = 0, right = deviceList.size() - 1;
    while (left <= right) {
      int mid = left + (right - left) / 2;
      if (deviceList[mid].ipAddress == ip) {
        cout << "[Address Lookup] Found device: " << ip << " (MAC: " << deviceList[mid].macAddress << ")\n";
        return &deviceList[mid];
      }
      if (deviceList[mid].ipAddress < ip) left = mid + 1;
      else right = mid - 1;
    }
    cout << "[Address Lookup] Device " << ip << " not found.\n";
    return nullptr;
  }

  // --- 5. Usage Sorter Methods ---
  void addNetworkLink(const string &id, const string &src, const string &dest, double util, int cap) {
    networkLinks.push_back({id, src, dest, util, cap});
    cout << "[Usage Sorter] Added link: " << id << " (Util: " << util << "%)\n";
  }

  void identifyBottlenecks() {
    // Sort descending by utilization
    sort(networkLinks.begin(), networkLinks.end(), [](const NetworkLink& a, const NetworkLink& b) {
      return a.utilization > b.utilization;
    });
    cout << "\n[Usage Sorter] Network Links Sorted by Utilization:\n";
    for (const auto& link : networkLinks) {
      cout << "  - Link " << link.linkId << ": " << link.utilization << "%";
      if (link.utilization > 80.0) cout << " [BOTTLENECK WARNING]";
      cout << "\n";
    }
  }

  // --- 6. Network Map & Fastest Route Methods ---
  void addNetworkConnection(const string &src, const string &dest,
                            int latency) {
    networkMap[src].push_back({dest, latency});
    networkMap[dest].push_back({src, latency}); // Bidirectional
    cout << "[Network Map] Added connection: " << src << " <--> " << dest
         << " (Latency: " << latency << "ms)\n";
  }

  void findFastestRoute(const string &source, const string &destination) {
    unordered_map<string, int> distances;
    unordered_map<string, string> previous;
    // Min-Priority Queue for Dijkstra
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    for (const auto &pair : networkMap) {
      distances[pair.first] = INT_MAX;
    }

    if (distances.find(source) == distances.end() ||
        distances.find(destination) == distances.end()) {
      cout
          << "[Fastest Route] Error: Source or destination not found in map.\n";
      return;
    }

    distances[source] = 0;
    pq.push({source, 0});

    while (!pq.empty()) {
      Node current = pq.top();
      pq.pop();

      if (current.id == destination)
        break; // Optimized to stop at destination
      if (current.dist > distances[current.id])
        continue; // Stale path in heap

      for (const Edge &edge : networkMap[current.id]) {
        int newDist = distances[current.id] + edge.latency;
        if (newDist < distances[edge.dest]) {
          distances[edge.dest] = newDist;
          previous[edge.dest] = current.id;
          pq.push({edge.dest, newDist});
        }
      }
    }

    if (distances[destination] == INT_MAX) {
      cout << "[Fastest Route] No route found from " << source << " to "
           << destination << ".\n";
      return;
    }

    // Path Reconstruction
    vector<string> path;
    for (string at = destination; at != ""; at = previous[at]) {
      path.push_back(at);
    }
    reverse(path.begin(), path.end());

    cout << "[Fastest Route] Shortest path from " << source << " to "
         << destination << ": ";
    for (size_t i = 0; i < path.size(); ++i) {
      cout << path[i] << (i < path.size() - 1 ? " -> " : "");
    }
    cout << " (Total Latency: " << distances[destination] << "ms)\n";
  }

  // --- 7. Bandwidth Allocator Methods ---
  void requestBandwidth(const string &taskId, int priority, int bandwidth,
                        const string &desc) {
    taskQueue.push({taskId, priority, bandwidth, desc});
    cout << "[Bandwidth Allocator] Task " << taskId << " requested "
         << bandwidth << "Mbps (Priority: " << priority << ")\n";
  }

  void allocateBandwidth() {
    if (taskQueue.empty()) {
      cout << "[Bandwidth Allocator] No tasks pending.\n";
      return;
    }
    Task topTask = taskQueue.top(); // Retrieves highest priority O(1)
    taskQueue.pop();                // Removes highest priority O(log N)
    cout << "[Bandwidth Allocator] Allocated " << topTask.requiredBandwidthMbps
         << "Mbps to Task: " << topTask.taskId << " - " << topTask.description
         << " (Priority: " << topTask.priorityLevel << ")\n";
  }
};

// --- Driver Program ---
void showMenu() {
  cout << "\n=== NetMap Simulator Menu ===\n"
       << "1. Router DB (Add/Get)\n2. Firewall (Apply/Undo)\n"
       << "3. Traffic Line (Enqueue/Process)\n4. Address Lookup (Add/Find)\n"
       << "5. Usage Sorter (Add Link/Bottlenecks)\n6. Network Map (Add Conn/Route)\n"
       << "7. Bandwidth Allocator (Request/Alloc)\n8. Run Automated Test\n0. Exit\nChoice: ";
}

int main() {
  NetMapSimulator sim; int c, sc, i1, i2; string s1, s2, s3, s4; double d1;
  do {
    showMenu();
    if (!(cin >> c)) { cin.clear(); cin.ignore(10000, '\n'); continue; }
    switch (c) {
    case 1: cout << "1. Add 2. Get: "; cin >> sc;
            if (sc==1) { cout<<"ID IP FW: "; cin>>s1>>s2>>s3; sim.addRouterConfig(s1,s2,s3); }
            else { cout<<"ID: "; cin>>s1; sim.getRouterConfig(s1); } break;
    case 2: cout << "1. Apply 2. Undo: "; cin >> sc;
            if (sc==1) { cout<<"ID Action Src Dest: "; cin>>s1>>s2>>s3>>s4; sim.applyFirewallRule(s1,s2,s3,s4); }
            else sim.undoLastFirewallRule(); break;
    case 3: cout << "1. Enqueue 2. Process: "; cin >> sc;
            if (sc==1) { cout<<"ID Src Dest Size: "; cin>>s1>>s2>>s3>>i1; sim.enqueuePacket(s1,s2,s3,i1); }
            else sim.processNextPacket(); break;
    case 4: cout << "1. Add 2. Find: "; cin >> sc;
            if (sc==1) { cout<<"IP MAC Type: "; cin>>s1>>s2>>s3; sim.addDevice(s1,s2,s3); }
            else { cout<<"IP: "; cin>>s1; sim.findDeviceByIp(s1); } break;
    case 5: cout << "1. Add Link 2. Bottlenecks: "; cin >> sc;
            if (sc==1) { cout<<"ID Src Dest Util Cap: "; cin>>s1>>s2>>s3>>d1>>i1; sim.addNetworkLink(s1,s2,s3,d1,i1); }
            else sim.identifyBottlenecks(); break;
    case 6: cout << "1. Add Conn 2. Route: "; cin >> sc;
            if (sc==1) { cout<<"Src Dest Latency: "; cin>>s1>>s2>>i1; sim.addNetworkConnection(s1,s2,i1); }
            else { cout<<"Src Dest: "; cin>>s1>>s2; sim.findFastestRoute(s1,s2); } break;
    case 7: cout << "1. Request 2. Allocate: "; cin >> sc;
            if (sc==1) { cout<<"Task Prio BW Desc: "; cin>>s1>>i1>>i2>>s2; sim.requestBandwidth(s1,i1,i2,s2); }
            else sim.allocateBandwidth(); break;
    case 8: cout << "\n--- Running Full Automated Test ---\n";
            sim.addRouterConfig("RTR-Core", "10.0.0.1", "Cisco"); sim.getRouterConfig("RTR-Core");
            sim.applyFirewallRule("R1", "DENY", "1.1.1.1", "2.2.2.2"); sim.undoLastFirewallRule();
            sim.enqueuePacket("P1", "10.0.0.5", "8.8.8.8", 1500); sim.processNextPacket();
            sim.addDevice("192.168.1.5", "MAC-AB", "PC"); sim.findDeviceByIp("192.168.1.5");
            sim.addNetworkLink("L1", "A", "B", 90.0, 1000); sim.identifyBottlenecks();
            sim.addNetworkConnection("HQ", "Branch", 35); sim.findFastestRoute("HQ", "Branch");
            sim.requestBandwidth("T1", 10, 500, "Vid"); sim.allocateBandwidth(); break;
    }
  } while (c != 0);
  return 0;
}
