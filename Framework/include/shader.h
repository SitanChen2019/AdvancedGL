//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_SHADER_H
#define ADVANCEDGL_SHADER_H

#include <string>
#include <map>
#include <iostream>

#include "glad/glad.h"


class ShaderMgr;

class ShaderObj
{
public:
    GLuint Id() {
        return m_programId;
    }

private:
    GLuint m_programId = 0;

    ShaderObj(GLuint programId )
    {
        m_programId = programId;
    }

    void  Swap( ShaderObj& other )
    {
        std::swap( m_programId, other.m_programId);
    }

    friend  class ShaderMgr;


};


struct ShaderBluePrint
{
    std::string shaderName;
    std::string vertexShaderFilePath;
    std::string fragmentShaderFilePath;
    std::string geometryShaderFilePath;

    ShaderBluePrint(){}
    ShaderBluePrint( const std::string& _shaderName ,
                     const std::string& _vertexShaderFilePath,
                     const std::string& _fragmentShaderFilePath,
                     const std::string _geometryShaderFilePath = std::string()
            )
            :shaderName( _shaderName )
            ,vertexShaderFilePath( _vertexShaderFilePath )
            ,fragmentShaderFilePath(_fragmentShaderFilePath)
            ,geometryShaderFilePath(_geometryShaderFilePath)
    {}
};

struct ShaderCompileResult
{
    bool bSuccess = false;
    std::string errorMessage;
    GLuint program = 0;
};

struct ShaderTmpData
{
    GLuint vertexShader = 0;
    GLuint geometryShader = 0;
    GLuint fragShader = 0;
};



class ShaderMgr {
public:
    static  ShaderCompileResult buildShaderProgram(const char* vertexStr = nullptr, const char* fragStr = nullptr, const char* gemoStr = nullptr);

private:
    static  ShaderCompileResult clearBuildTmps(ShaderCompileResult& result, ShaderTmpData& tmp );
    static bool compileShader(GLuint shader, const char* source, std::string& errorMsg);

    static bool fileToString( std::string filePath, std::string& fileStr );

public:
    void init();
    void registerShader( const std::string&  name,
            const std::string& vertexShaderFilePath,
            const std::string& fragmentShaderFilePath,
            const std::string geometryShaderFilePath = "")
    {
        m_shaderFactory[name] = {name, vertexShaderFilePath, fragmentShaderFilePath, geometryShaderFilePath};
    }

    ShaderObj* getShader( const std::string& name  )
    {
        auto it1 = m_shaders.find( name );
        if( it1 != m_shaders.end() )
            return it1->second;

        ShaderCompileResult result = createShaderFromFile( name );
        if( result.bSuccess )
        {
            ShaderObj* shader = new ShaderObj(result.program );
            m_shaders.insert( std::make_pair(name, shader ) );

            return shader;
        }
        else
        {
            std::cout << result.errorMessage << std::endl;
        }

        return nullptr;
    }

    void reloadShader( std::string name )
    {
        auto it1 = m_shaders.find( name );
        if( it1 == m_shaders.end() )
            return;

        ShaderObj* oldShader = it1->second;
        ShaderCompileResult result = createShaderFromFile( name );
        if( result.bSuccess )
        {
            ShaderObj* shader = new ShaderObj(result.program );
            oldShader->Swap( *shader);
            glDeleteProgram( shader->Id() );
            delete shader;
            return ;
        }
        else
        {
            std::cout << result.errorMessage << std::endl;
        }
    }

private:
    ShaderCompileResult createShaderFromFile(const std::string name)
    {
        ShaderCompileResult ret;
        auto it2 = m_shaderFactory.find( name );
        if( it2 != m_shaderFactory.end() )
        {
            std::string vertexShaderSrc;
            if( !fileToString(it2->second.vertexShaderFilePath, vertexShaderSrc) )
            {
                ret.errorMessage = vertexShaderSrc;
                return ret;
            }

            std::string fragmentShaderSrc;
            if( !fileToString(it2->second.fragmentShaderFilePath, fragmentShaderSrc) )
            {
                ret.errorMessage = fragmentShaderSrc;
                return ret;
            }
            std::string geometryShaderSrc;
            if( !fileToString(it2->second.geometryShaderFilePath, geometryShaderSrc) )
            {
                ret.errorMessage = geometryShaderSrc;
                return ret;
            }

            ret = buildShaderProgram(vertexShaderSrc.empty() ? nullptr : vertexShaderSrc.c_str(),
                                     fragmentShaderSrc.empty() ? nullptr : fragmentShaderSrc.c_str(),
                                     geometryShaderSrc.empty() ? nullptr : geometryShaderSrc.c_str());

        } else{
            ret.bSuccess = 0;
            ret.program = 0;
            ret.errorMessage = std::string("Shader Program ") + name  + " does't exist." ;
        }
        return ret;
    }
private:
    std::map<std::string, ShaderBluePrint> m_shaderFactory;

    std::map<std::string, ShaderObj* > m_shaders;
};

#endif //ADVANCEDGL_SHADER_H
