/* Copyright (c) 2026 [Marco4413](https://github.com/Marco4413/jpp) */

#ifndef _JPP_PARSER_H
#define _JPP_PARSER_H

#include "unicode.h"
#include "value.h"

namespace jpp
{
    class parser
    {
    public:
        struct location
        {
            size_t line;
            size_t character;

            string to_string(string_view file_path="") const;
        };

        struct error
        {
            parser::location location;
            string message;

            bool is_set() const;
            string to_string(string_view file_path="") const;
        };

        struct options
        {
            bool allow_partial_parsing = false;
            bool allow_comments        = false;
            bool allow_duplicate_keys  = false;
            bool allow_trailing_commas = false;

            size_t max_depth = 0;
        };

    public:
        parser(string_view source, options options);
        ~parser() = default;

        value parse(error *error);

    private:
        bool parse_value(value &result, size_t depth);
        bool parse_object(value &result, size_t depth);
        bool parse_array(value &result, size_t depth);
        bool parse_string(value &result);
        bool parse_number(value &result);
        bool parse_bool(value &result);
        bool parse_null(value &result);

        string_view get_identifier();
        bool skip_comments();
        bool skip_whitespaces();
        bool skip_decorations();

        bool emit_error(string_view message);

    private:
        static bool is_ident_start(unicode::codepoint ch);
        static bool is_ident_continuation(unicode::codepoint ch);

    private:
        utf8::decoder m_decoder;
        options m_options;

        error m_error;
    };

    value parse(string_view source, parser::error *error=nullptr, parser::options options={});
}

#endif // _JPP_PARSER_H
