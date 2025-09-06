//
// Created by YZK on 2025/9/6.
//

#include "helper.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

vector<string> helper::load_file(string &filename) {
    ifstream file(filename);
    vector<string> data;
    string x;

    while (file >> x) {
        data.push_back(x);
    }

    for (string v : data) {
        cout << v << " ";
    }
    cout << endl;
    return data;
}


vector<string> helper::load_csv(string &filename) {
    ifstream file(filename);
    vector<string> data;
    string x;

    while (file >> x) {
        data.push_back(x);
    }

    for (string v : data) {
        cout << v << " ";
    }
    cout << endl;
    return data;
}

vector<json> helper::load_json(string &filename) {
    ifstream file(filename);
    vector<json> data;
    string x;

    // while (file >> x) {
    //     data.push_back(x);
    // }
    //
    // for (string v : data) {
    //     cout << v << " ";
    // }
    // cout << endl;
    return data;
}