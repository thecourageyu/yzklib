#include <iostream>
#include "DataProcessor.h"
#include <regex>

 // 將空的 "areaId": "" 補成 doa_location（僅在 name 為 control_car_properties 且確實有空的 areaId 時）
std::string fill_doa_location(const std::string& doa_location, const std::string& reply)
{
    static const std::regex name_pat(R"("name"\s*:\s*"control_car_properties")");
    static const std::regex empty_area_pat(R"("areaId"\s*:\s*""\s*)");
    if (std::regex_search(reply, name_pat) &&
        std::regex_search(reply, empty_area_pat)) {
        return std::regex_replace(
            reply,
            empty_area_pat,
            std::string("\"areaId\":\"") + doa_location + "\""
        );
    }
    return reply;
}

// 產生符合 OpenAI Tool Calling 的 JSON 字串
// 規則：
//  1) parameters 是 array → arguments 就是該 array
//  2) parameters 唯一鍵為 "properties" 且其值為 array → arguments 就是該 array
//  3) 其他情況 → arguments 為完整的 parameters 物件
std::string wrap_openai_toolcall(const std::string& tool_name, const nlohmann::json& parameters) 
{
    nlohmann::json arguments_obj;

    if (parameters.is_array()) {
        arguments_obj = parameters;
    } else if (parameters.is_object() &&
            parameters.size() == 1 &&
            parameters.contains("properties") &&
            parameters.at("properties").is_array()) {
        arguments_obj = parameters.at("properties");
    } else {
        arguments_obj = parameters;
    }

    nlohmann::json payload;
    payload["tool_calls"] = nlohmann::json::array();
    nlohmann::json call;
    call["id"] = "call_1";
    call["type"] = "function";
    nlohmann::json fn;
    fn["name"] = tool_name;
    fn["arguments"] = arguments_obj.dump(); // 內層 arguments 要是「字串化的 JSON」
    call["function"] = fn;
    payload["tool_calls"].push_back(call);

    // ex. {tool_calls: [{'name': ..., 'arguments': ..., 'id': call_1, 'type': function, }]}
    return payload.dump(); // 緊湊輸出、保留 UTF-8

    //payload["tool_calls"] = [ {function: {arg, name}, id: ..., type: ...}, {}]
}

void slm_parse(std::string llm_result, std::string position)
{
    
    // llm_result: {"name": "search_and_show_place", "parameters": {"keyword": "國父紀念館"}}
    // position: "SEAT_ROW_1_RIGHT"

    std::string doa_location       = position;
    std::string reply              = llm_result;
    std::string function_call_json = "";

    // std::string doa_reply = fill_doa_location(doa_location, reply);
    // nlohmann::json parsed_reply = nlohmann::json::parse(doa_reply);

    // std::string tool_name = parsed_reply.at("name").get<std::string>();
    // nlohmann::json arguments = parsed_reply.at("parameters");

    

    // Result
    function_call_json = wrap_openai_toolcall(tool_name, arguments);
}


int main() {
    DataProcessor dp;

    // test tool call parsing
    std::string text = "<nav_start>({'keyword': '超商', 'name': '南方百貨', 'address': '屏東市勝利路210號', 'latitude': 22.678345, 'longitude': 120.494321, 'fromMemory': False})<hhev_split><search_and_show_place>({'keyword': '汽車維修廠'})<hhev_split><control_car_properties>({'properties': [{'propertyId': 'SRDC_HVAC_FAN_OUTPUT_MODE', 'areaId': 'SEAT_ROW_2_RIGHT', 'operation': 'set', 'value': 'FREE'}]})<hhev_end>";
    auto calls = dp.parseToolCalls(text);
    for (auto &c : calls) {
        std::cout << "Func: " << c.name << " Args: " << c.arguments << std::endl;

        std::string wot = wrap_openai_toolcall(c.name, c.arguments);
        std::cout << wot << std::endl;
    }

    // test chat template
    std::vector<std::pair<std::string, std::string>> history = {
        {"user", "Hello"},
        {"assistant", "<nav_start>({'keyword': '超商', 'name': '南方百貨', 'address': '屏東市勝利路210號', 'latitude': 22.678345, 'longitude': 120.494321, 'fromMemory': False})<hhev_end>"},
        {"ipython", "{'message': 'good good good'}"},
    };
    // std::cout << dp.build_chat_template(history) << std::endl;
    std::cout << dp.buildPlannerChatTemplate(history) << std::endl;
    std::cout << dp.buildSolverChatTemplate(history) << std::endl;


    return 0;
}


   