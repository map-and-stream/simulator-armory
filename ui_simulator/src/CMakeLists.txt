set(LIBRARY_NAME civilcodec)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

if (MSVC)
    add_compile_options(/W3 /WX)
else ()
    # "-Wno-unused-parameter" to remove later on
    add_compile_options(-W -Wall #[[-Werror -Wno-unused-parameter]])
endif ()

project(
        ${LIBRARY_NAME}
        VERSION 0.9
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

add_library(${LIBRARY_NAME} STATIC
    src/config/CommonVariables.cpp
    src/config/FacadeCommonVariables.cpp
    src/config/enum/FunctionNameEnum.cpp
    src/config/enum/FilterTypeEnum.cpp
    src/config/enum/ItemTypeEnum.cpp
    src/config/enum/RepresentationModeEnum.cpp
    src/data_model/civil_description/external/UapAndMandatoryItems.cpp
    src/data_model/decoder_objects/IntegerValue.cpp
    src/data_model/decoder_objects/FloatValue.cpp
    src/data_model/decoder_objects/DoubleValue.cpp
    src/data_model/decoder_objects/StringValue.cpp
    src/data_model/decoder_objects/UnsignedInteger8bitValue.cpp
    src/data_model/decoder_objects/UnsignedInteger16bitValue.cpp
    src/data_model/decoder_objects/UnsignedInteger32bitValue.cpp
    src/data_model/decoder_objects/UnsignedInteger64bitValue.cpp
    src/data_model/decoder_objects/SignedInteger8bitValue.cpp
    src/data_model/decoder_objects/SignedInteger16bitValue.cpp
    src/data_model/decoder_objects/SignedInteger32bitValue.cpp
    src/data_model/decoder_objects/SignedInteger64bitValue.cpp
    src/data_model/ItemsToFilter.cpp
    src/data_model/CategoryItemsFilter.cpp
    src/data_model/CategoryConfiguration.cpp
    src/data_model/civilConfiguration.cpp
    src/data_model/CategoryFilter.cpp
    src/data_model/CategoriesToFilter.cpp
    src/data_model/CategoriesFilters.cpp
    src/data_model/civil_description/Item.cpp 
    src/data_model/civil_description/CategoryDescription.cpp 
    src/data_model/civil_description/external/ExternalItem.cpp 
    src/data_model/civil_description/external/ExternalCategoryDescription.cpp
    src/data_model/blocking_queue/BlockingQueueManager.cpp
    src/data_model/blocking_queue/DecoderBlockingQueue.cpp
    src/data_model/blocking_queue/ParserBlockingQueue.cpp
    src/data_model/decoder_objects/ParsedElement.cpp
    src/data_model/decoder_object_pool/DecodedValueObjectPoolManager.cpp
    src/data_model/decoder_object_pool/ParsedRecord.cpp
    src/utils/civil_json_description/JsonCategoriesDescriptionLoader.cpp
    src/utils/civil_json_description/JsonCategoriesDescriptionValidator.cpp
    src/utils/BitUtils.cpp
    src/utils/decoderForParsedRecord.cpp
    src/utils/DecodingUtils.cpp
    src/utils/ElementEncodingUtils.cpp
    src/utils/ItemEncodingUtils.cpp
    src/utils/ParsingUtils.cpp
    src/utils/PrintingUtils.cpp
    include/civil_codec/facade/CodecInterface.cpp
    include/civil_codec/facade/DecoderInterface.cpp
    include/civil_codec/facade/EncoderInterface.cpp
    include/civil_codec/facade/Record.cpp
    include/civil_codec/facade/RecordCollection.cpp
    include/civil_codec/facade/reports/FailureReport.cpp
    src/data_model/decoder_objects/SignedInteger16bitBEValue.cpp
    src/data_model/decoder_objects/SignedInteger32bitBEValue.cpp
    src/data_model/decoder_objects/UnsignedInteger64bitBEValue.cpp
    src/data_model/decoder_objects/UnsignedInteger32bitBEValue.cpp
    src/data_model/decoder_objects/UnsignedInteger16bitBEValue.cpp
    src/data_model/decoder_objects/SignedInteger64bitBEValue.cpp
    include/config/CommonVariables.h
    include/config/Constants.h
    include/config/FacadeCommonVariables.h
    include/config/enum/FilterTypeEnum.h
    include/config/enum/FunctionNameEnum.h
    include/config/enum/ItemTypeEnum.h
    include/config/enum/RepresentationModeEnum.h
    include/data_model/CategoryItemsFilter.h
    include/data_model/CategoryConfiguration.h
    include/data_model/civilConfiguration.h
    include/data_model/CategoryFilter.h
    include/data_model/CategoriesToFilter.h
    include/data_model/CategoriesFilters.h
    include/data_model/ItemsToFilter.h
    include/data_model/civil_description/Item.h
    include/data_model/civil_description/CategoryDescription.h
    include/data_model/civil_description/external/ExternalItem.h
    include/data_model/civil_description/external/ExternalCategoryDescription.h
    include/data_model/civil_description/external/UapAndMandatoryItems.h
    include/data_model/blocking_queue/BlockingQueueManager.h
    include/data_model/blocking_queue/DecoderBlockingQueue.h
    include/data_model/blocking_queue/ParserBlockingQueue.h
    include/data_model/decoder_objects/DoubleValue.h
    include/data_model/decoder_objects/ElementValue.hpp
    include/data_model/decoder_objects/FloatValue.h
    include/data_model/decoder_objects/IntegerValue.h
    include/data_model/decoder_objects/ParsedElement.h
    include/data_model/decoder_objects/SignedInteger8bitValue.h
    include/data_model/decoder_objects/SignedInteger16bitValue.h
    include/data_model/decoder_objects/SignedInteger32bitValue.h
    include/data_model/decoder_objects/SignedInteger64bitValue.h
    include/data_model/decoder_objects/StringValue.h
    include/data_model/decoder_objects/UnsignedInteger8bitValue.h
    include/data_model/decoder_objects/UnsignedInteger16bitValue.h
    include/data_model/decoder_objects/UnsignedInteger32bitValue.h
    include/data_model/decoder_objects/UnsignedInteger64bitValue.h
    include/data_model/decoder_objects/SignedInteger32bitBEValue.h
    include/data_model/decoder_objects/SignedInteger16bitBEValue.h
    include/data_model/decoder_objects/UnsignedInteger64bitBEValue.h
    include/data_model/decoder_objects/UnsignedInteger32bitBEValue.h
    include/data_model/decoder_objects/UnsignedInteger16bitBEValue.h
    include/data_model/decoder_objects/SignedInteger64bitBEValue.h
    include/data_model/decoder_object_pool/DecodedValueObjectPoolManager.h
    include/data_model/decoder_object_pool/ObjectPool.hpp
    include/data_model/decoder_object_pool/ParsedRecord.h
    include/utils/ConversionFromStringToEnum.hpp
    include/utils/civil_json_description/JsonCategoriesDescriptionLoader.h
    include/utils/civil_json_description/JsonCategoriesDescriptionValidator.h
    include/utils/BitUtils.h
    include/utils/decoderForParsedRecord.h
    include/utils/DecodingUtils.h
    include/utils/ElementEncodingUtils.h
    include/utils/encoderForDecodedRecord.hpp
    include/utils/ItemEncodingUtils.h
    include/utils/ParsingUtils.h
    include/utils/PrintingUtils.h
    include/civil_codec/CodecInterface.h
    include/civil_codec/DecoderInterface.h
    include/civil_codec/EncoderInterface.h
    include/civil_codec/RecordCollection.h
    include/civil_codec/Record.h
    include/civil_codec/facade/CodecInterface.cpp
    include/civil_codec/facade/DecoderInterface.cpp
    include/civil_codec/facade/EncoderInterface.cpp
    include/civil_codec/facade/Record.cpp
    include/civil_codec/facade/RecordCollection.cpp
    include/civil_codec/facade/reports/FailureReport.cpp)

## Nlohmann JSON - header_only
## Thread_Pool - header_only
add_library(thread_pool INTERFACE)
target_include_directories(thread_pool
        INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib/thread_pool>
        $<INSTALL_INTERFACE:lib/thread_pool>
        )

## PTHREADS library
find_package(Threads REQUIRED)
if (Threads_FOUND)
    #    message(STATUS ${CMAKE_THREAD_LIBS_INIT})
    target_link_libraries(${LIBRARY_NAME} PUBLIC ${CMAKE_THREAD_LIBS_INIT})
endif ()

target_link_libraries(${LIBRARY_NAME}
        PUBLIC
        Threads::Threads
        INTERFACE
        thread_pool
        )

#####

target_include_directories(${LIBRARY_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/lib>
        PRIVATE
        include/config
        include/data_model
        include/utils
        )

#####

# da leggere
#https://stackoverflow.com/questions/25676277/cmake-target-include-directories-prints-an-error-when-i-try-to-add-the-source
#####

install(TARGETS ${LIBRARY_NAME} thread_pool
        EXPORT AsterixCodecLibraryTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

install(EXPORT AsterixCodecLibraryTargets
        FILE AsterixCodecLibraryTargets.cmake
        NAMESPACE AsterixCodec::
        DESTINATION lib/cmake/AsterixCodecLibrary
        )

install(FILES "AsterixCodecLibraryTargets.cmake"
        DESTINATION
        lib/cmake/AsterixCodecLibrary
        )

#####

# UNIT TEST - w/GTest
# add_subdirectory(test)
