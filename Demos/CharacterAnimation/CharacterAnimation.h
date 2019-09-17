//
// Created by sitan on 19-9-17.
//

#ifndef ADVANCEDGL_CHARACTERANIMATION_H
#define ADVANCEDGL_CHARACTERANIMATION_H

#include "idemo.h"

class CharacterAnimation : public IDemo {
public:
    virtual ~CharacterAnimation() {}

     bool init() override ;

     bool update()  override ;

     bool destroy()override ;

};


#endif //ADVANCEDGL_CHARACTERANIMATION_H
