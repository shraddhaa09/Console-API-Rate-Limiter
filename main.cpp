#include <iostream>
#include "Common.h"
#include "InputHandler.h"
#include "BinomialHeap.h"

int main() {
    std::vector<Request> requests = loadRequestsFromFile("requests.txt");
    BinomialHeap penaltyHeap;

    // now process each request...
}