//
// Created by sitan on 19-9-18.
//

#include "AnimationRenderable.h"
#include "global.h"

AnimationRenderable::AnimationRenderable(MeshBuffer buffers,Vec3Sequence& originalVertex)
        :IRenderable(50)
        ,m_buffers(buffers)
        ,m_originVertex( originalVertex )
{

}



void AnimationRenderable::attach() {
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id() );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id());
    glBindVertexArray(0);

    glBindVertexArray(0);

    m_shader = Global::shaderMgr().getShader("triangle");

    m_diffuseAttrLoc = glGetUniformLocation(m_shader->Id(), "diffMat" );
    m_viewAttrLoc = glGetUniformLocation (m_shader->Id(), "view");
    m_projAttrLoc = glGetUniformLocation (m_shader->Id(), "projection");

    //assert( m_diffuseAttrLoc != -1);
    assert( m_viewAttrLoc != -1);
    assert( m_projAttrLoc != -1);

}

void AnimationRenderable::detach() {
    glDeleteVertexArrays( 1, &m_vao);

    m_shader = nullptr;
}

void  AnimationRenderable::render() {
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    Matrix4 mat = Global::renderWindow().getProjViewMatrix();

    glUseProgram( m_shader->Id() );
    glBindVertexArray(m_vao);

    Matrix4  viewMat = Global::renderWindow().getViewMatrix();
    glUniformMatrix4fv( m_viewAttrLoc, 1, GL_FALSE ,(float*) &viewMat);

    Matrix4  projMat = Global::renderWindow().getProjMatrix();
    glUniformMatrix4fv( m_projAttrLoc, 1, GL_FALSE ,(float*) &projMat);

    glUniform3f( m_diffuseAttrLoc, m_color.x, m_color.y, m_color.z );

    glDrawElements(GL_TRIANGLES, m_buffers.m_indicesCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}


void AnimationRenderable::updateBoneMatrix( const Mat4Sequence& matList , AABB& box  )
{
    assert( matList.size() == m_buffers.m_vertexCount );


    Vec3Sequence boneTransformedVertices;
    boneTransformedVertices.resize( m_buffers.m_vertexCount );

    for(int i =0 ;i < m_buffers.m_vertexCount; ++i )
    {
        Vec4 vertex = glm::vec4( m_originVertex[i].x,m_originVertex[i].y,m_originVertex[i].z,1);
        Vec4 boneTransformedVertex = matList[i]*vertex;
        boneTransformedVertices[i]= Vec3(boneTransformedVertex.x, boneTransformedVertex.y, boneTransformedVertex.z);
        box.merge( boneTransformedVertices[i] );
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id());
    glBufferData(GL_ARRAY_BUFFER, boneTransformedVertices.size()*sizeof(Vec3), boneTransformedVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}