#include <iostream>
#include "DataProcessor.h"
#include <regex>
#include <vector>

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
    // function_call_json = wrap_openai_toolcall(tool_name, arguments);
}


int main() {

    std::vector<std::string> raw_tool_calls = {
        "<control_car_properties>({'propertyId': '<HVAC_AC_ON>', 'areaId': '', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_RECIRC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'INTERNAL'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_RECIRC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'EXTERNAL'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_RECIRC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'AUTO'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_RECIRC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_AC_ON>', 'areaId': '', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_RECIRC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_RECIRC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_RECIRC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SCENARIO_MODE>', 'areaId': '', 'operation': 'set', 'value': 'MAINTAIN'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SCENARIO_MODE>', 'areaId': '', 'operation': 'set', 'value': 'CAMPING'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SCENARIO_MODE>', 'areaId': '', 'operation': 'set', 'value': 'PET'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SCENARIO_MODE>', 'areaId': '', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SCENARIO_MODE>', 'areaId': '', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SCENARIO_MODE>', 'areaId': '', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_POWER_ON>', 'areaId': '', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_POWER_ON>', 'areaId': '', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '', 'operation': 'set', 'value': '27'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '', 'operation': 'increase', 'value': '10'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '', 'operation': 'decrease', 'value': '10'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '', 'operation': 'set', 'value': '32'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '', 'operation': 'set', 'value': '17'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '', 'operation': 'increase', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '', 'operation': 'decrease', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '', 'operation': 'set', 'value': '5'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '', 'operation': 'increase', 'value': '5'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '', 'operation': 'decrease', 'value': '3'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '', 'operation': 'set', 'value': '8'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '', 'operation': 'set', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '', 'operation': 'increase', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '', 'operation': 'decrease', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_DIRECTION>', 'areaId': '', 'operation': 'set', 'value': 'FACE'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_DIRECTION>', 'areaId': '', 'operation': 'set', 'value': 'BODY'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_DIRECTION>', 'areaId': '', 'operation': 'set', 'value': 'FOOT'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_DIRECTION>', 'areaId': '', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_MODE>', 'areaId': '', 'operation': 'set', 'value': 'FREE'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_MODE>', 'areaId': '', 'operation': 'set', 'value': 'SWING'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_MODE>', 'areaId': '', 'operation': 'set', 'value': 'SYMMETRIC'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_ON>', 'areaId': '', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_ON>', 'areaId': '', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_AUTO_ON>', 'areaId': '', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SYNC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_MAX_AC_ON>', 'areaId': '', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_AUTO_ON>', 'areaId': '', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SYNC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_MAX_AC_ON>', 'areaId': '', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_POWER_ON>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_POWER_ON>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': '27'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': '32'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': '17'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '<SEAT_ROW_1_RIGHT>', 'operation': 'increase', 'value': '1'}<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_TEMPERATURE_SET>', 'areaId': '<SEAT_ROW_1>', 'operation': 'decrease', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '<SEAT_ROW_2>', 'operation': 'set', 'value': '5'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '<SEAT_ROW_1>', 'operation': 'increase', 'value': '5'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '<SEAT_ROW_2>', 'operation': 'decrease', 'value': '3'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '<SEAT_ROW_2>', 'operation': 'set', 'value': '8'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '<SEAT_ROW_1>', 'operation': 'increase', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_SPEED>', 'areaId': '<SEAT_ROW_2>', 'operation': 'decrease', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_DIRECTION>', 'areaId': '<SEAT_ROW_2_LEFT>', 'operation': 'set', 'value': 'FACE'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_DIRECTION>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': 'BODY'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_DIRECTION>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'FOOT'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_DIRECTION>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_MODE>', 'areaId': '<SEAT_ROW_2>', 'operation': 'set', 'value': 'FREE'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_MODE>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'SWING'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_MODE>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'SYMMETRIC'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_MODE>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_ON>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_FAN_OUTPUT_ON>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_AUTO_ON>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SYNC_MODE>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_MAX_AC_ON>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_AUTO_ON>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_SYNC_MODE>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_MAX_AC_ON>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_DEFROSTER>', 'areaId': '<FRONT_WINDSHIELD>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_DEFROSTER>', 'areaId': '<REAR_WINDSHIELD>', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<EV_CHARGE_PORT_OPEN>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<EV_CHARGE_PORT_OPEN>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'false'})<hhev_end>"
        // "<control_car_properties>({'propertyId': '<FRONT_TRUNK_ON>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<POWER_TAILGATE_ON>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<POWER_TAILGATE_ON>', 'areaId': '<SEAT_ROW_1>', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_STEERING_WHEEL_HEAT>', 'areaId': '', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<HVAC_STEERING_WHEEL_HEAT>', 'areaId': '', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SLIDING_DOOR_ON>', 'areaId': '', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SLIDING_DOOR_ON>', 'areaId': '', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SLIDING_DOOR_ON>', 'areaId': '<SLIDING_DOOR_LEFT>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SLIDING_DOOR_ON>', 'areaId': '<SLIDING_DOOR_LEFT>', 'operation': 'set', 'value': 'false'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_MASSAGE_MODE>', 'areaId': '', 'operation': 'set', 'value': 'MODEL_5'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_MASSAGE_INTENSITY>', 'areaId': '', 'operation': 'set', 'value': 'MEDIUM'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_MASSAGE_REGION>', 'areaId': '', 'operation': 'set', 'value': 'SEAT'})<hhev_end>",
        // "<set_seat_mode>({'areaId': '', 'operation': 'set', 'value': 'ZERO_GRAVITY'})<hhev_end>",
        // "<set_seat_mode>({'areaId': '', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_POSITION_MEMORY_SET>', 'areaId': '', 'operation': 'set', 'value': '1'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_POSITION_MEMORY_SET>', 'areaId': '', 'operation': 'set', 'value': '2'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_STOW_MODE>', 'areaId': '', 'operation': 'set', 'value': 'FOLD'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_STOW_MODE>', 'areaId': '', 'operation': 'set', 'value': 'STOW'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_STOW_MODE>', 'areaId': '', 'operation': 'set', 'value': 'RESTORE'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_FORE_AFT_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_FORE_AFT_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_HEIGHT_POS>', 'areaId': '<SEAT_ROW_1_RIGHT>', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_HEIGHT_POS>', 'areaId': '<SEAT_ROW_1_RIGHT>', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_BACKREST_ANGLE_POS>', 'areaId': '<SEAT_ROW_1_RIGHT>', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_BACKREST_ANGLE_POS>', 'areaId': '<SEAT_ROW_1_RIGHT>', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_HEADREST_HEIGHT_POS>', 'areaId': '<SEAT_ROW_2_LEFT>', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_HEADREST_HEIGHT_POS>', 'areaId': '<SEAT_ROW_2_LEFT>', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_LUMBAR_HEIGHT_POS>', 'areaId': '<SEAT_ROW_2_RIGHT>', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_LUMBAR_HEIGHT_POS>', 'areaId': '<SEAT_ROW_2_RIGHT>', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_LUMBAR_FORE_AFT_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_LUMBAR_FORE_AFT_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_CUSHION_SIDE_SUPPORT_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_CUSHION_SIDE_SUPPORT_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_LEGREST_HEIGHT_POS>', 'areaId': '<SEAT_ROW_2_RIGHT>', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_LEGREST_HEIGHT_POS>', 'areaId': '<SEAT_ROW_2_RIGHT>', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_LEGREST_FORE_AFT_POS>', 'areaId': '<SEAT_ROW_2_RIGHT>', 'operation': 'increase'})<hhev_end>",
        // "<set_seat_mode>({'areaId': '<SEAT_ROW_2_RIGHT>', 'operation': 'set', 'value': 'FOCUS'})<hhev_end>",
        // "<set_seat_mode>({'areaId': '<SEAT_ROW_2_RIGHT>', 'operation': 'set', 'value': 'OFF'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_STOW_MODE>', 'areaId': '<SEAT_ROW_3_RIGHT>', 'operation': 'set', 'value': 'FOLD'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_STOW_MODE>', 'areaId': '<SEAT_ROW_3_RIGHT>', 'operation': 'set', 'value': 'STOW'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<SEAT_STOW_MODE>', 'areaId': '<SEAT_ROW_3_LEFT>', 'operation': 'set', 'value': 'RESTORE'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<POWER_SUNSHADE>', 'areaId': '<SUNROOF>', 'operation': 'set', 'value': '100'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<POWER_SUNSHADE>', 'areaId': '<SUNROOF>', 'operation': 'set', 'value': '0'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<POWER_SUNSHADE>', 'areaId': '', 'operation': 'set', 'value': '100'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<POWER_SUNSHADE>', 'areaId': '', 'operation': 'set', 'value': '0'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<POWER_SUNSHADE>', 'areaId': '<SEAT_ROW_2_RIGHT>', 'operation': 'set', 'value': '100'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<POWER_SUNSHADE>', 'areaId': '<SEAT_ROW_2_LEFT>', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'set', 'value': '100'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'set', 'value': '0'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'set', 'value': '30'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'set', 'value': 30})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'increase'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'set', 'value': '10'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'set', 'value': '10'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'set', 'value': '100'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '', 'operation': 'set', 'value': '0'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_1_RIGHT>', 'operation': 'set', 'value': '100'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_1_RIGHT>', 'operation': 'set', 'value': '0'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_1_RIGHT>', 'operation': 'set', 'value': '50'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_1_RIGHT>', 'operation': 'set', 'value': 70})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': '10'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'decrease'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': '10'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': '100'})<hhev_end>",
        // "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': '0'})<hhev_end>",
        // "<search_and_show_place>({'keyword': '時尚傢俱坊'})<hhev_end>",
        // "<search_and_show_place>({'keyword': '龍山寺'})<hhev_end>"
        "<control_car_properties>({'propertyId': '<POWER_TAILGATE_ON>', 'areaId': '<SEAT_ROW_1_LEFT>', 'operation': 'set', 'value': 'true'})<hhev_end>",
        "<control_car_properties>({'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_2>', 'operation': 'increase'})<hhev_split><nav_start>({'keyword': '超商', 'name': '南方百貨', 'address': '屏東市勝利路210號', 'latitude': 22.678345, 'longitude': 120.494321, 'fromMemory': False})<hhev_split><search_and_show_place>({'keyword': '汽車維修廠'})<hhev_split><control_car_properties>({'properties': [{'propertyId': 'SRDC_HVAC_FAN_OUTPUT_MODE', 'areaId': 'SEAT_ROW_2_RIGHT', 'operation': 'set', 'value': 'FREE'}]})<hhev_split><control_car_properties>({'properties': [{'propertyId': 'SRDC_HVAC_FAN_OUTPUT_MODE', 'areaId': None, 'operation': 'set', 'value': 'FREE'}]})<hhev_split><control_car_properties>([{'propertyId': '<HVAC_SYNC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'false'}])<hhev_end>"
    };

    DataProcessor dp;

    // test tool call parsing
    std::string text = u8"<control_car_properties>([{'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_2>', 'operation': 'increase'}])<hhev_split><nav_start>({'keyword': '超商', 'name': '南方百貨', 'address': '屏東市勝利路210號', 'latitude': 22.678345, 'longitude': 120.494321, 'fromMemory': False})<hhev_split><search_and_show_place>({'keyword': '汽車維修廠'})<hhev_split><control_car_properties>({'properties': [{'propertyId': 'SRDC_HVAC_FAN_OUTPUT_MODE', 'areaId': 'SEAT_ROW_2_RIGHT', 'operation': 'set', 'value': 'FREE'}]})<hhev_split><control_car_properties>({'properties': [{'propertyId': 'SRDC_HVAC_FAN_OUTPUT_MODE', 'areaId': None, 'operation': 'set', 'value': 'FREE'}]})<hhev_split><control_car_properties>([{'propertyId': '<HVAC_SYNC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'false'}])<hhev_end>";
    
    for (auto &tt: raw_tool_calls) {
        std::cout << "\n" << tt << "\n" << std::endl;
        text = tt;
        auto calls = dp.parseToolCalls(text);

        for (auto &c : calls) {
            std::cout << "Func: " << c.name << " Args: " << c.arguments << std::endl;

            std::string wot = wrap_openai_toolcall(c.name, c.arguments);
            std::cout << wot << std::endl;
        }
    }

    std::string toolCalls = dp.getOpenAIToolCall(text, "GLOBAL");
    std::cout << "\n\n[toolCalls] " << toolCalls << std::endl;

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


   