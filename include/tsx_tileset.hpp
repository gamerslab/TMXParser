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
        std::string transparentColor;
        unsigned int width;
        unsigned int height;
    };

    struct Terrain {
        std::string name;
        unsigned int tile;
        std::map<std::string, std::string> property;
    };

    struct Tile {
        unsigned int id;
        std::vector<unsigned int> terrain;
        std::map<std::string, std::string> property;
    };

    struct Tileset {
        std::string name;
        unsigned int tile_width;
        unsigned int tile_height;
        unsigned int width;
        unsigned int height;
        float horizontal_ratio;
        float vertical_ratio;
        unsigned int spacing;
        unsigned int margin;
        int offsetX;
        int offsetY;


        TilesetImage image;
        std::map<std::string, std::string> property;
        std::vector<Terrain> terrainList;
        std::vector<Tile> tileList;

        void Print() const;
    };

    Tileset* parse(const char* filename);
}
#endif // TSXPARSER_H
