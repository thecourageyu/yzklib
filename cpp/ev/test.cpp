#include <iostream>
#include <regex>
#include <vector>
#include "DataProcessor.h"
#include "../LeetCode/YZKHelper.h"

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


int main() {

    // std::vector<std::pair<std::string, std::string>> history_vector = {
    //     {"user",       "我想要去國父紀念館。"},
    //     // {"assistants", "<search_and_show_place>([{'keyword': '國父紀念館'}])<hhev_end>"},
    //     {"assistants", "<search_and_show_place>({'keyword': '國父紀念館'})<hhev_end>"},
    //     {"ipython",    "[{\"name\": \"search_and_show_place\", \"status\": \"success\", \"message\": \"地點查詢成功\", \"poi\": [{\"name\": \"國父紀念館\", \"address\": \"台北市信義區仁愛路四段505號\", \"latitude\": 25.040112, \"longitude\": 121.560503, \"fromMemory\": false}]}]"},
    //     {"assistants", "response 123"},
    // };

    // std::vector<std::pair<std::string, std::string>> history_vector = {
    //     {"user",       "我想要去國父紀念館。"},
    //     {"assistants", "<search_and_show_place>(國父紀念館)<hhev_end>"},  // <<< json 解不出 國父紀念館 ? 要 "國父紀念館"?
    //     {"ipython",    "{\"name\": \"search_and_show_place\", \"status\": \"success\", \"message\": \"地點查詢成功\", \"poi\": [{\"name\": \"國父紀念館\", \"address\": \"台北市信義區仁愛路四段505號\", \"latitude\": 25.040112, \"longitude\": 121.560503, \"fromMemory\": false}]}"}
    // };

    std::vector<std::pair<std::string, std::string>> history_vector = {
        {"user",       "開啟休憩模式"},
        {"assistants", "<set_seat_mode>(<args_split>set<args_split>BREAK)<hhev_end>"},  // <<< json 解不出 國父紀念館 ? 要 "國父紀念館"?
        // {"ipython",    "{"role":"tool","tool_call_id":"call_1","content":"[{\0"name\": \"control_car_properties\", \"status\": \"success\", \"message\": \"操作成功\", \"affectedArea\": \"SEAT_ROW_1\"}]"}"}
        {"ipython",    "[{\"name\": \"control_car_properties\", \"status\": \"success\", \"message\": \"操作成功\", \"affectedArea\": \"SEAT_ROW_1\"}]"}
    };

    // std::vector<std::pair<std::string, std::string>> history_vector = {
    //     {"user",       "開啟休憩模式"},
    //     {"assistants", "<set_seat_mode>(<args_split>set<args_split>BREAK)<hhev_end>"},
    //     {"ipython", "[{\"content\":{\"name\":\"set_seat_mode\",\"status\":\"success\",\"message\":\"操作成功\",\"affectedArea\":[]},\"role\":\"tool\",\"tool_call_id\":\"call_1\"}]"},
    // };



    // std::vector<std::string> raw_tool_calls = {
    //     // "<control_car_properties>({'propertyId': 'SEAT_CUSHION_SIDE_SUPPORT_POS', 'areaId': 'SEAT_ROW_3', 'operation': 'decrease'})<hhev_end>",
    //     // "<hide_poi_list>({})<hhev_end>",
    //     // "<nav_stop>({})<hhev_end>",
    //     // "<search_and_show_place>({'keyword': '童年時光托嬰中心'})<hhev_end>",
    //     // "<nav_start>({'keyword': '義式鄉村風的眼科診所', 'name': '基隆光明眼科', 'address': '基隆市仁愛區孝三路166號', 'latitude': 25.1296, 'longitude': 121.7424, 'routeType': 'FAST', 'isWaypoints': False, 'fromMemory': True})<hhev_end>",
    //     // "<get_hhtd_info>(<args_split>論壇地點在哪裡？)<hhev_end>",
    //     "<search_and_show_place>(文心運動館 淡水老街附近)<hhev_end>",
    //     "<nav_start>(文心運動館<args_split>榮星體育館<args_split>台北市中山區松江路367號<args_split>25.065321<args_split>121.533234<args_split>FAST<args_split>False<args_split>)<hhev_end>",
    //     "<control_car_properties>(HVAC_TEMPERATURE_SET<args_split>SEAT_ALL<args_split>increase<args_split>3)<hhev_end>",
    // };

    std::string fname = "/work/YZKMSI/yzklib/cpp/ev/tool_call_examples.txt";

    DataProcessor dp;
    YZKHelper hp;
    

    vector<string> JeffInput = {
        "[{\"arguments\":{\"areaId\":\"SEAT_ROW_1_RIGHT\",\"operation\":\"set\",\"value\":\"BREAK\"},\"name\":\"set_seat_mode\"}]",
        "[{\"name\": \"control_car_properties\", \"arguments\": {\"propertyId\": \"HVAC_MAX_AC_ON\", \"areaId\": \"\", \"operation\": \"set\", \"value\": \"on\"}}]",
        "[{\"name\": \"control_car_properties\", \"arguments\": {\"propertyId\": \"HVAC_MAX_AC_ON\", \"operation\": \"set\", \"value\": \"on\"}}]",
        "[{\"name\": \"get_hhtd_info\", \"arguments\": {\"areaId\": \"\", \"query\": \"論壇地點在哪裡？\"}}]",
        "[{\"name\": \"get_hhtd_info\", \"arguments\": {\"areaId\": \"YZKAreaId\", \"query\": \"論壇地點在哪裡？\"}}]",
        "[{\"name\": \"control_car_properties\", \"arguments\": {\"propertyId\": \"HVAC_MAX_AC_ON\", \"areaId\": \"YZKAreaId\", \"operation\": \"set\", \"value\": \"on\"}}]",
        "[{\"name\": \"get_hhtd_info\", \"arguments\": {\"areaId\": \"YZKAreaId\", \"query\": \"論壇地點在哪裡？\"}}, {\"name\": \"control_car_properties\", \"arguments\": {\"propertyId\": \"HVAC_MAX_AC_ON\", \"areaId\": \"YZKAreaId\", \"operation\": \"set\", \"value\": \"on\"}}]",
        "[{\"name\": \"nav_stop\", \"arguments\": {}}]",
        
    };

    for (auto& c : JeffInput) {
        cout << "[JeffToolCall]" << c << endl;
        string jeff2SLMToolCall = dp.Jeff2SLMToolCalls(c);
        cout << "[Jeff2SLMToolCall]" << jeff2SLMToolCall << endl;
    }

    // string jimmyText = dp.toJimmyMessage123(history_vector);
    // printf("[Jimmy] %s\n", jimmyText.c_str());

    vector<json> messages_manual = {
        json{{"message", "開啟休憩模式"}, {"role", "user"}},
        json{{"message","[{\"arguments\":{\"areaId\":\"\",\"operation\":\"set\",\"value\":\"BREAK\"},\"name\":\"set_seat_mode\"}]"},{"role","function_call"}},
        json{{"message","[{\"name\": \"control_car_properties\", \"status\": \"success\", \"message\": \"操作成功\", \"affectedArea\": \"SEAT_ROW_1\"}]"},{"role","observation"}}
    };

    vector<nlohmann::ordered_json> jimmyJSON = dp.toJimmyMessage(history_vector);
    for (const auto& l : jimmyJSON) {
        printf("[Jimmy] %s\n", l.dump().c_str());
    }


    std::vector<std::string> raw_tool_calls = hp.load_file(fname);

    // test tool call parsing
    std::string otext = u8"<control_car_properties>([{'propertyId': '<WINDOW_POS>', 'areaId': '<SEAT_ROW_2>', 'operation': 'increase'}])<hhev_split><nav_start>({'keyword': '超商', 'name': '南方百貨', 'address': '屏東市勝利路210號', 'latitude': 22.678345, 'longitude': 120.494321, 'fromMemory': False})<hhev_split><search_and_show_place>({'keyword': '汽車維修廠'})<hhev_split><control_car_properties>({'properties': [{'propertyId': 'SRDC_HVAC_FAN_OUTPUT_MODE', 'areaId': 'SEAT_ROW_2_RIGHT', 'operation': 'set', 'value': 'FREE'}]})<hhev_split><control_car_properties>({'properties': [{'propertyId': 'SRDC_HVAC_FAN_OUTPUT_MODE', 'areaId': None, 'operation': 'set', 'value': 'FREE'}]})<hhev_split><control_car_properties>([{'propertyId': '<HVAC_SYNC_MODE>', 'areaId': '', 'operation': 'set', 'value': 'false'}])<hhev_end>";
    
    for (auto &text: raw_tool_calls) {
        if (text.substr(0, 1) == "#" or text.length() == 0) continue;
        std::cout << "[InputText]\n" << text << "\n" << std::endl;
        
        // text = tt;

        // auto calls = dp.parseToolCalls(text);
        // for (auto &c : calls) {
        //     std::cout << "Func: " << c.name << " Args: " << c.arguments << std::endl;

        //     std::string wot = wrap_openai_toolcall(c.name, c.arguments);
        //     std::cout << wot << std::endl;
        // }

        std::string toolCalls = dp.getOpenAIToolCall(text, "GLOBAL123YZK");
        
        std::cout << "\n\n[toolCalls] " << toolCalls << "\n" << std::endl;
    }



    // test chat template
    // std::vector<std::pair<std::string, std::string>> history = {
    //     {"user", "Hello"},
    //     {"assistant", "<nav_start>({'keyword': '超商', 'name': '南方百貨', 'address': '屏東市勝利路210號', 'latitude': 22.678345, 'longitude': 120.494321, 'fromMemory': False})<hhev_end>"},
    //     {"ipython", "{'message': 'good good good'}"},
    // };
    
    std::vector<std::pair<std::string, std::string>> history = {
        {"user", "開車到成功大學醫院"},
        {"assistant", "<search_and_show_place>({'keyword': '成功大學醫院'})<hhev_end>"},
        {"ipython", "[{'content':'{\"name\":\"search_and_show_place\",\"status\":\"success\",\"message\":\"地點查詢成功\",\"poi\":[{\"name\":\"成大醫院\",\"type\":\"其他\",\"address\":\"台南市北區勝利路138號\",\"latitude\":23.000703,\"longitude\":120.219661,\"description\":\"成大醫院是國立成功大學附設醫院，為南台灣重要的醫學中心，提供完整的醫療服務與先進的醫療設備。結合醫學教育與臨床研究，擁有優秀的醫療團隊，是台南地區民眾信賴的大型綜合醫院，也是醫學教育的重要基地。\",\"fromMemory\":false}]}','role':'tool','tool_call_id':'call_1'}]"},
    };

    // std::cout << dp.buildPlannerChatTemplate(history) << std::endl;
    // std::cout << dp.buildSolverChatTemplate(history) << std::endl;


    return 0;
}


   