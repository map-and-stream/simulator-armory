#ifndef DATA_MODEL_DECODER_OBJECTS_SignedInteger16bitBEValue_HPP_
#define DATA_MODEL_DECODER_OBJECTS_SignedInteger16bitBEValue_HPP_

#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>

#include "data_model/decoder_objects/ElementValue.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is an int16_t - 2 Bytes.
 * This class is used when the decoded value is a integer.
 */

class SignedInteger16bitBEValue : public ElementValue {
public:
    SignedInteger16bitBEValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Signed16bitIntegerBEValue;
    }

    SignedInteger16bitBEValue(DecodedValueObjectPoolManager *_ptrToManager, int16_t _decodedValue,
                            RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }

    virtual ~SignedInteger16bitBEValue() = default;

    int16_t getDecodedValue() const;

    void setDecodedValue(int16_t _decodedValue,
                         RepresentationModeEnum _representationMode = RepresentationModeEnum::Signed16bitIntegerBEValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    int16_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_SignedInteger16bitBEValue_HPP_ */
