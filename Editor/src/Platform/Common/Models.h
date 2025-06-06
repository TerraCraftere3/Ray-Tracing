#pragma once
#include <tiny_obj_loader.h>
#include <Rendering/Scene.h>
#include <Core/Log.h>
#include <tuple>
#include <unordered_map>

namespace std {
    template<>
    struct hash<std::tuple<int, int, int>> {
        size_t operator()(const std::tuple<int, int, int>& key) const {
            auto h1 = std::hash<int>{}(std::get<0>(key));
            auto h2 = std::hash<int>{}(std::get<1>(key));
            auto h3 = std::hash<int>{}(std::get<2>(key));
            // Combine the hashes
            return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
        }
    };
}

bool LoadOBJMesh(const std::string& filepath, Mesh& meshOut)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str());
	if (!warn.empty()) LOG_WARN("tinyobj Warning: {}", warn);
	if (!err.empty()) LOG_ERROR("tinyobj Error: {}", err);
    if (!ret) return false;

    meshOut.Vertices.clear();
    meshOut.Normals.clear();
    meshOut.UVs.clear();
    meshOut.Indices.clear();

    std::unordered_map<std::tuple<int, int, int>, uint32_t> uniqueVertices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            auto key = std::make_tuple(index.vertex_index, index.normal_index, index.texcoord_index);
            if (uniqueVertices.count(key) == 0) {
                glm::vec3 pos{ 0 }, norm{ 0 };
                glm::vec2 uv{ 0 };

                if (index.vertex_index >= 0) {
                    pos = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                }

                if (index.normal_index >= 0) {
                    norm = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                if (index.texcoord_index >= 0) {
                    uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1] // flip V
                    };
                }

                meshOut.Vertices.push_back(pos);
                meshOut.Normals.push_back(norm);
                meshOut.UVs.push_back(uv);

                uint32_t newIndex = (uint32_t)meshOut.Vertices.size() - 1;
                uniqueVertices[key] = newIndex;
                meshOut.Indices.push_back(newIndex);
            }
            else {
                meshOut.Indices.push_back(uniqueVertices[key]);
            }
        }
    }

    return true;
}
