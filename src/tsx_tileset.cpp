#include "tsx_tileset.hpp"

#include <iostream>
#include <sstream>
#include "xml_utils.hpp"

namespace TSX {
    Tileset* parse(const char* filename) {
        // TODO: Clean this function
        std::string dirname = TMXUtils::dirname(filename);
        Tileset* tileset = new Tileset();

        rapidxml::xml_node<> *root_node;
        rapidxml::xml_document<> doc;
        rapidxml::file<> file(filename);
        doc.parse<0>(file.data());

        root_node = doc.first_node("tileset");

        tileset->name = root_node->first_attribute("name")->value();
        tileset->tile_width = (unsigned int) std::atoi(root_node->first_attribute("tilewidth")->value());
        tileset->tile_height = (unsigned int) std::atoi(root_node->first_attribute("tileheight")->value());

        if(root_node->first_node("properties") != 0) {
            for(rapidxml::xml_node<> *properties_node = root_node->first_node("properties")->first_node("property"); properties_node; properties_node = properties_node->next_sibling()) {
                tileset->property[properties_node->first_attribute("name")->value()] = properties_node->first_attribute("value")->value();
            }
        }

        auto it = tileset->property.find("frames");
        tileset->frames = it == tileset->property.end() ? 1 : (unsigned int) std::atoi(it->second.c_str());
        it = tileset->property.find("interval");
        tileset->interval = it == tileset->property.end() ? 0 : (float) std::atof(it->second.c_str());
        tileset->random = tileset->property.find("random") != tileset->property.end();
        tileset->show_hud = tileset->property.find("hide_hud") == tileset->property.end();

        tileset->music = tileset->property["music"];
        tileset->intro = tileset->property["intro"];

        // Tileset image
        std::stringstream source;
        source << dirname << root_node->first_node("image")->first_attribute("source")->value();
        tileset->image.source = source.str();
        tileset->image.width = (unsigned int) std::atoi(root_node->first_node("image")->first_attribute("width")->value());
        tileset->image.height = (unsigned int) std::atoi(root_node->first_node("image")->first_attribute("height")->value());

        tileset->width = tileset->image.width / tileset->tile_width;
        tileset->height = tileset->image.height / tileset->tile_height;
        tileset->horizontal_ratio = 1.0f / tileset->width;
        tileset->vertical_ratio = 1.0f / tileset->height;

        // Parse tiles
        if(root_node->first_node("tile") != 0) {
            for(rapidxml::xml_node<> *tile_node = root_node->first_node("tile"); tile_node; tile_node = tile_node->next_sibling()) {

                unsigned int tile_id = (unsigned int) std::atoi(tile_node->first_attribute("id")->value());

                Tile& tile = tileset->tiles[tile_id];
                tile.id = tile_id;

                //parse tile properties
                if(tile_node->first_node("properties") != 0) {
                    for(rapidxml::xml_node<> *properties_node = tile_node->first_node("properties")->first_node("property"); properties_node; properties_node = properties_node->next_sibling()) {
                        tile.properties[properties_node->first_attribute("name")->value()] = properties_node->first_attribute("value")->value();
                    }
                }

                tile.animated = tile.properties.find("animated") != tile.properties.end();
                tile.first_frame = tile.animated && tileset->random ? rand() % tileset->frames : 0;
            }
        }

        return tileset;
    }

    void Tileset::Print() const {
        std::cout << "Tileset[" << name << '@' << image.source << ']' << std::endl;
    }

    std::string Tile::Property(const std::string& name) const {
        std::map<std::string, std::string>::const_iterator it = properties.find(name);
        if(it == properties.end())
            return "N/A";

        return it->second;
    }
}
