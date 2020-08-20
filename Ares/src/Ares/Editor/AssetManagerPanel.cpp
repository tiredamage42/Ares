
#include "AresPCH.h"
#include "AssetManagerPanel.h"

#include "Ares/Editor/AssetManager.h"
#include "Ares/Editor/EditorResources.h"

#include "Ares/Core/Application.h"
#include "Ares/Core/Core.h"
#include <imgui.h>

//#include <unordered_map>

namespace Ares
{
	static const std::string NO_EXTENSION = "NO_EXTENSION";
	
	static std::unordered_map<std::string, Ref<Texture2D>> s_Extension2Icon;




	Ref<Texture2D> AssetManagerPanel::GetIcon(const std::string& extension)
	{

		if (s_Extension2Icon.find(extension) != s_Extension2Icon.end())
			return s_Extension2Icon.at(extension);

		return s_Extension2Icon.at(NO_EXTENSION);

		
		/*if (extension == "blend")	return m_bldTex;
		if (extension == "fbx")		return m_fbxTex;
		if (extension == "obj")		return m_objTex;
		if (extension == "wav")		return m_wavTex;
		if (extension == "cs")		return m_cscTex;
		if (extension == "png")		return m_pngTex;
		if (extension == "scene")	return m_sceneTex;

		return m_FileTex;*/
	}
	AssetManagerPanel::AssetManagerPanel()
	{
			_tex0 = EditorResources::GetTexture("file.png")	;
			_tex1 = EditorResources::GetTexture("blend.png");
			_tex2 = EditorResources::GetTexture("fbx.png")	;
			_tex3 = EditorResources::GetTexture("obj.png")	;
			_tex4 = EditorResources::GetTexture("wav.png")	;
			_tex5 = EditorResources::GetTexture("csc.png")	;
			_tex6 = EditorResources::GetTexture("png.png")	;
			_tex7 = EditorResources::GetTexture("scene.png");
		s_Extension2Icon = {
			{ NO_EXTENSION,		_tex0	},
			{ "blend" ,			_tex1	},
			{ "fbx"	,			_tex2	},
			{ "obj"	,			_tex3	},
			{ "wav"	,			_tex4	},
			{ "cs"	,			_tex5	},
			{ "png"	,			_tex6	},
			{ "scene" ,			_tex7	},
		};

		m_folderTex =		EditorResources::GetTexture("folder.png");
		m_favoritesTex =	EditorResources::GetTexture("favourites.png");
		//m_FileTex =			EditorResources::GetTexture("file.png");
		m_goBackTex =		EditorResources::GetTexture("back.png");
		/*m_scriptTex =		EditorResources::GetTexture("script.png");
		m_resourceTex =		EditorResources::GetTexture("resource.png");
		m_sceneTex =		EditorResources::GetTexture("scene.png");
		m_fbxTex =			EditorResources::GetTexture("fbx.png");
		m_objTex =			EditorResources::GetTexture("obj.png");
		m_cscTex =			EditorResources::GetTexture("csc.png");
		m_wavTex =			EditorResources::GetTexture("wav.png");
		m_pngTex =			EditorResources::GetTexture("png.png");
		m_bldTex =			EditorResources::GetTexture("blend.png");*/
		/*m_hzlTex =			EditorResources::GetTexture("hazel.png");
		m_hzlWTex =			EditorResources::GetTexture("hazel_warn.png");
		m_hzlETex =			EditorResources::GetTexture("hazel_errr.png");*/
		m_backbtnTex =		EditorResources::GetTexture("btn_back.png");
		m_fwrdbtnTex =		EditorResources::GetTexture("btn_fwrd.png");
		m_folderRightTex =	EditorResources::GetTexture("folder_hierarchy.png");
		m_searchTex =		EditorResources::GetTexture("search.png");
		m_TagsTex =			EditorResources::GetTexture("tags.png");
		m_gridView =		EditorResources::GetTexture("grid.png");
		m_listView =		EditorResources::GetTexture("list.png");

		m_BaseDirPath = "Assets";
		m_CurrentDirPath = m_BaseDirPath;
		m_prevDirPath = m_CurrentDirPath;
		m_lastNavPath = m_BaseDirPath;
		m_BaseProjectDirContents = AssetManager::GetFsContents();
		m_CurrentDirContents = m_BaseProjectDirContents;
		m_basePathLen = strlen(m_BaseDirPath.c_str());

		m_IsDragging = false;
		m_isInListView = true;
		//m_updateBreadCrumbs = true;
		//m_showSearchBar = false;

		/*assetIconMaps[-1] = m_FileTex;
		assetIconMaps[0] = m_fbxTex;
		assetIconMaps[1] = m_objTex;
		assetIconMaps[2] = m_wavTex;
		assetIconMaps[3] = m_cscTex;
		assetIconMaps[4] = m_pngTex;
		assetIconMaps[5] = m_bldTex;*/
		//assetIconMaps[6] = m_hzlTex;

		inputHint = "Search...";
		inputText = "";
	}

