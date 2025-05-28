/* =========================
 * File: main.cpp
 * ========================= */
#include <iostream>
#include "mathlib.h"

int main() {
    MathLib math;
    std::cout << "3 + 4 = " << math.add(3, 4) << std::endl;
    std::cout << "9 - 5 = " << math.subtract(9, 5) << std::endl;
    return 0;
}
