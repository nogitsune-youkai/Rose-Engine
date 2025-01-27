#include "ShaderCompiler.h"

#ifdef NDEBUG
constexpr bool enableDebugFacilities = false;
#else
constexpr bool enableDebugFacilities = true;
#endif

std::string ShaderCompiler::processShaderFile(const std::string& shaderName, shaderc_shader_kind shaderKind, std::string shaderSourceFile)
{
	
	//compileOptions.AddMacroDefinition("Preprocess", "-E"); // add -o flag for compileOptions
	shaderc::PreprocessedSourceCompilationResult compilationResult = compiler.PreprocessGlsl(shaderName, shaderKind, shaderSourceFile.c_str(), compileOptions);

	if (enableDebugFacilities) {
		if (shaderName.empty()) {
			std::cerr << "Shader preprocessing failed" << std::endl;
		}
		else {
			std::cerr << "Shader preprocessed successfully" << std::endl << shaderSourceFile << std::endl;
		}
	}
	return { compilationResult.cbegin(), compilationResult.cend() };
}

std::string ShaderCompiler::compileShaderToAssembly(const std::string& glslShaderText, shaderc_shader_kind shaderKind, std::string shaderSourcePath)
{
	
	if (enableDebugFacilities) {
		compileOptions.SetOptimizationLevel(shaderc_optimization_level_zero);
	}
	else {
		compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);
	}

	shaderc::AssemblyCompilationResult assemblyCompilationResult = compiler.CompileGlslToSpvAssembly(glslShaderText, shaderKind, shaderSourcePath.c_str(), compileOptions);

	if (assemblyCompilationResult.GetCompilationStatus() != shaderc_compilation_status_success) {
		std::cerr << assemblyCompilationResult.GetErrorMessage();
		return "";
	}

	return { assemblyCompilationResult.cbegin(), assemblyCompilationResult.cend() };
}

std::string ShaderCompiler::readGlslShaderText(std::filesystem::path)
{
	return std::string();
}
