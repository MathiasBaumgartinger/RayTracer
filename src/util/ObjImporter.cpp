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
public:
    ObjImporter()
    {
        buffers["v"] = &vBuffer;
        buffers["vn"] = &vnBuffer;
        buffers["vt"] = &vtBuffer;
    }

    void import(std::string path)
    {
        objFile.open(path);
        std::string line;

        if (objFile.is_open())
        {
            while (std::getline(objFile, line))
            {
                //std::cout << line<< std::endl;
                parseLine(line);
            }
        }

        objFile.close();
    }

    std::vector<Vector3> vBufferOrdered;
    std::vector<Vector3> vnBufferOrdered;
    std::vector<Vector3> vtBufferOrdered;

private:
    // Find prefix and parse the rest of the line
    void parseLine(std::string line) {
        std::string prefix = line.substr(0, 2);
        prefix.erase(remove(prefix.begin(), prefix.end(), ' '), prefix.end());
        
        line = line.substr(prefix.length() + 1);

        // For each prefix we declare a different lambda
        if (prefix == "f")
        {
            //std::cout << "While in f" << std::endl;
            
            // Since the format of f is kinda random we need to find the callbacks in an additional step 
            for (int i = 0; i < 2; i++)
            {
                std::string tmp = line.substr(0, line.find(' '));
                //std::cout << "pushing back to v: " << std::stoi(tmp.substr(0, tmp.find("/"))) - 1 << std::endl;
                vBufferOrdered.push_back(vBuffer[std::stoi(tmp.substr(0, tmp.find("/"))) - 1]);
                //std::cout <<  "pushing back to vt: " << std::stoi(tmp.substr(tmp.find("/") + 1, tmp.find_last_of("/")))  - 1 << std::endl;
                vtBufferOrdered.push_back(vtBuffer[std::stoi(tmp.substr(tmp.find("/") + 1, tmp.find_last_of("/")))  - 1]);
                //std::cout <<  "pushing back to vn: " << std::stoi(tmp.substr(tmp.find_last_of("/") + 1)) - 1 << std::endl;
                vnBufferOrdered.push_back(vnBuffer[std::stoi(tmp.substr(tmp.find_last_of("/") + 1)) - 1]);

                line = line.substr(line.find(' ') + 1);
                //std::cout << line << std::endl;
            }
            
            vBufferOrdered.push_back(vBuffer[std::stoi(line.substr(0, line.find("/"))) - 1]);
            vtBufferOrdered.push_back(vtBuffer[std::stoi(line.substr(line.find("/") + 1, line.find_last_of("/")))  - 1]);
            vnBufferOrdered.push_back(vnBuffer[std::stoi(line.substr(line.find_last_of("/") + 1)) - 1]);
        }
        else if (prefix == "vn")
        {
            //std::cout << "While in vn" << std::endl;
            saveDataInBuffer(prefix, line,
                [](std::string line) { return std::stod(line.substr(0, line.find(' '))); },
                [](std::string line) { return std::stod(line.substr(0)); }
            );
        }
        else if(prefix == "v")
        {
            //std::cout << "While in v" << std::endl;
            saveDataInBuffer(prefix, line,
                [](std::string line) { return std::stod(line.substr(0, line.find(' '))); },
                [](std::string line) { return std::stod(line.substr(0)); }
            );
        }
        else if(prefix == "vt")
        {
            //std::cout << "While in vt" << std::endl;
            saveDataInBuffer(prefix, line,
                [](std::string line) { return std::stod(line.substr(0, line.find(' '))); },
                [](std::string line) { return std::stod(line.substr(0)); }
            );
        }
    }

    // The lamda declared above will then be utlizied in the line, pushing one or possible three (f v1/vt1/vn1) coordinates back
    void saveDataInBuffer(std::string prefix, std::string line, 
        std::function<double (std::string)> callBackStandard, 
        std::function<double (std::string)> callBackLast) 
    {
        Vector3 newEntry(0, 0, 0);
        for (int i = 0; line != ""; i++) {
            if (line.find(' ') == std::string::npos) 
            {
                auto coordinate_s = callBackLast(line);
                newEntry[i] = coordinate_s;
                
                buffers[prefix]->push_back(newEntry);
                //std::cout << newEntry << std::endl;
                return;
            }
            auto coordinate_s = callBackStandard(line);
            newEntry[i] = coordinate_s;

            line = line.substr(line.find(' ') + 1);
        }
    }

    std::vector<Vector3> vBuffer;
    std::vector<Vector3> vnBuffer;
    std::vector<Vector3> vtBuffer;

    std::map<std::string, std::vector<Vector3>*> buffers;

    std::fstream objFile;
};