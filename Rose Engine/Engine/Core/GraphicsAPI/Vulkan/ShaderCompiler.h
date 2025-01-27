#pragma once

#include <string>
#include <iostream>
#include <filesystem>

#include "shaderc/shaderc.hpp"
class ShaderCompiler
{
public:
	
	std::string processShaderFile(const std::string & shaderName, shaderc_shader_kind shaderKind, std::string shaderSourceFile);
	std::string compileShaderToAssembly(const std::string& glslShaderText, shaderc_shader_kind shaderKind, std::string shaderSourcePath);



private:
	std::string readGlslShaderText(std::filesystem::path); // not implemented yet
	std::string defineShaderType(); // not implemented yet
	shaderc::Compiler compiler;
	shaderc::CompileOptions compileOptions;
};

