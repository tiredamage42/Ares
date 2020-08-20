#pragma once
#include <Ares.h>
namespace Ares
{
	class StatsWindow
	{
    private:

        static const uint32_t SAMPLES = 50;
        float m_FrameTimeGraph[SAMPLES];
        int values_offset = 0;
        float m_FrameTime;
        uint32_t m_FPS;
	public:
        StatsWindow()
        {
            memset(m_FrameTimeGraph, 0, sizeof(float) * SAMPLES);
        }
		void DrawWindow(ImVec2 parentWindowPos, ImVec2 parentWindowSize, uint32_t width, uint32_t height, uint32_t pad)
		{
            ImGui::SetCursorPos(ImVec2(parentWindowPos.x + (parentWindowSize.x - (width + pad * .25f)), parentWindowPos.y + pad * .25f));
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, .5f));
            ImGui::BeginChild("Stats", ImVec2(width, height), true, ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoSavedSettings);
            ImGui::PopStyleColor();

            ImVec2 cursorPos = ImGui::GetCursorPos();
            float x = cursorPos.x + pad;

            float frameTimeMS = (float)(Time::GetDeltaTime() * 1000.0);
            m_FrameTimeGraph[values_offset] = frameTimeMS;
            values_offset = (values_offset + 1) % SAMPLES;
            if (values_offset == 1)
            {
                m_FrameTime = 0;
                for (uint32_t i = 0; i < SAMPLES; i++)
                    m_FrameTime += m_FrameTimeGraph[i];

                m_FrameTime /= (float)SAMPLES;

                m_FPS = (uint32_t)(1000.0f / m_FrameTime);
            }

            ImGui::SetCursorPos(ImVec2(x, cursorPos.y + pad * .5f));
            ImGui::PlotLines("##Frametime", m_FrameTimeGraph, SAMPLES, values_offset, "Frametime (ms)", 0.0f, 66.6f, ImVec2(width - pad * 2, 100));

            ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
            ImGui::Text("Frametime: %.2fms", m_FrameTime);

            ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
            ImGui::Text("FPS: %d", m_FPS);

            auto& caps = RendererAPI::GetCapabilities();

            ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
            ImGui::Text("Vendor: %s", caps.Vendor.c_str());

            ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
            ImGui::Text("Renderer: %s", caps.Renderer.c_str());

            ImGui::SetCursorPos(ImVec2(x, ImGui::GetCursorPos().y));
            ImGui::Text("Version: %s", caps.Version.c_str());

            ImGui::EndChild();
		}
	};
}