#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <string>
#include <vector>
#include <utility>

// #include "include/nlohmann/json.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;   // alias (optional)

// Define ToolCall in the header so both .h and .cpp can use it
struct ToolCall {
    string name;
    json arguments;
};

class DataProcessor {
// private:
//     std::vector<string> newSpecialTokens;
public:

    string pythonDictToJson(string s);
    vector<ToolCall> parseToolCalls(const string &text, bool posi2Kw = true);
    string getOpenAIToolCall(string modelResponse, string position);
    string buildPlannerChatTemplate(const vector<pair<string, string>> &history);
    string buildSolverChatTemplate(const vector<pair<string, string>> &history);

    string build_chat_template(
        const vector<pair<string, string>> &history);

    string toJimmyMessage(vector<pair<string, string>>& chatHistory);
};

#endif // DATA_PROCESSOR_H
