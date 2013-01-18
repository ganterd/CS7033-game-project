#include "Shader.h"

Shader::Shader(const char* vertFilePath, const char* fragFilePath){
	init(vertFilePath, fragFilePath);
}

void Shader::init(const char* vertFilePath, const char* fragFilePath){
	const char* vertFile = readFile(vertFilePath);
	const char* fragFile = readFile(fragFilePath);

	if(!vertFile || !fragFile){
		std::cout << "[Shader] Could not initialise shaders..." << std::endl;
		return;
	}

	uiShaderVertP = glCreateShader(GL_VERTEX_SHADER);
	uiShaderFragP = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(uiShaderVertP, 1, &vertFile, 0);
	glShaderSource(uiShaderFragP, 1, &fragFile, 0);

	glCompileShader(uiShaderVertP);
	glCompileShader(uiShaderFragP);

	uiShaderID = glCreateProgram();

	glAttachShader(uiShaderID, uiShaderVertP);
	glAttachShader(uiShaderID, uiShaderFragP);
	glLinkProgram(uiShaderID);
}

void Shader::bind(){
	glUseProgram(uiShaderID);
}

void Shader::unbind(){
	glUseProgram(0);
}

const char* Shader::readFile(const char* filePath){
    char* text;
    
    FILE *file;
	fopen_s(&file, filePath, "rt");
        
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        int count = ftell(file);
        rewind(file);

             
        if (count > 0) {
            text = (char*)malloc(sizeof(char) * (count + 1));
            count = fread(text, sizeof(char), count, file);
            text[count] = '\0';
        }
        fclose(file);
    }
    return text;
}