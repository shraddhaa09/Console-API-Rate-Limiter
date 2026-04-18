#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "InputHandler.h"

std::vector<Request> loadRequestsFromFile(const std::string& filename) {
    std::vector<Request> requests;
    std::ifstream fin(filename);

    if (!fin.is_open()) {
        std::cout << "Error: Could not open file " << filename << "\n";
        return requests;
    }

    std::string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string userIDStr, path, ip, timestampStr;

        getline(ss, userIDStr, ',');
        getline(ss, path, ',');
        getline(ss, ip, ',');
        getline(ss, timestampStr, ',');

        Request req;
        req.userID = std::stoi(userIDStr);
        req.path = path;
        req.ip = ip;
        req.timestamp = std::stol(timestampStr);

        requests.push_back(req);
    }

    fin.close();
    return requests;
}