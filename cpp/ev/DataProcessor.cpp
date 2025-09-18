#include "DataProcessor.h"   // make sure this is at the top

#include <iostream>
// #include <ranges>
#include <regex>
#include <sstream>
// #include "include/nlohmann/json.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;   // alias (optional)
// sudo apt-get install nlohmann-json3-dev
// wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -O json.hpp


static json keywordPositionalArgsMapping = {
    {
        "control_car_properties",
        {
            {"propertyId", 0},
            {"areaId", 1},
            {"operation", 2},
            {"value", 3}
        },
    },
    {
        "get_hhtd_info", 
            {
                {"areaId", 0}, 
                {"query", 1}
            }
    },
    {
        "get_vehicle_info", 
        {
            {"areaId", 0}, 
            {"query", 1}
        }
    },
    {
        "handle_miscellaneous_task", 
            {
                {"user_query", 0}
            }
    },
    {
        "hide_poi_list", json::object()
    },
    {
        "nav_start", 
        {
            {"keyword", 0},
            {"name", 1},
            {"address", 2},
            {"latitude", 3},
            {"longitude", 4},
            {"routeType", 5},
            {"isWaypoints", 6},
            {"fromMemory", 7}
        }
    },
    {
        "nav_stop", json::object()
    },
    {
        "search_and_show_place", {
            {"keyword", 0}
        }
    },
    {
        "set_seat_mode", 
        {
            {"areaId", 0}, 
            {"operation", 1}, 
            {"value", 2}
        }
    },
};

static vector<string> newSpecialTokens = {
    "<FRONT_WINDSHIELD>",
    "<REAR_WINDSHIELD>",
    "<SEAT_ALL>",
    "<SEAT_ROW_1>",
    "<SEAT_ROW_1_LEFT>",
    "<SEAT_ROW_1_RIGHT>",
    "<SEAT_ROW_2>",
    "<SEAT_ROW_2_LEFT>",
    "<SEAT_ROW_2_RIGHT>",
    "<SEAT_ROW_3>",
    "<SEAT_ROW_3_LEFT>",
    "<SEAT_ROW_3_RIGHT>",
    "<SLIDING_DOOR_LEFT>",
    "<SLIDING_DOOR_RIGHT>",
    "<SUNROOF>",
    "<CHILD_LOCK>",
    "<DASHBOARD_DISPLAY>",
    "<EV_CHARGE_PORT_OPEN>",
    "<FRONT_TRUNK_ON>",
    "<HVAC_AC_ON>",
    "<HVAC_AUTO_ON>",
    "<HVAC_DEFROSTER>",
    "<HVAC_FAN_DIRECTION>",
    "<HVAC_FAN_OUTPUT_MODE>",
    "<HVAC_FAN_OUTPUT_ON>",
    "<HVAC_FAN_SPEED>",
    "<HVAC_MAX_AC_ON>",
    "<HVAC_POWER_ON>",
    "<HVAC_RECIRC_MODE>",
    "<HVAC_SCENARIO_MODE>",
    "<HVAC_SEAT_TEMPERATURE_POWER_ON>",
    "<HVAC_SEAT_TEMPERATURE_SET>",
    "<HVAC_SEAT_VENTILATION_POWER_ON>",
    "<HVAC_SEAT_VENTILATION_SET>",
    "<HVAC_STEERING_WHEEL_HEAT>",
    "<HVAC_SYNC_MODE>",
    "<HVAC_TEMPERATURE_SET>",
    "<INFOTAINMENT_SYSTEM>",
    "<INTERIOR_LIGHT>",
    "<POWER_SUNSHADE>",
    "<POWER_TAILGATE_ON>",
    "<SEAT_BACKREST_ANGLE_POS>",
    "<SEAT_CUSHION_SIDE_SUPPORT_POS>",
    "<SEAT_FORE_AFT_POS>",
    "<SEAT_HEADREST_HEIGHT_POS>",
    "<SEAT_HEIGHT_POS>",
    "<SEAT_LEGREST_FORE_AFT_POS>",
    "<SEAT_LEGREST_HEIGHT_POS>",
    "<SEAT_LUMBAR_FORE_AFT_POS>",
    "<SEAT_LUMBAR_HEIGHT_POS>",
    "<SEAT_MASSAGE_INTENSITY>",
    "<SEAT_MASSAGE_MODE>",
    "<SEAT_MASSAGE_ON>",
    "<SEAT_MASSAGE_REGION>",
    "<SEAT_POSITION_MEMORY_SET>",
    "<SEAT_STOW_MODE>",
    "<SLIDING_DOOR_ON>",
    "<SMARTPHONE_INTEGRATION>",
    "<SURROUND_VIEW_CAMERA_SYSTEM>",
    "<WINDOW_POS>"
};




