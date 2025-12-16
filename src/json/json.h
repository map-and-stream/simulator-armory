#pragma once
#include <memory>
#include <string>
#include <vector>

class IJson {
  public:
    virtual ~IJson() = default;

    // Creation
    virtual bool isNull() const = 0;
    static std::unique_ptr<IJson> createObject();
    static std::unique_ptr<IJson> createArray();

    // Parsing from string
    virtual std::unique_ptr<IJson> parse(const std::string& jsonString) = 0;

    // Getters
    virtual bool has(const std::string& key) const = 0;
    virtual std::string getString(const std::string& key,
                                  const std::string& defaultValue = "") const = 0;
    virtual double getNumber(const std::string& key, double defaultValue = 0.0) const = 0;
    virtual bool getBool(const std::string& key, bool defaultValue = false) const = 0;
    virtual std::unique_ptr<IJson> getChild(const std::string& key) const = 0;

    // Setters
    virtual void set(const std::string& key, const std::string& value) = 0;
    virtual void set(const std::string& key, const char* value) = 0;
    virtual void set(const std::string& key, double value) = 0;
    virtual void set(const std::string& key, bool value) = 0;
    virtual void set(const std::string& key, std::unique_ptr<IJson> value) = 0;

    // In IJson
    virtual void set(const std::string& key, int value) = 0;

    // For arrays
    virtual void push(std::unique_ptr<IJson> value) = 0;

    // Serialization
    virtual std::string stringify(bool pretty = false) const = 0;
};
