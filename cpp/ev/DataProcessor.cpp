#include "DataProcessor.h"   // make sure this is at the top

#include <iostream>
// #include <ranges>
#include <regex>
#include <sstream>

// #include "include/nlohmann/json.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;   // alias (optional)
// sudo apt-get install nlohmann-json3-dev
// wget https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp -O json.hpp

static std::vector<std::string> newSpecialTokens = {
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




std::string replaceAll(std::string text, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
        text.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return text;
}


// helper: convert Python-style dict string into JSON-like string
std::string DataProcessor::pythonDictToJson(std::string s) {
    // replace single quotes with double quotes
    for (auto& c : s) {
        if (c == '\'') c = '"';
    }

    // replace Python booleans with JSON booleans
    s = std::regex_replace(s, std::regex("\\bTrue\\b"), "true");
    s = std::regex_replace(s, std::regex("\\bFalse\\b"), "false");

    // replace Python None with JSON null (if ever appears)
    s = std::regex_replace(s, std::regex("\\bNone\\b"), "null");
    // s = std::regex_replace(s, std::regex("\\bNone\\b"), "");
    // std::cout << s << std::endl;
    return s;
}

std::vector<ToolCall> DataProcessor::parseToolCalls(const std::string &inputText) {
    std::string text = inputText;
    std::vector<ToolCall> tool_calls;

    // Remove <hhev_end>
    size_t pos;
    std::string cleaned = text;
    while ((pos = cleaned.find("<hhev_end>")) != std::string::npos) {  // no position
        cleaned.erase(pos, std::string("<hhev_end>").length());
    }
    
    for (const auto& token : newSpecialTokens) {
        if (cleaned.find(token) != std::string::npos) {
            // 去掉前後的 "<" 和 ">"
            std::string stripped = token.substr(1, token.size() - 2);  // ex. <nav_start>, 取(第一個) 至 (原長度 - 2) => nav_start
            cleaned = replaceAll(cleaned, token, stripped);
        }
    }

    // Split on <hhev_split>
    std::vector<std::string> tools;
    size_t start = 0, end;
    while ((end = cleaned.find("<hhev_split>", start)) != std::string::npos) {
        tools.push_back(cleaned.substr(start, end - start));
        start = end + std::string("<hhev_split>").length();
    }
    tools.push_back(cleaned.substr(start));

    // Regex to match <tool>(args)
    std::regex re("<(.*)>\\((.*)\\)");

    for (const auto& tool : tools) {
        std::smatch match;
        if (std::regex_search(tool, match, re)) {
            std::string tool_name = match[1];
            std::string tool_args = match[2];
            
            try {
                std::string fixed_args = DataProcessor::pythonDictToJson(tool_args);
                json parsed = json::parse(fixed_args);
                tool_calls.push_back({tool_name, parsed});
            } catch (std::exception& e) {
                std::string fixed_args = DataProcessor::pythonDictToJson(tool_args);
                // Fallback: store raw string if JSON parse fails
                tool_calls.push_back({tool_name, fixed_args});
                std::cerr << "[ERR] parse tool args failed! " 
                          << tool_name << ", " << fixed_args << "\n" 
                          << e.what() << std::endl;
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
// std::string DataProcessor::getOpenAIToolCall(const std::string& tool_name, const nlohmann::json& parameters) 
std::string DataProcessor::getOpenAIToolCall(std::string modelResponse, std::string position) 
{
    std::vector<ToolCall> toolCalls = DataProcessor::parseToolCalls(modelResponse);
    // for (auto &c : toolCalls) {
    //     std::cout << "Func: " << c.name << " Args: " << c.arguments << std::endl;

    // }
    nlohmann::json payload;
    payload["tool_calls"] = nlohmann::json::array();



    nlohmann::json parameters;
    nlohmann::json arguments_obj;
    nlohmann::json fn;
    nlohmann::json call;
    // for (auto &tc : toolCalls) {
    //    std::vector<int> a = {10, 20, 30};

    for (int idx = 0; idx < toolCalls.size(); idx++) {
        ToolCall tc = toolCalls[idx];
        // std::cout << idx << ". name: " << tc.name << "args: " << tc.arguments << "\n" << std::endl;
        parameters = tc.arguments;
        
        if (parameters.is_array()) {
            // [{k: v , ...}, {}, ...]
            arguments_obj = parameters;
        } else if (parameters.is_object() &&
                parameters.size() == 1 &&
                parameters.contains("properties") &&
                parameters.at("properties").is_array()) {
            // {properties: []}
            // std::cout << "parameter is object!" << std::endl;
            arguments_obj = parameters.at("properties");
        } else {
            // {k: v, ...}
            // std::cout << "parameter is ?!" << std::endl;
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

        call["id"] = "call_" + std::to_string(idx + 1);
        call["type"] = "function";
        call["function"] = fn;
        payload["tool_calls"].push_back(call);
    }
    
    // ex. {tool_calls: [{'name': ..., 'arguments': ..., 'id': call_1, 'type': function, }]}
    return payload.dump(); // 緊湊輸出、保留 UTF-8

    //payload["tool_calls"] = [ {function: {arg, name}, id: ..., type: ...}, {}]
}

std::string DataProcessor::build_chat_template(
        const std::vector<std::pair<std::string, std::string>> &history) {
    std::ostringstream oss;
    for (auto &msg : history) {
        oss << "<|start_header_id|>" << msg.first << "<|end_header_id|>\n\n"
            << msg.second << "<|eot_id|>";
    }
    oss << "<|start_header_id|>assistant<|end_header_id|>\n\n";
    return oss.str();
}


std::string DataProcessor::buildPlannerChatTemplate(const std::vector<std::pair<std::string, std::string>> &history) {

    std::unordered_map<std::string, std::string> roleMapping;
    roleMapping["user"] = "user";
    roleMapping["assistant"] = "assistant";
    roleMapping["ipython"] = "ipython";
    // roleMapping["observation"] = "ipython";
    // roleMapping["tool"] = "ipython";

    std::ostringstream oss;
    for (auto &msg : history) {

        oss << "<|start_header_id|>" << roleMapping[msg.first] << "<|end_header_id|>\n\n"
            << msg.second << "<|eot_id|>";
    }
    oss << "<|start_header_id|>assistant<|end_header_id|>\n\n";
    return oss.str();
}


std::string DataProcessor::buildSolverChatTemplate(const std::vector<std::pair<std::string, std::string>> &history) {
    std::string SOLVER_SYSTEM_PROMPT = R"(You are an AI assistant tasked with improving a response using full context from a multi-turn interaction.

You are given:
- The user query
- The function call and its result
- The original assistant response

Your goal is to revise or regenerate the assistant response to be more helpful, accurate, and fluent.
Use the function result meaningfully. Improve clarity, tone, or completeness if needed. If the original response is already good, polish it slightly.

Respond only with the improved message (do not explain or output metadata).
)";

    std::string SOLVER_PROMPT = R"(
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

   
// std::string build_solver_prompt(const std::vector<json>& messages) {
    std::string chat_hist;
    std::string prev_role;
    std::string prev_content;

    for (const auto &msg : history) {
        // std::string role = msg.at("role");
        // std::string content = msg.at("content");
        std::string role = msg.first;
        std::string content = msg.second;

        if (role == "user") {
            chat_hist += "user: " + content + "\n";
        } 
        else if (role == "assistant") {
            if (prev_role == "ipython") {
                chat_hist += "assistant: " + content + "\n";
            } else {
                if (content.find("<hhev_end>") != std::string::npos) {
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
    std::string prompt;
    prompt += "<|begin_of_text|><|start_header_id|>system<|end_header_id|>\n\n" + SOLVER_SYSTEM_PROMPT + "\n<|eot_id|>";
    
    // Replace "{}" in SOLVER_PROMPT with chat_hist
    std::string user_block = SOLVER_PROMPT;
    size_t pos = user_block.find("{}");
    if (pos != std::string::npos) {
        user_block.replace(pos, 2, chat_hist);
    }

    prompt += "<|start_header_id|>user<|end_header_id|>\n\n" + user_block + "<|eot_id|>";
    prompt += "<|start_header_id|>assistant<|end_header_id|>\n\n";

    return prompt;
}


