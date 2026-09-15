/* Copyright (c) 2026 [Marco4413](https://github.com/Marco4413/jpp) */

#include "../serializer.h"
#include "../unicode.h"

#include <cmath>
#include <limits>

namespace jpp
{

serializer::serializer(options options)
    : m_options(options)
{}

void serializer::serialize(const value &value, string &result, size_t depth)
{
    switch (value.get_type()) {
    case type::null:    break;
    case type::boolean: return serialize(value.as_bool(),    result);
    case type::integer: return serialize(value.as_integer(), result);
    case type::number:  return serialize(value.as_number(),  result);
    case type::string:  return serialize(value.as_string(),  result);
    case type::array:   return serialize(value.as_array(),   result, depth);
    case type::object:  return serialize(value.as_object(),  result, depth);
    }
    return serialize(nullptr, result);
}

void serializer::serialize(std::nullptr_t value, string &result)
{
    (void)value;
    result += "null";
}

void serializer::serialize(bool value, string &result)
{
    result += value ? "true" : "false";
}

void serializer::serialize(integer value, string &result)
{
    result += std::to_string(value);
}

void serializer::serialize(number value, string &result)
{
    if (std::isnan(value)) {
        result += "-0.0";
        return;
    }

    if (std::isinf(value)) {
        if (value >= 0) {
            value = std::numeric_limits<number>::max();
        } else {
            value = std::numeric_limits<number>::min();
        }
    }

    if (value < 0) {
        result += '-';
        value   = -value;
    }

    integer exponent = static_cast<integer>(std::floor(std::log10(value)));
    bool use_exponential_notation = std::abs(exponent) >= static_cast<integer>(m_options.exponential_threshold);
    if (use_exponential_notation) {
        value *= std::pow(number(10), static_cast<number>(-exponent));
    }

    number a, b;
    b = std::modf(value, &a);

    result += std::to_string(static_cast<integer>(a));
    result += '.';

    string floating_part;
    if (m_options.number_precision <= 0) {
        floating_part += '0';
    } else {
        floating_part.reserve(m_options.number_precision);
        while (floating_part.length() < m_options.number_precision) {
            b = std::modf(b * number(10), &a);
            floating_part += '0' + static_cast<char>(a);
        }
    }

    while (floating_part.length() > 1 && floating_part.back() == '0')
        floating_part.pop_back();

    result += floating_part;

    if (use_exponential_notation) {
        result += 'e';
        result += std::to_string(exponent);
    }
}

void serializer::serialize(string_view value, string &result)
{
    utf8::decoder decoder(value);
    result += '"';
    while (decoder) {
        unicode::codepoint ch = decoder.next();
        switch (ch) {
        case '"':  result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '\b': result += "\\b";  break;
        case '\f': result += "\\f";  break;
        case '\n': result += "\\n";  break;
        case '\r': result += "\\r";  break;
        case '\t': result += "\\t";  break;
        default:
            if (unicode::is_control(ch)) {
                constexpr size_t DIGITS_COUNT = 4;

                result += "\\u";
                char digits[DIGITS_COUNT];
                for (size_t i = 0; i < DIGITS_COUNT; ++i) {
                    char digit_value = ch % 16;
                    ch /= 16;

                    char digit = '0';
                    if (digit_value < 10) {
                        digit = '0' + digit_value;
                    } else {
                        digit = 'A' + (digit_value - 10);
                    }

                    digits[DIGITS_COUNT-i-1] = digit;
                }
                result.append(digits, DIGITS_COUNT);
            } else {
                utf8::encode(ch, result);
            }
        }
    }
    result += '"';
}

void serializer::serialize(const string &value, string &result)
{
    return serialize(string_view(value), result);
}

void serializer::serialize(const array &value, string &result, size_t depth)
{
    if (m_options.max_depth > 0 && depth >= m_options.max_depth) {
        result += "[ /* MAX DEPTH REACHED */ ]";
        return;
    }

    bool first_element = true;
    result += '[';
    for (array::const_iterator::result item : value) {
        if (!first_element) result += ',';
        if (m_options.indent_size > 0) {
            result += '\n';
            // INDENT
            result.append(m_options.indent_size*(depth+1), ' ');
        }
        serialize(item, result, depth+1);

        first_element = false;
    }
    if (m_options.indent_size > 0) {
        result += '\n';
        // INDENT
        result.append(m_options.indent_size*depth, ' ');
    }
    result += ']';
}

void serializer::serialize(const object &value, string &result, size_t depth)
{
    if (m_options.max_depth > 0 && depth >= m_options.max_depth) {
        result += "{ /* MAX DEPTH REACHED */ }";
        return;
    }

    bool first_element = true;
    result += '{';
    for (object::const_iterator::result field : value) {
        if (!first_element) result += ',';
        if (m_options.indent_size > 0) {
            result += '\n';
            // INDENT
            result.append(m_options.indent_size*(depth+1), ' ');
        }
        serialize(field.first, result, depth+1);
        result += ':';
        if (m_options.indent_size > 0)
            result += ' ';
        serialize(field.second, result, depth+1);

        first_element = false;
    }
    if (m_options.indent_size > 0) {
        result += '\n';
        // INDENT
        result.append(m_options.indent_size*depth, ' ');
    }
    result += '}';
}

string serialize(const value &value, serializer::options options)
{
    string result;
    serializer serializer(options);
    serializer.serialize(value, result, 0);
    return result;
}

}