	void AssetManagerPanel::RenderAssetWindow()
	{
		nManager.Render();

		static bool open = true;
		ImGui::Begin("Project", &open, ImGuiWindowFlags_MenuBar);
		if (
			ImGui::BeginMenuBar()
			)
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Import New Asset", "Ctrl + O"))
				{
					std::string filename = Application::Get().OpenFile("");
					std::vector<std::string> outData;


					AssetManager::ImportAsset(filename, m_CurrentDirPath);
					//AssetManager::ProcessAseets(filename);
				}

				if (ImGui::MenuItem("Refresh", "Ctrl + R"))
				{
					auto data = AssetManager::GetFsContents();
					for (int i = 0; i < data.size(); i++)
					{
						ARES_CORE_INFO(data[i].filename);
					}
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}



		{
			ImGui::Columns(2, "AB", true);
			ImGui::SetColumnOffset(1, 240);

			ImGui::BeginChild("##folders_common");
			{
				/*if (ImGui::CollapsingHeader("res://", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
				{*/
					if (ImGui::TreeNode("Contents"))
					{
						for (int i = 0; i < m_BaseProjectDirContents.size(); i++)
						{
							if (ImGui::TreeNode(m_BaseProjectDirContents[i].filename.c_str()))
							{
								auto dirData = AssetManager::ReadDirectory(m_BaseProjectDirContents[i].absolutePath.c_str());
								for (int d = 0; d < dirData.size(); d++)
								{
									if (!dirData[d].isFile)
									{
										if (ImGui::TreeNode(dirData[d].filename.c_str()))
										{
											ImGui::TreePop();
										}
									}
									else
									{
										auto parentDir = AssetManager::GetParentPath(dirData[d].absolutePath);
										ImGui::Indent();
										ImGui::Selectable(dirData[d].filename.c_str(), false);
										ImGui::Unindent();
									}
								}
								ImGui::TreePop();
							}

							if (m_IsDragging && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
							{
								m_MovePath = m_BaseProjectDirContents[i].absolutePath.c_str();
							}
						}
						ImGui::TreePop();
					}

					if (ImGui::IsMouseDown(1))
					{
						ImGui::OpenPopup("window");
					}
				//}

				ImGui::EndChild();
			}

			if (ImGui::BeginDragDropTarget())
			{
				auto data = ImGui::AcceptDragDropPayload("selectable", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
				
				if (data)
				{

					std::string file = (char*)data->Data;
					if (AssetManager::MoveFileTo(file, m_MovePath))
					{

						ARES_CORE_INFO("Moved File: " + file + " to " + m_MovePath);
						m_CurrentDirContents = AssetManager::ReadDirectory(m_CurrentDirPath);
					}
					m_IsDragging = false;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::NextColumn();

			ImGui::BeginChild("##directory_structure", ImVec2(ImGui::GetColumnWidth() - 12, 0));// 250));
			{
				RenderBreadCrumbs();
				ImGui::SameLine();
				ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() - 350, 0));
				//ImGui::SameLine();
				//RenderSearch();
				//ImGui::EndChild();

				ImGui::BeginChild("Scrolling");

				if (!m_isInListView)
					ImGui::Columns(17, nullptr, false);

				for (int i = 0; i < m_CurrentDirContents.size(); i++)
				{
					/*if (m_CurrentDir.size() > 0)
					{*/
						if (!m_CurrentDirContents[i].isFile)
						{
							if (!m_isInListView)
								RenderDircGridView(i);
							else
								RenderDircListView(i);
						}
						else
						{
							if (!m_isInListView)
								RenderFileGridView(i);
							else
								RenderFileListView(i);
						}

						ImGui::NextColumn();
					//}
				}

				ImGui::EndChild();
				ImGui::EndChild();
			}

			if (ImGui::BeginDragDropTarget())
			{
				auto data = ImGui::AcceptDragDropPayload("selectable", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);
				if (data)
				{
					std::string a = (char*)data->Data;
					if (AssetManager::MoveFileTo(a, m_MovePath))
					{
						ARES_CORE_INFO("Moved File: " + a + " to " + m_MovePath);
					}
					m_IsDragging = false;
				}
				ImGui::EndDragDropTarget();
			}

			
			ImGui::End();
		}
	}

	void AssetManagerPanel::RenderBreadCrumbs()
	{
		//ImGui::BeginChild("##directory_breadcrumbs", ImVec2(ImGui::GetColumnWidth() - 100, 30));
		{
			if (m_isInListView)
			{
				if (ImGui::ImageButton((void*)(size_t)m_gridView->GetRendererID(), ImVec2(20, 18)))
				{
					m_isInListView = !m_isInListView;
				}
				ImGui::SameLine();

			}
			else
			{
				if (ImGui::ImageButton((void*)(size_t)m_listView->GetRendererID(), ImVec2(20, 18)))
				{
					m_isInListView = !m_isInListView;
				}
				ImGui::SameLine();

			}

			/*if (ImGui::ImageButton((void*)m_searchTex->GetRendererID(), ImVec2(20, 18)))
			{*/
				//m_showSearchBar = !m_showSearchBar;

				//if (m_showSearchBar)
				//{
				//	//Notification n("Info", "Search Bar Shown!", m_hzlTex);// , 3);
				//	//nManager.Dispatch(n);

				//	nManager.Dispatch("Search Bar Shown!", NotificationType::Log);
				//}
				//else
				//{
				//	//Notification n("Warn", "Search Bar Hidden!", m_hzlWTex);// , 3);
				//	//nManager.Dispatch(n);

				//	nManager.Dispatch("Search Bar Hidden!", NotificationType::Warn);
				//	nManager.Dispatch("Search Bar Hidden!", NotificationType::Error);
				//}
			//}
			//ImGui::SameLine();

			/*if (m_showSearchBar)
			{*/
				char buff[100] = { 0 };
				ImGui::SameLine();
				ImGui::PushItemWidth(200);
				ImGui::InputTextWithHint(inputText, inputHint, buff, 100);
				ImGui::PopItemWidth();
				ImGui::SameLine();
			//}

			if (ImGui::ImageButton((void*)(size_t)m_backbtnTex->GetRendererID(), ImVec2(20, 18)))
			{
				if (strlen(m_CurrentDirPath.c_str()) != m_basePathLen)
				{
					m_prevDirPath = AssetManager::GetParentPath(m_CurrentDirPath);
					m_CurrentDirPath = m_prevDirPath;
					m_CurrentDirContents = AssetManager::ReadDirectory(m_CurrentDirPath);
				}
			}
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)(size_t)m_fwrdbtnTex->GetRendererID(), ImVec2(20, 18)))
			{
				m_prevDirPath = AssetManager::GetParentPath(m_CurrentDirPath);
				m_CurrentDirPath = m_lastNavPath;
				m_CurrentDirContents = AssetManager::ReadDirectory(m_lastNavPath);
			}
			ImGui::SameLine();

			auto data = AssetManager::GetDirectories(m_CurrentDirPath);

			for (int i = 0; i < data.size(); i++)
			{
				if (data[i] != m_BaseDirPath) {
					ImGui::Image((void*)(size_t)m_folderRightTex->GetRendererID(), ImVec2(22, 23));
				}
				ImGui::SameLine();
				ImGui::Text(data[i].c_str());
				ImGui::SameLine();
			}

			ImGui::SameLine();

			ImGui::Dummy(ImVec2(ImGui::GetColumnWidth() - 400, 0));

			ImGui::SameLine();
		}
	}

	void AssetManagerPanel::RenderFileListView(int dirIndex)
	{


		/*auto fileID = AssetTypes::GetParsedAssetID(m_CurrentDir[dirIndex].fileType);
		auto iconRef = assetIconMaps[fileID]->GetRendererID();*/

		auto iconRef = (size_t)GetIcon(m_CurrentDirContents[dirIndex].fileType)->GetRendererID();

		ImGui::Image((void*)iconRef, ImVec2(20, 20));
		ImGui::SameLine();
		if (ImGui::Selectable(m_CurrentDirContents[dirIndex].filename.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
		{
			if (ImGui::IsMouseDoubleClicked(0))
			{
				std::string cmd = '"' + m_CurrentDirContents[dirIndex].absolutePath + '"';
				system(cmd.c_str());
			}
		}


		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::Image((void*)iconRef, ImVec2(20, 20));
			ImGui::SameLine();
			ImGui::Text(m_CurrentDirContents[dirIndex].filename.c_str());
			size_t size = sizeof(const char*) + strlen(m_CurrentDirContents[dirIndex].absolutePath.c_str());
			ImGui::SetDragDropPayload("selectable", m_CurrentDirContents[dirIndex].absolutePath.c_str(), size);
			m_IsDragging = true;
			ImGui::EndDragDropSource();
		}
	}

	void AssetManagerPanel::RenderFileGridView(int dirIndex)
	{
		ImGui::BeginGroup();

		/*auto fileID = AssetTypes::GetParsedAssetID(m_CurrentDir[dirIndex].fileType);
		auto iconRef = assetIconMaps[fileID]->GetRendererID();*/
		auto iconRef = (size_t)GetIcon(m_CurrentDirContents[dirIndex].fileType)->GetRendererID();



		ImGui::ImageButton((void*)iconRef, ImVec2(70, 70));
		auto fname = m_CurrentDirContents[dirIndex].filename;
		auto newFname = AssetManager::StripExtras(fname);

		ImGui::TextWrapped(newFname.c_str());
		ImGui::EndGroup();

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			ImGui::Image((void*)iconRef, ImVec2(20, 20));
			ImGui::SameLine();

			ImGui::Text(m_CurrentDirContents[dirIndex].filename.c_str());
			size_t size = sizeof(const char*) + strlen(m_CurrentDirContents[dirIndex].absolutePath.c_str());
			ImGui::SetDragDropPayload("selectable", m_CurrentDirContents[dirIndex].absolutePath.c_str(), size);
			m_IsDragging = true;
			ImGui::EndDragDropSource();
		}
	}

	void AssetManagerPanel::RenderDircListView(int dirIndex)
	{
		ImGui::Image((void*)(size_t)m_folderTex->GetRendererID(), ImVec2(20, 20));
		ImGui::SameLine();

		if (ImGui::Selectable(m_CurrentDirContents[dirIndex].filename.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
		{
			if (ImGui::IsMouseDoubleClicked(0))
			{
				m_prevDirPath = m_CurrentDirContents[dirIndex].absolutePath;
				m_CurrentDirPath = m_CurrentDirContents[dirIndex].absolutePath;
				m_CurrentDirContents = AssetManager::ReadDirectory(m_CurrentDirContents[dirIndex].absolutePath);
			}
		}

		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		{
			ImGui::Image((void*)(size_t)m_folderTex->GetRendererID(), ImVec2(20, 20));
			ImGui::SameLine();
			ImGui::Text(m_CurrentDirContents[dirIndex].filename.c_str());
			size_t size = sizeof(const char*) + strlen(m_CurrentDirContents[dirIndex].absolutePath.c_str());
			ImGui::SetDragDropPayload("selectable", m_CurrentDirContents[dirIndex].absolutePath.c_str(), size);
			m_IsDragging = true;
			ImGui::EndDragDropSource();
		}
	}

	void AssetManagerPanel::RenderDircGridView(int dirIndex)
	{
		ImGui::BeginGroup();
		ImGui::ImageButton((void*)(size_t)m_folderTex->GetRendererID(), ImVec2(70, 70));

		if (ImGui::IsMouseDoubleClicked(0))
		{
			m_prevDirPath = m_CurrentDirContents[dirIndex].absolutePath;
			m_CurrentDirPath = m_CurrentDirContents[dirIndex].absolutePath;
			m_CurrentDirContents = AssetManager::ReadDirectory(m_CurrentDirContents[dirIndex].absolutePath);
		}

		auto fname = m_CurrentDirContents[dirIndex].filename;
		auto newFname = AssetManager::StripExtras(fname);
		ImGui::TextWrapped(newFname.c_str());
		ImGui::EndGroup();

		/*if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
		{
			ImGui::Image((void*)m_folderTex->GetRendererID(), ImVec2(20, 20));
			ImGui::SameLine();
			ImGui::Text(m_CurrentDir[dirIndex].filename.c_str());
			int size = sizeof(const char*) + strlen(m_CurrentDir[dirIndex].absolutePath.c_str());
			ImGui::SetDragDropPayload("selectable", m_CurrentDir[dirIndex].absolutePath.c_str(), size);
			m_IsDragging = true;
			ImGui::EndDragDropSource();
		}*/
	}

	void AssetManagerPanel::RenderSearch()
	{
		/*ImGui::BeginChild("##search_menu", ImVec2(320, 30));
		{
			char buff[100] = { 0 };
			ImGui::Image((void*)m_searchTex->GetRendererID(), ImVec2(22, 22));
			ImGui::SameLine();
			ImGui::InputTextWithHint(inputText, inputHint, buff, 100);
			ImGui::SameLine();
			ImGui::ImageButton((void*)m_favoritesTex->GetRendererID(), ImVec2(19, 19));
			ImGui::SameLine();
			ImGui::ImageButton((void*)m_TagsTex->GetRendererID(), ImVec2(19, 19));
		}
		ImGui::EndChild();*/
	}

	void AssetManagerPanel::RenderBottom()
	{
		ImGui::BeginChild("##nav", ImVec2(ImGui::GetColumnWidth() - 12, 23));
		{
			ImGui::EndChild();
		}
	}
}
