#ifndef JSON_PARSER_H
#define JSON_PARSER_H
#include "JSON.hpp"
class JsonParser {
public:
    JsonValue parse(std::istream& in) {
        skip_ws(in);
        char c = in.peek();
        if (c == '{') return parse_dict(in);
        if (c == '[') return parse_array(in);
        throw std::runtime_error("Invalid JSON start");
    }

private:
    // skip whitespace
    void skip_ws(std::istream& in) {
        while (std::isspace(in.peek())) in.get();
    }

    // ---------- parse_dict ----------
    JsonValue parse_dict(std::istream& in) {
        JsonObject obj;
        char c;
        in.get(c); // consume '{'
        skip_ws(in);

        std::string key;
        while (true) {
            c = in.peek();
            if (c == '}') { in.get(); break; }

            // parse key
            key = parse_string(in);
            skip_ws(in);

            // expect ':'
            if (in.get() != ':')
                throw std::runtime_error("Expected ':' in object");
            skip_ws(in);

            // parse value
            obj[key] = parse_value(in);
            skip_ws(in);

            c = in.peek();
            if (c == ',') { in.get(); skip_ws(in); continue; }
            else if (c == '}') { in.get(); break; }
            else throw std::runtime_error("Expected ',' or '}' in object");
        }
        return JsonValue(obj);
    }

    // ---------- parse_array ----------
    JsonValue parse_array(std::istream& in) {
        JsonArray arr;
        char c;
        in.get(c); // consume '['
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
        return  JsonValue(arr);
    }

    // ---------- parse_value ----------
    JsonValue parse_value(std::istream& in) {
        skip_ws(in);
        char c = in.peek();
        if (c == '{') return parse_dict(in);
        if (c == '[') return parse_array(in);
        if (c == '"') return parse_string(in);
        if (std::isdigit(c) || c == '-') return parse_number(in);
        if (c == 't' || c == 'f') return parse_bool(in);
        if (c == 'n') return parse_null(in);
        throw std::runtime_error("Unexpected character in value");
    }

    // ---------- parse_string ----------
    std::string parse_string(std::istream& in) {
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

    // ---------- parse_number ----------
    JsonValue parse_number(std::istream& in) {
        std::string num;
        char c;
        while (std::isdigit(in.peek()) || in.peek() == '.' || in.peek() == '-' || in.peek() == '+'
               || in.peek() == 'e' || in.peek() == 'E') {
            in.get(c);
            num.push_back(c);
        }
        return JsonValue(std::stod(num));
    }

    // ---------- parse_bool ----------
    JsonValue parse_bool(std::istream& in) {
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

    // ---------- parse_null ----------
    JsonValue parse_null(std::istream& in) {
        std::string word;
        for (int i = 0; i < 4 && std::isalpha(in.peek()); ++i) {
            char c = in.peek();
            in.get(c);
            word.push_back(c);
        }
        if (word == "null") return JsonValue(nullptr);
        throw std::runtime_error("Invalid null value");
    }
};
#endif