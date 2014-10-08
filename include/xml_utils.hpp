#include "rapidxml/rapidxml.hpp"

namespace TMX {
    const char* get_attribute(rapidxml::xml_node<> *node, const char *attr, const char *default_value);
}
