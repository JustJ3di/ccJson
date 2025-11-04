#ifndef CCJSON_JSON_HPP
#define CCJSON_JSON_HPP
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <variant>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>


class JsonValue;
//*****************ALIAS*********
using JsonObject = std::map<std::string, JsonValue>;
using JsonArray  = std::vector<JsonValue>;

// JsonValue can hold null, bool, number (double), string, array or object.
class JsonValue {
public:
    using Variant = std::variant<std::nullptr_t, bool, double, std::string, JsonArray, JsonObject>;
    Variant v;

    // Constructors
    JsonValue() noexcept : v(nullptr) {}
    JsonValue(std::nullptr_t) noexcept : v(nullptr) {}
    JsonValue(bool b) : v(b) {}
    JsonValue(int i) : v(static_cast<double>(i)) {}
    JsonValue(long l) : v(static_cast<double>(l)) {}
    JsonValue(double d) : v(d) {}
    JsonValue(const char* s) : v(std::string(s)) {}
    JsonValue(const std::string& s) : v(s) {}
    JsonValue(const JsonArray& a) : v(a) {}
    JsonValue(const JsonObject& o) : v(o) {}



    // Helpers for type-checking
    bool is_null()   const { return std::holds_alternative<std::nullptr_t>(v); }
    bool is_bool()   const { return std::holds_alternative<bool>(v); }
    bool is_number() const { return std::holds_alternative<double>(v); }
    bool is_string() const { return std::holds_alternative<std::string>(v); }
    bool is_array()  const { return std::holds_alternative<JsonArray>(v); }
    bool is_object() const { return std::holds_alternative<JsonObject>(v); }

    bool& as_bool() { return std::get<bool>(v); }
    double& as_number() { return std::get<double>(v); }
    std::string& as_string() { return std::get<std::string>(v); }
    JsonArray& as_array() { return std::get<JsonArray>(v); }
    JsonObject& as_object() { return std::get<JsonObject>(v); }

    const bool& as_bool() const { return std::get<bool>(v); }
    const double& as_number() const { return std::get<double>(v); }
    const std::string& as_string() const { return std::get<std::string>(v); }
    const JsonArray& as_array() const { return std::get<JsonArray>(v); }
    const JsonObject& as_object() const { return std::get<JsonObject>(v); }

    // OPERATORS OVERLOAD :)
    JsonValue& operator[](const std::string& key) {
        if (!is_object()) v = JsonObject{};
        return std::get<JsonObject>(v)[key];
    }

    JsonValue& operator[](std::size_t i) {
        if (!is_array()) throw std::runtime_error("JsonValue is not an array");
        auto& arr = std::get<JsonArray>(v);
        if (i >= arr.size()) throw std::out_of_range("Json array index out of range");
        return arr[i];
    }

    // push_back
    void push_back(const JsonValue& item) {
        if (!is_array()) v = JsonArray{};
        std::get<JsonArray>(v).push_back(item);
    }



    // Serialize to JSON string
    std::string to_string() const {
        std::ostringstream os;
        write(os);
        return os.str();
    }

    std::ofstream dump(const char *filename)const {
        std::ofstream os(filename);
        if (!os.is_open())throw std::runtime_error("Could not open file");
        write(os);
        return os;

    }

    friend std::ostream& operator<<(std::ostream& os, const JsonValue& obj) {
        os<<obj.to_string();
    }


private:




    static std::string escape_string(const std::string& s) {
        std::ostringstream o;
        for (unsigned char c : s) {
            switch (c) {
                case '\"': o << "\\\""; break;
                case '\\': o << "\\\\"; break;
                case '\b': o << "\\b";  break;
                case '\f': o << "\\f";  break;
                case '\n': o << "\\n";  break;
                case '\r': o << "\\r";  break;
                case '\t': o << "\\t";  break;
                default:
                    if (c < 0x20) {
                        o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c << std::dec;
                    } else {
                        o << c;
                    }
            }
        }
        return o.str();
    }

    void write(std::ostream& os) const {
        if (is_null()) {
            os << "null";
        } else if (is_bool()) {
            os << (std::get<bool>(v) ? "true" : "false");
        } else if (is_number()) {
            // print number without unnecessary trailing zeros
            double d = std::get<double>(v);
            if (std::isfinite(d)) {
                std::ostringstream tmp;
                tmp << std::setprecision(15) << d;
                std::string s = tmp.str();
                // trim trailing zeros and decimal dot
                if (s.find('.') != std::string::npos) {
                    while (!s.empty() && s.back() == '0') s.pop_back();
                    if (!s.empty() && s.back() == '.') s.pop_back();
                    if (s.empty()) s = "0";
                }
                os << s;
            } else {
                os << "null"; // JSON has no NaN/Inf; choose null
            }
        } else if (is_string()) {
            os << '"' << escape_string(std::get<std::string>(v)) << '"';
        } else if (is_array()) {
            os << '[';
            const auto& arr = std::get<JsonArray>(v);
            for (size_t i = 0; i < arr.size(); ++i) {
                if (i) os << ',';
                arr[i].write(os);
            }
            os << ']';
        } else if (is_object()) {
            os << '{';
            const auto& obj = std::get<JsonObject>(v);
            bool first = true;
            for (const auto& kv : obj) {
                if (!first) os << ',';
                first = false;
                os << '"' << escape_string(kv.first) << "\":";
                kv.second.write(os);
            }
            os << '}';
        }
    }

};


#endif //CCJSON_JSON_HPP