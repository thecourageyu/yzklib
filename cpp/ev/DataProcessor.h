#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

#include <string>
#include <vector>
#include <utility>

// #include "include/nlohmann/json.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using basic_json = nlohmann::json;   // alias (optional)
using json = nlohmann::ordered_json;   // alias (optional)

// Define ToolCall in the header so both .h and .cpp can use it
struct ToolCall {
    string name;
    basic_json arguments;
};

class DataProcessor {
// private:
//     std::vector<string> newSpecialTokens;
public:

    string pythonDictToJson(string s);
    vector<ToolCall> parseToolCalls(const string &text, bool posi2Kw = true);
    // vector<ToolCall> parseToolCalls(const string &text, bool posi2Kw = false);
    string getOpenAIToolCall(string modelResponse, string position);
    string buildPlannerChatTemplate(const vector<pair<string, string>> &history);
    string buildSolverChatTemplate(const vector<pair<string, string>> &history);

    string build_chat_template(
        const vector<pair<string, string>> &history);

    string toJimmyMessage123(vector<pair<string, string>>& chatHistory);
    string Jeff2SLMToolCalls(const string &inputText);
    vector<json> toJimmyMessage(vector<pair<string, string>>& chatHistory);

};

#endif // DATA_PROCESSOR_H
