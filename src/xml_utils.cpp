#include "xml_utils.hpp"
#include <iostream>

namespace TMXUtils
{
    const char* get_attribute(rapidxml::xml_node<> *node, const char *attr_name, const char *default_value) {
        rapidxml::xml_attribute<> *attr = node->first_attribute(attr_name);

        if(attr)
            return attr->value();

        return default_value;
    }

    const char* dirname(const char* filename) {
        std::string fs = filename;
        std::string dirname = fs.substr(0, fs.find_last_of("/"));

        if(not dirname.empty())
            dirname = dirname.append("/");

        return dirname.c_str();
    }
}
