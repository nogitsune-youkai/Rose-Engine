#pragma once

#include <string>
#include <iostream>

#include "shaderc/shaderc.hpp"
class ShaderCompiler
{
	std::string processShaderFile(const std::string & glslTextFilePath, shaderc_shader_kind shaderKind, std::string shaderFilePath);
};

