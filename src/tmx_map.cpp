#include "tmx_map.hpp"
#include "xml_utils.hpp"
#include <iostream>
#include <sstream>


namespace TMX {
    Map *parse(const char *filename) {
        Map* map = new Map();
        std::string dirname = TMXUtils::dirname(filename);

        rapidxml::file<> file(filename);
        rapidxml::xml_node<> *root_node;
        rapidxml::xml_document<> doc;
        doc.parse<0>(file.data());

        std::string version = VERSION;

        //get root nodes
        root_node = doc.first_node("map");

        //load map element
        if(root_node->first_attribute("version")->value() != version) {
            std::stringstream ss;
            ss << "ERROR: Map version mismatch. Required version: " << VERSION << "." << std::endl;
            throw ss.str();
        }

        map->version = root_node->first_attribute("version")->value();
        map->orientation = root_node->first_attribute("orientation")->value();
        map->background_color = TMXUtils::get_attribute(root_node, "backgroundcolor", "000000");
        map->width = (unsigned int) std::atoi(root_node->first_attribute("width")->value());
        map->height = (unsigned int) std::atoi(root_node->first_attribute("height")->value());
        map->tile_width = (unsigned int) std::atoi(root_node->first_attribute("tilewidth")->value());
        map->tile_height = (unsigned int) std::atoi(root_node->first_attribute("tileheight")->value());
        map->width_pixels = map->width * map->tile_width;
        map->height_pixels = map->height * map->tile_height;

        if(root_node->first_node("properties") != 0) {
            for(rapidxml::xml_node<> *properties_node = root_node->first_node("properties")->first_node("property");
                properties_node; properties_node = properties_node->next_sibling()) {
                map->property[properties_node->first_attribute("name")->value()] =
                        properties_node->first_attribute("value")->value();
            }
        }

        for(rapidxml::xml_node<> *tileset_node = root_node->first_node("tileset"); tileset_node;
            tileset_node = tileset_node->next_sibling("tileset"))
        {
            std::stringstream path;
            path << dirname << tileset_node->first_attribute("source")->value();

            TSX::Tileset* tileset = TSX::parse(path.str().c_str());
            map->tilesets.push_back(tileset);
        }

        // Create an above TileLayer to render some tiles after entities
        map->AddAboveLayer();

        for(rapidxml::xml_node<> *layer_node = root_node->first_node("layer"); layer_node;
            layer_node = layer_node->next_sibling("layer"))
        {
            TileLayer* layer = new TileLayer();
            layer->name = layer_node->first_attribute("name")->value();
            layer->width = (unsigned int) atoi(layer_node->first_attribute("width")->value());
            layer->height = (unsigned int) atoi(layer_node->first_attribute("height")->value());

            // CSV only
            rapidxml::xml_node<> *data_node = layer_node->first_node("data");

            if(data_node->first_attribute("encoding")->value() != std::string("csv"))
                throw "Only CSV encoding supported en TMX maps.";

            layer->tiles = std::vector<std::vector<int>>(layer->height, std::vector<int>(layer->width, 0));

            // We only support one tileset per map
            TSX::Tileset* tileset = map->tilesets[0];

            std::stringstream data(data_node->value());
            int tile_id;
            char comma;

            for(int i = 0; i < layer->height; i++) {
                for(int j = 0; j < layer->width; j++) {
                    if(i != 0 || j != 0)
                        data >> comma;

                    data >> tile_id;

                    if(tileset->tiles[tile_id-1].Property("above") == "true") {
                        int layer_index = 0;

                        while(layer_index < map->tile_layers_above.size() &&
                                map->tile_layers_above[layer_index]->tiles[i][j] != 0)
                            layer_index++;

                        if(layer_index == map->tile_layers_above.size())
                            map->AddAboveLayer();

                        map->tile_layers_above[layer_index]->tiles[i][j] = tile_id;
                    } else {
                        layer->tiles[i][j] = tile_id;
                    }
                }
            }

            map->tile_layers_below.push_back(layer);
        }

        for(rapidxml::xml_node<> *oGroup_node = root_node->first_node("objectgroup"); oGroup_node;
            oGroup_node = oGroup_node->next_sibling("objectgroup"))
        {
            ObjectGroup oGroup;

            oGroup.name = oGroup_node->first_attribute("name")->value();
            oGroup.color = TMXUtils::get_attribute(oGroup_node, "color", "000000");
            oGroup.opacity = std::atof(TMXUtils::get_attribute(oGroup_node, "opacity", "1.0"));
            oGroup.visible = (bool) std::atoi(TMXUtils::get_attribute(oGroup_node, "visible", "1"));

            if(oGroup_node->first_node("properties") != 0) {
                for(rapidxml::xml_node<> *properties_node = oGroup_node->first_node("properties")->first_node("property"); properties_node; properties_node = properties_node->next_sibling()) {
                    oGroup.property[properties_node->first_attribute("name")->value()] = properties_node->first_attribute("value")->value();
                }
            }

            int unknown_count = 1;
            for(rapidxml::xml_node<> *object_node = oGroup_node->first_node("object"); object_node;
                object_node = object_node->next_sibling()) {
                Object object;
                object.name = TMXUtils::get_attribute(object_node, "name", "");
                object.type = TMXUtils::get_attribute(object_node, "type", "");
                object.gid = (unsigned int) atoi(TMXUtils::get_attribute(object_node, "gid", "0"));
                object.width = (unsigned int) atoi(TMXUtils::get_attribute(object_node, "width", "0"));
                object.height = (unsigned int) atoi(TMXUtils::get_attribute(object_node, "height", "0"));
                object.x = atoi(object_node->first_attribute("x")->value());
                object.y = atoi(object_node->first_attribute("y")->value());

                if(object.name.empty()) {
                    std::stringstream name;
                    name << "unknown_" << unknown_count;
                    unknown_count++;
                    object.name = name.str();
                }

                if(object_node->first_node("properties") != 0) {
                    for(rapidxml::xml_node<> *properties_node = object_node->first_node("properties")->first_node("property"); properties_node; properties_node = properties_node->next_sibling()) {
                        object.property[properties_node->first_attribute("name")->value()] = properties_node->first_attribute("value")->value();
                    }
                }

                oGroup.objects[object.name] = object;
            }

            map->object_groups[oGroup.name] = oGroup;
        }

        for(rapidxml::xml_node<> *image_node = root_node->first_node("imagelayer"); image_node;
            image_node = image_node->next_sibling("imagelayer"))
        {
            ImageLayer imgLayer;
            imgLayer.name = image_node->first_attribute("name")->value();
            imgLayer.opacity = std::atof(TMXUtils::get_attribute(image_node, "opacity", "1"));
            imgLayer.visible = (bool) std::atoi(image_node->first_attribute("visible")->value());
            imgLayer.image.source = image_node->first_node("image")->first_attribute("source")->value();

            if(image_node->first_node("image")->first_attribute("trans") != 0) {
                imgLayer.image.transparency_color = image_node->first_node("image")->first_attribute("trans")->value();
            }

            if(image_node->first_node("properties") != 0) {
                for(rapidxml::xml_node<> *properties_node = image_node->first_node("properties")->first_node("property"); properties_node; properties_node = properties_node->next_sibling("property")) {
                    imgLayer.property[properties_node->first_attribute("name")->value()] = properties_node->first_attribute("value")->value();
                }
            }

            map->image_layers[imgLayer.name] = imgLayer;
        }

        return map;
    }

