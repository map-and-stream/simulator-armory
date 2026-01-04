#pragma once

#include <cstdint>
#include <vector>

#include "include/civil_codec/civil_codec.h"

enum CAT {
    CATEGORY_150 = 150,
    CATEGORY_151 = 151,
    CATEGORY_152 = 152,
    CATEGORY_153 = 153,
    CATEGORY_154 = 154,
    CATEGORY_101 = 101,
    CATEGORY_102 = 102,
    CATEGORY_103 = 103,
    CATEGORY_104 = 104,
    CATEGORY_105 = 105,
    CATEGORY_106 = 106,
    CATEGORY_107 = 107,
    CATEGORY_108 = 108,
    CATEGORY_109 = 109,
    CATEGORY_110 = 110,
    CATEGORY_111 = 111,
    CATEGORY_112 = 112,
    CATEGORY_113 = 113,
    CATEGORY_114 = 114,
};

using PAYAM_DATA = std::vector<std::pair<std::string, std::string>>;
using PAYAM_ELEMENT = std::pair<std::string, std::string>;

class Payam {
  public:
    Payam() { configPayam(); }
    void encode(int cat, const std::vector<std::pair<std::string, std::string>>& elements,
                std::vector<uint8_t>& out) {
        Record* record = new Record(cat, elements);

        // VARIABLE DEFINITIONS
        unsigned char* datablockWithSingleRecord = nullptr;
        unsigned int currentEncodedRecordNumber = 0;
        unsigned int datablockLength;

        if (record->getElements()->size() > 0) {
            datablockWithSingleRecord =
                encoderInterface->encodeSingleRecord(record, &currentEncodedRecordNumber);

            datablockLength = (unsigned int)(datablockWithSingleRecord[5] +
                                             ((datablockWithSingleRecord[4] << 8) & 0xFF00)) +
                              5;
            if (datablockWithSingleRecord != nullptr && datablockLength > 0) {
                out.assign(datablockWithSingleRecord, datablockWithSingleRecord + datablockLength);
            }
        }
    }
    void decode(const std::vector<uint8_t>& input, RecordCollection** decodedRecords) {
        unsigned char* civil_datastream =
            const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(input.data()));
        unsigned int lengthOfBytestreamToParse = static_cast<unsigned int>(input.size());

        *decodedRecords =
            decoderInterface->startDecodingDatablocks(civil_datastream, &lengthOfBytestreamToParse);

//         FailureReport* report = codecInterface->getFinalReport();
//         if (report != nullptr) {
//             if (report->getFailuresNumber() > 0) {
//                 report->printOnFile("failure_report.log");
//             } else {
//                 remove("failure_report.log");
//             }
//             report->clearFailures();
//         }
         if (!decodedRecords) {
             std::cerr << "enter if !decodedRecords" << std::endl;
             return;
         }
    }

  private:
    void configPayam() {
        string civil_categoryRepositoryPath = "codec/codecConfiguration.json";
        ReturnStatus status = ReturnStatus();

        codecInterface->loadcivilConfiguration(civil_categoryRepositoryPath, status);
        if (status.getCode() != ReturnCodes::SUCCESS) {
            cout << status.getMessage() << endl;
            //        if (status.getCode() != ReturnCodes::WARNING)
            //            return status.getCode();
        }

        // ** SET UP **
        codecInterface->setUpEnvironment(status);

        if (status.getCode() == ReturnCodes::SPEED_ISSUE) {
            cout << status.getMessage() << endl;
        } else if (status.getCode() != ReturnCodes::SUCCESS) {
            cout << status.getMessage() << endl;
            //        return status.getCode();
        }
        decoderInterface = codecInterface->getDecoderInterface();
        encoderInterface = codecInterface->getEncoderInterface();
    }
    CodecInterface* codecInterface = CodecInterface::getInstance();
    DecoderInterface* decoderInterface = nullptr;
    EncoderInterface* encoderInterface = nullptr;
};
