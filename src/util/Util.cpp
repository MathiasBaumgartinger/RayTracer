#pragma once

#include <string>
#include "Vector3.cpp"
#include "Vector2.cpp"
#include "../pugixml/src/pugixml.cpp"

/*
* Utility features for Lab2a
*/
namespace Util 
{
    // From https://parkyo.github.io/categories/snake_to_camel/
    std::string snakeToCamel(std::string snake) 
    {
        std::string camel;
        bool cap = false;
        for(char c : snake){
            if(!cap && (int)c < 91){//if the char is cap when it shouldn't be
                camel.push_back(tolower(c)); 
            }else if(c == '_'){//if there is an underdash, the next char should be capitalized
                cap = true;
            }else if(cap){//if there should be a cap char convert the char to cap if it is not. Else, just push it back
                if((int)c > 97){
                    camel.push_back(toupper(c));
                }else{
                    camel.push_back(c);
                }
                cap = false;
            }else{
                camel.push_back(c);
            }
        }
        camel[0] = std::toupper(camel[0]);
        return camel;
    }

    // Creates a vector from the specified node with the identifiers
    Vector3 vec3FromXML(pugi::xml_node node, std::string x="x", std::string y="y", std::string z="z")
    {
        return Vector3(node.attribute(x.c_str()).as_double(), node.attribute(y.c_str()).as_double(), node.attribute(z.c_str()).as_double());
    }

    // Creates a vector from the specified node with the identifiers
    Vector2 vec2FromXML(pugi::xml_node node, std::string x="x", std::string y="y")
    {
        return Vector2(node.attribute(x.c_str()).as_double(), node.attribute(y.c_str()).as_double());
    }
}