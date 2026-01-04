#ifndef DATA_MODEL_DECODER_OBJECTS_UnsignedInteger64bitBEValue_HPP_
#define DATA_MODEL_DECODER_OBJECTS_UnsignedInteger64bitBEValue_HPP_

#include <cstdint>
#include <string>

#include "data_model/decoder_objects/ElementValue.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * Store decoded data of an element.
 * The type of value is a uint64_t - 8 Bytes.
 * This class is used when the decoded value is a integer.
 */

class UnsignedInteger64bitBEValue : public ElementValue {
public:
    UnsignedInteger64bitBEValue(DecodedValueObjectPoolManager *_ptrToManager)
            : ElementValue(_ptrToManager) {
        decodedValue = 0;
        representationMode = RepresentationModeEnum::Unsigned64bitIntegerBEValue;
    }

    UnsignedInteger64bitBEValue(DecodedValueObjectPoolManager *_ptrToManager, uint64_t _decodedValue,
                              RepresentationModeEnum _representationMode)
            : ElementValue(_ptrToManager) {
        decodedValue = _decodedValue;
        representationMode = _representationMode;
    }

    virtual ~UnsignedInteger64bitBEValue() = default;

    uint64_t getDecodedValue() const;

    void setDecodedValue(uint64_t _decodedValue,
                         RepresentationModeEnum _representationMode = RepresentationModeEnum::Unsigned64bitIntegerBEValue);

    void resetElementValue() override;

    string getDecodedDataInString() const override;

    string *getPointerToDecodedDataInString() override;

private:
    uint64_t decodedValue;
    RepresentationModeEnum representationMode;
};

#endif /* DATA_MODEL_DECODER_OBJECTS_UnsignedInteger64bitBEValue_HPP_ */
