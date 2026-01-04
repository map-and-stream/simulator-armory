#ifndef DATA_MODEL_DECODER_OBJECTS_STRINGVALUE_HPP_
#define DATA_MODEL_DECODER_OBJECTS_STRINGVALUE_HPP_

#include <string>

#include "data_model/decoder_objects/ElementValue.hpp"
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a string.
 * This class is used when the decoded value is a string.
 */
class StringValue : public ElementValue {
public:

    StringValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
    }

    StringValue(DecodedValueObjectPoolManager *_ptrToManager, const std::vector<unsigned char>& _decodedValue)
            : ElementValue(_ptrToManager), decodedValue(_decodedValue) {
        decodedValueInString = string(_decodedValue.begin(), _decodedValue.end());
    }

    ~StringValue() {
        decodedValue.clear();
    }

    string getDecodedValue();

    void setDecodedValue(const std::vector<unsigned char>& _decodedValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:

    std::vector<unsigned char> decodedValue;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_STRINGVALUE_HPP_ */
