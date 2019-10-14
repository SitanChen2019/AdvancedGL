//
// Created by Administrator on 10/14/2019.
//

#include "UntangleDemoView.h"
#include <imgui.h>

void UntangleDemoView::drawUI()
{
    ImGui::SetNextWindowSize({0,0});

    ImGui::Begin("Setting");

    if (ImGui::Button("Step")) {
        m_pModel->exeOneStep();
    }

    ImGui::End();
}