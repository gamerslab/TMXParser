#include "xml_utils.hpp"

namespace TMX
{
    const char* get_attribute(rapidxml::xml_node<> *node, const char *attr_name, const char *default_value) {
        rapidxml::xml_attribute<> *attr = node->first_attribute(attr_name);

        if(attr)
            return attr->value();

        return default_value;
    }
}
