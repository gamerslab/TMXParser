#include "tsx_tileset.hpp"

#include <iostream>
#include <sstream>

namespace TSX {
    Tileset* parse(const char* dirname, const char* filename) {
        // TODO: Clean this function
        Tileset* tileset = new Tileset();
        std::stringstream path;
        path << dirname << filename;

        rapidxml::xml_node<> *root_node;
        rapidxml::xml_document<> doc;
        rapidxml::file<> file(path.str().c_str());
        doc.parse<0>(file.data());

        // Get root node
        root_node = doc.first_node("tileset");

        // Parse tileset element
        tileset->name = root_node->first_attribute("name")->value();
        tileset->tile_width = (unsigned int) std::atoi(root_node->first_attribute("tilewidth")->value());
        tileset->tile_height = (unsigned int) std::atoi(root_node->first_attribute("tileheight")->value());
        //tileset->spacing = (unsigned int) std::atoi(root_node->first_attribute("spacing")->value());
        //tileset->margin = (unsigned int) std::atoi(root_node->first_attribute("margin")->value());

        if(root_node->first_node("tileoffset") != 0) {
            tileset->offsetX = std::atoi(root_node->first_node("tileoffset")->first_attribute("x")->value());
            tileset->offsetY = std::atoi(root_node->first_node("tileoffset")->first_attribute("y")->value());
        } else {
            tileset->offsetX = 0;
            tileset->offsetY = 0;
        }

        // Parse tileset properties
        if(root_node->first_node("properties") != 0) {
            for(rapidxml::xml_node<> *properties_node = root_node->first_node("properties")->first_node("property"); properties_node; properties_node = properties_node->next_sibling()) {
                tileset->property[properties_node->first_attribute("name")->value()] = properties_node->first_attribute("value")->value();
            }
        }

        //parse tileset image
        tileset->image.source = root_node->first_node("image")->first_attribute("source")->value();
        tileset->image.width = (unsigned int) std::atoi(root_node->first_node("image")->first_attribute("width")->value());
        tileset->image.height = (unsigned int) std::atoi(root_node->first_node("image")->first_attribute("height")->value());

        if(root_node->first_node("image")->first_attribute("trans") != 0) {
            tileset->image.transparentColor = std::atoi(root_node->first_node("image")->first_attribute("trans")->value());
        } else {
            tileset->image.transparentColor = "ffffff";
        }

        //parse tileset terrains
        if(root_node->first_node("terraintypes") != 0) {
            for(rapidxml::xml_node<> *terrain_node = root_node->first_node("terraintypes")->first_node("terrain"); terrain_node; terrain_node = terrain_node->next_sibling()) {
                Terrain terrain;
                terrain.name = terrain_node->first_attribute("name")->value();
                terrain.tile = std::atoi(terrain_node->first_attribute("tile")->value());

                //parse tileset terrain properties
                if(terrain_node->first_node("properties") != 0) {
                    for(rapidxml::xml_node<> *properties_node = terrain_node->first_node("properties")->first_node("property"); properties_node; properties_node = properties_node->next_sibling()) {
                        terrain.property[properties_node->first_attribute("name")->value()] = properties_node->first_attribute("value")->value();
                    }
                }

                tileset->terrainList.push_back(terrain);
            }
        }

        // Parse tiles
        if(root_node->first_node("tile") != 0) {
            for(rapidxml::xml_node<> *tile_node = root_node->first_node("tile"); tile_node; tile_node = tile_node->next_sibling()) {
                Tile tile;

                tile.id = (unsigned int) std::atoi(tile_node->first_attribute("id")->value());

                std::string tmp = tile_node->first_attribute("terrain")->value();
                std::stringstream ss(tmp);
                std::string tmpValue;

                while(std::getline(ss, tmpValue, ',')) {
                    tile.terrain.push_back((unsigned int) std::atoi(tmpValue.c_str()));
                }

                //parse tile properties
                if(tile_node->first_node("properties") != 0) {
                    for(rapidxml::xml_node<> *properties_node = tile_node->first_node("properties")->first_node("property"); properties_node; properties_node = properties_node->next_sibling()) {
                        tile.property[properties_node->first_attribute("name")->value()] = properties_node->first_attribute("value")->value();
                    }
                }

                tileset->tileList.push_back(tile);
            }
        }

        return tileset;
    }

    void Tileset::Print() const {
        std::cout << "Tileset Name: " << name << std::endl;
    }
}