    void Map::AddAboveLayer() {
        TileLayer* layer_above = new TileLayer();
        layer_above->name = "Above Layer";
        layer_above->width = width;
        layer_above->height = height;
        layer_above->tiles = std::vector<std::vector<int>>(height, std::vector<int>(width, 0));

        tile_layers_above.push_back(layer_above);
    }

    void Map::Print() const {
        std::cout << "Version: " << version << std::endl;
        std::cout << "Orientation: " << orientation << std::endl;
        std::cout << "Width: " << width << std::endl;
        std::cout << "Height: " << height << std::endl;
        std::cout << "Tile Width: " << tile_width << std::endl;
        std::cout << "Tile Height: " << tile_height << std::endl;
        std::cout << "Background Color: " << background_color << std::endl;

        std::cout << "Properties: " << std::endl;
        for(std::map<std::string, std::string>::const_iterator it = property.begin(); it != property.end(); ++it) {
            std::cout << "-> " << it->first << " : " << it->second << std::endl;
        }

        std::cout << std::endl;

        for(const TSX::Tileset* tileset : tilesets) {
            tileset->Print();
        }

        for(const TileLayer* layer : tile_layers_below) {
            layer->Print();
        }

        for(const TileLayer* layer : tile_layers_above) {
            layer->Print();
        }

        for(const auto &name_ogroup : object_groups) {
            const ObjectGroup &oGroup = name_ogroup.second;

            std::cout << "Object Group Name: " << oGroup.name << std::endl;
            std::cout << "Object Group Color: " << oGroup.color << std::endl;
            std::cout << "Object Group Opacity: " << oGroup.opacity << std::endl;
            std::cout << "Object Group Visible: " << oGroup.visible << std::endl;

            std::cout << "Properties: " << std::endl;
            for(const auto &property : oGroup.property) {
                std::cout << "-> " << property.first << " : " << property.second << std::endl;
            }

            std::cout << "Objects:" << std::endl;
            for(const auto& object : oGroup.objects) {
                std::cout << object.second.name << " (" << object.second.type << ")" << std::endl;
            }

            std::cout << std::endl;
        }

        for(const auto &name_imglayer : image_layers) {
            const ImageLayer& imgLayer = name_imglayer.second;

            std::cout << "Image Layer Name: " << imgLayer.name << std::endl;
            std::cout << "Image Layer Opacity: " << imgLayer.opacity << std::endl;
            std::cout << "Image Layer Visibility: " << imgLayer.visible << std::endl;
            std::cout << "Image Layer Source: " << imgLayer.visible << std::endl;
            std::cout << "Image Layer Transparent Color: " << imgLayer.image.transparency_color << std::endl;

            std::cout << "Properties: " << std::endl;
            for(const auto& property : imgLayer.property) {
                std::cout << "-> " << property.first << " : " << property.second << std::endl;
            }
        }
    }

    void TileLayer::Print() const {
        std::cout << std::endl << "Layer Name: " << name << std::endl;

        std::cout << "Layer Data Below: " << std::endl;
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                if(j != 0)
                    std::cout << ' ';

                std::cout << tiles[i][j];
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    Map::~Map() {
        for(TSX::Tileset* tileset : tilesets)
            delete tileset;

        for(TileLayer* layer : tile_layers_below)
            delete layer;

        for(TileLayer* layer : tile_layers_above)
            delete layer;
    }
}