string replaceAll(string text, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = text.find(from, start_pos)) != string::npos) {
        text.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return text;
}


// helper: convert Python-style dict string into JSON-like string
string DataProcessor::pythonDictToJson(string s) {
    // replace single quotes with double quotes
    for (auto& c : s) {
        if (c == '\'') c = '"';
    }

    // replace Python booleans with JSON booleans
    s = regex_replace(s, regex("\\bTrue\\b"), "true");
    s = regex_replace(s, regex("\\bFalse\\b"), "false");

    // replace Python None with JSON null (if ever appears)
    s = regex_replace(s, regex("\\bNone\\b"), "null");
    // s = regex_replace(s, regex("\\bNone\\b"), "");
    // cout << s << endl;
    return s;
}

vector<ToolCall> DataProcessor::parseToolCalls(const string &inputText, bool posi2Kw) {

    // nlohmann::json keyword2Position = {
    //     {"ok", {{"k", 1}, {"k2", 2}}},
    //     {"keyword", 0},
    //     {"name", 1},
    //     {"address", 2}
    // };
    // for (auto& [kw, posi] : keyword2Position.items()) {
    //     std::cout << "key=" << kw << " value=" << posi << "\n";
    // }

    // for (auto& [kw, posi] : keywordPositionalArgsMapping.items()) {
    //     std::cout << "key=" << kw << " value=" << posi << "\n";
    // }


    string text = inputText;
    vector<ToolCall> tool_calls;

    // Remove <hhev_end>
    size_t pos;
    string cleaned = text;
    while ((pos = cleaned.find("<hhev_end>")) != string::npos) {  // no position
        cleaned.erase(pos, string("<hhev_end>").length());
    }
    
    for (const auto& token : newSpecialTokens) {
        if (cleaned.find(token) != string::npos) {
            // 去掉前後的 "<" 和 ">"
            string stripped = token.substr(1, token.size() - 2);  // ex. <nav_start>, 取(第一個) 至 (原長度 - 2) => nav_start
            cleaned = replaceAll(cleaned, token, stripped);
        }
    }

    // Split on <hhev_split>
    vector<string> tools;
    size_t start = 0, end;
    while ((end = cleaned.find("<hhev_split>", start)) != string::npos) {
        tools.push_back(cleaned.substr(start, end - start));
        start = end + string("<hhev_split>").length();
    }
    tools.push_back(cleaned.substr(start));

    
    // Regex to match <tool>(args)
    regex re("<(.*)>\\((.*)\\)");
    vector<string> posiArgs;
    json posi2Kwargs;
    for (const auto& tool : tools) {
        smatch match;
        if (regex_search(tool, match, re)) {
            string toolName = match[1];
            string toolArgs = match[2];
            
            if (posi2Kw) {
                posiArgs.clear();
                // posiArgs.shrink_to_fit();   // optional, forces memory release
                posi2Kwargs.clear(); 

                json keyword2Position = keywordPositionalArgsMapping[toolName];
                
                // cout << keyword2Position << endl;
                
                start = 0;
                while ((end = toolArgs.find("<args_split>", start)) != string::npos) {
                    posiArgs.push_back(toolArgs.substr(start, end - start));    
                    start = end + string("<args_split>").length();
                }
                
                // if (toolArgs.substr(toolArgs.length() - string("<args_split>").length(), string("<args_split>").length()) == "<args_split>") {
                //     posiArgs.push_back("");
                // }
                posiArgs.push_back(toolArgs.substr(start));

                if (posiArgs.size() == 0) {
                    posiArgs.push_back(toolArgs);
                }

                for (auto& [kw, posi] : keyword2Position.items()) {
                    // cout << "keyword: " << kw << "; posi: " << posi << "; value: " << posiArgs.at(posi) << endl;
                    posi2Kwargs[kw] = posiArgs.at(posi);
                }
                tool_calls.push_back({toolName, posi2Kwargs});
                
            } else {
                try {
                    string fixed_args = DataProcessor::pythonDictToJson(toolArgs);
                    json parsed = json::parse(fixed_args);
                    tool_calls.push_back({toolName, parsed});
                } catch (exception& e) {
                    string fixed_args = DataProcessor::pythonDictToJson(toolArgs);
                    // Fallback: store raw string if JSON parse fails
                    tool_calls.push_back({toolName, fixed_args});
                    cerr << "[ERR] parse tool args failed! " 
                            << toolName << ", " << fixed_args << "\n" 
                            << e.what() << endl;
                }
            }
        }
    }

    return tool_calls;
}


