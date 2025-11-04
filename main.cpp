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

    std::cout<<root["hobbies"][1].to_string()<<'\n';

    // More complex nested structures
    root["scores"] = JsonArray{
        JsonObject{{"subject","math"},{"score",95}},
        JsonObject{{"subject","physics"},{"score",88}}
    };

    std::cout << root.to_string() << std::endl;

    std::ofstream os = root.dump("ciao.json");
    std::ofstream outFile("example.txt");
    // Check if the file is open
    if (!outFile.is_open()) {
        std::cout << "Error opening file for writing." << std::endl;
        return 1;
    }
    // Write data to the file
    outFile << "Hello, World!" << std::endl;
    outFile << "This is a file handling example in C++." << std::endl;
    // Close the file
    outFile.close();
    std::cout << "Data written to the file successfully." << std::endl;

    // Pretty example (not implemented here): you could write a pretty-print function that indents
    return 0;
}
