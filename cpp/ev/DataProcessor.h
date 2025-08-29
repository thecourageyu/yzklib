#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <string>
#include <vector>
#include <utility>

// #include "include/nlohmann/json.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;   // alias (optional)

// Define ToolCall in the header so both .h and .cpp can use it
struct ToolCall {
    std::string name;
    // std::string arguments;
    json arguments;
};

class DataProcessor {
// private:
//     std::vector<string> newSpecialTokens;
public:

    std::string pythonDictToJson(std::string s);
    std::vector<ToolCall> parseToolCalls(const std::string &text);
    std::string getOpenAIToolCall(std::string modelResponse, std::string position);
    std::string buildPlannerChatTemplate(const std::vector<std::pair<std::string, std::string>> &history);
    std::string buildSolverChatTemplate(const std::vector<std::pair<std::string, std::string>> &history);

    std::string build_chat_template(
        const std::vector<std::pair<std::string, std::string>> &history);
};

#endif // DATA_PROCESSOR_H
