#include "JSON.hpp"

#include "json.hpp"

//===========================================================
//                  JsonValue implementation
//===========================================================

JsonValue::JsonValue() noexcept : v(nullptr) {}
JsonValue::JsonValue(std::nullptr_t) noexcept : v(nullptr) {}
JsonValue::JsonValue(bool b) : v(b) {}
JsonValue::JsonValue(int i) : v(static_cast<double>(i)) {}
JsonValue::JsonValue(long l) : v(static_cast<double>(l)) {}
JsonValue::JsonValue(double d) : v(d) {}
JsonValue::JsonValue(const char* s) : v(std::string(s)) {}
JsonValue::JsonValue(const std::string& s) : v(s) {}
JsonValue::JsonValue(const JsonArray& a) : v(a) {}
JsonValue::JsonValue(const JsonObject& o) : v(o) {}

bool JsonValue::is_null()   const { return std::holds_alternative<std::nullptr_t>(v); }
bool JsonValue::is_bool()   const { return std::holds_alternative<bool>(v); }
bool JsonValue::is_number() const { return std::holds_alternative<double>(v); }
bool JsonValue::is_string() const { return std::holds_alternative<std::string>(v); }
bool JsonValue::is_array()  const { return std::holds_alternative<JsonArray>(v); }
bool JsonValue::is_object() const { return std::holds_alternative<JsonObject>(v); }

bool& JsonValue::as_bool() { return std::get<bool>(v); }
double& JsonValue::as_number() { return std::get<double>(v); }
std::string& JsonValue::as_string() { return std::get<std::string>(v); }
JsonArray& JsonValue::as_array() { return std::get<JsonArray>(v); }
JsonObject& JsonValue::as_object() { return std::get<JsonObject>(v); }

const bool& JsonValue::as_bool() const { return std::get<bool>(v); }
const double& JsonValue::as_number() const { return std::get<double>(v); }
const std::string& JsonValue::as_string() const { return std::get<std::string>(v); }
const JsonArray& JsonValue::as_array() const { return std::get<JsonArray>(v); }
const JsonObject& JsonValue::as_object() const { return std::get<JsonObject>(v); }

JsonValue& JsonValue::operator[](const std::string& key) {
    if (!is_object()) v = JsonObject{};
    return std::get<JsonObject>(v)[key];
}

JsonValue& JsonValue::operator[](std::size_t i) {
    if (!is_array()) throw std::runtime_error("JsonValue is not an array");
    auto& arr = std::get<JsonArray>(v);
    if (i >= arr.size()) throw std::out_of_range("Json array index out of range");
    return arr[i];
}

void JsonValue::push_back(const JsonValue& item) {
    if (!is_array()) v = JsonArray{};
    std::get<JsonArray>(v).push_back(item);
}

