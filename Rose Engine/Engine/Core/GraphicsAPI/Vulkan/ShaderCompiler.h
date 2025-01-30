#pragma once

#include <cstring>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "shaderc/shaderc.hpp"
class ShaderCompiler
{
public:
	
	//const char readGlslShaderText(); // not implemented yet
	std::vector<uint32_t> compileShader(const std::string& shaderSourceFile, shaderc_shader_kind shaderKind, const std::string& shaderName);
	static std::vector<char> readFile(const std::string& filename);



private:
	std::string processShaderFile(const std::string& shaderSourceFile, shaderc_shader_kind shaderKind, const std::string& shaderName);
	std::string compileShaderToAssembly(const std::string& shaderSourceFile, shaderc_shader_kind shaderKind, const std::string& shaderName);
	
	std::string defineShaderType(); // not implemented yet
	shaderc::Compiler compiler;
	shaderc::CompileOptions compileOptions;
};

