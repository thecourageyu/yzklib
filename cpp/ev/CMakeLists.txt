cmake_minimum_required(VERSION 3.10)
project(TestApp)

set(CMAKE_CXX_STANDARD 17)


# Add your source files
add_executable(test
    /work/YZKMSI/yzklib/cpp/LeetCode/YZKHelper.cpp
    DataProcessor.cpp
    test.cpp
)

# include lib installed by vcpkg
target_include_directories(test PRIVATE
    /work/YZKMSI/yzklib/cpp/ev/vcpkg_installed/x64-linux/include
    /work/YZKMSI/yzklib/cpp/LeetCode
)
# target_link_libraries(test PRIVATE nlohmann_json::nlohmann_json)

#mkdir build && cd build
#cmake ..
#cmake --build .
#./test
