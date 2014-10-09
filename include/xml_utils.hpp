#include "rapidxml/rapidxml.hpp"

namespace TMXUtils {
    const char* get_attribute(rapidxml::xml_node<> *node, const char *attr, const char *default_value);
    const char* dirname(const char* filename);
}
