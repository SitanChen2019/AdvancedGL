//
// Created by Sitan Chen on 2019/9/3.
//

#ifndef ADVANCEDGL_MODELDISPLAY_H
#define ADVANCEDGL_MODELDISPLAY_H

#include "idemo.h"
#include <string>
#include "screen_quad.h"
#include "aabb.h"

#include "ModelDisplayUIView.h"

class ShadowRender;


class ModelDisplay : public IDemo
        , public  IModelDIsplayUIModel
{
public:
    virtual ~ModelDisplay(){}
    bool init() override ;
    bool update() override;
    bool destroy() override ;

     void displayAsPureColor() override ;
     void displayAsHardLighting() override ;
     void displayAsSmoothLighting() override ;
     void displayAsShadowSmoothLighting() override ;
     void reloadShader() override ;

private:
    void removeMeshGroup(std::set<IRenderable*>& meshGroup);
    void showMeshGroup(const std::set<IRenderable*>& meshGroup, bool enable );

    void renderShadowTexture();

public:
    std::set<IRenderable*> m_wf_meshes;
    std::set<IRenderable*> m_triagnle_meshes;
    std::set<IRenderable*> m_smooth_triangle_meshes;
    std::set<IRenderable*> m_shadow_triangle_meshes;

    ShadowRender*          m_pShadowRender;
    std::set<IRenderable*> m_otherMesh;
    ModelDisplayUIView*    m_pUI = nullptr;
    AABB                   m_bbox;
};


#endif //ADVANCEDGL_MODELDISPLAY_H
