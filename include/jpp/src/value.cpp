/* Copyright (c) 2026 [Marco4413](https://github.com/Marco4413/jpp) */

#include "../value.h"

#include <cstdio>

#include <array>

namespace jpp
{

namespace _internals
{
    static void abort(const char *message)
    {
        std::fprintf(stderr, "abort: %s\n", message);
        std::abort();
    }
}

array::array(const array &other)
{
    *this = other;
}

array &array::operator=(const array &other)
{
    m_value.clear();
    m_value.resize(other.m_value.size());
    for (size_t i = 0; i < other.m_value.size(); ++i) {
        m_value[i] = std::make_unique<value>(*other.m_value[i]);
    }
    return *this;
}

value &array::operator[](size_t index)
{
    while (index >= size()) emplace_back();
    return at(index);
}

const value &array::operator[](size_t index) const
{
    if (index >= size()) return null;
    return at(index);
}

value &array::at(size_t index)
{
    return *m_value[index];
}

const value &array::at(size_t index) const
{
    return *m_value[index];
}

value &array::emplace_back()
{
    return *m_value.emplace_back(std::make_unique<value>());
}

array &array::push_back()
{
    m_value.emplace_back(std::make_unique<jpp::value>());
    return *this;
}

array &array::push_back(const value &value)
{
    m_value.emplace_back(std::make_unique<jpp::value>(value));
    return *this;
}

array &array::push_back(value &&value)
{
    m_value.emplace_back(std::make_unique<jpp::value>(std::move(value)));
    return *this;
}

size_t array::size() const
{
    return m_value.size();
}

array::iterator array::begin()
{
    return m_value.cbegin();
}

array::iterator array::end()
{
    return m_value.cend();
}

array::const_iterator array::begin() const
{
    return m_value.begin();
}

array::const_iterator array::end() const
{
    return m_value.end();
}

array::const_iterator array::cbegin() const
{
    return m_value.cbegin();
}

array::const_iterator array::cend() const
{
    return m_value.cend();
}

array::const_iterator::const_iterator(list_type::const_iterator it)
    : m_it(it)
{}

array::const_iterator &array::const_iterator::operator++()
{
    ++m_it;
    return *this;
}

bool array::const_iterator::operator==(const const_iterator &other) const
{
    return m_it == other.m_it;
}

bool array::const_iterator::operator!=(const const_iterator &other) const
{
    return !(*this == other);
}

array::const_iterator::result array::const_iterator::operator*() const
{
    return **m_it;
}

array::iterator &array::iterator::operator++()
{
    const_iterator::operator++();
    return *this;
}

array::iterator::result array::iterator::operator*()
{
    return **m_it;
}

object::object(const object &other)
{
    *this = other;
}

object &object::operator=(const object &other)
{
    m_value.clear();
    m_value.reserve(other.m_value.bucket_count());
    for (const auto &bucket : other.m_value) {
        m_value.emplace(bucket.first, std::make_unique<value>(*bucket.second));
    }
    return *this;
}

value &object::operator[](const char *key)
{
    return (*this)[string_view(key)];
}

value &object::operator[](string_view key)
{
    s_temp_key = key;
    return (*this)[s_temp_key];
}

value &object::operator[](const string &key)
{
    auto &value_ptr = m_value[key];
    if (!value_ptr) value_ptr = std::make_unique<value>();
    return *value_ptr;
}

const value &object::operator[](const char *key) const
{
    return (*this)[string_view(key)];
}

const value &object::operator[](string_view key) const
{
    if (!contains(key)) return null;
    return at(key);
}

const value &object::operator[](const string &key) const
{
    if (!contains(key)) return null;
    return at(key);
}

bool object::contains(string_view key) const
{
    s_temp_key = key;
    return contains(s_temp_key);
}

bool object::contains(const string &key) const
{
    return m_value.find(key) != m_value.end();
}

value &object::at(string_view key)
{
    s_temp_key = key;
    return at(s_temp_key);
}

const value &object::at(string_view key) const
{
    s_temp_key = key;
    return at(s_temp_key);
}

value &object::at(const string &key)
{
    return *m_value.at(key);
}

const value &object::at(const string &key) const
{
    return *m_value.at(key);
}

object::iterator object::begin()
{
    return m_value.cbegin();
}

object::iterator object::end()
{
    return m_value.cend();
}

object::const_iterator object::begin() const
{
    return m_value.begin();
}

object::const_iterator object::end() const
{
    return m_value.end();
}

object::const_iterator object::cbegin() const
{
    return m_value.cbegin();
}

object::const_iterator object::cend() const
{
    return m_value.cend();
}

object::const_iterator::const_iterator(map_type::const_iterator it)
    : m_it(it)
{}

object::const_iterator &object::const_iterator::operator++()
{
    ++m_it;
    return *this;
}

bool object::const_iterator::operator==(const const_iterator &other) const
{
    return m_it == other.m_it;
}

bool object::const_iterator::operator!=(const const_iterator &other) const
{
    return !(*this == other);
}

object::const_iterator::result object::const_iterator::operator*() const
{
    const auto &inner_pair = *m_it;
    return { inner_pair.first, *inner_pair.second };
}

object::iterator &object::iterator::operator++()
{
    const_iterator::operator++();
    return *this;
}

object::iterator::result object::iterator::operator*()
{
    const auto &inner_pair = *m_it;
    return { inner_pair.first, *inner_pair.second };
}

thread_local string object::s_temp_key;

value::value() : m_value(nullptr) {}

value::value(std::nullptr_t value) : m_value(value) {};
value::value(bool value)           : m_value(value) {}
value::value(int value)            : m_value(static_cast<integer>(value)) {}
value::value(integer value)        : m_value(value) {}
value::value(number value)         : m_value(value) {}

value::value(const char *value) : m_value(string(value)) {}
value::value(string_view value) : m_value(string(value)) {}
value::value(string &&value)    : m_value(std::move(value)) {}
value::value(array &&value)     : m_value(std::move(value)) {}
value::value(object &&value)    : m_value(std::move(value)) {}

const value &value::as_const() const
{
    return *this;
}

const value &value::operator*() const
{
    return as_const();
}

type value::get_type() const
{
    static constexpr std::array<type, std::variant_size_v<variant_type>> s_types{
        type::null, type::boolean, type::integer, type::number, type::string, type::array, type::object
    };

    size_t type_index = m_value.index();
    if (type_index == std::variant_npos)
        _internals::abort("value::get_type() invalid variant type index");
    return s_types[type_index];
}

bool value::is_null() const { return std::holds_alternative<std::nullptr_t>(m_value); }

bool value::is_bool() const { return std::holds_alternative<bool>(m_value); }

bool value::as_bool() const
{
    if (const auto *value = std::get_if<bool>(&m_value); value)
        return *value;
    return false;
}

bool value::is_integer() const { return std::holds_alternative<integer>(m_value); }

integer value::as_integer() const
{
    if (const auto *value = std::get_if<integer>(&m_value); value)
        return *value;
    return 0;
}

bool value::is_number() const { return std::holds_alternative<number>(m_value); }

number value::as_number() const
{
    if (is_integer()) return static_cast<number>(as_integer());
    if (const auto *value = std::get_if<number>(&m_value); value)
        return *value;
    return 0.0;
}

bool value::is_string() const { return std::holds_alternative<string>(m_value); }

string_view value::as_string_view() const
{
    if (const auto *value = std::get_if<string>(&m_value); value)
        return *value;
    return "";
}

const string &value::as_string() const
{
    return std::get<string>(m_value);
}

string &value::as_string()
{
    return std::get<string>(m_value);
}

bool value::is_array() const { return std::holds_alternative<array>(m_value); }

const array &value::as_array() const
{
    return std::get<array>(m_value);
}

array &value::as_array()
{
    return std::get<array>(m_value);
}

bool value::is_object() const { return std::holds_alternative<object>(m_value); }

const object &value::as_object() const
{
    return std::get<object>(m_value);
}

object &value::as_object()
{
    return std::get<object>(m_value);
}

value &value::operator[](int index)
{
    if (index < 0) _internals::abort("value::operator[](int) negative array index");
    return (*this)[static_cast<size_t>(index)];
}

value &value::operator[](size_t index)
{
    if (!is_array()) *this = array();
    return as_array()[index];
}

const value &value::operator[](int index) const
{
    if (index < 0) _internals::abort("value::operator[](int) negative array index");
    return (*this)[static_cast<size_t>(index)];
}

const value &value::operator[](size_t index) const
{
    if (!is_array()) return null;
    return as_array()[index];
}

value &value::operator[](const char *key)
{
    return (*this)[string_view(key)];
}

value &value::operator[](string_view key)
{
    if (!is_object()) *this = object();
    return as_object()[key];
}

value &value::operator[](const string &key)
{
    if (!is_object()) *this = object();
    return as_object()[key];
}

const value &value::operator[](const char *key) const
{
    return (*this)[string_view(key)];
}

const value &value::operator[](string_view key) const
{
    if (!is_object()) return null;
    return as_object()[key];
}

const value &value::operator[](const string &key) const
{
    if (!is_object()) return null;
    return as_object()[key];
}

}
