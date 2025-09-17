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
    
    vector<int>* numsPtr = &nums;
    vector<int> nonDuplicates;

    for (int num : nums) {
        
        if (find(nonDuplicates.begin(), nonDuplicates.end(), num) == nonDuplicates.end()) {
            nonDuplicates.push_back(num);
            cout << "[INFO] add num: " << num << endl;
            // cout << nonDuplicates.begin() << "; " << nonDuplicates.end() << endl;
        }
    }

    return nonDuplicates.size();
}
