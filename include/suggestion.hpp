// Copyright 2021 by FORTYSS

#ifndef INCLUDE_SUGGESTION_HPP_
#define INCLUDE_SUGGESTION_HPP_

#include <shared_mutex>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <unistd.h>

struct config{
  std :: string id;
  std :: string name;
  int cost;
};

struct answer{
  std :: string text;
  int position;
};

class suggestion {
 public:
  explicit suggestion(std :: string file_name) : f_name(file_name){}

  nlohmann::json get_json_suggestion(std :: string input);

  void update_suggestion();

 private:
  nlohmann::json data;

  std::shared_mutex mutex;

  std::vector<config> suggestions;

  std :: string f_name;
};
#endif  // INCLUDE_SUGGESTION_HPP_
