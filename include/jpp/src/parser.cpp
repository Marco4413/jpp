/* Copyright (c) 2026 [Marco4413](https://github.com/Marco4413/jpp) */

#include "../parser.h"

#include <cmath>

namespace jpp
{

string parser::location::to_string(string_view file_path) const
{
    string result;
    if (!file_path.empty()) {
        result += file_path;
        result += ':';
    }
    result += std::to_string(line+1);
    result += ':';
    result += std::to_string(character+1);
    return result;
}

bool parser::error::is_set() const
{
    return !message.empty();
}

string parser::error::to_string(string_view file_path) const
{
    if (!is_set()) return "";

    string result;
    result += location.to_string(file_path);
    result += ": ";
    result += message;
    return result;
}

parser::parser(string_view source, options options)
    : m_decoder(source)
    , m_options(options)
{}

value parser::parse(error *error)
{
    m_error = {};
    value result;
    if (parse_value(result, 0) && m_decoder.get_remaining_bytes() > 0) {
        emit_error("did not reach end of file");
    }
    if (m_error.is_set()) {
        if (error) *error = std::move(m_error);
        if (!m_options.allow_partial_parsing) result = value();
    }
    return result;
}

bool parser::parse_value(value &result, size_t depth)
{
    skip_decorations();
    unicode::codepoint ch = m_decoder.peek();
    switch (ch) {
    case '{':
        if (!parse_object(result, depth)) return false;
        break;
    case '[':
        if (!parse_array(result, depth)) return false;
        break;
    case '"':
        if (!parse_string(result)) return false;
        break;
    case 't':
    case 'f':
        if (!parse_bool(result)) return false;
        break;
    case 'n':
        if (!parse_null(result)) return false;
        break;
    default:
        if (ch != '-' && !unicode::is_digit(ch))
            return emit_error("expected object, array, string, number, 'true', 'false' or 'null'");
        if (!parse_number(result)) return false;
    }
    skip_decorations();
    return true;
}

bool parser::parse_object(value &result, size_t depth)
{
    if (m_options.max_depth > 0 && depth >= m_options.max_depth) {
        return emit_error("max depth reached trying to parse object");
    }

    unicode::codepoint ch;

    ch = m_decoder.peek();
    if (ch != '{') return emit_error("expected '{' to begin object");
    m_decoder.next(); // '{'
    skip_decorations();
    ch = m_decoder.peek();
    result = jpp::object();
    if (ch == '}') {
        m_decoder.next(); // '}'
        return true;
    }

    while (true) {
        skip_decorations();
        value key;
        if (!parse_string(key)) return emit_error("expected field key");
        skip_decorations();
        ch = m_decoder.peek();
        if (ch != ':') return emit_error("expected ':' to assign field value");
        if (!m_options.allow_duplicate_keys) {
            if (result.as_object().contains(key.as_string()))
                return emit_error("object field reassignment");
        }
        m_decoder.next(); // ':'
        if (!parse_value(result[key.as_string()], depth+1)) return false;
        ch = m_decoder.peek();
        if (ch == ',') {
            m_decoder.next(); // ','
            if (m_options.allow_trailing_commas) {
                skip_decorations();
                ch = m_decoder.peek();
                if (ch == '}') {
                    m_decoder.next(); // '}'
                    break;
                }
            }
            continue;
        } else if (ch == '}') {
            m_decoder.next(); // '}'
            break;
        } else {
            return emit_error("expected ',' to add a field or '}' to end object");
        }
    }

    return true;
}

bool parser::parse_array(value &result, size_t depth)
{
    if (m_options.max_depth > 0 && depth >= m_options.max_depth) {
        return emit_error("max depth reached trying to parse array");
    }

    unicode::codepoint ch;

    ch = m_decoder.peek();
    if (ch != '[') return emit_error("expected '[' to begin array");
    m_decoder.next(); // '['
    skip_decorations();
    ch = m_decoder.peek();
    result = jpp::array();
    if (ch == ']') {
        m_decoder.next(); // ']'
        return true;
    }

    for (size_t i = 0; ; ++i) {
        if (!parse_value(result[i], depth+1)) return false;
        ch = m_decoder.peek();
        if (ch == ',') {
            m_decoder.next(); // ','
            if (m_options.allow_trailing_commas) {
                skip_decorations();
                ch = m_decoder.peek();
                if (ch == ']') {
                    m_decoder.next(); // ']'
                    break;
                }
            }
            continue;
        } else if (ch == ']') {
            m_decoder.next(); // ']'
            break;
        } else {
            return emit_error("expected ',' to add an item or ']' to end array");
        }
    }

    return true;
}

bool parser::parse_string(value &result)
{
    unicode::codepoint ch;

    ch = m_decoder.peek();
    if (ch != '"') return emit_error("expected '\"' to begin string");
    m_decoder.next(); // '"'
    string &buffer = (result = "").as_string();
    ch = m_decoder.peek();
    if (ch == '"') {
        m_decoder.next(); // '"'
        return true;
    }

    while (true) {
        ch = m_decoder.peek();
        if (ch == '\\') {
            m_decoder.next(); // '\\'
            ch = m_decoder.peek();
            switch (ch) {
            case '"':  m_decoder.next(); /* '"'  */ buffer += '"';  break;
            case '\\': m_decoder.next(); /* '\\' */ buffer += '\\'; break;
            case '/':  m_decoder.next(); /* '/'  */ buffer += '/';  break;
            case 'b':  m_decoder.next(); /* 'b'  */ buffer += '\b'; break;
            case 'f':  m_decoder.next(); /* 'f'  */ buffer += '\f'; break;
            case 'n':  m_decoder.next(); /* 'n'  */ buffer += '\n'; break;
            case 'r':  m_decoder.next(); /* 'r'  */ buffer += '\r'; break;
            case 't':  m_decoder.next(); /* 't'  */ buffer += '\t'; break;
            case 'u': {
                m_decoder.next(); // 'u'
                unicode::codepoint code = 0;
                for (size_t i = 0; i < 4; ++i) {
                    if (!unicode::is_hexadecimal(m_decoder.peek()))
                        return emit_error("expected 4 consecutive hexadecimal digits for unicode escape sequence");
                    ch = m_decoder.next(); // hexadecimal
                    code *= 16;
                    if (unicode::is_digit(ch)) {
                        code += ch - '0';
                    } else {
                        code += unicode::to_lower(ch) - 'a' + 10;
                    }
                }
                if (!utf8::encode(code, buffer))
                    return emit_error("failed to encode unicode escape sequence");
            } break;
            default:
                return emit_error("expected escape sequence '\"', '\\', '/', 'b', 'f', 'n', 'r', 't' or 'uXXXX'");
            }
        } else if (ch == '"') {
            m_decoder.next(); // '"'
            break;
        } else if (unicode::is_control(ch)) {
            return emit_error("expected a non-control character, '\\' to begin an escape sequence or '\"' to end string");;
        } else {
            string_view data = m_decoder.data();
            m_decoder.next(); // whatever char
            data.remove_suffix(m_decoder.get_remaining_bytes());
            buffer += data;
        }
    }

    return true;
}

bool parser::parse_number(value &result)
{
    unicode::codepoint ch;

    int sign = 1;

    ch = m_decoder.peek();
    if (ch == '-') {
        sign = -1;
        m_decoder.next(); // '-'
    }

    integer integral_part = 0;
    ch = m_decoder.peek();
    if (ch == '0') {
        m_decoder.next(); // '0'
    } else {
        if (!unicode::is_digit(ch)) return emit_error("expected digit for integral part of number");
        while (unicode::is_digit(m_decoder.peek())) {
            ch = m_decoder.next(); // digit
            integral_part *= 10;
            integral_part += ch - '0';
        }
    }

    bool is_floating = false;
    integer fraction_part = 0;
    number  fraction_exp  = 1;
    ch = m_decoder.peek();
    if (ch == '.') {
        is_floating = true;

        m_decoder.next(); // '.'
        ch = m_decoder.peek();
        if (!unicode::is_digit(ch)) return emit_error("expected digit for fractional part of number");
        while (unicode::is_digit(m_decoder.peek())) {
            ch = m_decoder.next(); // digit
            fraction_exp  /= 10;
            fraction_part *= 10;
            fraction_part += ch - '0';
        }
    }

    int exponent_sign = 1;
    integer exponent_part = 0;
    ch = m_decoder.peek();
    if (ch == 'e' || ch == 'E') {
        is_floating = true;

        m_decoder.next(); // 'e' or 'E'
        ch = m_decoder.peek();
        if (ch == '-') {
            exponent_sign = -1;
            m_decoder.next(); // '-'
        } else if (ch == '+') {
            exponent_sign = 1;
            m_decoder.next(); // '+'
        }

        ch = m_decoder.peek();
        if (!unicode::is_digit(ch)) return emit_error("expected digit for exponent of number");
        while (unicode::is_digit(m_decoder.peek())) {
            ch = m_decoder.next(); // digit
            exponent_part *= 10;
            exponent_part += ch - '0';
        }
    }

    if (!is_floating) {
        result = sign * integral_part;
    } else {
        number integral = static_cast<number>(integral_part);
        number fraction = static_cast<number>(fraction_part) * fraction_exp;
        number exponent = std::pow(number(10), static_cast<number>(exponent_sign * exponent_part));
        result = sign * (integral + fraction) * exponent;
    }

    return true;
}

bool parser::parse_bool(value &result)
{
    string_view ident = get_identifier();
    if (ident == "true") {
        result = true;
        return true;
    } else if (ident == "false") {
        result = false;
        return true;
    }
    return emit_error("expected 'true' or 'false");
}

bool parser::parse_null(value &result)
{
    if (get_identifier() == "null") {
        result = nullptr;
        return true;
    }
    return emit_error("expected 'null'");
}

string_view parser::get_identifier()
{
    if (!is_ident_start(m_decoder.peek()))
        return "";

    string_view result = m_decoder.data();
    while (is_ident_continuation(m_decoder.peek())) {
        m_decoder.next();
    }
    result.remove_suffix(m_decoder.get_remaining_bytes());
    return result;
}

bool parser::skip_comments()
{
    unicode::codepoint ch;
    utf8::decoder decoder = m_decoder;

    ch = decoder.peek();
    if (ch != '/') return false;
    decoder.next(); // '/'

    ch = decoder.peek();
    if (ch == '/') {
        decoder.skip_until_new_line();
        m_decoder = decoder;
        return true;
    }

    if (ch != '*') return false;
    decoder.next(); // '*'

    size_t open_multilines = 1;
    while (decoder) {
        ch = decoder.next();
        if (ch == '/' && decoder.peek() == '*') {
            open_multilines += 1;
            decoder.next(); // '*'
        } else if (ch == '*' && decoder.peek() == '/') {
            open_multilines -= 1;
            decoder.next(); // '/'

            if (open_multilines <= 0) {
                m_decoder = decoder;
                return true;
            }
        }
    }

    return false;
}

bool parser::skip_whitespaces()
{
    bool skipped = false;
    while (unicode::is_whitespace(m_decoder.peek())) {
        skipped = true;
        m_decoder.next(); // whitespace
    }
    return skipped;
}

bool parser::skip_decorations()
{
    bool skipped = false;
    while (skip_whitespaces() || (m_options.allow_comments && skip_comments())) {
        skipped = true;
    }
    return skipped;
}

bool parser::emit_error(string_view message)
{
    m_error = {
        location{
            m_decoder.get_line(),
            m_decoder.get_decoded_codepoints() - m_decoder.get_line_start_codepoint(),
        },
        string(message),
    };
    return false;
}

bool parser::is_ident_start(unicode::codepoint ch)
{
    return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool parser::is_ident_continuation(unicode::codepoint ch)
{
    return is_ident_start(ch) || unicode::is_digit(ch);
}

value parse(string_view source, parser::error *error, parser::options options)
{
    jpp::parser parser(source, options);
    return parser.parse(error);
}

}
