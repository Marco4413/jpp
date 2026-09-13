/* Copyright (c) 2026 [Marco4413](https://github.com/Marco4413/jpp) */

#ifndef _JPP_SERIALIZER_H
#define _JPP_SERIALIZER_H

#include "value.h"

namespace jpp
{
    class serializer
    {
    public:
        struct options
        {
            size_t indent_size = 2;
            size_t max_depth   = 0;
            uint8_t number_precision      = 6;
            uint8_t exponential_threshold = 7;
        };

    public:
        serializer(options options);
        ~serializer() = default;

        void serialize(const value &value, string &result, size_t depth);

        void serialize(std::nullptr_t value, string &result);
        void serialize(bool value, string &result);
        void serialize(integer value, string &result);
        void serialize(number value, string &result);
        void serialize(string_view value, string &result);
        void serialize(const string &value, string &result);
        void serialize(const array &value, string &result, size_t depth);
        void serialize(const object &value, string &result, size_t depth);

    private:
        options m_options;
    };

    string serialize(const value &value, serializer::options options={});
}

#endif // _JPP_SERIALIZER_H
