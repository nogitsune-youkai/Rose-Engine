#pragma once

#include <cstring>
#include <string>
#include <iostream>
#include <filesystem>

#include "shaderc/shaderc.hpp"
class ShaderCompiler
{
public:
	
	std::string processShaderFile(const std::string& shaderSourceFile, shaderc_shader_kind shaderKind, const std::string & shaderName);
	std::string compileShaderToAssembly(const std::string& shaderSourceFile, shaderc_shader_kind shaderKind, const std::string& shaderName);
	std::vector<uint32_t> compileShader(const std::string& shaderSourceFile, shaderc_shader_kind shaderKind, const std::string& shaderName);



private:
	std::string readGlslShaderText(std::filesystem::path); // not implemented yet
	std::string defineShaderType(); // not implemented yet
	shaderc::Compiler compiler;
	shaderc::CompileOptions compileOptions;
};

