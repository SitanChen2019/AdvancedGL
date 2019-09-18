//
// Created by sitan on 19-9-17.
//

#ifndef ADVANCEDGL_CHARACTERANIMATION_H
#define ADVANCEDGL_CHARACTERANIMATION_H

#include "idemo.h"
#include "animator.h"



class CharacterAnimation : public IDemo {
public:
    virtual ~CharacterAnimation() {}

     bool init() override ;

     bool update()  override ;

     bool destroy()override ;

private:
    void removeMeshGroup(std::vector<IRenderable*>& meshGroup);
    void showMeshGroup(const std::vector<IRenderable*>& meshGroup, bool enable );

private:
    std::vector<IRenderable*> m_wf_meshes;

    AABB                   m_bbox;

     Animator* m_pAnimator = nullptr;
};


#endif //ADVANCEDGL_CHARACTERANIMATION_H
