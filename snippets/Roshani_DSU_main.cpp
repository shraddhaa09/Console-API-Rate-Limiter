// #include <iostream>
// #include <unordered_map>
// #include <string>
// #include "Common.h"
// #include "DSU.h"

// using namespace std;

// int main() {
//     cout << "=== DSU Unit Tests ===" << endl;
    
//     DSU dsu;
    
//     // Test 1: Basic IP insertion and find
//     cout << "\nTest 1: Basic Operations" << endl;
//     dsu.makeSet("192.168.1.1");
//     dsu.makeSet("192.168.1.2");
//     cout << "✓ IPs initialized" << endl;
    
//     // Test 2: Union operation
//     cout << "\nTest 2: Union Operation" << endl;
//     dsu.unite("192.168.1.1", "192.168.1.2");
//     string root1 = dsu.find("192.168.1.1");
//     string root2 = dsu.find("192.168.1.2");
    
//     if (root1 == root2) {
//         cout << "✓ IPs successfully united (same root: " << root1 << ")" << endl;
//     } else {
//         cout << "✗ Union failed!" << endl;
//     }
    
//     // Test 3: Ban single IP
//     cout << "\nTest 3: Ban Single IP" << endl;
//     dsu.banIP("10.0.0.1");
    
//     if (dsu.isBanned("10.0.0.1")) {
//         cout << "✓ IP successfully banned" << endl;
//     } else {
//         cout << "✗ Ban failed!" << endl;
//     }
    
//     if (!dsu.isBanned("192.168.1.1")) {
//         cout << "✓ Non-banned IP correctly identified as clean" << endl;
//     } else {
//         cout << "✗ False positive on clean IP!" << endl;
//     }
    
//     // Test 4: Botnet detection (unite multiple IPs, ban one, all should be banned)
//     cout << "\nTest 4: Botnet Detection" << endl;
//     dsu.makeSet("10.0.0.2");
//     dsu.makeSet("10.0.0.3");
//     dsu.makeSet("10.0.0.4");
    
//     // Group them as a botnet
//     dsu.unite("10.0.0.2", "10.0.0.3");
//     dsu.unite("10.0.0.3", "10.0.0.4");
    
//     // Ban just one IP from the botnet
//     dsu.banIP("10.0.0.3");
    
//     // All three should now be banned
//     if (dsu.isBanned("10.0.0.2") && dsu.isBanned("10.0.0.3") && dsu.isBanned("10.0.0.4")) {
//         cout << "✓ Entire botnet successfully banned via single ban operation" << endl;
//     } else {
//         cout << "✗ Botnet banning incomplete!" << endl;
//     }
    
//     // Test 5: Path compression efficiency
//     cout << "\nTest 5: Path Compression" << endl;
//     // Create a chain: ip5 -> ip6 -> ip7 -> ip8
//     for (int i = 5; i <= 8; i++) {
//         dsu.makeSet("10.0.0." + to_string(i));
//     }
//     dsu.unite("10.0.0.5", "10.0.0.6");
//     dsu.unite("10.0.0.6", "10.0.0.7");
//     dsu.unite("10.0.0.7", "10.0.0.8");
    
//     string root = dsu.find("10.0.0.5");
//     cout << "✓ Path compression applied (root: " << root << ")" << endl;
    
//     // Test 6: Debug state
//     cout << "\nTest 6: Current DSU State" << endl;
//     dsu.debugPrint();
    
//     // Test 7: Cluster size
//     cout << "\nTest 7: Cluster Size Query" << endl;
//     int botnetSize = dsu.getClusterSize("10.0.0.2");
//     cout << "Botnet cluster size: " << botnetSize << endl;
    
//     cout << "\n=== All Tests Complete ===" << endl;
    
//     return 0;
// }