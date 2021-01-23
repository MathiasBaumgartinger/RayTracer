#pragma once

#include "Vector3.cpp"
#include "Vector2.cpp"
#include "vector"
#include "fstream"
#include <map>
#include <string>
#include <functional>
#include <tuple>
#include "../scene/nodes/Mesh.cpp"

class Mesh;

class ObjImporter 
{
    std::vector<Vector3> vBuffer;
    std::vector<Vector3> vnBuffer;
    std::vector<Vector3> vtBuffer;

    std::map<std::string, std::vector<Vector3>> buffers;

    std::fstream objFile;

    ObjImporter()
    {
        buffers["v"] = vBuffer;
        buffers["vn"] = vnBuffer;
        buffers["vt"] = vtBuffer;
    }

    void import(std::string path, Mesh& mesh)
    {
        objFile.open(path);
        std::string line;

        if (objFile.is_open())
        {
            while (std::getline(objFile, line))
            {
                parseLine(line, mesh);
            }
        }

        objFile.close();
    }

    // Find prefix and parse the rest of the line
    void parseLine(std::string line, Mesh& mesh) {
        std::string prefix = line.substr(0, 2);
        prefix = prefix.replace(prefix.begin(), prefix.end(), " ", "");

        line = line.substr(prefix.length() + 1);
        

        // For each prefix we declare a different lambda
        if (prefix == "f")
        {
            // Since the format of f is kinda random we need to find the callbacks in an additional step 
            for (int i = 0; i < 2; i++)
            {
                line = line.substr(0, line.find(' '));
                mesh.vBuffer.push_back(vBuffer[std::stoi(line.substr(0, line.find("/"))) - 1]);
                mesh.vtBuffer.push_back(vtBuffer[std::stoi(line.substr(line.find("/") + 1, line.find_last_of("/")))  - 1]);
                mesh.vnBuffer.push_back(vnBuffer[std::stoi(line.substr(line.find_last_of("/") + 1)) - 1]);
            }
            mesh.vBuffer.push_back(vBuffer[std::stoi(line.substr(0, line.find("/"))) - 1]);
            mesh.vtBuffer.push_back(vtBuffer[std::stoi(line.substr(line.find("/") + 1, line.find_last_of("/")))  - 1]);
            mesh.vnBuffer.push_back(vnBuffer[std::stoi(line.substr(line.find_last_of("/") + 1)) - 1]);
        }
        else if (prefix == "vn")
        {
            saveDataInBuffer(prefix, line,
                [](std::string line) { return std::stoi(line.substr(0, line.find(' '))); },
                [](std::string line) { return std::stoi(line.substr(0)); }
            );
        }
        else if(prefix == "v")
        {
            saveDataInBuffer(prefix, line,
                [](std::string line) { return std::stoi(line.substr(0, line.find(' '))); },
                [](std::string line) { return std::stoi(line.substr(0)); }
            );
        }
        else if(prefix == "vt")
        {
            saveDataInBuffer(prefix, line,
                [](std::string line) { return std::stoi(line.substr(0, line.find(' '))); },
                [](std::string line) { return std::stoi(line.substr(0)); }
            );
        }
    }

    // The lamda declared above will then be utlizied in the line, pushing one or possible three (f v1/vt1/vn1) coordinates back
    void saveDataInBuffer(std::string prefix, std::string line, 
        std::function<int (std::string)> callBackStandard, 
        std::function<int (std::string)> callBackLast) 
    {
        Vector3 newEntry(0, 0, 0);
        for (int i = 0; line != ""; i++) {
            if (line.find(' ') == std::string::npos) 
            {
                auto coordinate_s = callBackLast(line);
                newEntry[i] = coordinate_s;
                
                buffers[prefix].push_back(newEntry);
                return;
            }
            auto coordinate_s = callBackStandard(line);
            newEntry[i] = coordinate_s;

            line = line.substr(line.find(' ') + 1);
        }
    }
};