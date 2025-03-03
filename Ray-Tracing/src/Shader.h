#pragma once

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

#include "spdlog/stopwatch.h"

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath);
	Shader(unsigned int identification);
    Shader() = default;

    void use();

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setDouble(const std::string& name, double value) const;
    void setFloat(const std::string& name, float value) const;
    void setFloat2(const std::string& name, glm::vec2 value) const;
    void setFloat3(const std::string& name, glm::vec3 value) const;
	void setMat4(const std::string& name, glm::mat4 value) const;
};