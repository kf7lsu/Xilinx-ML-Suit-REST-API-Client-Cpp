#include <iostream>
#include <vector>

#include "BaseRestClient.h"

int main() {
  std::vector<float> request;
  request.push_back(1.2);
  request.push_back(3);
  request.push_back(0.000000000001);
  RestAPIClient::BaseClient client(request);
  std::cout << client.getReply() << std::endl;
  
  return 0;
}