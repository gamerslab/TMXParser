#include "rapidxml/rapidxml.hpp"
#include <string>

namespace TMXUtils {
    const char* get_attribute(rapidxml::xml_node<> *node, const char *attr, const char *default_value);
    std::string dirname(const char* filename);
}
