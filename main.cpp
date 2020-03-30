#include <iostream>
#include <vector>
#include <chrono>

#include "BaseRestClient.h"

int main() {
  int batchSize = 50;
  int numRequests = 100;
  std::chrono::time_point<std::chrono::steady_clock> startTimes[numRequests];
  std::chrono::time_point<std::chrono::steady_clock> endTimes[numRequests];
  int latencies[numRequests];

  std::vector<float> request;
  for (int i = 0; i < batchSize*224*224*3; i++) {
    request.push_back(0);
  }
  RestAPIClient::BaseClient client(request);
  for(int i = 0; i < numRequests; i++){
    RestAPIClient::BaseClient client(request);
    auto startTime = std::chrono::steady_clock::now();
    client.getReply();
    auto endTime = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(endTime-startTime).count() << std::endl;
  }

  return 0;
}
