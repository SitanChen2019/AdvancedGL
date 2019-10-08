//
// Created by Sitan Chen on 2019/9/30.
//

#include "BesizerRenderable.h"
#include "shader.h"
#include "global.h"

void BesizerRenderable::attach() {
    m_BezierProgram =  glCreateProgram();
    std::string error_msg;

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    std::string vs_str;
    if( !ShaderMgr::fileToString("../Demos/TessellationShader/shader/Bezier.vs", vs_str) )
        return;
    if( ! ShaderMgr::compileShader(vs, vs_str.c_str(), error_msg ) )
    {
        std::cout << error_msg << std::endl;
        return;
    }

    GLuint tcs = glCreateShader(GL_TESS_CONTROL_SHADER);
    std::string tcs_str ;
    if( !ShaderMgr::fileToString("../Demos/TessellationShader/shader/Bezier.tcs", tcs_str)  )
    {
        return;
    }
    if( ! ShaderMgr::compileShader(tcs, tcs_str.c_str(), error_msg ) )
    {
        std::cout << error_msg << std::endl;
        return;
    }


    GLuint tes = glCreateShader(GL_TESS_EVALUATION_SHADER);
    std::string tes_str;
    if( !ShaderMgr::fileToString("../Demos/TessellationShader/shader/Bezier.tes", tes_str) )
    {
        return;
    }
    if( ! ShaderMgr::compileShader(tes, tes_str.c_str(), error_msg ) )
    {
        std::cout << error_msg << std::endl;
        return;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fs_str;
    if( !ShaderMgr::fileToString("../Demos/TessellationShader/shader/Bezier.fs", fs_str) )
    {
        return;
    }
    if( ! ShaderMgr::compileShader(fs, fs_str.c_str(), error_msg ) )
    {
        std::cout << error_msg << std::endl;
        return;
    }

    glAttachShader( m_BezierProgram, vs );
    //glAttachShader( m_BezierProgram, tcs );
    //glAttachShader( m_BezierProgram, tes );
    glAttachShader( m_BezierProgram, fs );

    glLinkProgram(m_BezierProgram);
    GLint status;
    glGetProgramiv(m_BezierProgram, GL_LINK_STATUS, &status);
    if (!status)
    {
        char buffer[512];
        glGetProgramInfoLog(m_BezierProgram, 512, NULL, buffer);
        error_msg = "Error: Failed to link shader.\n" ;
        std::cout << error_msg << std::endl;
        return;
    }

    glDeleteShader( tcs);
    glDeleteShader( tes);
    glDeleteShader( vs);
    glDeleteShader( fs);


    glGenVertexArrays(1,&m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    Vector<Vec3> controlsPoints;
    controlsPoints.push_back( Vec3(-1,0,0));
    controlsPoints.push_back( Vec3(0,1,0));
    controlsPoints.push_back( Vec3(1,1,0));
    controlsPoints.push_back( Vec3(2,0,0));

    Vector<unsigned> indices;
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(1);

    for( auto& vertex : controlsPoints)
        m_box.merge(vertex);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData( GL_ARRAY_BUFFER, sizeof(Vec3)*controlsPoints.size(), controlsPoints.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(), indices.data(), GL_STATIC_DRAW);
    m_indicesCount = indices.size();

    glBindVertexArray(0);

    Global::cameraControl().fitBox( m_box );
    std::cout << " OK " << std::endl;
    return;
}

void BesizerRenderable::render() {
    glUseProgram(m_BezierProgram);
    int attrLoc;

    Matrix4  viewMat = Global::renderWindow().getViewMatrix();

    attrLoc =  glGetUniformLocation(m_BezierProgram, "view");
    glUniformMatrix4fv(attrLoc, 1, GL_FALSE ,(float*) &viewMat);

    Matrix4  projMat = Global::renderWindow().getProjMatrix();
    attrLoc =  glGetUniformLocation(m_BezierProgram, "projection");
    glUniformMatrix4fv( attrLoc, 1, GL_FALSE ,(float*) &projMat);

    attrLoc = glGetUniformLocation(m_BezierProgram,"ourColor");
    glUniform3f( attrLoc,1,1,1 );

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo );

    //glPatchParameteri(GL_PATCH_VERTICES, 4);
    glPointSize(100.0f);
    glDrawElements(GL_TRIANGLES, m_indicesCount,  GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);

    return;
}

void BesizerRenderable::detach() {
    glDeleteShader(m_BezierProgram);
    m_BezierProgram = 0;

    glDeleteVertexArrays( 1, &m_vao);
    m_vao = 0;

    glDeleteBuffers(1, &m_vbo);
    m_vbo = 0;

    glDeleteBuffers(1,&m_ebo);
    m_ebo = 0;
}
