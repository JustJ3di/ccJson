#include "JSON.hpp"
// Simple demo usage
int main() {
    JsonValue root = JsonObject{}; // root object

    // Add basic values
    root["name"] = "Alice";
    root["age"] = 29;
    root["married"] = false;
    root["bio"] = nullptr;

    // Nested object
    root["address"] = JsonObject{
        {"street", "Via Roma 1"},
        {"city", "Rome"},
        {"zip", "00100"}
    };

    // Array
    JsonValue hobbies = JsonArray{};
    hobbies.push_back("reading");
    hobbies.push_back("cooking");
    hobbies.push_back("climbing");
    root["hobbies"] = hobbies;

    //test overload []
    std::cout<<root["hobbies"][1].to_string()<<'\n';
    root["hobbies"][1] = 10;

    //test nested structures
    root["scores"] = JsonArray{
        JsonObject{{"subject","math"},{"score",95}},
        JsonObject{{"subject","physics"},{"score",88}}
    };

    std::cout << root.to_string() << std::endl;

    std::ofstream os = root.dump("ciao.json");
    os.close();

    // Pretty example (not implemented here): you could write a pretty-print function that indents
    return 0;
}
