#ifndef DATA_MODEL_civil_DESCRIPTION_EXTERNAL_EXTERNALCATEGORYDESCRIPTION_H_
#define DATA_MODEL_civil_DESCRIPTION_EXTERNAL_EXTERNALCATEGORYDESCRIPTION_H_

#include <vector>

#include "data_model/civil_description/external/ExternalItem.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * The ExternalCategoryDescription class maps the civil Category JSON description to the internal Codec data model
 */
class ExternalCategoryDescription {
public:
    ExternalCategoryDescription();

    //GETTERS AND SETTERS

    /**
     * Returns a vector of ExternalItem instances, where the civil Category JSON description is mapped to.
     * @return a vector<ExternalItem> holding the Item collection of an civil Category
     */
    vector<ExternalItem> getItemCollection() const;

    vector<ExternalItem> *getPointerToItemCollection();

    /**
     * Sets a vector of ExternalItem instances, representing the Item collection of an civil Category JSON description.
     */
    void setItemCollection(vector<ExternalItem> itemCollection);

    /**
     * Returns a vector of ExternalItem instances, where the civil Category JSON description is mapped to.
     * @return a vector<ExternalItem> holding the collection of Compound Subitems of an civil Category
     */
    vector<ExternalItem> getCompoundSubitemsCollection() const;

    vector<ExternalItem> *getPointerToCompoundSubitemsCollection();

    /**
     * Sets a vector of ExternalItem instances, representing the Compound Subitem collection of an civil Category JSON description.
     */
    void setCompoundSubitemsCollection(vector<ExternalItem> compoundSubitemsCollection);

private:
    vector<ExternalItem> itemCollection;
    vector<ExternalItem> compoundSubitemCollection;
};

// NLOHMANN-JSON
void to_json(json &j, const ExternalCategoryDescription &categoryDescription);

void from_json(const json &j, ExternalCategoryDescription &categoryDescription);

#endif /* DATA_MODEL_civil_DESCRIPTION_EXTERNAL_EXTERNALCATEGORYDESCRIPTION_H_ */