// 產生符合 OpenAI Tool Calling 的 JSON 字串
// 規則：
//  1) parameters 是 array → arguments 就是該 array
//  2) parameters 唯一鍵為 "properties" 且其值為 array → arguments 就是該 array
//  3) 其他情況 → arguments 為完整的 parameters 物件
// string DataProcessor::getOpenAIToolCall(const string& toolName, const json& parameters) 
string DataProcessor::getOpenAIToolCall(string modelResponse, string position) 
{
    vector<ToolCall> toolCalls = DataProcessor::parseToolCalls(modelResponse);
    // for (auto &c : toolCalls) {
    //     cout << "Func: " << c.name << " Args: " << c.arguments << endl;

    // }
    json payload;
    payload["tool_calls"] = json::array();

    json parameters;
    json arguments_obj;
    json fn;
    json call;
    // for (auto &tc : toolCalls) {
    //    vector<int> a = {10, 20, 30};

    for (int idx = 0; idx < toolCalls.size(); idx++) {
        ToolCall tc = toolCalls[idx];
        // cout << idx << ". name: " << tc.name << "args: " << tc.arguments << "\n" << endl;
        parameters = tc.arguments;
        
        if (parameters.is_array()) {
            // [{k: v , ...}, {}, ...]
            arguments_obj = parameters;
        } else if (parameters.is_object() &&
                parameters.size() == 1 &&
                parameters.contains("properties") &&
                parameters.at("properties").is_array()) {
            // {properties: []}
            // cout << "parameter is object!" << endl;
            arguments_obj = parameters.at("properties");
        } else {
            // {k: v, ...}
            // cout << "parameter is ?!" << endl;
            arguments_obj = parameters;
        }


        if (tc.name == "control_car_properties") {
            if (arguments_obj.is_array()) {
                if (arguments_obj[0].contains("areaId")) {
                    if (arguments_obj[0]["areaId"].is_string() && arguments_obj[0]["areaId"] == "") {
                        arguments_obj[0]["areaId"] = position;
                    } else if (arguments_obj[0]["areaId"].is_null()) {
                        arguments_obj[0]["areaId"] = position;
                    }
                    
                } else {
                    arguments_obj[0]["areaId"] = position;
                }
            } else {    
                if (arguments_obj.contains("areaId")) {
                    if (arguments_obj["areaId"].is_string() && arguments_obj["areaId"] == "") {
                        arguments_obj["areaId"] = position;
                    } else if (arguments_obj["areaId"].is_null()) {
                        arguments_obj["areaId"] = position;
                    }
                    
                } else {
                    arguments_obj["areaId"] = position;
                }
            }
        }


        fn["name"] = tc.name;
        fn["arguments"] = arguments_obj.dump(); // 內層 arguments 要是「字串化的 JSON」

        call["id"] = "call_" + to_string(idx + 1);
        call["type"] = "function";
        call["function"] = fn;
        payload["tool_calls"].push_back(call);
    }
    
    // ex. {tool_calls: [{'name': ..., 'arguments': ..., 'id': call_1, 'type': function, }]}
    return payload.dump(); // 緊湊輸出、保留 UTF-8

    //payload["tool_calls"] = [ {function: {arg, name}, id: ..., type: ...}, {}]
}

string DataProcessor::build_chat_template(
        const vector<pair<string, string>> &history) {
    ostringstream oss;
    for (auto &msg : history) {
        oss << "<|start_header_id|>" << msg.first << "<|end_header_id|>\n\n"
            << msg.second << "<|eot_id|>";
    }
    oss << "<|start_header_id|>assistant<|end_header_id|>\n\n";
    return oss.str();
}


string DataProcessor::buildPlannerChatTemplate(const vector<pair<string, string>> &history) {

    unordered_map<string, string> roleMapping;
    roleMapping["user"] = "user";
    roleMapping["assistant"] = "assistant";
    roleMapping["ipython"] = "ipython";
    // roleMapping["observation"] = "ipython";
    // roleMapping["tool"] = "ipython";

    ostringstream oss;
    for (auto &msg : history) {

        oss << "<|start_header_id|>" << roleMapping[msg.first] << "<|end_header_id|>\n\n"
            << msg.second << "<|eot_id|>";
    }
    oss << "<|start_header_id|>assistant<|end_header_id|>\n\n";
    return oss.str();
}


