#include <iostream>
#include "DataProcessor.h"

int main() {
    DataProcessor dp;

    // test tool call parsing
    std::string text = "<nav_start>({'keyword': '超商', 'name': '南方百貨', 'address': '屏東市勝利路210號', 'latitude': 22.678345, 'longitude': 120.494321, 'fromMemory': False})<hhev_split><search_and_show_place>({'keyword': '汽車維修廠'})<hhev_split><control_car_properties>({'properties': [{'propertyId': 'SRDC_HVAC_FAN_OUTPUT_MODE', 'areaId': 'SEAT_ROW_2_RIGHT', 'operation': 'set', 'value': 'FREE'}]})<hhev_end>";
    auto calls = dp.parseToolCalls(text);
    for (auto &c : calls) {
        std::cout << "Func: " << c.name << " Args: " << c.arguments << std::endl;
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
