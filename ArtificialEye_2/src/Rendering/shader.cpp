#include "shader.hpp"

#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>

using namespace std;

ee::Shader::Shader(Shader&& shader) :
    m_programID(shader.m_programID)
{
    shader.m_programID = 0;
}

ee::Shader::Shader() :
    m_programID(0)
{
}

void ee::Shader::assignTexture(const std::string name, const int textureUnit)
{
    int unit = textureUnit - GL_TEXTURE0;
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), unit);
}

void ee::Shader::assignMat4f(const std::string name, const glm::mat4& mat)
{
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void ee::Shader::assignBool(const std::string name, const bool value)
{
    glUniform1i(glGetUniformLocation(m_programID, name.c_str()), value);
}

void ee::Shader::assignColor(const std::string name, const Color3 vec)
{
    glUniform3f(glGetUniformLocation(m_programID, name.c_str()), vec.x, vec.y, vec.z);
}

void ee::Shader::assignColor(const std::string name, const Color4 vec)
{
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    glUniform4f(loc, vec.x, vec.y, vec.z, vec.w);
}

void ee::Shader::assignVec3(std::string name, Vector3 vec)
{
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    glUniform3f(loc, vec.x, vec.y, vec.z);
}

bool ee::Shader::initialize(const std::string& vtxPath, const std::string& frgPath)
{
	std::ifstream vtxStream, frgStream;
	std::stringstream vtxCode, frgCode;
	vtxStream.exceptions(std::ios_base::badbit | std::ios_base::failbit);
	frgStream.exceptions(std::ios_base::badbit | std::ios_base::failbit);

	// open and load code into stream
	try
	{
		vtxStream.open(vtxPath);
		frgStream.open(frgPath);

		vtxCode << vtxStream.rdbuf();
		frgCode << frgStream.rdbuf();

		vtxStream.close();
		frgStream.close();
	}
	catch (std::ifstream::failure excp)
	{
        return false;
	}

	GLuint vtxID, frgID;
	GLint resultChk;
	GLchar resultLog[512];

	vtxID = glCreateShader(GL_VERTEX_SHADER);
	frgID = glCreateShader(GL_FRAGMENT_SHADER);
	
	std::string strTmpCode = std::move(vtxCode.str());
	const GLchar *ptrCode = strTmpCode.c_str();
	glShaderSource(vtxID, 1, &ptrCode, nullptr);
	glCompileShader(vtxID);

	glGetShaderiv(vtxID, GL_COMPILE_STATUS, &resultChk);
	if (!resultChk)
	{
		glGetShaderInfoLog(vtxID, 512, nullptr, resultLog);
		std::cout << resultLog << endl;
        return false;
	}

	strTmpCode = std::move(frgCode.str());
	ptrCode = strTmpCode.c_str();
	glShaderSource(frgID, 1, &ptrCode, nullptr);
	glCompileShader(frgID);

	glGetShaderiv(frgID, GL_COMPILE_STATUS, &resultChk);
	if (!resultChk)
	{
		glGetShaderInfoLog(frgID, 512, nullptr, resultLog);
		std::cout << resultLog << endl;
        return false;
	}

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vtxID);
	glAttachShader(m_programID, frgID);

	glLinkProgram(m_programID);
	glGetProgramiv(m_programID, GL_LINK_STATUS, &resultChk);
	if (!resultChk)
	{
		glGetProgramInfoLog(m_programID, 512, nullptr, resultLog);
        std::cout << resultLog << endl;
        return false;
	}

	glDeleteShader(vtxID);
	glDeleteShader(frgID);

    return true;
}

ee::Shader::~Shader()
{
	glDeleteProgram(m_programID);
}

void ee::Shader::use() const
{
	glUseProgram(m_programID);
}