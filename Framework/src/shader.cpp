
#include "shader.h"
#include <sstream>
#include <fstream>
#include "global.h"

bool ShaderMgr::fileToString( std::string filePath , std::string& fileStr )
{
    if( filePath.empty() )
    {
        fileStr = filePath;
        return true;
    }


    std::ifstream infile( filePath );
    if( infile.fail() )
    {
        fileStr = "Error: Failded to read "+filePath;
        return false;
    }
    std::stringstream buffer;
    buffer << infile.rdbuf();
    infile.close();

    fileStr = buffer.str();

    return true;
}

bool ShaderMgr::compileShader( GLuint shader, const char* source, std::string& errorMsg)
{
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        char buffer[512];
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        errorMsg = std::string(buffer);
        return false;
    }
    else
    {
        return true;
    }
}

ShaderCompileResult ShaderMgr::buildShaderProgram(
        const char* vertexStr /*= nullptr*/, const char* fragStr/* = nullptr*/, const char* gemoStr /*= nullptr*/)
{
    ShaderCompileResult ret;
    ShaderTmpData tmp;
    if (!vertexStr && !fragStr && !gemoStr)
        return ret;

    ret.bSuccess = true;

    if (vertexStr)
    {
        tmp.vertexShader = glCreateShader(GL_VERTEX_SHADER);
        ret.bSuccess &= compileShader( tmp.vertexShader, vertexStr, ret.errorMessage);
        if( !ret.bSuccess )
            ret.errorMessage = "Error: Failed to build vertex shader.\n" + ret.errorMessage + "\nCode:\n" +vertexStr;

        if (!ret.bSuccess)
            return clearBuildTmps( ret, tmp);
    }

    if (fragStr)
    {
        tmp.fragShader = glCreateShader(GL_FRAGMENT_SHADER);
        ret.bSuccess &= compileShader( tmp.fragShader, fragStr, ret.errorMessage);
        if( !ret.bSuccess )
            ret.errorMessage = "Error: Failed to build fragment shader.\n" + ret.errorMessage + "\nCode:\n" +fragStr;

        if (!ret.bSuccess)
            return clearBuildTmps( ret,tmp);

    }

    if (gemoStr)
    {
        tmp.geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        ret.bSuccess &= compileShader( tmp.geometryShader, gemoStr, ret.errorMessage);

        if( !ret.bSuccess )
            ret.errorMessage = "Error: Failed to build geometry shader.\n" + ret.errorMessage + "\nCode:\n" + gemoStr;

        if (!ret.bSuccess)
            return clearBuildTmps( ret,tmp);
    }

    ret.program = glCreateProgram();
    if (tmp.vertexShader)
        glAttachShader(ret.program, tmp.vertexShader);
    if (tmp.geometryShader)
        glAttachShader(ret.program, tmp.geometryShader);
    if (tmp.fragShader)
        glAttachShader(ret.program, tmp.fragShader);



    glLinkProgram(ret.program);
    GLint status;
    glGetProgramiv(ret.program, GL_LINK_STATUS, &status);
    if (!status)
    {
        char buffer[512];
        glGetProgramInfoLog(ret.program, 512, NULL, buffer);
        ret.errorMessage = "Error: Failed to link shader.\n"  + std::string(buffer)
                + vertexStr + "\n" + fragStr + "\n" + gemoStr + "\n";
        ret.bSuccess = false;

        return clearBuildTmps( ret,tmp);
    }

    return clearBuildTmps(ret,tmp);
}

ShaderCompileResult ShaderMgr::clearBuildTmps(ShaderCompileResult& result, ShaderTmpData& tmp)
{
    if (tmp.vertexShader)
        glDeleteShader(tmp.vertexShader);

    if (tmp.geometryShader)
        glDeleteShader(tmp.geometryShader);

    if (tmp.fragShader)
        glDeleteShader(tmp.fragShader);

    tmp.vertexShader = 0;
    tmp.geometryShader = 0;
    tmp.fragShader = 0;

    if( !result.bSuccess )
    {
        glDeleteProgram( result.program );
        result.program = 0;
    }
    
    return result;
}

void ShaderMgr::init() {
    registerShader("simple",
       Global::SystemResPath("/shader/simple.vs"), Global::SystemResPath("/shader/simple.fs"));
    registerShader("triangle",
           Global::SystemResPath("/shader/triangle/triangle.vs"),
            Global::SystemResPath("/shader/triangle/triangle.fs"),
           Global::SystemResPath("/shader/triangle/triangle.gs"));

    registerShader("tri_vn_pc",
                   Global::SystemResPath("/shader/tri_vn_pc/tri_vn_pc.vs"),
                   Global::SystemResPath("/shader/tri_vn_pc/tri_vn_pc.fs"));

    registerShader("shadow_depth",
                   Global::SystemResPath("/shader/shadow_depth/shadow_depth.vs"),
                   Global::SystemResPath("/shader/shadow_depth/shadow_depth.fs"));

    registerShader("tri_vn_shadow",
                   Global::SystemResPath("/shader/tri_vn_shadow/tri_vn_shadow.vs"),
                   Global::SystemResPath("/shader/tri_vn_shadow/tri_vn_shadow.fs"));

    registerShader("simple_bone",
                   Global::SystemResPath("/shader/simple_bone.vs"), Global::SystemResPath("/shader/tri_vn_pc/tri_vn_pc.fs"));
}
