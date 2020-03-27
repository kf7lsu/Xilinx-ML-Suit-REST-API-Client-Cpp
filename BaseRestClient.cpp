#include <curl/curl.h>
#include <iostream>
#include <fstream>
#include <iterator>

#include "BaseRestClient.h"
#include "Config.cpp"

namespace RestAPIClient {

BaseClient::BaseClient(const std::string fileName) {
  std::ifstream file(fileName);
  if (!file) {
    throw new std::runtime_error(std::string("Rest API Client: failed to open file ") + fileName);
  }

  std::istream_iterator<char> start(file), end;
  requestData = std::vector<char>(start, end);
}

BaseClient::BaseClient(const std::vector<float> requestData) {
  std::string request;
  for (float elem : requestData) {
    char *str;
    asprintf(&str, "%e", elem);
    request += str;
    free(str);
    request += ',';
  }
  request = "[" + request + "]";
  this->requestData = std::vector<char>(request.begin(), request.end());
}

size_t BaseClient::handle_data(const char *data, size_t n, size_t l, void *userp) {
  BaseClient *client = (BaseClient*) userp;
  client->response += std::string(data, n * l);
  return n * l;
}

void BaseClient::sendRequest() {
  if (requestSent) {
    return;
  }

  requestSent = true;

  CURL *curl = curl_easy_init();
  if (!curl) {
    throw std::runtime_error("Rest API Client: Failed to initialize curl");
  }

  // Set URL
  curl_easy_setopt(curl, CURLOPT_URL, Config::SERVER_URL.c_str());

  // Set handler
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &handle_data);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) this);

  // Upload file
  curl_mime *form = curl_mime_init(curl);
  curl_mimepart *field = curl_mime_addpart(form);;
  curl_mime_name(field, "image");
  // curl_mime_data(field, "123abc", 6);
  curl_mime_data(field, requestData.data(), requestData.size());
  curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

  // Send request
  CURLcode res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    curl_easy_cleanup(curl);

    throw std::runtime_error(
      std::string("Rest API Client: curl_easy_perform failed with error code ")
      + std::to_string(res) + " (" + curl_easy_strerror(res) + ")");
  }

  // Check response 
  long response_code;
  res = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
  if((res != CURLE_OK) ||
      ((response_code / 100) != 2)) {
    std::string message;
    message += "Rest API Client: Connection error. error code = ";
    message += std::to_string(res);
    message += ", response code = ";
    message += std::to_string(response_code);
    curl_easy_cleanup(curl);
    throw std::runtime_error(message);
  }

  curl_easy_cleanup(curl);
}

std::string BaseClient::getReply(){
  sendRequest();
  return response;
}

}