std::string JsonValue::escape_string(const std::string& s) {
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

void JsonValue::write(std::ostream& os) const {
    if (is_null()) {
        os << "null";
    } else if (is_bool()) {
        os << (std::get<bool>(v) ? "true" : "false");
    } else if (is_number()) {
        double d = std::get<double>(v);
        if (std::isfinite(d)) {
            std::ostringstream tmp;
            tmp << std::setprecision(15) << d;
            std::string s = tmp.str();
            if (s.find('.') != std::string::npos) {
                while (!s.empty() && s.back() == '0') s.pop_back();
                if (!s.empty() && s.back() == '.') s.pop_back();
                if (s.empty()) s = "0";
            }
            os << s;
        } else {
            os << "null";
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

std::string JsonValue::to_string() const {
    std::ostringstream os;
    write(os);
    return os.str();
}

std::ofstream JsonValue::dump(const char *filename) const {
    std::ofstream os(filename);
    if (!os.is_open())
        throw std::runtime_error("Could not open file");
    write(os);
    return os;
}

std::ostream& operator<<(std::ostream& os, const JsonValue& obj) {
    os << obj.to_string();
    return os;
}

//===========================================================
//                  JsonParser implementation
//===========================================================

JsonValue JsonParser::parse(std::istream& in) {
    skip_ws(in);
    char c = in.peek();
    if (c == '{') return parse_dict(in);
    if (c == '[') return parse_array(in);
    throw std::runtime_error("Invalid JSON start");
}

void JsonParser::skip_ws(std::istream& in) {
    while (std::isspace(in.peek())) in.get();
}

JsonValue JsonParser::parse_dict(std::istream& in) {
    JsonObject obj;
    char c;
    in.get(c);
    skip_ws(in);

    std::string key;
    while (true) {
        c = in.peek();
        if (c == '}') { in.get(); break; }

        key = parse_string(in);
        skip_ws(in);

        if (in.get() != ':')
            throw std::runtime_error("Expected ':' in object");
        skip_ws(in);

        obj[key] = parse_value(in);
        skip_ws(in);

        c = in.peek();
        if (c == ',') { in.get(); skip_ws(in); continue; }
        else if (c == '}') { in.get(); break; }
        else throw std::runtime_error("Expected ',' or '}' in object");
    }
    return JsonValue(obj);
}

JsonValue JsonParser::parse_array(std::istream& in) {
    JsonArray arr;
    char c;
    in.get(c);
    skip_ws(in);

    while (true) {
        c = in.peek();
        if (c == ']') { in.get(); break; }
        arr.push_back(parse_value(in));
        skip_ws(in);
        c = in.peek();
        if (c == ',') { in.get(); skip_ws(in); continue; }
        else if (c == ']') { in.get(); break; }
        else throw std::runtime_error("Expected ',' or ']' in array");
    }
    return JsonValue(arr);
}

JsonValue JsonParser::parse_value(std::istream& in) {
    skip_ws(in);
    char c = in.peek();
    if (c == '{') return parse_dict(in);
    if (c == '[') return parse_array(in);
    if (c == '"') return JsonValue(parse_string(in));
    if (std::isdigit(c) || c == '-') return parse_number(in);
    if (c == 't' || c == 'f') return parse_bool(in);
    if (c == 'n') return parse_null(in);
    throw std::runtime_error("Unexpected character in value");
}

std::string JsonParser::parse_string(std::istream& in) {
    char c;
    if (in.get() != '"')
        throw std::runtime_error("Expected string opening quote");

    std::ostringstream s;
    while (in.get(c)) {
        if (c == '"') break;
        if (c == '\\') {
            char esc; in.get(esc);
            switch (esc) {
                case 'n': s << '\n'; break;
                case 't': s << '\t'; break;
                case 'r': s << '\r'; break;
                case '"': s << '"';  break;
                case '\\': s << '\\'; break;
                default: s << esc; break;
            }
        } else {
            s << c;
        }
    }
    return s.str();
}

JsonValue JsonParser::parse_number(std::istream& in) {
    std::string num;
    char c;
    while (std::isdigit(in.peek()) || in.peek() == '.' || in.peek() == '-' ||
           in.peek() == '+' || in.peek() == 'e' || in.peek() == 'E') {
        in.get(c);
        num.push_back(c);
    }
    return JsonValue(std::stod(num));
}

JsonValue JsonParser::parse_bool(std::istream& in) {
    std::string word;
    for (int i = 0; i < 5 && std::isalpha(in.peek()); ++i) {
        char c = in.peek();
        if (!std::isalpha(c)) break;
        in.get(c);
        word.push_back(c);
    }
    if (word == "true") return JsonValue(true);
    if (word == "false") return JsonValue(false);
    throw std::runtime_error("Invalid boolean value");
}

JsonValue JsonParser::parse_null(std::istream& in) {
    std::string word;
    for (int i = 0; i < 4 && std::isalpha(in.peek()); ++i) {
        char c = in.peek();
        in.get(c);
        word.push_back(c);
    }
    if (word == "null") return JsonValue(nullptr);
    throw std::runtime_error("Invalid null value");
}
