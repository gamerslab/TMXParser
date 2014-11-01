#ifndef TSXPARSER_H
#define TSXPARSER_H

#include <string>
#include <vector>
#include <utility>
#include <map>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

namespace TSX {
    struct TilesetImage {
        std::string source;
        unsigned int width;
        unsigned int height;
    };

    struct Tile {
        unsigned int id;
        std::map<std::string, std::string> properties;
        bool animated;
        unsigned int first_frame;

        std::string Property(const std::string& name) const;
    };

    struct Tileset {
        std::string name;
        unsigned int tile_width;
        unsigned int tile_height;
        unsigned int width;
        unsigned int height;
        unsigned int frames;
        float interval;
        bool random;
        bool show_hud;
        float horizontal_ratio;
        float vertical_ratio;
        std::string music;
        std::string intro;


        TilesetImage image;
        std::map<std::string, std::string> property;
        std::map<int, Tile> tiles;

        void Print() const;
    };

    Tileset* parse(const char* filename);
}
#endif // TSXPARSER_H
