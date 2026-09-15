/* Copyright (c) 2026 [Marco4413](https://github.com/Marco4413/jpp) */

#ifndef _JPP_VALUE_H
#define _JPP_VALUE_H

#include <cinttypes>

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace jpp
{
    using integer     = int64_t;
    using number      = double;
    using string      = std::string;
    using string_view = std::string_view;

    /* inline const value null; */

    class value; // forward declaration

    class array
    {
    public:
        class const_iterator;
        class iterator;

        using list_type = std::vector<std::unique_ptr<value>>;

    public:
        array() = default;
        ~array() = default;

        array(const array &other);
        array &operator=(const array &other);

        value &operator[](size_t index);
        const value &operator[](size_t index) const;

        value &at(size_t index);
        const value &at(size_t index) const;

        value &emplace_back();

        array &push_back();
        array &push_back(const value &value);
        array &push_back(value &&value);

        array &pop_back();

        value &back();
        const value &back() const;

        size_t size() const;
        bool empty() const;

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;

    public:
        class const_iterator
        {
        public:
            using result = const value &;

        public:
            const_iterator(list_type::const_iterator it);
            ~const_iterator() = default;

            const_iterator &operator++();
            bool operator==(const const_iterator &other) const;
            bool operator!=(const const_iterator &other) const;

            result operator*() const;

        protected:
            list_type::const_iterator m_it;
        };

        class iterator : public const_iterator
        {
        public:
            using result = value &;

        public:
            using const_iterator::const_iterator;
            ~iterator() = default;

            iterator &operator++();
            result operator*();
        };

    private:
        list_type m_value;
    };

    class object
    {
    public:
        class const_iterator;
        class iterator;

        using map_type = std::unordered_map<string, std::unique_ptr<value>>;

    public:
        object() = default;
        ~object() = default;

        object(const object &other);
        object &operator=(const object &other);

        value &operator[](const char *key);
        value &operator[](string_view key);
        value &operator[](const string &key);

        const value &operator[](const char *key) const;
        const value &operator[](string_view key) const;
        const value &operator[](const string &key) const;

        bool contains(string_view key) const;
        bool contains(const string &key) const;

        value &at(const char *key);
        value &at(string_view key);
        value &at(const string &key);

        const value &at(const char *key) const;
        const value &at(string_view key) const;
        const value &at(const string &key) const;

        object &erase(const char *key);
        object &erase(string_view key);
        object &erase(const string &key);

        bool empty() const;

        iterator begin();
        iterator end();

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator cbegin() const;
        const_iterator cend() const;

    public:
        class const_iterator
        {
        public:
            using result = std::pair<string_view, const value &>;

        public:
            const_iterator(map_type::const_iterator it);
            ~const_iterator() = default;

            const_iterator &operator++();
            bool operator==(const const_iterator &other) const;
            bool operator!=(const const_iterator &other) const;

            result operator*() const;

        protected:
            map_type::const_iterator m_it;
        };

        class iterator : public const_iterator
        {
        public:
            using result = std::pair<string_view, value &>;

        public:
            using const_iterator::const_iterator;
            ~iterator() = default;

            iterator &operator++();
            result operator*();
        };

    private:
        static thread_local string s_temp_key;

    private:
        map_type m_value;
    };

    enum class type
    {
        null = 0,
        boolean,
        integer,
        number,
        string,
        array,
        object,
    };

    class value
    {
    public:
        using variant_type = std::variant<std::nullptr_t, bool, integer, number, string, array, object>;

    public:
        value();
        ~value() = default;

        value(std::nullptr_t value);
        value(bool value);
        value(int value);
        value(integer value);
        value(number value);
        value(const char *value);
        value(string_view value);
        value(string &&value);
        value(array &&value);
        value(object &&value);

        const value &as_const() const;
        const value &operator*() const;

        type get_type() const;

        bool is_null() const;

        bool is_bool() const;
        bool as_bool() const;

        bool is_integer() const;
        integer as_integer() const;

        bool is_number() const;
        number as_number() const;

        bool is_string() const;
        string_view as_string_view() const;

        const string &as_string() const;
        string &as_string();

        bool is_array() const;
        const array &as_array() const;
        array &as_array();

        bool is_object() const;
        const object &as_object() const;
        object &as_object();

    public:
        /* these methods redirect to as_array() and as_object().
           non-const methods assing either array or object to this
           if the type does not match */
        value &operator[](int index);
        value &operator[](size_t index);
        const value &operator[](int index) const;
        const value &operator[](size_t index) const;

        value &operator[](const char *key);
        value &operator[](string_view key);
        value &operator[](const string &key);
        const value &operator[](const char *key) const;
        const value &operator[](string_view key) const;
        const value &operator[](const string &key) const;

    private:
        variant_type m_value;
    };

    inline const value null = value();
}

#endif // _JPP_VALUE_H
