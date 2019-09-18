//
// Created by sitan on 19-9-18.
//

#ifndef ADVANCEDGL_ANIMATIONRENDERABLE_H
#define ADVANCEDGL_ANIMATIONRENDERABLE_H


#include "mesh_buffer.h"
#include "shader.h"
#include "irenderable.h"
#include "aabb.h"

class AnimationRenderable : public IRenderable  {
public:
    AnimationRenderable( MeshBuffer buffers, Vec3Sequence& originalVertex);


    void attach() override;
    void render() override;
    void detach() override;
    void updateBoneMatrix( const Mat4Sequence& matList  , AABB& box);

private:
    ShaderObj* m_shader;

    MeshBuffer m_buffers;


    GLint m_diffuseAttrLoc;
    GLint m_viewAttrLoc;
    GLint m_projAttrLoc;
    GLuint m_vao;

    Vec3Sequence m_originVertex;
    Vec3 m_color = COLOR_WHITE ;
};

#endif //ADVANCEDGL_ANIMATIONRENDERABLE_H
