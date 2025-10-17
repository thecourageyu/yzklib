#include "DataProcessor.h"   // make sure this is at the top

#include <algorithm>
#include <cctype>
#include <iostream>
// #include <ranges>
#include <regex>
#include <sstream>
// #include "include/nlohmann/json.hpp"
#include <nlohmann/json.hpp>

using namespace std;
using basic_json = nlohmann::json;   // alias (optional)
using json = nlohmann::ordered_json;   // alias (optional)
// sudo apt-get install nlohmann-json3-dev
// wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -O json.hpp


static basic_json keywordPositionalArgsMapping = {
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
        "get_news",
        {
            {"category", 0}, 
            {"index", 1}, 
            {"action", 2}, 
            {"areaId", 3}
        },
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
        "hide_poi_list", basic_json::object()
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
        // "nav_stop", basic_json::object()
        "nav_stop", {}
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
    "<POWER_ALL_SUNSHADE>",
    "<POWER_SUNSHADE>",
    "<POWER_TAILGATE_ON>",
    "<READING_LIGHT_LEVEL>",
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
    "<SEAT_POSITION_MEMORY_SAVE>",
    "<SEAT_POSITION_MEMORY_SET>",
    "<SEAT_STOW_MODE>",
    "<SLIDING_DOOR_ON>",
    "<WINDOW_POS>"
};




