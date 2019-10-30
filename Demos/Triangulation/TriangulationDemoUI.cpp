#include "TriangulationDemoUI.h"
#include <imgui.h>

void TriangulationUIView::drawUI()
{
	if (ImGui::Button("PreMesh")) {
		m_pModel->displayPreModel();
	}

	if (ImGui::Button("PostMesh")) {
		m_pModel->displayPostModel();
	}

	if (ImGui::Button("RefineMesh")) {
		m_pModel->displayRefineModel();
	}
}