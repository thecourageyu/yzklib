//
// Created by YZK on 2025/8/23.
//

#include "ProgramingEaxm.h"
#include "YZKHelper.h"
#include <iostream>

using namespace std;

// int main() {
//     ProgramingEaxm programingEaxm;
//     cout << "Hello World!";
//     vector<int> num = {1, 2, 3, 4};
//     programingEaxm.removeDuplicatesFromSortedArray(num);

//     return 0;
// }


// 替換字串中的所有子字串
std::string replaceAll(std::string text, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
        text.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return text;
}

int main() {

    string fname = "/work/YZKMSI/yzklib/cpp/LeetCode/AICoE_testing_20250905_125028.csv";
    YZKHelper hp;

    std::vector<std::string> file_data = hp.load_file(fname);
    // for (const auto& r : file_data) {
    //     std::cout << r[0] << std::endl;
    // }

    std::vector<vector<std::string>> csv_data = hp.load_csv(fname);
    // for (const auto& r : csv_data) {
    //     std::cout << r[0] << std::endl;
    // }

    ProgramingEaxm programingEaxm;

    vector<int> num = {1, 2, 3, 4, 5, 5, 66, 1};
    int k = programingEaxm.removeDuplicatesFromSortedArray(num);
    cout << "26. Remove Duplicates from Sorted Array, k = " << k << endl;

    // 相當於 Python 的 list
    std::vector<std::string> new_special_tokens = {
        "<hhev_split>",
        "<hhev_end>",
        "<handle_miscellaneous_task>",
        "<control_car_properties>",
        "<nav_start>",
        "<nav_stop>",
        "<search_and_show_place>",
        "<TEMPERATURE_SET>"
    };

    std::string text = "this is a <TEMPERATURE_SET> and <nav_start> example <nav_start><nav_start><nav_start><nav_start>";

    // 遍歷 tokens，找出 text 裡有的，並做替換
    for (const auto& token : new_special_tokens) {
        if (text.find(token) != std::string::npos) {
            // 去掉前後的 "<" 和 ">"
            std::string stripped = token.substr(1, token.size() - 2);  // ex. <nav_start>, 取(第一個) 至 (原長度 - 2) => nav_start
            text = replaceAll(text, token, stripped);
        }
    }

    std::cout << text << std::endl;
    return 0;
}