#include "civil_codec/Record.h"

Record::Record() {
    categoryNumber = 0;
}

Record::Record(unsigned int _categoryNumber, vector<pair<string, string>> _elements) {
    // Variables declaration
    Item *itemFound = nullptr;
    short int elementIndexInItemDesc = -1;
    ElementValue *elementValue = nullptr;
    RepresentationModeEnum representationModeEnum;
    FunctionNameEnum functionName;

    CommonVariables *common = CommonVariables::getInstance();
    CategoryDescription *categoryDescription = nullptr;
    map<string, Item *> *elementsMap = nullptr;
    map<string, Item *>::iterator element_iterator;
    stringstream categoryDescriptionKey;
    unordered_map<int, CategoryDescription *>::iterator categoryDescription_it;
    unordered_map<int, CategoryDescription *> *categoriesDescription;

    FailureReport *encoderFailureReport = FailureReport::getInstance();


    // Looking for the "SAC" and "SIC" Elements, in order to get the specific Category description for the datasource
    auto sacElement = std::find_if(_elements.begin(), _elements.end(),
                                   [](const std::pair<std::string, std::string> &element) {
        return strcasecmp((element.first).c_str(), "SAC") == 0;
    });

    auto sicElement = std::find_if(_elements.begin(), _elements.end(),
                                   [](const std::pair<std::string, std::string> &element) {
        return strcasecmp((element.first).c_str(), "SIC") == 0;
    });


    categoriesDescription = common->getCategoriesDescription();

    // Start to build the key in order to looking in the 'categoriesDescription' map
    categoryDescriptionKey << _categoryNumber;

    // If "SAC" and "SIC" Elements exist -> looking for the Category description for this datasource
    if ((sacElement != _elements.end()) && (sicElement != _elements.end())) {

        if (common->isEncodingDebugEnabled()) {
            cout << "SAC: " << (*sacElement).second << endl;
            cout << "SIC: " << (*sicElement).second << endl;
        }

        // Build the key in order to looking for the category description in the 'categoriesDescription' map
        categoryDescriptionKey << (*sacElement).second << (*sicElement).second;

        categoryDescription_it = categoriesDescription->find(stoi(categoryDescriptionKey.str()));

        // If the proper description has NOT been found..
        if (categoryDescription_it == categoriesDescription->end()) {
            // ..we look for the DEFAULT Category description (not SourceId-specific)
            categoryDescriptionKey.str(to_string(_categoryNumber));
        }
    }

    // look for the DEFAULT Category description.
    categoryDescription_it = categoriesDescription->find(stoi(categoryDescriptionKey.str()));

    // If the description has NOT been found..
    if (categoryDescription_it == categoriesDescription->end()) {
        encoderFailureReport->addFailure(_categoryNumber, 0, "", nullptr,
                                         "The Category number provided has not been found in the Category Description repository!",
                                         ReturnCodes::UNKNOWN_CATEGORY);
        return;
    }

    categoryDescription = categoryDescription_it->second;

    elementsMap = categoryDescription->getElementsMap();

    // For each Element in the '_elements' vector, get the RepresentationModeEnum from the Item description
    // and create an instance of ElementValue in order to store the value of current Element
    for (auto el : _elements) {

        if (el.second == "") {
            string message = "Element " + el.first + " is empty";
            encoderFailureReport->addFailure(_categoryNumber, 0, "", nullptr, message, ReturnCodes::UNKNOWN_CATEGORY);
            this->elements.clear(); //Remove Elements already stored
            break;
        }

        element_iterator = elementsMap->find(el.first);

        if (element_iterator == elementsMap->end()) {
            string message = "Element " + el.first + " has not been found in the Category Description";
            encoderFailureReport->addFailure(_categoryNumber, 0, "", nullptr, message, ReturnCodes::UNKNOWN_CATEGORY);
            this->elements.clear(); //Remove Elements already stored
            return;
        }

        itemFound = element_iterator->second;

        elementIndexInItemDesc = itemFound->getIndexofElement(el.first);

        functionName = itemFound->getFunctionNameInEnumForDecodingAtIndex(elementIndexInItemDesc);
        representationModeEnum = itemFound->getRepresentationModeInEnumForDecodingAtIndex(elementIndexInItemDesc);

        std::string elementValueInString = el.second;

        // Convert string based on functionName
        if (functionName == FunctionNameEnum::toHexadecimal) {
            elementValueInString = std::to_string(std::stoi(elementValueInString, nullptr, 16));
        } else if (functionName == FunctionNameEnum::toOctal) {
            elementValueInString = std::to_string(std::stoi(elementValueInString, nullptr, 8));
        }

        // Handle different representation modes
        switch (representationModeEnum) {
        case RepresentationModeEnum::Unsigned8bitIntegerValue:
            elementValue = new UnsignedInteger8bitValue(nullptr, static_cast<uint8_t>(std::stoi(elementValueInString)),
                                                        representationModeEnum);
            break;
        case RepresentationModeEnum::Unsigned16bitIntegerValue:
            elementValue = new UnsignedInteger16bitValue(nullptr, static_cast<uint16_t>(std::stoi(elementValueInString)),
                                                         representationModeEnum);
            break;
        case RepresentationModeEnum::Unsigned32bitIntegerValue:
            elementValue = new UnsignedInteger32bitValue(nullptr, static_cast<uint32_t>(std::stoi(elementValueInString)),
                                                         representationModeEnum);
            break;
        case RepresentationModeEnum::Unsigned64bitIntegerValue:
            elementValue = new UnsignedInteger64bitValue(nullptr, static_cast<uint64_t>(std::stoll(elementValueInString)),
                                                         representationModeEnum);
            break;
        case RepresentationModeEnum::Signed8bitIntegerValue:
            elementValue = new SignedInteger8bitValue(nullptr, static_cast<int8_t>(std::stoi(elementValueInString)),
                                                      representationModeEnum);
            break;
        case RepresentationModeEnum::Signed16bitIntegerValue:
            elementValue = new SignedInteger16bitValue(nullptr, static_cast<int16_t>(std::stoi(elementValueInString)),
                                                       representationModeEnum);
            break;
        case RepresentationModeEnum::Signed32bitIntegerValue:
            elementValue = new SignedInteger32bitValue(nullptr, static_cast<int32_t>(std::stoi(elementValueInString)),
                                                       representationModeEnum);
            break;
        case RepresentationModeEnum::Signed64bitIntegerValue:
            elementValue = new SignedInteger64bitValue(nullptr, static_cast<int64_t>(std::stoll(elementValueInString)),
                                                       representationModeEnum);
            break;
        case RepresentationModeEnum::FloatValue:
            elementValue = new FloatValue(nullptr, std::stof(elementValueInString));
            break;
        case RepresentationModeEnum::DoubleValue:
            elementValue = new DoubleValue(nullptr, std::stod(elementValueInString));
            break;
        case RepresentationModeEnum::StringValue:
            elementValue = new StringValue(nullptr, std::vector<unsigned char>(elementValueInString.begin(), elementValueInString.end()));
            break;
        default:
            break;
        }
        this->elements.insert(pair<string, ElementValue *>(el.first, elementValue));
    }
    categoryNumber = _categoryNumber;
}

unsigned short int Record::getCategoryNumber() {
    return categoryNumber;
}

void Record::setCategoryNumber(unsigned short int _categoryNumber) {
    this->categoryNumber = _categoryNumber;
}

map<string, ElementValue *> *Record::getElements() {
    return &elements;
}

void Record::setElements(map<string, ElementValue *> elements) {
    this->elements = elements;
}
