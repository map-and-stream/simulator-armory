#ifndef DATA_MODEL_DECODER_OBJECTS_UnsignedInteger32bitBEValue_HPP_
#define DATA_MODEL_DECODER_OBJECTS_UnsignedInteger32bitBEValue_HPP_

#include <cstdint>
#include <string>
#include <iostream>
#include <sstream>

#include "data_model/decoder_objects/ElementValue.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a uint32_t - 4 Bytes.
 * This class is used when the decoded value is a integer.
 */

class UnsignedInteger32bitBEValue : public ElementValue {
public:
    UnsignedInteger32bitBEValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Unsigned32bitIntegerBEValue;
    }

    UnsignedInteger32bitBEValue(DecodedValueObjectPoolManager *_ptrToManager, uint32_t _decodedValue,
                              RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }

    virtual ~UnsignedInteger32bitBEValue() = default;

    uint32_t getDecodedValue() const;

    void setDecodedValue(uint32_t _decodedValue,
                         RepresentationModeEnum _representationMode = RepresentationModeEnum::Unsigned32bitIntegerBEValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    uint32_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_UnsignedInteger32bitBEValue_HPP_ */
