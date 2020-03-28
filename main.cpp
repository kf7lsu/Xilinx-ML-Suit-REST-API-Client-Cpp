#include <iostream>
#include <vector>

#include "BaseRestClient.h"

int main() {
  std::vector<float> request;
  for (int i = 0; i < 10*224*224*3; i++) {
    request.push_back(0);
  }
  RestAPIClient::BaseClient client(request);
  std::cout << client.getReply() << std::endl;
  
  return 0;
}