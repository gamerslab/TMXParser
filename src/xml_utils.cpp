#include "xml_utils.hpp"

namespace TMXUtils
{
    const char* get_attribute(rapidxml::xml_node<> *node, const char *attr_name, const char *default_value) {
        rapidxml::xml_attribute<> *attr = node->first_attribute(attr_name);

        if(attr)
            return attr->value();

        return default_value;
    }

    std::string dirname(const char* filename) {
        std::string fs = filename;
        std::string dir = fs.substr(0, fs.find_last_of("/"));

        if(! dir.empty())
            dir.append("/");

        return dir;
    }
}
