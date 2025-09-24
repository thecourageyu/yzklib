//
// Created by YZK on 2025/8/23.
//

#include "ProgramingEaxm.h"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int ProgramingEaxm::removeDuplicatesFromSortedArray(vector<int>& nums) {
    // 26. Remove Duplicates from Sorted Array

    // vector<int>* numsPtr = &nums;
    vector<int> nonDuplicates;

    cout << nums.size() << endl;
    // int result[static_cast<int>(10)];
    // int result[10];
    int n = 0;
    for (int num : nums) {
        
        if (find(nonDuplicates.begin(), nonDuplicates.end(), num) == nonDuplicates.end()) {
            nonDuplicates.push_back(num);
            cout << "[INFO] add num: " << num << endl;
            // cout << nonDuplicates.begin() << "; " << nonDuplicates.end() << endl;
            // result[n] = num;
            n += 1;
        }
    }

    int idx = 0;
    for (const auto& v : nonDuplicates) {
        nums[idx] = v;
        idx += 1;

    }

    int k = static_cast<int>(nonDuplicates.size());
    sort(nums.begin(), nums.begin() + k);
    
    cout << "k = " << k << endl;
    return k;
}


int removeElement(vector<int>& nums, int val) {
    // 27. Remove Element
}