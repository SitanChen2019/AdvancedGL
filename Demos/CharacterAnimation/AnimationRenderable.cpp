//
// Created by sitan on 19-9-18.
//

#include "AnimationRenderable.h"
#include "global.h"

AnimationRenderable::AnimationRenderable(MeshBuffer buffers,
                                         Vec3Sequence& originalVertex,
                                         const BoneWeightsForMesh& boneWeightsList )
        :IRenderable(50)
        ,m_buffers(buffers)
        ,m_originVertex( originalVertex )
        ,m_boneWeightForMesh( boneWeightsList )
{
    assert( m_buffers.m_vbo_normals.Id() != 0);
    assert( boneWeightsList.size() == originalVertex.size() );

    m_vbo_bone_weights = Global::glObjMgr().createBufferObject();
    m_vbo_bone_ids = Global::glObjMgr().createBufferObject();


    std::vector<int> boneIDs;
    std::vector<float> boneWeights;

    //support max 16 bones per vertex
    boneIDs.resize( boneWeightsList.size() * 16 , 1);
    boneWeights.resize( boneWeightsList.size()*16, 0 );
    for( int i = 0; i < boneWeightsList.size(); ++i )
    {
        for( int j = 0; j < boneWeightsList[i].size(); ++j )
        {
            boneIDs[16*i+j]  = boneWeightsList[i][j].boneId;
            boneWeights[16*i+j] = boneWeightsList[i][j].weight;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bone_ids.Id() );
    glBufferData(GL_ARRAY_BUFFER, boneIDs.size()*sizeof(int), boneIDs.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bone_weights.Id());
    glBufferData(GL_ARRAY_BUFFER, boneWeights.size()*sizeof(float), boneWeights.data(), GL_STATIC_DRAW);
}



void AnimationRenderable::attach() {
    glGenVertexArrays(1, &m_vao);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_vertives.Id() );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);


    glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_vbo_normals.Id());
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bone_ids.Id() );
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 4, GL_INT, 16 * sizeof(int), 0);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, 16 * sizeof(int), (void*)(4*sizeof(int)));
    glEnableVertexAttribArray(4);
    glVertexAttribIPointer(4, 4, GL_INT, 16 * sizeof(int), (void*)(8*sizeof(int)));
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, 16 * sizeof(int), (void*)(12*sizeof(int)));

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_bone_weights.Id());
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), 0);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4*sizeof(float)));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8*sizeof(float)));
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12*sizeof(float)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_ebo.Id());
    glBindVertexArray(0);



    m_shader = Global::shaderMgr().getShader("simple_bone");

    m_diffuseAttrLoc = glGetUniformLocation(m_shader->Id(), "diffMat" );
    m_viewAttrLoc = glGetUniformLocation (m_shader->Id(), "view");
    m_projAttrLoc = glGetUniformLocation (m_shader->Id(), "projection");
    m_modelMatAttrLoc =  glGetUniformLocation (m_shader->Id(), "model");
    m_boneMatrArrayAttrLoc = glGetUniformLocation(m_shader->Id(), "boneMatrices");

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


    glBindVertexArray(m_vao);
    glUseProgram( m_shader->Id() );

    Matrix4  viewMat = Global::renderWindow().getViewMatrix();
    glUniformMatrix4fv( m_viewAttrLoc, 1, GL_FALSE ,(float*) &viewMat);

    Matrix4  projMat = Global::renderWindow().getProjMatrix();
    glUniformMatrix4fv( m_projAttrLoc, 1, GL_FALSE ,(float*) &projMat);

    glUniformMatrix4fv( m_modelMatAttrLoc, 1, GL_FALSE ,(float*) &IdentifyMatrix4);

    glUniform3f( m_diffuseAttrLoc, m_color.x, m_color.y, m_color.z );

    assert( m_boneMatrices.size() < 200);
    glUniformMatrix4fv( m_boneMatrArrayAttrLoc,  m_boneMatrices.size(), GL_FALSE, (float*)m_boneMatrices.data());

    glDrawElements(GL_TRIANGLES, m_buffers.m_indicesCount, GL_UNSIGNED_INT, 0);


    glUseProgram(0);
    glBindVertexArray(0);

}


void AnimationRenderable::updateBoneMatrix( const Mat4Sequence& matList  )
{
    m_boneMatrices = matList;
}

Vec3Sequence AnimationRenderable::calcuateAnimationVertexList( )
{
    Vec3Sequence boneTransformedVertices;
    boneTransformedVertices.resize( m_buffers.m_vertexCount );
    for( int j = 0; j< m_originVertex.size(); ++j )
    {
        BoneWeightsForVertex& vertexBoneData = m_boneWeightForMesh[j];

        Matrix4  currentVertexBoneMatrix = Matrix4(0);
        float totalWeight  = 0;
        for( int k = 0; k < vertexBoneData.size(); ++k )
        {
            totalWeight += vertexBoneData[k].weight;
            currentVertexBoneMatrix  += vertexBoneData[k].weight * m_boneMatrices[ vertexBoneData[k].boneId];
        }

        glm::vec4 pos = currentVertexBoneMatrix *glm::vec4( m_originVertex.at(j),1);
        boneTransformedVertices.push_back( glm::vec3(pos));
    }

    return boneTransformedVertices;
}