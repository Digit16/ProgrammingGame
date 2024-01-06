#include <vector>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "LoadOBJ.h"

bool loadOBJ(const std::string path, std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& outUVs, std::vector<glm::vec3>& outNormals) {

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec2> tempUvs;
    std::vector<glm::vec3> tempNormals;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Unable to load .obj file" << std::endl;
        return false;
    }

    std::string line;

    while (std::getline(file, line)) {
        if (line.substr(0, 2) == "v ") {
            std::istringstream iss(line.substr(2));
            glm::vec3 vertex;
            iss >> vertex[0] >> vertex[1] >> vertex[2];
            tempVertices.push_back(vertex);
        } else if (line.substr(0, 3) == "vt ") {
            std::istringstream iss(line.substr(3));
            glm::vec2 uv;
            iss >> uv[0] >> uv[1];
            tempUvs.push_back(uv);
        } else if (line.substr(0, 3) == "vn ") {
            std::istringstream iss(line.substr(3));
            glm::vec3 normal;
            iss >> normal[0] >> normal[1] >> normal[2];
            tempNormals.push_back(normal);
        } else if (line.substr(0, 2) == "f ") {
            unsigned int v[3], vt[3], vn[3];
            sscanf_s(line.c_str(), "f %i/%i/%i %i/%i/%i %i/%i/%i", &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2]);
            vertexIndices.push_back(v[0]);
            vertexIndices.push_back(v[1]);
            vertexIndices.push_back(v[2]);

            uvIndices.push_back(vt[0]);
            uvIndices.push_back(vt[1]);
            uvIndices.push_back(vt[2]);

            normalIndices.push_back(vn[0]);
            normalIndices.push_back(vn[1]);
            normalIndices.push_back(vn[2]);
        }
    }

    file.close();
    
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        outVertices.push_back(tempVertices[vertexIndices[i] - 1]);
    }

    for (unsigned int i = 0; i < uvIndices.size(); i++) {
        outUVs.push_back(tempUvs[uvIndices[i] - 1]);
    }

    for (unsigned int i = 0; i < normalIndices.size(); i++) {
        outNormals.push_back(tempNormals[normalIndices[i] - 1]);
    }

    return true;
}