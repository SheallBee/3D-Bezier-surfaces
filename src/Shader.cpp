#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Shader.h"

#include "Renderer.h"

Shader::Shader(const std::string& filepath) : m_FilePath(filepath), m_RendererId(0)
{
    ShaderProgramSource sources = ParseShader(filepath);
    m_RendererId = CreateShader(sources.VertexSource, sources.FragmentSource);

}
Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererId));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{

    enum class shaderType
    {
        None = -1, Vertex = 0, Fragment = 1
    };

    shaderType type = shaderType::None;

    std::ifstream stream(filepath);
    std::string line;

    std::stringstream ss[2];

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = shaderType::Vertex;
            else if (line.find("fragment") != std::string::npos)
                type = shaderType::Fragment;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::string t = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT OR NON-VERTEX";
        std::cout << "Failed " << t << " shader compilation" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererId));
}
void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}
void Shader::SetUniform1iv(const std::string& name, int* value)
{
    GLCall(glUniform1iv(GetUniformLocation(name), 2, value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &(matrix[0][0])));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLCall(int location = glGetUniformLocation(m_RendererId, name.c_str()));
    if (location == -1)
        std::cout << "Warning: uniform " << name << " doesn't exist" << std::endl;
    m_UniformLocationCache[name] = location;
    return location;
}