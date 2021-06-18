// Copyright 2021 by FORTYSS

#include <suggestion.hpp>

bool comp(const config& first, const config& second) {
  return first.cost > second.cost;
}

  nlohmann::json suggestion::get_json_suggestion(std :: string input) {
  nlohmann::json j = nlohmann::json::parse(input);
  std::vector<nlohmann::json> out_suggestions;
  int count = 0;
  mutex.lock();
  std::sort(suggestions.begin(), suggestions.end(), comp);
  for (auto& sug : suggestions){
    if (sug.id == j["input"].get<std :: string>()) {
      answer ans = answer{sug.name, ++count};
      out_suggestions.push_back(nlohmann::json{{"text", ans.text},
                                    {"position", ans.position}
      });
    }
  }
  mutex.unlock();
  nlohmann::json out_json;
  out_json["suggestions"] = out_suggestions;
  return out_json;
}

void suggestion::update_suggestion() {
  for (;;){
    mutex.lock();
    std::ifstream file(f_name);
    suggestions.clear();
    data.clear();
    if (f_name.empty()){
      throw std::invalid_argument("empty file\n");
    }
    if (!file) {
      throw std::runtime_error("bad open json_file: " + f_name);
    }
    file >> data;
    file.close();
    for (auto const& sug : data.at("suggestions")){
      suggestions.push_back({
          sug["id"].get<std :: string>(),
              sug["name"].get<std :: string>(),
                  sug["cost"].get<int>()
                      });
    }
    mutex.unlock();
    sleep(40);
  }
}
