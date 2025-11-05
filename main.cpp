#include "JSON.hpp"


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

        std::istringstream json(R"({
        "name": "Emanuele",
        "age": 27,
        "active": true,
        "skills": ["C++", "Python", "JSON"],
        "address": { "city": "Rome", "zip": "00100" },
        "height": 1.82,
        "married": false,
        "nothing": null
        })");

        JsonParser parser;
        JsonValue second_root = parser.parse(json);

        std::cout << "JSON parsed successfully!\n";

        //test
        std::cout<<second_root["skills"][1]<<'\n';




    return 0;
}
