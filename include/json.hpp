#pragma once
#include <json-c/json.h>
#include <memory>
#include <string>

namespace json
{
    // Use this instead of default json_object
    using Object = std::unique_ptr<json_object, decltype(&json_object_put)>;

    // Use this instead of json_object_from_x. Pass the function and its arguments instead.
    template <typename... Args>
    static inline json::Object new_object(json_object *(*function)(Args...), Args... args)
    {
        return json::Object((*function)(args...), json_object_put);
    }

    /// @brief Inline wrapper function for getting a json object by it's key.
    /// @param json json::Object to get the key from.
    /// @param key Key to get.
    /// @return json_object on success. NULL on failure.
    static inline json_object *get_object(json::Object &json, std::string_view key)
    {
        return json_object_object_get(json.get(), key.data());
    }

    /// @brief Inline wrapper function to add an object to a json_object
    /// @param json Json object to add an object to.
    /// @param key Key of the object.
    /// @param object Object to add to json.
    /// @return True on success. False on failure.
    static inline bool add_object(json::Object &json, std::string_view key, json_object *object)
    {
        return json_object_object_add(json.get(), key.data(), object) == 0;
    }

    /// @brief Returns the json string.
    static inline const char *get_string(json::Object &json) { return json_object_get_string(json.get()); }

    /// @brief Returns the length of the string. I find json_object_get_string_len is unreliable?
    static inline int64_t length(json::Object &json)
    {
        const char *string = json_object_get_string(json.get());
        return std::char_traits<char>::length(string);
    }

    /// @brief Returns the beginning for iterating.
    static inline json_object_iterator iter_begin(json::Object &json) { return json_object_iter_begin(json.get()); }

    /// @brief Returns the end for iterating.
    static inline json_object_iterator iter_end(json::Object &json) { return json_object_iter_end(json.get()); }
} // namespace json
