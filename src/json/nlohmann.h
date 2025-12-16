#pragma once
#include <iostream>
#include <nlohmann/json.hpp>
#include <ostream>
#include <string>

#include "json.h"

class NlohmannJson : public IJson {
  public:
    nlohmann::json data;

    NlohmannJson() : data(nlohmann::json::object()) {}
    NlohmannJson(const nlohmann::json& d) : data(d) {}

    bool isNull() const override { return data.is_null(); }

    bool has(const std::string& key) const override { return data.contains(key); }

    // In NlohmannJson.cpp or header if header-only
    std::unique_ptr<IJson> parse(const std::string& jsonString) {
        try {
            auto parsed = nlohmann::json::parse(jsonString);
            return std::make_unique<NlohmannJson>(parsed);
        } catch (const std::exception& e) {
            // Return null or empty object on error:
            return nullptr;
        }
    }

    std::string getString(const std::string& key, const std::string& defaultValue) const override {
        if (has(key) && data[key].is_string())
            return data[key].get<std::string>();
        return defaultValue;
    }

    double getNumber(const std::string& key, double defaultValue) const override {
        if (has(key) && data[key].is_number())
            return data[key].get<double>();
        return defaultValue;
    }

    bool getBool(const std::string& key, bool defaultValue) const override {
        if (has(key) && data[key].is_boolean())
            return data[key].get<bool>();
        return defaultValue;
    }

    std::unique_ptr<IJson> getChild(const std::string& key) const override {
        if (!has(key))
            return nullptr;
        return std::make_unique<NlohmannJson>(data[key]);
    }

    void set(const std::string& key, const std::string& value) override { data[key] = value; }

    void set(const std::string& key, const char* value) override { data[key] = std::string(value); }

    void set(const std::string& key, double value) override { data[key] = value; }

    void set(const std::string& key, bool value) override { data[key] = value; }

    void set(const std::string& key, std::unique_ptr<IJson> value) override {
        auto* ptr = dynamic_cast<NlohmannJson*>(value.get());
        if (ptr)
            data[key] = ptr->data;
    }

    // In NlohmannJson
    void set(const std::string& key, int value) override { data[key] = value; }

    void push(std::unique_ptr<IJson> value) override {
        if (!data.is_array())
            data = nlohmann::json::array();
        auto* ptr = dynamic_cast<NlohmannJson*>(value.get());
        data.push_back(ptr ? ptr->data : nlohmann::json());
    }

    std::string stringify(bool pretty = false) const override {
        return pretty ? data.dump(4) : data.dump();
    }
};
