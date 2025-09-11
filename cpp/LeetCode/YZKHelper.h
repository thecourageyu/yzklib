//
// Created by YZK on 2025/9/6.
//

#ifndef LEETCODE_HELPER_H
#define LEETCODE_HELPER_H
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class YZKHelper {
public:
    vector<string> load_file(string &filename);
    vector<vector<string>> load_csv(string &filename);;
    vector<json> load_json(string &filename);;
};


#endif //LEETCODE_HELPER_H