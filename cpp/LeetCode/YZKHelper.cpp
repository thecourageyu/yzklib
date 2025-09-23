//
// Created by YZK on 2025/9/6.
//

#include "YZKHelper.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;
// using json = nlohmann::ordered_json;

vector<string> YZKHelper::load_file(string &filename) {
    ifstream file(filename);
    vector<string> data;
    if (!file.is_open()) {
        cerr << "[ERR] could not open file." << endl;
        return data;
    }
    
    string x;
    while (getline(file, x)) {
        data.push_back(x);
    }

    // for (string v : data) {
    //     cout << v << endl;
    // }
    // cout << endl;
    return data;
}


vector<vector<string>> YZKHelper::load_csv(string &filename) {
    // ifstream file(filename);
    // vector<string> data;
    // string x;

    ifstream file(filename);
    vector<vector<string>> data;
    if (!file.is_open()) {
        cerr << "[ERR] could not open file." << endl;
        return data;
    }

    string line;
    while (getline(file, line)) {   // read each row
        stringstream ss(line);
        string cell;
        vector<string> row;

        while (getline(ss, cell, ',')) {  // split by comma
            row.push_back(cell);
        }

        // Print parsed row
        // for (const auto &col : row) {
        //     cout << col << " | ";
        // }
        data.push_back(row);
        // cout << endl;
    }

    file.close();

    return data;
}

vector<json> YZKHelper::load_json(string &filename) {
    ifstream file(filename);
    vector<json> data;
    string x;

    while (file >> x) {
        x = json::parse(x);
        data.push_back(x);
    }
    
    for (string v : data) {
        cout << v << " ";
    }
    cout << endl;
    return data;
}