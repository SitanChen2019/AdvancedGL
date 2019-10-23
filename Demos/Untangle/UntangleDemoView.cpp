//
// Created by Administrator on 10/14/2019.
//

#include "UntangleDemoView.h"
#include <imgui.h>
#include <sstream>

void UntangleDemoView::drawUI()
{
    ImGui::SetNextWindowSize({0,0});

    ImGui::Begin("Setting");

    if (ImGui::Button("Step")) {
        m_pModel->exeOneStep();
    }

	char modelName[512]{0};
	if (ImGui::InputText("Model", modelName, 512, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
	{
		m_pModel->reloadModel(modelName);
	}

	bool isGlobalScheme = m_pModel->isGlobalScheme();
	bool oldValue = isGlobalScheme;
	ImGui::Checkbox("Global Scheme", &isGlobalScheme);
	if( isGlobalScheme != oldValue )
	{
		m_pModel->setGlobalScheme(isGlobalScheme);
	}

	int meshcount = m_pModel->getMeshCount();
	for (int i = 0; i < meshcount; ++i)
	{
		std::stringstream ss;
		ss << "Mesh" << i << std::endl;

		float curValue = m_pModel->getMeshInvMass(i);
		if (ImGui::InputFloat(ss.str().c_str(), &curValue, 0.1, 1, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			m_pModel->setMeshInvMass(i, curValue);
		}
	}

    ImGui::End();
}