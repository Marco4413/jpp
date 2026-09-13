/* Copyright (c) 2026 [Marco4413](https://github.com/Marco4413/jpp) */

// Shamelessly stolen from:
// - https://github.com/Marco4413/Pulsar/blob/master/include/pulsar/lexer.h
// - https://github.com/Marco4413/Pulsar/blob/master/include/pulsar/unicode.h
// - https://github.com/Marco4413/Pulsar/blob/master/include/pulsar/utf8.h

#ifndef _JPP_UNICODE_H
#define _JPP_UNICODE_H

#include "value.h"

namespace jpp::unicode
{
    using codepoint = uint32_t;

    constexpr codepoint MAX_CODEPOINT = 0x10FFFF;
    constexpr codepoint REPLACEMENT_CHARACTER = 0xFFFD;
    constexpr bool is_valid_codepoint(codepoint code) { return code <= MAX_CODEPOINT; }

    constexpr bool is_control(codepoint ch)
    {
        return (/* ch >= 0x00 && */ ch <= 0x1F) /* || ch == 0x7F */;
    }

    constexpr bool is_whitespace(codepoint ch)
    {
        return ch == 0x09
            || ch == 0x0A
            || ch == 0x0D
            || ch == 0x20;
    }

    constexpr bool is_digit(codepoint ch)
    {
        return ch >= '0' && ch <= '9';
    }

    constexpr bool is_hexadecimal(codepoint ch)
    {
        return is_digit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
    }

    constexpr codepoint to_lower(codepoint ch)
    {
        return (ch >= 'A' && ch <= 'F') ? (ch - 'A' + 'a') : ch;
    }
}

namespace jpp::utf8
{
    class decoder
    {
    public:
        decoder(string_view data);
        decoder(const decoder&) = default;

        ~decoder() = default;

        decoder& operator=(const decoder&) = default;

        operator bool() const;

        bool has_data() const;
        bool is_invalid_encoding() const;

        size_t get_remaining_bytes() const;
        size_t get_decoded_bytes() const;
        size_t get_decoded_codepoints() const;

        string_view data() const;

        unicode::codepoint next();
        // non-const peek with caching
        unicode::codepoint peek();
        unicode::codepoint peek() const;

        void skip_until_new_line();
        size_t get_line() const;
        size_t get_line_start_codepoint() const;

    private:
        unicode::codepoint next_impl();

    private:
        const char* m_data_start;
        string_view m_data;

        unicode::codepoint m_peeked_codepoint;
        bool m_is_invalid_encoding;
        size_t m_decoded_codepoints;

        size_t m_line;
        size_t m_line_start_codepoint;
    };

    bool encode(unicode::codepoint code, string &out);

    constexpr size_t MAX_ENCODED_SIZE = 4;
    constexpr size_t get_encoded_size(unicode::codepoint code)
    {
        if (code <= 0x7F) return 1;
        else if (code <= 0x7FF) return 2;
        else if (code <= 0xFFFF) return 3;
        return 4;
    }
}

#endif // _JPP_UNICODE_H
