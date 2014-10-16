#ifndef TMXPARSER_H
#define TMXPARSER_H

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "tsx_tileset.hpp"

#include <string>
#include <vector>
#include <utility>
#include <map>

#define VERSION "1.0"

namespace TMX
{
    struct TileLayer {
        std::string name;
        unsigned int width;
        unsigned int height;
        bool visible;
        double opacity;
        std::vector<std::vector<int> > tiles;
        std::map<std::string, std::string> property;
    };

    struct Object {
        std::string name;
        std::string type;
        int x;
        int y;
        unsigned int width;
        unsigned int height;
        unsigned int gid;
        bool visible;
        std::map<std::string, std::string> property;
    };

    struct ObjectGroup {
        std::string color;
        std::string name;
        double opacity;
        bool visible;
        std::vector<Object> object;
        std::map<std::string, std::string> property;
    };

    struct Image {
        std::string source;
        std::string transparency_color;
    };

    struct ImageLayer {
        std::string name;
        double opacity;
        bool visible;
        std::map<std::string, std::string> property;
        Image image;
    };

    struct Map {
        std::string version;
        std::string orientation;
        std::string background_color;
        unsigned int width;
        unsigned int height;
        unsigned int tile_width;
        unsigned int tile_height;
        unsigned int width_pixels;
        unsigned int height_pixels;
        std::map<std::string, std::string> property;
        std::vector<TSX::Tileset*> tilesets;
        std::vector<TileLayer*> tile_layers;
        std::map<std::string, ObjectGroup> object_groups;
        std::map<std::string, ImageLayer> image_layers;

        void Print();
    };

    Map *parse(const char *filename);
}
#endif // TMXPARSER_H
