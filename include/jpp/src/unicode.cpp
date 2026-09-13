/* Copyright (c) 2026 [Marco4413](https://github.com/Marco4413/jpp) */

#include "../unicode.h"

namespace jpp::utf8
{

decoder::decoder(string_view data)
    : m_data_start(data.data())
    , m_data(data)
    , m_peeked_codepoint(0)
    , m_is_invalid_encoding(false)
    , m_decoded_codepoints(0)
    , m_line(0)
    , m_line_start_codepoint(0)
{}


decoder::operator bool() const { return !is_invalid_encoding() && has_data(); }

bool decoder::has_data() const { return m_data.length() > 0; }
bool decoder::is_invalid_encoding() const { return m_is_invalid_encoding; }

size_t decoder::get_remaining_bytes() const    { return m_data.length(); }
size_t decoder::get_decoded_bytes() const      { return static_cast<size_t>(m_data.data()-m_data_start); }
size_t decoder::get_decoded_codepoints() const { return m_decoded_codepoints; }

string_view decoder::data() const { return m_data; }

unicode::codepoint decoder::next()
{
    unicode::codepoint ch = next_impl();
    if (ch == '\n') {
        ++m_line;
        m_line_start_codepoint = get_decoded_codepoints();
    } else if (ch == '\r') {
        if (peek() != '\n') {
            ++m_line;
            m_line_start_codepoint = get_decoded_codepoints();
        }
    }
    return ch;
}

unicode::codepoint decoder::peek()
{
    // == PEEK CACHE == //
    if (m_peeked_codepoint != 0)
        return m_peeked_codepoint;

    decoder decoder = *this;
    m_peeked_codepoint = decoder.next();
    return m_peeked_codepoint;
    // == PEEK CACHE == //
}

unicode::codepoint decoder::peek() const
{
    // == PEEK CACHE == //
    if (m_peeked_codepoint != 0)
        return m_peeked_codepoint;
    // == PEEK CACHE == //
    decoder decoder = *this;
    return decoder.next();
}

void decoder::skip_until_new_line()
{
    size_t init_line = get_line();
    while (*this && init_line == get_line()) next();
}

size_t decoder::get_line() const                 { return m_line; }
size_t decoder::get_line_start_codepoint() const { return m_line_start_codepoint; }

unicode::codepoint decoder::next_impl()
{
    if (!*this) return 0;

    // == PEEK CACHE == //
    if (m_peeked_codepoint != 0 && get_encoded_size(m_peeked_codepoint) <= m_data.length()) {
        m_data.remove_prefix(get_encoded_size(m_peeked_codepoint));
        ++m_decoded_codepoints;

        unicode::codepoint next = m_peeked_codepoint;
        m_peeked_codepoint = 0;
        return next;
    }

    m_peeked_codepoint = 0;
    // == PEEK CACHE == //

    unsigned char byte1 = (unsigned char)m_data[0];
    if ((byte1 & 0x80) == 0) {
        // ASCII is always valid Unicode
        m_data.remove_prefix(1);
        ++m_decoded_codepoints;
        return byte1;
    }

    if (m_data.length() < 2) {
        m_is_invalid_encoding = true;
        return unicode::REPLACEMENT_CHARACTER;
    }

    if ((byte1 & 0xE0) == 0xC0) {
        unsigned char byte2 = (unsigned char)m_data[1];
        if ((byte2 & 0xC0) == 0x80) {
            m_data.remove_prefix(2);
            ++m_decoded_codepoints;
            unicode::codepoint code =
                (static_cast<unicode::codepoint>(byte1 & 0x1F) << 6) |
                (static_cast<unicode::codepoint>(byte2 & 0x3F)     ) ;
            if (get_encoded_size(code) == 2 && unicode::is_valid_codepoint(code))
                return code;
        }
        m_is_invalid_encoding = true;
        return unicode::REPLACEMENT_CHARACTER;
    }

    if (m_data.length() < 3) {
        m_is_invalid_encoding = true;
        return unicode::REPLACEMENT_CHARACTER;
    }

    if ((byte1 & 0xF0) == 0xE0) {
        unsigned char byte2 = (unsigned char)m_data[1];
        unsigned char byte3 = (unsigned char)m_data[2];
        if (((byte2 | byte3) & 0xC0) == 0x80) {
            m_data.remove_prefix(3);
            ++m_decoded_codepoints;
            unicode::codepoint code =
                (static_cast<unicode::codepoint>(byte1 & 0x0F) << 12) |
                (static_cast<unicode::codepoint>(byte2 & 0x3F) <<  6) |
                (static_cast<unicode::codepoint>(byte3 & 0x3F)      ) ;
            if (get_encoded_size(code) == 3 && unicode::is_valid_codepoint(code))
                return code;
        }
        m_is_invalid_encoding = true;
        return unicode::REPLACEMENT_CHARACTER;
    }

    if (m_data.length() < 4) {
        m_is_invalid_encoding = true;
        return unicode::REPLACEMENT_CHARACTER;
    }

    if ((byte1 & 0xF8) == 0xF0) {
        unsigned char byte2 = (unsigned char)m_data[1];
        unsigned char byte3 = (unsigned char)m_data[2];
        unsigned char byte4 = (unsigned char)m_data[3];
        if (((byte2 | byte3 | byte4) & 0xC0) == 0x80) {
            m_data.remove_prefix(4);
            ++m_decoded_codepoints;
            unicode::codepoint code =
                (static_cast<unicode::codepoint>(byte1 & 0x07) << 18) |
                (static_cast<unicode::codepoint>(byte2 & 0x3F) << 12) |
                (static_cast<unicode::codepoint>(byte3 & 0x3F) <<  6) |
                (static_cast<unicode::codepoint>(byte4 & 0x3F)      ) ;
            if (get_encoded_size(code) == 4 && unicode::is_valid_codepoint(code))
                return code;
        }
        m_is_invalid_encoding = true;
        return unicode::REPLACEMENT_CHARACTER;
    }

    m_is_invalid_encoding = true;
    return unicode::REPLACEMENT_CHARACTER;
}

bool encode(unicode::codepoint code, string &out)
{
    char encoded[MAX_ENCODED_SIZE] = {0};

    if (!unicode::is_valid_codepoint(code))
        code = unicode::REPLACEMENT_CHARACTER;

    size_t encoded_length = get_encoded_size(code);
    switch (encoded_length) {
    case 1: {
        // We don't really need & 0x7F
        // However, I prefer to play it safe.
        encoded[0] = (char)(code & 0x7F);
    } break;
    case 2: {
        encoded[0] = (char)(( code >>  6) | 0xC0);
        encoded[1] = (char)(((code      ) & 0x3F) | 0x80);
    } break;
    case 3: {
        encoded[0] = (char)(( code >> 12) | 0xE0);
        encoded[1] = (char)(((code >>  6) & 0x3F) | 0x80);
        encoded[2] = (char)(((code      ) & 0x3F) | 0x80);
    } break;
    case 4: {
        encoded[0] = (char)(( code >> 18) | 0xF0);
        encoded[1] = (char)(((code >> 12) & 0x3F) | 0x80);
        encoded[2] = (char)(((code >>  6) & 0x3F) | 0x80);
        encoded[3] = (char)(((code      ) & 0x3F) | 0x80);
    } break;
    default:
        return false;
    }

    out.append(encoded, encoded_length);
    return true;
}

}
