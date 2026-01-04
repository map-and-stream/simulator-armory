#ifndef DATA_MODEL_civilCONFIGURATION_HPP_
#define DATA_MODEL_civilCONFIGURATION_HPP_

#include <string>
#include <vector>

#include "data_model/civil_description/external/ExternalItem.h"
#include "data_model/CategoryConfiguration.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * \brief This class contains Category configurations with all file paths needed for each available Category. It is directly loaded from the JSON configuration file and
 * all information loaded are used later to create an civil categories description map.
 */
class civilConfiguration {

public:
    civilConfiguration() = default;

    ~civilConfiguration() = default;

    /**
     * Getter for the categoriesConfiguration object (it makes a copy of the private variable).
     */
    const vector<CategoryConfiguration> &getCategoriesConfiguration() const;

    /**
     * Setter for the categoriesConfiguration object.
     */
    void setCategoriesConfiguration(const vector<CategoryConfiguration> &categoriesConfiguration);

    /**
     * Getter for the categoriesFilterFile path.
     */
    const string &getCategoriesFilterFile() const;

    /**
     * Setter for the categoriesFilterFile path.
     */
    void setCategoriesFilterFile(const string &categoriesFilterFile);

private:
    /**
     * Vector of Category Description coming from civil Configuration File
     */
    vector<CategoryConfiguration> categoriesConfiguration;
    string categoriesFilterFile;
};

// NLOHMANN-JSON
void to_json(json &j, const civilConfiguration &civilConfig);

void from_json(const json &j, civilConfiguration &civilConfig);

#endif /* DATA_MODEL_civilCONFIGURATION_HPP_ */
