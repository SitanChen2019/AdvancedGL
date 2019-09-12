//
// Created by sitan on 19-9-12.
//

#include "ModelDisplayUIView.h"
#include <imgui.h>

void ModelDisplayUIView::drawUI()
{
    ImGui::SetNextWindowSize({0,0});

    ImGui::Begin("Setting");

    if (ImGui::Button("PureColor")) {
        m_pModel->displayAsPureColor();
    }

    if (ImGui::Button("HardLighting")) {
        m_pModel->displayAsHardLighting();
    }

    if (ImGui::Button("SmoothLighting")) {
        m_pModel->displayAsSmoothLighting();
    }


    if (ImGui::Button("Reload")) {
        m_pModel->reloadShader();
    }

    ImGui::End();
}