#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <string>

bool loadOBJ(const std::string path, std::vector<glm::vec3>& out_vertices, std::vector<glm::vec2>& out_uvs, std::vector<glm::vec3>& out_normals); // 