#include "ShaderCompiler.h"

std::string ShaderCompiler::processShaderFile(const std::string& glslTextFilePath, shaderc_shader_kind shaderKind, std::string shaderFilePath)
{
	shaderc::Compiler compiler;
	shaderc::CompileOptions compileOptions;
	compileOptions.AddMacroDefinition("-o"); // add -o flag for compileOptions
	shaderc::PreprocessedSourceCompilationResult compilationResult = compiler.PreprocessGlsl(glslTextFilePath, shaderKind, shaderFilePath.c_str(), compileOptions);

	if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << compilationResult.GetErrorMessage();
		return "";
	}
	else {
		std::cerr << "Shader preprocessed successfully" << std::endl;
	}

	return { compilationResult.cbegin(), compilationResult.cend() };
}
