//
// Created by sitan on 19-9-18.
//

#ifndef ADVANCEDGL_ANIMATIONRENDERABLE_H
#define ADVANCEDGL_ANIMATIONRENDERABLE_H


#include "mesh_buffer.h"
#include "shader.h"
#include "irenderable.h"
#include "aabb.h"
#include "animator.h"

class AnimationRenderable : public IRenderable  {
public:
    AnimationRenderable( MeshBuffer buffers, Vec3Sequence& originalVertex,const BoneWeightsForMesh&  boneWights);


    void attach() override;
    void render() override;
    void detach() override;
    void updateBoneMatrix( const Mat4Sequence& matList  );
    Vec3Sequence calcuateAnimationVertexList(  );

private:
    ShaderObj* m_shader;

    MeshBuffer m_buffers;


    GLint m_diffuseAttrLoc;
    GLint m_viewAttrLoc;
    GLint m_projAttrLoc;
    GLint m_modelMatAttrLoc;
    GLint m_boneMatrArrayAttrLoc;

    Mat4Sequence m_boneMatrices;
    GLuint m_vao;

    SPGLBuffer  m_vbo_bone_ids ;
    SPGLBuffer  m_vbo_bone_weights ;


    Vec3Sequence m_originVertex;
    BoneWeightsForMesh m_boneWeightForMesh;
    Vec3 m_color = COLOR_WHITE ;
};

#endif //ADVANCEDGL_ANIMATIONRENDERABLE_H