string DataProcessor::buildSolverChatTemplate(const vector<pair<string, string>> &history) {
    string SOLVER_SYSTEM_PROMPT = R"(You are an AI assistant tasked with improving a response using full context from a multi-turn interaction.

You are given:
- The user query
- The function call and its result
- The original assistant response

Your goal is to revise or regenerate the assistant response to be more helpful, accurate, and fluent.
Use the function result meaningfully. Improve clarity, tone, or completeness if needed. If the original response is already good, polish it slightly.

Respond only with the improved message (do not explain or output metadata).
)";

    string SOLVER_PROMPT = R"(
---

{}

---

Improved Response:
)";

// <|eot_id|><|start_header_id|>user<|end_header_id|>


// ---

// user: 我想要去科學實驗室風的夜市看看
// function_call: <search_and_show_place>({'keyword': '科學實驗室風的夜市'})<hhev_end>
// observation: {"name": "search_and_show_place", "status": "success", "message": "地點查詢成功", "poi": [{"name": "新竹城隍夜市", "address": "新竹市北區中山路88號", "latitude": 24.8071, "longitude": 120.9688, "fromMemory": true}, {"name": "士林觀光夜市", "address": "台北市士林區大東路100號", "latitude": 25.087611, "longitude": 121.525621, "fromMemory": false}]}
// assistant: 上次您是去新竹城隍夜市，這次還要去這裡嗎？
// user: 應該不是這一間
// assistant: 請告訴我目的地附近的標誌性景點或描述科學實驗室風的夜市的具體特徵，這樣我可以更精準地幫你搜尋並導航到正確的地址
// user: 店名有逢甲
// function_call: <search_and_show_place>({'keyword': '科學實驗室風的夜市 逢甲'})<hhev_end>
// observation: {"name": "search_and_show_place", "status": "success", "message": "地點查詢成功", "poi": [{"name": "逢甲夜市", "address": "台中市西屯區文華路100號", "latitude": 24.178943, "longitude": 120.6461, "fromMemory": false}]}
// assistant: 我找到逢甲夜市，請問是要到這裡嗎？


// ---

// Improved Response:

   
// string build_solver_prompt(const vector<json>& messages) {
    string chat_hist;
    string prev_role;
    string prev_content;

    for (const auto &msg : history) {
        // string role = msg.at("role");
        // string content = msg.at("content");
        string role = msg.first;
        string content = msg.second;

        if (role == "user") {
            chat_hist += "user: " + content + "\n";
        } 
        else if (role == "assistant") {
            if (prev_role == "ipython") {
                chat_hist += "assistant: " + content + "\n";
            } else {
                if (content.find("<hhev_end>") != string::npos) {
                    chat_hist += "function_call: " + content + "\n";
                } else {
                    chat_hist += "assistant: " + content + "\n";
                }
            }
        } 
        else if (role == "ipython") {
            chat_hist += "observation: " + content + "\n";
        }

        prev_role = role;
        prev_content = content;
    }

    // Equivalent to Python f-string formatting
    string prompt;
    prompt += "<|begin_of_text|><|start_header_id|>system<|end_header_id|>\n\n" + SOLVER_SYSTEM_PROMPT + "\n<|eot_id|>";
    
    // Replace "{}" in SOLVER_PROMPT with chat_hist
    string user_block = SOLVER_PROMPT;
    size_t pos = user_block.find("{}");
    if (pos != string::npos) {
        user_block.replace(pos, 2, chat_hist);
    }

    prompt += "<|start_header_id|>user<|end_header_id|>\n\n" + user_block + "<|eot_id|>";
    prompt += "<|start_header_id|>assistant<|end_header_id|>\n\n";

    return prompt;
}


// vector<pair<string, string>> DataProcessor::toJimmyMessage(vector<pair<string, string>>& chatHistory) {
string DataProcessor::toJimmyMessage(vector<pair<string, string>>& chatHistory) {
    // vector<pair<string, string>> message;
    json message = json::array();
    for(size_t i = 0; i < chatHistory.size(); ++i) {
        string role = chatHistory[i].first;
        string content = chatHistory[i].second;
        printf("[role] %s\n", role.c_str());
        printf("[message] %s\n", content.c_str());
        if (role == "user") {
            message.push_back({{"role", "user"}, {"message", content}});
        } else if (role == "ipython") {
            message.push_back({{"role", "observation"}, {"message", content}});
        } else {
            if (content.find("<hhev_end>") != string::npos) {
                json fc = json::array();
                vector<ToolCall> toolCalls = DataProcessor::parseToolCalls(content, false);
                for (const auto& r : toolCalls) {
                    fc.push_back({{"name", r.name}, {"arguments", r.arguments}});
                }
                message.push_back({{"role", "function_call"}, {"message", fc}});
            } else {
                message.push_back({{"role", "ai"}, {"message", content}});
            }

        }
    }
    return message.dump();
}