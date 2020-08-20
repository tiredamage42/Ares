#pragma once

//#include "Ares/Core/Core.h"
#include "Ares/Renderer/Texture.h"

#include "Ares/Editor/NotificationManager.h"
#include "Ares/Editor/DirectoryInformation.h"
//#include "Notification.h"
//#include "Hazel/Utilities/AssetTypes.h"
#include <map>
#include <string>
#include <vector>

namespace Ares
{
	class AssetManagerPanel
	{
	public:
		AssetManagerPanel();
		void RenderAssetWindow();
	private:

		Ref<Texture2D> GetIcon(const std::string& extension);

		Ref<Texture2D> m_folderTex;
		Ref<Texture2D> m_favoritesTex;
		//Ref<Texture2D> m_FileTex;
		Ref<Texture2D> m_goBackTex;
		//Ref<Texture2D> m_scriptTex;
		//Ref<Texture2D> m_resourceTex;
		//Ref<Texture2D> m_sceneTex;

		/*
		Ref<Texture2D> m_fbxTex;
		Ref<Texture2D> m_objTex;
		Ref<Texture2D> m_wavTex;
		Ref<Texture2D> m_mp3Tex;
		Ref<Texture2D> m_cscTex;
		Ref<Texture2D> m_pngTex;
		Ref<Texture2D> m_bldTex;*/

		/*Ref<Texture2D> m_hzlTex;
		Ref<Texture2D> m_hzlWTex;
		Ref<Texture2D> m_hzlETex;*/

		Ref<Texture2D> m_backbtnTex;
		Ref<Texture2D> m_fwrdbtnTex;
		Ref<Texture2D> m_folderRightTex;
		Ref<Texture2D> m_TagsTex;
		Ref<Texture2D> m_searchTex;
		Ref<Texture2D> m_gridView;
		Ref<Texture2D> m_listView;


		Ref<Texture2D> _tex0;
		Ref<Texture2D> _tex1;
		Ref<Texture2D> _tex2;
		Ref<Texture2D> _tex3;
		Ref<Texture2D> _tex4;
		Ref<Texture2D> _tex5;
		Ref<Texture2D> _tex6;
		Ref<Texture2D> _tex7;

		std::string m_CurrentDirPath;
		std::string m_BaseDirPath;
		std::string m_prevDirPath;
		std::string m_MovePath;
		std::string m_lastNavPath;

		size_t m_basePathLen;
		bool m_IsDragging;
		bool m_isInListView;
		//bool m_updateBreadCrumbs;
		//bool m_showSearchBar;

		char* inputText;
		char* inputHint;
		char  inputBuffer[1024];

		std::vector<DirectoryInformation> m_CurrentDirContents;
		std::vector<DirectoryInformation> m_BaseProjectDirContents;

		//std::map<int, Ref<Texture2D>> assetIconMaps;
		NotificationManager nManager;

		void RenderFileListView(int dirIndex);
		void RenderFileGridView(int dirIndex);

		void RenderDircListView(int dirIndex);
		void RenderDircGridView(int dirIndex);

		void RenderBreadCrumbs();

		void RenderSearch();

		void RenderBottom();
	};
}