string trim(const string& str) {

    // Remove whitespace / newline characters
    // ' ', \t, \n, \v, \f, \r

    auto start = str.begin();
    while (start != str.end() && isspace((unsigned char)*start)) {
        start++;
    }

    auto end = str.end();
    do {
        end--;
    } while (end != start && isspace((unsigned char)*end));

    return string(start, end + 1);
}

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

    // nlohmann::basic_json keyword2Position = {
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
    basic_json posi2Kwargs;
    for (const auto& tool : tools) {
        smatch match;
        if (regex_search(tool, match, re)) {
            string toolName = match[1];
            string toolArgs = match[2];
            string fixedArgs = DataProcessor::pythonDictToJson(toolArgs);

            // cout << ">>>>>>>>>>>>>>>>>>>>>>>>>>>" << fixedArgs << endl;

            if (!keywordPositionalArgsMapping.contains(toolName)) {
                printf("[ERR] keywordPositionalArgsMapping doesn't contain: %s\n", toolName.c_str());
                continue;
            }

            if (posi2Kw) {
                posiArgs.clear();
                // posiArgs.shrink_to_fit();   // optional, forces memory release
                posi2Kwargs.clear(); 

                basic_json keyword2Position = keywordPositionalArgsMapping[toolName];
                
                // cout << keyword2Position << endl;
                
                start = 0;
                while ((end = fixedArgs.find("<args_split>", start)) != string::npos) {
                    
                    string arg_str = fixedArgs.substr(start, end - start);
                    // cout << "check areaId: " << arg_text << endl;
                    if (arg_str == "\"\"" && arg_str.size() == 2) {  // ex. <set_seat_mode>(''<args_split>set<args_split>OFF)<hhev_end> will get areaId = "\"\""
                        posiArgs.push_back("");    

                    } else {
                        posiArgs.push_back(fixedArgs.substr(start, end - start));    
                    }
                    start = end + string("<args_split>").length();
                }
                

                // the last part, which is after <hhev_split> and before <hhev_end>
                if (static_cast<int>(keyword2Position.size()) > 0 && posiArgs.size() == keyword2Position.size() - 1) {
                    string arg_str = fixedArgs.substr(start);
                    if (arg_str == "\"\"" && arg_str.size() == 2) {  // ex. <set_seat_mode>(''<args_split>set<args_split>OFF)<hhev_end> will get areaId = "\"\""
                        posiArgs.push_back("");    
                    } else {
                        posiArgs.push_back(arg_str);
                    }
                }

                if (posiArgs.size() != keyword2Position.size()) {
                    printf("[WARN] %s posiArgs.size() != keyword2Position.size() => %zu != %zu\n", toolName.c_str(), posiArgs.size(), keyword2Position.size());
                    // continue;
                }

                for (auto& [kw, posi] : keyword2Position.items()) {
                    
                    if (posi >= 0 && posi < static_cast<int>(posiArgs.size())) {
                        // cout << "keyword: " << kw << "; posi: " << posi << "; value: " << posiArgs.at(posi) << "; size: " << posiArgs.at(posi).size() <<endl;
                        
                        if (posiArgs.at(posi) == "\"\"" && posiArgs.at(posi).size() == 2) {
                            posi2Kwargs[kw] = "";
                        } else {

                            // drop " in the first and the last place in a string
                            string argVal = posiArgs.at(posi);

                            if (argVal.length() > 0) {
                                if (argVal.substr(0, 1) == "\"") {
                                    argVal = argVal.substr(1);
                                    // cout << argVal << endl;
                                }
                            
                            
                                if (argVal.substr(argVal.length() - 1, argVal.length()) == "\"") {
                                    argVal = argVal.substr(0, argVal.length() - 1);
                                    // cout << argVal << endl;
                                }
                            }
                            
                            posi2Kwargs[kw] = argVal;
                        }
                    }

                }

                if (posi2Kwargs.is_null() || posi2Kwargs.empty()) {
                    // cout << "JSON is null or empty\n";
                    posi2Kwargs = basic_json::object();  // reset to empty JSON object
                }

                tool_calls.push_back({toolName, posi2Kwargs});
                
            } else {
                try {
                    // string fixedArgs = DataProcessor::pythonDictToJson(toolArgs);
                    basic_json parsed = basic_json::parse(fixedArgs);
                    tool_calls.push_back({toolName, parsed});
                } catch (exception& e) {
                    // string fixedArgs = DataProcessor::pythonDictToJson(toolArgs);
                    // Fallback: store raw string if JSON parse fails
                    tool_calls.push_back({toolName, fixedArgs});
                    // tool_calls.push_back({toolName, basic_json{}});
                    cerr << "[ERR] parse tool args failed! " 
                            << toolName << ", " << fixedArgs << "\n" 
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
// string DataProcessor::getOpenAIToolCall(const string& toolName, const basic_json& parameters) 
string DataProcessor::getOpenAIToolCall(string modelResponse, string position) 
{
    basic_json payload;
    basic_json parameters;
    basic_json arguments_obj;
    basic_json fn;
    basic_json call;

    vector<string> checkAreaId;
    vector<ToolCall> toolCalls = DataProcessor::parseToolCalls(modelResponse);
    
    // these functions need to check areaId
    for (auto& [toolName, toolArgs] : keywordPositionalArgsMapping.items()) {
        
        if (toolArgs.contains("areaId")) {
            checkAreaId.push_back(toolName);
            // printf("[INFO] tool with areaId: %s\n", toolName.c_str());
        }

    }
    
    payload["tool_calls"] = basic_json::array();
    
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
            // {properties: []}, old format
            // cout << "parameter is object!" << endl;
            arguments_obj = parameters.at("properties");
        } else {
            // {k: v, ...}, new format (started at v1.4)
            arguments_obj = parameters;
        }


        // check tools with areaId
        // if (tc.name == "control_car_properties") {
        if (find(checkAreaId.begin(), checkAreaId.end(), tc.name) != checkAreaId.end()) {
            // cout << "check areaId for tool name: " << tc.name << endl;
            if (arguments_obj.is_array()) {
                if (arguments_obj[0].contains("areaId")) {
                    // cout << "check areaId for tool name (arr): " << tc.name << "; args: " << arguments_obj[0]["areaId"] << endl;
                    // if (arguments_obj[0]["areaId"].is_string() && arguments_obj[0]["areaId"] == "") {
                    if (arguments_obj[0]["areaId"].is_string() && (trim(arguments_obj["areaId"]).empty() || arguments_obj["areaId"] == "")) {
                        arguments_obj[0]["areaId"] = position;
                    } else if (arguments_obj[0]["areaId"].is_null()) {
                        arguments_obj[0]["areaId"] = position;
                    }
                    
                } else {
                    arguments_obj[0]["areaId"] = position;
                }
            } else {    
                if (arguments_obj.contains("areaId")) {
                    // cout << "check areaId for tool name (obj): " << tc.name << "; args: " << arguments_obj["areaId"] << "; size: " << arguments_obj["areaId"].size() << endl;
                    // if (arguments_obj["areaId"].is_string() && arguments_obj["areaId"] == "") {
                    if (arguments_obj["areaId"].is_string() && (trim(arguments_obj["areaId"]).empty() || arguments_obj["areaId"] == "")) {
                    // if (arguments_obj["areaId"].is_string() && trim(arguments_obj["areaId"]).empty()) {

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

   
// string build_solver_prompt(const vector<basic_json>& messages) {
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
string DataProcessor::toJimmyMessage123(vector<pair<string, string>>& chatHistory) {
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


string DataProcessor::Jeff2SLMToolCalls(const string &inputText) {

    json toolCalls = json::parse(inputText);
    string result = "";
    string toolName;
    json toolArgs;
    int num_tool_calls = 0;
    for (const auto& tool : toolCalls) {
        if (tool.is_object()) {
            if (num_tool_calls >= 1) {
                result += "<hhev_split>";
            }

            toolName = tool["name"];
            toolArgs = tool["arguments"];

            if (keywordPositionalArgsMapping.contains(toolName)) {

                vector<string> kw2Posi(keywordPositionalArgsMapping[toolName].size());   // runtime determined length 
                for (auto& [kw, posi] : keywordPositionalArgsMapping[toolName].items()) {
                    
                    if (toolArgs.contains(kw)) {
                        if (kw == "areaId" || kw == "propertyId") {
                            if (toolArgs[kw] != "" && !trim(toolArgs[kw]).empty()) {
                                kw2Posi[posi] = "<" + toolArgs[kw].get<std::string>() + ">";
                            }
                            
                        } else {
                            // kw2Posi[posi] = "\"" + toolArgs[kw].get<std::string>() + "\"";
                            kw2Posi[posi] = toolArgs[kw].get<std::string>();
                        }
                        
                    }

                }
                
                result += "<" + toolName + ">(";

                int idx = 0;
                for (const auto& s : kw2Posi) {
                    // std::cout << s << std::endl;
                    if (!s.empty()) {
                        result += s;
                    }
                    
                    idx += 1;
                    if (idx < kw2Posi.size()) {
                        result += "<args_split>";
                    }
                }
                result += ")";
                num_tool_calls += 1;
            }
            
        }
    }
    result += "<hhev_end>";

    // printf("[Jeff] %s\n", result.c_str());
    return result;
}

vector<json> DataProcessor::toJimmyMessage(vector<pair<string, string>>& chatHistory) {
    // vector<pair<string, string>> message;
    vector<json> message;
    for(size_t i = 0; i < chatHistory.size(); ++i) {
        string role = chatHistory[i].first;
        string content = chatHistory[i].second;
        // printf("[role] %s\n", role.c_str());
        // printf("[message] %s\n", content.c_str());
        if (role == "user") {
            message.push_back(json{{"role", "user"}, {"message", content}});
        } else if (role == "ipython") {
            // json SRDCObs = json::parse(content);
            // json obs;

            // if (SRDCObs.is_array()) {
            //     cout << "OK" << endl;
            //     for (const auto& o : SRDCObs) {
            //         cout << o.dump() << endl;
            //         if (o.is_object() && o.contains("content")) {
            //             obs.push_back(o["content"]);
            //         }
            //     }
            //     // message.push_back(json{{"role", "observation"}, {"message", content}});    

            //     message.push_back(json{{"role", "observation"}, {"message", obs.dump()}});
            // } else {
            //     message.push_back(json{{"role", "observation"}, {"message", content}});    
            //     // continue;
            // }

            message.push_back(json{{"role", "observation"}, {"message", content}});    

        } else {
            if (content.find("<hhev_end>") != string::npos) {
                json fc = json::array();
                // vector<ToolCall> toolCalls = DataProcessor::parseToolCalls(content, false);
                vector<ToolCall> toolCalls = DataProcessor::parseToolCalls(content, true);
                for (const auto& r : toolCalls) {
                    fc.push_back(json{{"name", r.name}, {"arguments", r.arguments}});
                }
                message.push_back(json{{"role", "function_call"}, {"message", fc.dump()}});
            } else {
                message.push_back(json{{"role", "ai"}, {"message", content}});
            }

        }
    }
    return message;
}