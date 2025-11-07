#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <stdexcept>
#include <cctype>
#include <cmath>

//***************** ALIASES *********
class JsonValue;
using JsonObject = std::map<std::string, JsonValue>;
using JsonArray  = std::vector<JsonValue>;

//===========================================================
//                       JsonValue
//===========================================================
class JsonValue {
public:
    using Variant = std::variant<std::nullptr_t, bool, double, std::string, JsonArray, JsonObject>;
    Variant v;

    // Constructors
    JsonValue() noexcept;
    JsonValue(std::nullptr_t) noexcept;
    JsonValue(bool b);
    JsonValue(int i);
    JsonValue(long l);
    JsonValue(double d);
    JsonValue(const char* s);
    JsonValue(const std::string& s);
    JsonValue(const JsonArray& a);
    JsonValue(const JsonObject& o);

    // Type check helpers
    bool is_null()   const;
    bool is_bool()   const;
    bool is_number() const;
    bool is_string() const;
    bool is_array()  const;
    bool is_object() const;

    // Accessors
    bool& as_bool();
    double& as_number();
    std::string& as_string();
    JsonArray& as_array();
    JsonObject& as_object();

    const bool& as_bool() const;
    const double& as_number() const;
    const std::string& as_string() const;
    const JsonArray& as_array() const;
    const JsonObject& as_object() const;

    // Operators
    JsonValue& operator[](const std::string& key);
    JsonValue& operator[](std::size_t i);
    void push_back(const JsonValue& item);

    // Serialization
    std::string to_string() const;
    std::ofstream dump(const char *filename) const;

    friend std::ostream& operator<<(std::ostream& os, const JsonValue& obj);

private:
    static std::string escape_string(const std::string& s);
    void write(std::ostream& os) const;
};

//===========================================================
//                       JsonParser
//===========================================================
class JsonParser {
public:
    JsonValue parse(std::istream& in);

private:
    void skip_ws(std::istream& in);
    JsonValue parse_dict(std::istream& in);
    JsonValue parse_array(std::istream& in);
    JsonValue parse_value(std::istream& in);
    std::string parse_string(std::istream& in);
    JsonValue parse_number(std::istream& in);
    JsonValue parse_bool(std::istream& in);
    JsonValue parse_null(std::istream& in);
};

