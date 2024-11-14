#include "ResUI.h"
#include "FileManager.h"
#include "resource.h"
#include <windowsx.h>
#include "lodepng.h"
#include <filesystem>

ResUI *ResUI::g_pInstance = 0;

ResUI::ResUI(HWND hWnd)
{
	g_pInstance = this;

	m_pSprite = 0;
	m_pBackSprite = 0;
	m_pCurrentModule = 0;
	m_pPalette = 0;
	m_bSetupUINow = false;

	m_pCurrentTexture = 0;
	m_pCurrentMask = 0;
	m_pCurrentMaterial = 0;
	m_pCurrentTextureReal = 0;
	m_pCurrentMaskReal = 0;
	m_pCurrentMaterialReal = 0;

	m_eCurrentView = ResUIViewType::ViewNone;

	m_hMainWnd = hWnd;
	m_hMainWndMenu = GetMenu(hWnd);
	m_pTree = WinTree::Create(hWnd, TVS_HASBUTTONS | TVS_LINESATROOT, 0, 0, 300, 560, ID_MAINTREE);
	m_hViewWnd = CreateChildWindow(hWnd, 0, 300, 0, 300, 400, ID_DIRECTXWND, ViewWndProc);
	m_hControlsWnd = CreateChildWindow(hWnd, 0, 300, 400, 300, 400, ID_TEXTURECTRLWND, ControlsWndProc);
	ShowWindow(m_hControlsWnd, 1);
	
	UI_SetElementFont(UI_CreateLabel(m_hControlsWnd, "Название: ", 10, 9, 60, 20, 0, 2650), hMainFont);
	m_hBoxItemName = UI_CreateTextBox(m_hControlsWnd, "", 70, 7, 150, 20, ES_AUTOHSCROLL, ID_RESUI_BOX_NAME);
	m_hBtnSave = UI_CreateButton(m_hControlsWnd, "Сохранить", 310, 5, 75, 25, 0, ID_RESUI_BTN_SAVE);
	m_hBtnReset = UI_CreateButton(m_hControlsWnd, "Сброс", 390, 5, 75, 25, 0, ID_RESUI_BTN_RESET);
	ShowWindow(m_hBoxItemName, 1);
	ShowWindow(m_hBtnSave, 1);
	ShowWindow(m_hBtnReset, 1);
	UI_SetElementFont(m_hBoxItemName, hMainFont);
	UI_SetElementFont(m_hBtnSave, hMainFont);
	UI_SetElementFont(m_hBtnReset, hMainFont);

	// init material UI
	memset(&m_pMaterialControls, 0, sizeof(m_pMaterialControls));
	int materialsId = ID_RESUI_START;

	m_pMaterialControls.m_grpConnection = { UI_CreateGroupBox(m_hControlsWnd, "Relations", 5, 40, 270, 100, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkTexture = { UI_CreateCheckBox(m_hControlsWnd, "Has Texture", 10, 55, 100, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkMaterial = { UI_CreateCheckBox(m_hControlsWnd, "Attached Material", 10, 80, 100, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkParMaterial = { UI_CreateCheckBox(m_hControlsWnd, "Parent Material", 10, 105, 100, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_cbTexture = { UI_CreateComboBox(m_hControlsWnd, 115, 58, 150, 25, WS_VSCROLL, materialsId), materialsId++ };
	m_pMaterialControls.m_cbMaterial = { UI_CreateComboBox(m_hControlsWnd, 115, 83, 150, 25, WS_VSCROLL, materialsId), materialsId++ };
	m_pMaterialControls.m_cbParMaterial = { UI_CreateComboBox(m_hControlsWnd, 115, 108, 150, 25, WS_VSCROLL, materialsId), materialsId++ };

	m_pMaterialControls.m_grpMove = { UI_CreateGroupBox(m_hControlsWnd, "Special Parameters", 5, 140, 475, 100, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkMove = { UI_CreateCheckBox(m_hControlsWnd, "Move", 10, 155, 100, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxMoveX = { UI_CreateTextBox(m_hControlsWnd, "", 115, 157, 74, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxMoveY = { UI_CreateTextBox(m_hControlsWnd, "", 191, 157, 74, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkRotationPoint = { UI_CreateCheckBox(m_hControlsWnd, "Rotation Point", 10, 180, 100, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxRotationX = { UI_CreateTextBox(m_hControlsWnd, "", 115, 182, 74, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxRotationY = { UI_CreateTextBox(m_hControlsWnd, "", 191, 182, 74, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkRotation = { UI_CreateCheckBox(m_hControlsWnd, "Rotation Angle", 10, 205, 100, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxRotation = { UI_CreateTextBox(m_hControlsWnd, "", 115, 207, 74, 20, 0, materialsId), materialsId++ };
	// part 2
	m_pMaterialControls.m_chkEnv = { UI_CreateCheckBox(m_hControlsWnd, "Env Scale", 285, 155, 70, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxEnv1 = { UI_CreateTextBox(m_hControlsWnd, "", 365, 157, 50, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxEnv2 = { UI_CreateTextBox(m_hControlsWnd, "", 420, 157, 50, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkSpecular = { UI_CreateCheckBox(m_hControlsWnd, "Specular", 285, 180, 60, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_lblSpecular = { UI_CreateLabel(m_hControlsWnd, "Value:", 380, 185, 50, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxSpecular = { UI_CreateTextBox(m_hControlsWnd, "", 420, 182, 50, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_lblPower = { UI_CreateLabel(m_hControlsWnd, "Power:", 380, 210, 50, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxPower = { UI_CreateTextBox(m_hControlsWnd, "", 420, 207, 50, 20, 0, materialsId), materialsId++ };

	m_pMaterialControls.m_grpColorGroup = { UI_CreateGroupBox(m_hControlsWnd, "Color Parameters", 280, 40, 200, 100, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkIsTransparent = { UI_CreateCheckBox(m_hControlsWnd, "Is Transparent", 285, 55, 100, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkColor = { UI_CreateCheckBox(m_hControlsWnd, "Material Color", 285, 80, 90, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_btnColor = { UI_CreateButton(m_hControlsWnd, "", 385, 55, 75, 50, BS_OWNERDRAW | WS_BORDER | WS_VISIBLE, materialsId), materialsId++ };
	m_pMaterialControls.m_chkTransparent = { UI_CreateCheckBox(m_hControlsWnd, "Opacity", 285, 105, 100, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxTransparent = { UI_CreateTextBox(m_hControlsWnd, "", 385, 107, 75, 20, 0, materialsId), materialsId++ };

	m_pMaterialControls.m_grpFlags = { UI_CreateGroupBox(m_hControlsWnd, "Flags", 5, 240, 475, 45, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkNoZ = { UI_CreateCheckBox(m_hControlsWnd, "No ZBuffer", 10, 255, 100, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkNoF = { UI_CreateCheckBox(m_hControlsWnd, "No Fog", 110, 255, 80, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_chkNoTile = { UI_CreateCheckBox(m_hControlsWnd, "No Tile", 190, 255, 60, 25, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_lblCoord = { UI_CreateLabel(m_hControlsWnd, "Layer Id:", 265, 260, 50, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxCoord = { UI_CreateTextBox(m_hControlsWnd, "", 325, 257, 30, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_lblEnvId = { UI_CreateLabel(m_hControlsWnd, "Env Id:", 380, 260, 50, 20, 0, materialsId), materialsId++ };
	m_pMaterialControls.m_boxEnvId = { UI_CreateTextBox(m_hControlsWnd, "", 435, 257, 30, 20, 0, materialsId), materialsId++ };

	m_hMaterialMenu = CreatePopupMenu();
	InsertMenu(m_hMaterialMenu, -1, MF_BYPOSITION | MF_STRING, 2500, "Удалить");

	m_hMaterialsMenu = CreatePopupMenu();
	InsertMenu(m_hMaterialsMenu, -1, MF_BYPOSITION | MF_STRING, 2510, "Добавить");

	m_pTextureControls.m_lblFormat = { UI_CreateLabel(m_hControlsWnd, "Формат:", 10, 57, 60, 25, 0, materialsId), materialsId++ };
	m_pTextureControls.m_cbFormat = { UI_CreateComboBox(m_hControlsWnd, 75, 54, 100, 25, 0, materialsId), materialsId++ };
	UI_ComboBoxAddLine(m_pTextureControls.m_cbFormat.hWnd, "RGB8");
	UI_ComboBoxAddLine(m_pTextureControls.m_cbFormat.hWnd, "RGB555");
	UI_ComboBoxAddLine(m_pTextureControls.m_cbFormat.hWnd, "RGB565");
	UI_ComboBoxAddLine(m_pTextureControls.m_cbFormat.hWnd, "RGBA4444");
	UI_ComboBoxAddLine(m_pTextureControls.m_cbFormat.hWnd, "RGBA8888");
	UI_ComboBoxAddLine(m_pTextureControls.m_cbFormat.hWnd, "BUMP4444");
	m_pTextureControls.m_chkNoLoad = { UI_CreateCheckBox(m_hControlsWnd, "Noload", 10, 110, 100, 25, 0, materialsId), materialsId++ };
	m_pTextureControls.m_chkMemfix = { UI_CreateCheckBox(m_hControlsWnd, "Memfix", 110, 110, 100, 25, 0, materialsId), materialsId++ };
	m_pTextureControls.m_chkMipmap = { UI_CreateCheckBox(m_hControlsWnd, "Use Mipmap", 210, 110, 75, 25, 0, materialsId), materialsId++ };
	m_pTextureControls.m_cbMipmap = { UI_CreateComboBox(m_hControlsWnd, 296, 112, 30, 25, 0, materialsId), materialsId++ };
	m_pTextureControls.m_btnApply = { UI_CreateButton(m_hControlsWnd, "Применить", 185, 52, 100, 25, 0, materialsId), materialsId++ };
	m_pTextureControls.m_btnImport = { UI_CreateButton(m_hControlsWnd, "Импорт", 295, 52, 100, 25, 0, materialsId), materialsId++ };
	m_pTextureControls.m_btnExport = { UI_CreateButton(m_hControlsWnd, "Экспорт", 295, 77, 100, 25, 0, materialsId), materialsId++ };
	m_pTextureControls.m_lblSize = { UI_CreateLabel(m_hControlsWnd, "Размер:", 10, 80, 60, 25, 0, materialsId), materialsId++ };
	m_pTextureControls.m_lblSizeValue = { UI_CreateLabel(m_hControlsWnd, "0x0", 75, 80, 100, 25, 0, materialsId), materialsId++ };

	m_pMaskControls.m_lblFormat = { UI_CreateLabel(m_hControlsWnd, "Формат:", 10, 57, 60, 25, 0, materialsId), materialsId++ };
	m_pMaskControls.m_cbFormat = { UI_CreateComboBox(m_hControlsWnd, 75, 54, 100, 25, 0, materialsId), materialsId++ };
	UI_ComboBoxAddLine(m_pMaskControls.m_cbFormat.hWnd, "MSK8");
	UI_ComboBoxAddLine(m_pMaskControls.m_cbFormat.hWnd, "MSK16");
	m_pMaskControls.m_chkNoLoad = { UI_CreateCheckBox(m_hControlsWnd, "Noload", 10, 110, 100, 25, 0, materialsId), materialsId++ };
	m_pMaskControls.m_btnApply = { UI_CreateButton(m_hControlsWnd, "Применить", 185, 52, 100, 25, 0, materialsId), materialsId++ };
	m_pMaskControls.m_btnImport = { UI_CreateButton(m_hControlsWnd, "Импорт", 295, 52, 100, 25, 0, materialsId), materialsId++ };
	m_pMaskControls.m_btnExport = { UI_CreateButton(m_hControlsWnd, "Экспорт", 295, 77, 100, 25, 0, materialsId), materialsId++ };
	m_pMaskControls.m_lblSize = { UI_CreateLabel(m_hControlsWnd, "Размер:", 10, 80, 60, 25, 0, materialsId), materialsId++ };
	m_pMaskControls.m_lblSizeValue = { UI_CreateLabel(m_hControlsWnd, "0x0", 75, 80, 100, 25, 0, materialsId), materialsId++ };

	m_hTextureMenu = CreatePopupMenu();
	InsertMenu(m_hTextureMenu, -1, MF_BYPOSITION | MF_STRING, 2600, "Импорт");
	InsertMenu(m_hTextureMenu, -1, MF_BYPOSITION | MF_STRING, 2601, "Экспорт");
	InsertMenu(m_hTextureMenu, -1, MF_BYPOSITION | MF_STRING, 2602, "Удалить");

	m_hTexturesMenu = CreatePopupMenu();
	//InsertMenu(m_hTexturesMenu, -1, MF_BYPOSITION | MF_STRING, 2610, "Экспорт всех");
	InsertMenu(m_hTexturesMenu, -1, MF_BYPOSITION | MF_STRING, 2611, "Добавить");

	m_hMaskMenu = CreatePopupMenu();
	InsertMenu(m_hMaskMenu, -1, MF_BYPOSITION | MF_STRING, 2620, "Импорт");
	InsertMenu(m_hMaskMenu, -1, MF_BYPOSITION | MF_STRING, 2621, "Экспорт");
	InsertMenu(m_hMaskMenu, -1, MF_BYPOSITION | MF_STRING, 2622, "Удалить");

	m_hMasksMenu = CreatePopupMenu();
	//InsertMenu(m_hMasksMenu, -1, MF_BYPOSITION | MF_STRING, 2630, "Экспорт всех");
	InsertMenu(m_hMasksMenu, -1, MF_BYPOSITION | MF_STRING, 2631, "Добавить");

	m_hViewWndMenu = CreatePopupMenu();
	InsertMenu(m_hViewWndMenu, -1, MF_BYPOSITION | MF_STRING, 2700, "Увеличить масшаб");
	InsertMenu(m_hViewWndMenu, -1, MF_BYPOSITION | MF_STRING, 2701, "Уменьшить масштаб");
	InsertMenu(m_hViewWndMenu, -1, MF_BYPOSITION | MF_STRING | MF_SEPARATOR, 2702, "");
	InsertMenu(m_hViewWndMenu, -1, MF_BYPOSITION | MF_STRING | MF_CHECKED, 2703, "Сетка");
	InsertMenu(m_hViewWndMenu, -1, MF_BYPOSITION | MF_STRING, 2704, "Черный");
	InsertMenu(m_hViewWndMenu, -1, MF_BYPOSITION | MF_STRING, 2705, "Белый");

	m_hModuleMenu = CreatePopupMenu();
	InsertMenu(m_hModuleMenu, -1, MF_BYPOSITION | MF_STRING, 2800, "Сохранить");
	InsertMenu(m_hModuleMenu, -1, MF_BYPOSITION | MF_STRING, 2801, "Сохранить как");
	InsertMenu(m_hModuleMenu, -1, MF_BYPOSITION | MF_STRING, 2802, "Закрыть");
	
	for (int i = 0; i < ARRAYSIZE(m_pMaterialControls.m_pAll); i++)
	{
		UI_SetElementFont(m_pMaterialControls.m_pAll[i].hWnd, hMainFont);
		ShowWindow(m_pMaterialControls.m_pAll[i].hWnd, 0);
	}
	ComboBox_Enable(m_pMaterialControls.m_cbTexture.hWnd, 0);
	ComboBox_Enable(m_pMaterialControls.m_cbMaterial.hWnd, 0);
	ComboBox_Enable(m_pMaterialControls.m_cbParMaterial.hWnd, 0);
	Edit_Enable(m_pMaterialControls.m_boxMoveX.hWnd, 0);
	Edit_Enable(m_pMaterialControls.m_boxMoveY.hWnd, 0);
	Edit_Enable(m_pMaterialControls.m_boxRotationX.hWnd, 0);
	Edit_Enable(m_pMaterialControls.m_boxRotationY.hWnd, 0);
	Edit_Enable(m_pMaterialControls.m_boxRotation.hWnd, 0);
	Edit_Enable(m_pMaterialControls.m_boxTransparent.hWnd, 0);

	for (int i = 0; i < ARRAYSIZE(m_pTextureControls.m_pAll); i++)
	{
		UI_SetElementFont(m_pTextureControls.m_pAll[i].hWnd, hMainFont);
		ShowWindow(m_pTextureControls.m_pAll[i].hWnd, 0);
	}
	for (int i = 0; i < ARRAYSIZE(m_pMaskControls.m_pAll); i++)
	{
		UI_SetElementFont(m_pMaskControls.m_pAll[i].hWnd, hMainFont);
		ShowWindow(m_pMaskControls.m_pAll[i].hWnd, 0);
	}

	m_hProgressWnd = CreateDialog(hInst,
		MAKEINTRESOURCE(IDD_DIALOGPROGRESS),
		m_hMainWnd,
		(DLGPROC)ProgressWndProc);
	m_hProgressBar = GetDlgItem(m_hProgressWnd, IDC_PROGRESS1);
	UI_ProgressBarSetRange(m_hProgressBar, 100);

	/*m_hColorsWnd = CreateDialog(hInst,
		MAKEINTRESOURCE(IDD_DIALOGCOLORS),
		m_hMainWnd,
		(DLGPROC)ProgressWndProc);
		*/
	m_pD3D = new D3D();
	m_pD3D->Init(0, m_hViewWnd);
	RECT rect;
	GetClientRect(m_hViewWnd, &rect);
	m_dwViewWidth = rect.right - rect.left;
	m_dwViewHeight = rect.bottom - rect.top;

	m_pSprite = 0;
	m_pSpriteTexture = 0;
	m_pBackSprite = 0;
	m_pBackTexture[0] = 0;
	m_pBackTexture[1] = 0;
	m_pBackTexture[2] = 0;
	m_dwActiveBackTexture = 0;
	InitBgSprite();
}

ResUI::~ResUI()
{
	m_pTree->Destroy();
	if (m_pBackSprite)
		delete m_pBackSprite;
	if (m_pSprite)
		delete m_pSprite;
	if (m_pCurrentMaterial)
		delete m_pCurrentMaterial;
	if (m_pCurrentTexture)
		delete m_pCurrentTexture;
	if (m_pPalette)
		delete m_pPalette;
	SafeRelease(m_pBackTexture[0]);
	SafeRelease(m_pBackTexture[1]);
	SafeRelease(m_pBackTexture[2]);
	SafeRelease(m_pSpriteTexture);

	for (auto it = m_pModules.begin(); it != m_pModules.end(); it++)
		delete *it;

	delete m_pD3D;
}

void ResUI::InitBgSprite()
{
	SafeRelease(m_pBackTexture[0]);
	SafeRelease(m_pBackTexture[1]);
	SafeRelease(m_pBackTexture[2]);

	if (m_pBackSprite)
		delete m_pBackSprite;

	if (!m_pBackTexture[0])
	{
		D3DCOLOR *data = (D3DCOLOR *)malloc(sizeof(D3DCOLOR)* 14 * 14);
		for (int i = 0; i < 14; i++)
		{
			for (int j = 0; j < 14; j++)
			{
				if (i < 7 && j < 7)
					data[i * 14 + j] = D3DCOLOR_RGBA(255, 255, 255, 255);
				else if (i < 7 && j >= 7)
					data[i * 14 + j] = D3DCOLOR_RGBA(208, 208, 208, 255);
				else if (i >= 7 && j < 7)
					data[i * 14 + j] = D3DCOLOR_RGBA(208, 208, 208, 255);
				else
					data[i * 14 + j] = D3DCOLOR_RGBA(255, 255, 255, 255);
			}
		}
		m_pBackTexture[0] = m_pD3D->CreateTexture(data, 14, 14);
		free(data);
	}
	if (!m_pBackTexture[1])
	{
		D3DCOLOR data = D3DCOLOR_XRGB(0, 0, 0);
		m_pBackTexture[1] = m_pD3D->CreateTexture(&data, 1, 1);
	}
	if (!m_pBackTexture[2])
	{
		D3DCOLOR data = D3DCOLOR_XRGB(255, 255, 255);
		m_pBackTexture[2] = m_pD3D->CreateTexture(&data, 1, 1);
	}
	m_pBackSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)m_dwViewWidth, (float)m_dwViewHeight },
		{ 0, 0 }, { (float)m_dwViewWidth / 14, (float)m_dwViewHeight / 14 },
		D3DCOLOR_RGBA(255, 255, 255, 255), m_pBackTexture[m_dwActiveBackTexture], false);
}

bool ResUI::SetupModuleEnv(GameModule *gm)
{
	m_pModules.push_back(gm);
	// заполним дерево
	std::string treeName;

	auto resFile = m_pTree->AddItem(gm->m_sName.c_str(), (DWORD)gm, FileType::FileRes);
	gm->m_pTreeItem = resFile;
	resFile->SetMenu(m_hModuleMenu);
	treeName = std::string("Palettes (") + std::to_string(gm->m_pPalettes.size()) + ")";
	auto palettesTree = resFile->AddChild(treeName.c_str(), 0, 0);
	gm->m_pPalettesTree = palettesTree;
	for (auto it = gm->m_pPalettes.begin(); it != gm->m_pPalettes.end(); it++)
	{
		palettesTree->AddChild((*it)->m_sName.c_str(), (DWORD)(*it), PaletteClick);
	}
	treeName = std::string("Soundfiles (") + std::to_string(gm->m_pSoundFiles.size()) + ")";
	auto soundfilesTree = resFile->AddChild(treeName.c_str(), 0, 0);
	gm->m_pSoundFilesTree = soundfilesTree;
	for (auto it = gm->m_pSoundFiles.begin(); it != gm->m_pSoundFiles.end(); it++)
	{
		soundfilesTree->AddChild((*it)->m_sName.c_str(), (DWORD)(*it), 0);
	}
	treeName = std::string("Sounds (") + std::to_string(gm->m_pSounds.size()) + ")";
	auto soundTree = resFile->AddChild(treeName.c_str(), 0, 0);
	gm->m_pSoundsTree = soundTree;
	for (auto it = gm->m_pSounds.begin(); it != gm->m_pSounds.end(); it++)
	{
		soundTree->AddChild((*it).c_str(), 0, 0);
	}
	treeName = std::string("Textures (") + std::to_string(gm->m_pTextures.size()) + ")";
	auto texturesTree = resFile->AddChild(treeName.c_str(), 0, 0);
	gm->m_pTexturesTree = texturesTree;
	texturesTree->SetMenu(m_hTexturesMenu);
	int i = 1;
	for (auto it = gm->m_pTextures.begin(); it != gm->m_pTextures.end(); it++)
	{
		auto texTree = texturesTree->AddChild(("[" + std::to_string(i++) + "] " + (*it)->m_sName).c_str(), 0, 0);
		texTree->SetMenu(m_hTextureMenu);
		texTree->SetCallback(TextureClick);
		texTree->SetData((DWORD)(*it));
		(*it)->SetTreeItem(texTree);
		(*it)->SetGameModule(gm);
	}
	i = 1;
	treeName = std::string("Masks (") + std::to_string(gm->m_pMasks.size()) + ")";
	auto masksTree = resFile->AddChild(treeName.c_str(), 0, 0);
	gm->m_pMasksTree = masksTree;
	masksTree->SetMenu(m_hMasksMenu);
	for (auto it = gm->m_pMasks.begin(); it != gm->m_pMasks.end(); it++)
	{
		auto masTree = masksTree->AddChild(("[" + std::to_string(i++) + "] " + (*it)->m_sName).c_str(), 0, 0);
		masTree->SetMenu(m_hMaskMenu);
		masTree->SetCallback(MaskClick);
		masTree->SetData((DWORD)(*it));
		(*it)->SetTreeItem(masTree);
		(*it)->SetGameModule(gm);
	}
	treeName = std::string("Materials (") + std::to_string(gm->m_pMaterials.size()) + ")";
	auto materialsTree = resFile->AddChild(treeName.c_str(), 0, 0);
	gm->m_pMaterialsTree = materialsTree;
	materialsTree->SetMenu(m_hMaterialsMenu);
	i = 1;
	for (auto it = gm->m_pMaterials.begin(); it != gm->m_pMaterials.end(); it++)
	{
		auto matTree = materialsTree->AddChild(("[" + std::to_string(i++) + "] " + (*it)->m_sName).c_str(), 0, 0);
		matTree->SetCallback(MaterialClick);
		matTree->SetData((DWORD)(*it));
		matTree->SetMenu(m_hMaterialMenu);
		(*it)->SetTreeItem(matTree);
		(*it)->SetGameModule(gm);
	}
	resFile->Expand();

	if (!m_pPalette && stricmp(gm->m_sName.c_str(), "load.res") != 0)
	{
		if (gm->m_pPalettes.size() > 0)
		{
			m_pPalette = new Palette();
			*m_pPalette = *gm->m_pPalettes[0];
		}
		else
		{
			// попробуем загрузить палитру из Load.res
			const char *filePath = FileManager::GetFilePath(gm->m_sFullPath.c_str());
			char *commonRes = SearchForCommonRes(filePath);
			bool commonFound = false;
			if (commonRes)
			{
				GameModule *mod = new GameModule();
				if (mod->LoadRes(commonRes, 0))
				{
					if (mod->m_pPalettes.size() > 0)
					{
						m_pPalette = new Palette();
						*m_pPalette = *mod->m_pPalettes[0];
						delete mod;
						commonFound = true;
					}
					else
					{
						delete mod;
					}
				}
			}
			if (!commonFound)
			{
				MessageBox(m_hMainWnd, "Для корректного отображения текстур и материалов\r\nтребуется палитра из файла Common.res", "Выберите Common.res", MB_OK | MB_ICONINFORMATION);
				char filename[512];
				strcpy(filename, "Common.res");
				while (true)
				{
					if (UI_FileOpenDlg(m_hMainWnd, filename, "common.res file (*.res)\0*.res\0\0", OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER))
					{
						GameModule *mod = new GameModule();
						if (mod->LoadRes(filename, 0))
						{
							if (mod->m_pPalettes.size() > 0)
							{
								m_pPalette = new Palette();
								*m_pPalette = *mod->m_pPalettes[0];
								delete mod;
								break;
							}
							else
							{
								delete mod;
							}
						}
					}
					else
					{
						break;
					}
				}
			}
		}
	}

	ResetViewUI();
	UpdateCurrentModule(gm, true);
	UpdateWindow(m_pTree->m_hWnd);

	return true;
}

bool ResUI::OpenModule(const char *path)
{
	const char *ext = FileManager::GetFileExtFromPath(path);
	if (ext && stricmp(ext, "fon") == 0)
	{
		ResTexture *tex = new ResTexture();
		const char *name = FileManager::GetFileNameFromPath(path);
		FILE *f = fopen(path, "rb");
		if (f)
		{
			tex->Load(f, name, true);
			fclose(f);
			auto texTree = m_pTree->AddItem(name, (DWORD)tex, FileType::FileFon);
			texTree->SetCallback(FontClick);
			return true;
		}
		else
		{
			delete tex;
		}
	}
	else
	{
		GameModule *gm = new GameModule();
		const char *fileName = FileManager::GetFileNameFromPath(path);
		if (!gm->LoadRes(path, LoadModuleCB))
		{
			delete gm;
			return false;
		}
		return SetupModuleEnv(gm);
	}
	return false;
}

bool ResUI::SaveModule(GameModule *module, const char *path)
{
	return module->SaveRes(path, LoadModuleCB);
}

bool ResUI::ProcessUICommand(HWND hWnd, int wmId, int wmCommand)
{
	if (m_bSetupUINow)
		return false;

	bool changed = false;
	HWND item = GetDlgItem(hWnd, wmId);
	DWORD style = GetWindowLong(item, GWL_STYLE);
	char className[512];
	GetClassName(item, className, 512); 
	if (stricmp(className, "BUTTON") == 0 && style & BS_CHECKBOX)
	{
		changed = true;
		if (UI_GetCheckBoxState(item))
			UI_SetCheckBoxState(item, 0);
		else
			UI_SetCheckBoxState(item, 1);
	}

	if (wmCommand == EN_CHANGE)
	{
		changed = true;
	}

	if (changed)
	{
		g_pInstance->ApplyUIChanges();
		g_pInstance->UpdateResourceView();
	}
	return false;
}

std::vector<std::string> ResUI::ParseFilesInput(const char *files)
{
	std::vector<std::string> result;
	const char *ptr = files;
	while (*ptr)
		ptr++;

	ptr++;
	if (!*ptr)
	{
		result.push_back(files);
	}
	else
	{
		std::string path = files;
		path += "\\";
		while (*ptr)
		{
			result.push_back(path + ptr);
			
			while (*ptr) // skip to end
				ptr++;

			ptr++;
		}
	}

	return result;
}

int ResUI::FixFilename(char *path, char *ext, int extId)
{
	if (path == ext)
	{
		if (extId == 1)
		{
			strcat(path, ".png");
			return 1;
		}
		else if (extId == 2)
		{
			strcat(path, ".tga");
			return 2;
		}
	}
	else
	{
		if (strcmp(ext, "png") == 0)
		{
			return 1;
		}
		else if (strcmp(ext, "tga") == 0)
		{
			return 2;
		}
	}
	return 1;
}

char *ResUI::SearchForCommonRes(const char *path)
{
	static char _path[512];
	static char _pathNormal[512];
	static char *pathes[] = { "common.res", "..\\common.res", "..\\common\\common.res" };
	for (int i = 0; i < ARRAYSIZE(pathes); i++)
	{
		strcpy(_path, path);
		strcat(_path, pathes[i]);
		GetFullPathNameA(_path, 512, _pathNormal, NULL);
		FILE *f = fopen(_pathNormal, "rb");
		if (f)
		{
			fclose(f);
			return _pathNormal;
		}
	}
	for (int i = 0; i < ARRAYSIZE(pathes); i++)
	{
		FILE *f = fopen(pathes[i], "rb");
		if (f)
		{
			fclose(f);
			return pathes[i];
		}
	}
	return 0;
}

void ResUI::RemoveTexture(WinTreeItem *item)
{
	if (item)
	{
		ResTexture *tex = (ResTexture *)item->GetData();
		if (tex)
		{
			if (m_pCurrentTextureReal == tex)
			{
				ResetViewUI();
			}
			GameModule *gm = tex->GetGameModule();
			int id = gm->GetTextureId(tex);
			gm->RemoveTexture(tex);
			auto next = item->GetNext();
			item->Destroy();
			while (next)
			{
				next->SetText(("[" + std::to_string(id+1) + "] " + gm->m_pTextures[id]->m_sName).c_str());
				next = next->GetNext();
				id++;
			}
			gm->m_pTexturesTree->SetText(std::string(std::string("Textures (") + std::to_string(m_pCurrentModule->m_pTextures.size()) + ")").c_str());
			UpdateCurrentModule(gm, true);
		}
	}
}

void ResUI::RemoveMaterial(WinTreeItem *item)
{
	if (item)
	{
		Material *mat = (Material *)item->GetData();
		if (mat)
		{
			if (m_pCurrentMaterialReal == mat)
			{
				ResetViewUI();
			}
			GameModule *gm = mat->GetGameModule();
			int id = gm->GetMaterialId(mat);
			gm->RemoveMaterial(mat);
			auto next = item->GetNext();
			item->Destroy();
			while (next)
			{
				next->SetText(("[" + std::to_string(id + 1) + "] " + gm->m_pMaterials[id]->m_sName).c_str());
				next = next->GetNext();
				id++;
			}
			gm->m_pMaterialsTree->SetText(std::string(std::string("Materials (") + std::to_string(m_pCurrentModule->m_pMaterials.size()) + ")").c_str());
			UpdateCurrentModule(gm, true);
		}
	}
}

void ResUI::RemoveMask(WinTreeItem *item)
{
	if (item)
	{
		Mask *mat = (Mask *)item->GetData();
		if (mat)
		{
			if (m_pCurrentMaskReal == mat)
			{
				ResetViewUI();
			}
			GameModule *gm = mat->GetGameModule();
			int id = gm->GetMaskId(mat);
			gm->RemoveMask(mat);
			auto next = item->GetNext();
			item->Destroy();
			while (next)
			{
				next->SetText(("[" + std::to_string(id + 1) + "] " + gm->m_pMasks[id]->m_sName).c_str());
				next = next->GetNext();
				id++;
			}
			gm->m_pMasksTree->SetText(std::string(std::string("Masks (") + std::to_string(m_pCurrentModule->m_pMasks.size()) + ")").c_str());
			UpdateCurrentModule(gm, true);
		}
	}
}

void ResUI::AddTexture(WinTreeItem *item)
{
	if (item)
	{
		auto modItem = item->GetParent();
		GameModule *mod = (GameModule *)modItem->GetData();
		if (mod)
		{
			ResTexture *newTex = new ResTexture();
			newTex->Blank();
			auto texTree = item->AddChild(("[" + std::to_string(mod->m_pTextures.size() + 1) + "] " + newTex->m_sName).c_str(), 0, 0);
			texTree->SetMenu(m_hTextureMenu);
			texTree->SetCallback(TextureClick);
			texTree->SetData((DWORD)newTex);
			newTex->SetTreeItem(texTree);
			newTex->SetGameModule(mod);

			mod->m_pTextures.push_back(newTex);
			UpdateCurrentModule(mod, true);
			mod->m_pTexturesTree->SetText(std::string(std::string("Textures (") + std::to_string(m_pCurrentModule->m_pTextures.size()) + ")").c_str());
			texTree->SetActive();
		}
	}
}

void ResUI::AddMaterial(WinTreeItem *item)
{
	if (item)
	{
		auto modItem = item->GetParent();
		GameModule *mod = (GameModule *)modItem->GetData();
		if (mod)
		{
			Material *newMat = new Material();
			auto matTree = item->AddChild(("[" + std::to_string(mod->m_pMaterials.size() + 1) + "] " + newMat->m_sName).c_str(), 0, 0);
			matTree->SetMenu(m_hMaterialMenu);
			matTree->SetCallback(MaterialClick);
			matTree->SetData((DWORD)newMat);
			newMat->SetTreeItem(matTree);
			newMat->SetGameModule(mod);

			mod->m_pMaterials.push_back(newMat);
			mod->m_pMaterialsTree->SetText(std::string(std::string("Materials (") + std::to_string(m_pCurrentModule->m_pMaterials.size()) + ")").c_str());
			UpdateCurrentModule(mod, true);
			matTree->SetActive();
		}
	}
}

void ResUI::AddMask(WinTreeItem *item)
{
	if (item)
	{
		auto modItem = item->GetParent();
		GameModule *mod = (GameModule *)modItem->GetData();
		if (mod)
		{
			Mask *msk = new Mask();
			msk->Blank();
			auto mskTree = item->AddChild(("[" + std::to_string(mod->m_pMasks.size() + 1) + "] " + msk->m_sName).c_str(), 0, 0);
			mskTree->SetMenu(m_hMaskMenu);
			mskTree->SetCallback(MaskClick);
			mskTree->SetData((DWORD)msk);
			msk->SetTreeItem(mskTree);
			msk->SetGameModule(mod);

			mod->m_pMasks.push_back(msk);
			UpdateCurrentModule(mod, true);
			mod->m_pMasksTree->SetText(std::string(std::string("Masks (") + std::to_string(m_pCurrentModule->m_pMasks.size()) + ")").c_str());
			mskTree->SetActive();
		}
	}
}

bool ResUI::ProcessEvents(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	WinTreeItem *item = 0;
	int controlsHeight = 300;
	LPMINMAXINFO lpMMI;
	GameModule *newGm = 0;
	switch (iMsg)
	{
		case WM_DROPFILES:
		{
			POINT ppt;
			DragQueryPoint((HDROP)wParam, &ppt);
			std::vector<std::string> files;
			HDROP fDrop = (HDROP)wParam;
			char fName[512];
			int count = DragQueryFile(fDrop, -1, 0, 0);
			for (int i = 0; i < count; i++) 
			{
				DragQueryFile(fDrop, i, (TCHAR*)fName, 512);
				files.push_back(fName);
			}
			
			DragFinish(fDrop);

			const char *ext = FileManager::GetFileExtFromPath(files[0].c_str());

			if (stricmp(ext, "res") != 0)
			{
				unsigned width = 0, height = 0;
				auto image = g_pInstance->LoadImageFromFile(files[0].c_str(), width, height);
				if (m_eCurrentView == ResUIViewType::ViewTexture) 
				{
					if (image.size() > 0)
					{
						UI_ComboBoxSetActiveLine(g_pInstance->m_pTextureControls.m_cbFormat.hWnd, 4);
						g_pInstance->m_pCurrentTexture->SetData(image.data(), ResTextureFormat::Texture8888, 0, width, height);
						g_pInstance->UpdateResourceView();
					}
				}
				else if (m_eCurrentView == ResUIViewType::ViewMask) 
				{
					if (image.size() > 0)
					{
						UI_ComboBoxSetActiveLine(g_pInstance->m_pMaskControls.m_cbFormat.hWnd, 1);
						g_pInstance->m_pCurrentMask->SetData(image.data(), 2, 0, width, height);
						g_pInstance->UpdateResourceView();
					}
				}
			}
			else 
			{
				OpenModules(files);
			}
			
			break;
		}
		case WM_GETMINMAXINFO:
			lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = 800;
			lpMMI->ptMinTrackSize.y = 400;
		break;
		case WM_COMMAND:
			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			item = m_pTree->GetSelected();
			// Разобрать выбор в меню:
			switch (wmId)
			{
			case ID_HELP_LINK:
				ShellExecuteW(NULL, L"open", L"https://rnr-wiki.ru/mods/tools/rignroll-res-editor/", NULL, NULL, SW_SHOW);
				break;

			case ID_ACC_ZOOM_IN:
				if (!g_pInstance->m_pSprite)
					break;
				g_pInstance->m_pSprite->Resize(2.0f);
				break;

			case ID_ACC_ZOOM_OUT:
				if (!g_pInstance->m_pSprite)
					break;
				g_pInstance->m_pSprite->Resize(0.5f);
				break;

			case ID_ACC_SAVE:
				if (m_pCurrentModule)
				{
					GameModule *mod = m_pCurrentModule;
					char title[512];
					UI_ProgressBarSetProgress(m_hProgressBar, 0);
					ShowWindow(m_hProgressWnd, SW_SHOW);
					sprintf(title, "Сохранение файла (%s)", FileManager::GetFileNameFromPath(mod->m_sFullPath.c_str()));
					SetWindowText(m_hProgressWnd, title);
					if (!SaveModule(mod, mod->m_sFullPath.c_str()))
					{
						MessageBox(hWnd, "Невозможно открыть файл!\nВозможно он занят другим процессом", "Ошибка", MB_OK | MB_ICONERROR);
					}
					ShowWindow(m_hProgressWnd, SW_HIDE);
				}
				break;

			case ID_BTN_ABOUT:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hWnd, AboutWndProc);
				break;
			case ID_BTN_OPEN:
				TCHAR filename[512];
				if (UI_FileOpenDlg(hWnd, filename, "RES files (*.res)\0*.res\0Fon files (*.fon)\0*.fon\0\0", OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER))
				{
					auto files = ParseFilesInput(filename);
					OpenModules(files);
				}
				break;

			case ID_BTN_NEW:
				newGm = new GameModule();
				newGm->m_sName = "NewFile.res";
				SetupModuleEnv(newGm);
				break;

			case 2800: // save?
				if (item)
				{
					GameModule *mod = (GameModule *)item->GetData();
					if (mod)
					{
						if (mod->m_bLoadedFromFile)
						{
							char title[512];
							UI_ProgressBarSetProgress(m_hProgressBar, 0);
							ShowWindow(m_hProgressWnd, SW_SHOW);
							sprintf(title, "Сохранение файла (%s)", FileManager::GetFileNameFromPath(mod->m_sFullPath.c_str()));
							SetWindowText(m_hProgressWnd, title);
							if (!SaveModule(mod, mod->m_sFullPath.c_str()))
							{
								MessageBox(hWnd, "Невозможно открыть файл!\nВозможно он занят другим процессом", "Ошибка", MB_OK | MB_ICONERROR);
							}
							ShowWindow(m_hProgressWnd, SW_HIDE);
						}
						else
						{
							char path[512];
							char title[512];
							strcpy(path, mod->m_sName.c_str());
							if (UI_FileSaveDlg(hWnd, path, "RES files (*.res)\0*.res\0\0"))
							{
								UI_ProgressBarSetProgress(m_hProgressBar, 0);
								ShowWindow(m_hProgressWnd, SW_SHOW);
								sprintf(title, "Сохранение файла (%s)", FileManager::GetFileNameFromPath(path));
								SetWindowText(m_hProgressWnd, title);
								if (!SaveModule(mod, path))
								{
									MessageBox(hWnd, "Невозможно открыть файл!\nВозможно он занят другим процессом", "Ошибка", MB_OK | MB_ICONERROR);
								}
								ShowWindow(m_hProgressWnd, SW_HIDE);
								mod->m_bLoadedFromFile = true;
								mod->m_sFullPath = path;
								mod->m_sName = FileManager::GetFileNameFromPath(path);
							}
						}
					}
				}
				break;

			case 2801: // save as
				if (item) 
				{
					GameModule *mod = (GameModule *)item->GetData();
					if (mod)
					{
						char path[512];
						char title[512];
						strcpy(path, FileManager::GetFileNameFromPath(mod->m_sName.c_str()));
						if (UI_FileSaveDlg(hWnd, path, "RES files (*.res)\0*.res\0\0"))
						{
							UI_ProgressBarSetProgress(m_hProgressBar, 0);
							ShowWindow(m_hProgressWnd, SW_SHOW);
							sprintf(title, "Сохранение файла (%s)", FileManager::GetFileNameFromPath(path));
							SetWindowText(m_hProgressWnd, title);
							if (!SaveModule(mod, path))
							{
								MessageBox(hWnd, "Невозможно открыть файл!\nВозможно он занят другим процессом", "Ошибка", MB_OK | MB_ICONERROR);
							}
							ShowWindow(m_hProgressWnd, SW_HIDE);
						}
					}
				}
				break;

			case 2802: //close
				if (item)
				{
					m_pCurrentModule = 0;
					ResetViewUI();
					GameModule *mod = (GameModule *)item->GetData();
					for (auto it = m_pModules.begin(); it != m_pModules.end(); it++)
					{
						if (*it == mod)
						{
							item->Destroy();
							ResetViewUI();
							delete mod;
							m_pModules.erase(it);
							break;
						}
					}
				}
				break;

			case ID_BTN_CLOSE:
				m_pCurrentModule = 0;
				ResetViewUI();
				m_pTree->Clear();
				for (auto it = m_pModules.begin(); it != m_pModules.end(); it++)
				{
					delete (*it);
				}
				m_pModules.clear();
				break;

			case ID_BTN_SAVE:
				UI_ProgressBarSetProgress(m_hProgressBar, 0);
				ShowWindow(m_hProgressWnd, SW_SHOW);
				for (int i = 0; i < m_pModules.size(); i++)
				{
					GameModule *mod = m_pModules[i];
					char title[512];
					UI_ProgressBarSetProgress(m_hProgressBar, 0);
					sprintf(title, "Сохранение файла %s (%d/%d)", FileManager::GetFileNameFromPath(mod->m_sFullPath.c_str()), i, m_pModules.size());
					SetWindowText(m_hProgressWnd, title);
					if (!SaveModule(mod, mod->m_sFullPath.c_str()))
					{
						MessageBox(hWnd, "Невозможно открыть файл!\nВозможно он занят другим процессом", "Ошибка", MB_OK | MB_ICONERROR);
					}
				}
				ShowWindow(m_hProgressWnd, SW_HIDE);
				break;

			case 2611: // add texture
				AddTexture(item);
				break;

			case 2510: // add material
				AddMaterial(item);
				break;

			case 2631: // add mask
				AddMask(item);
				break;

			case 2602: // remove texture
				RemoveTexture(item);
				break;

			case 2500: // remove material
				RemoveMaterial(item);
				break;

			case 2622: // remove mask
				RemoveMask(item);
				break;

			case 40019:
				SetBackground(0);
				break;

			case 40020:
				SetBackground(1);
				break;

			case 40021:
				SetBackground(2);
				break;

			case ID_BTN_POINT:
				m_pD3D->SetLinearSampler(false);
				CheckMenuItem(m_hMainWndMenu, ID_BTN_POINT, MF_CHECKED);
				CheckMenuItem(m_hMainWndMenu, ID_BTN_LINEAR, 0);
				break;

			case ID_BTN_LINEAR:
				m_pD3D->SetLinearSampler(true);
				CheckMenuItem(m_hMainWndMenu, ID_BTN_POINT, 0);
				CheckMenuItem(m_hMainWndMenu, ID_BTN_LINEAR, MF_CHECKED);
				break;

			default:
				return false;
			}
			break;

		case WM_SIZING:
		case WM_SIZE:
			ResetUISize();
			break;
		case WM_NOTIFY:
		case WM_CONTEXTMENU:
			if (m_pTree)
				m_pTree->ProcessEvent(hWnd, iMsg, wParam, lParam);
			break;
		default:
			return false;
	}
	return true;
}

LRESULT CALLBACK ResUI::ViewWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	static int mouseX = 0;
	static int mouseY = 0;
	static bool movingMouse = false;
	int tmpX, tmpY;
	static float oldX, oldY;
	switch (iMsg)
	{
		case WM_CONTEXTMENU:
			TrackPopupMenu(g_pInstance->m_hViewWndMenu, TPM_TOPALIGN | TPM_LEFTALIGN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, hWnd, NULL);
			break;

		case WM_LBUTTONDOWN:
			movingMouse = true;
			mouseX = GET_X_LPARAM(lParam);
			mouseY = GET_Y_LPARAM(lParam);
			if (g_pInstance->m_pSprite)
			{
				g_pInstance->m_pSprite->GetPos(&oldX, &oldY);
			}
			break;

		case WM_LBUTTONUP:
			movingMouse = false;
			break;

		case WM_MOUSEMOVE:
			if (!movingMouse)
				break;

			tmpX = GET_X_LPARAM(lParam);
			tmpY = GET_Y_LPARAM(lParam);
			if (g_pInstance->m_pSprite)
			{
				g_pInstance->m_pSprite->SetPos(oldX + (tmpX - mouseX), oldY + (tmpY - mouseY));
			}
			break;

		case WM_COMMAND:

			wmId = LOWORD(wParam);
			wmEvent = HIWORD(wParam);
			switch (wmId)
			{
			case 2700:
				if (!g_pInstance->m_pSprite)
					break;
				g_pInstance->m_pSprite->Resize(2.0f);
				break;

			case 2701:
				if (!g_pInstance->m_pSprite)
					break;
				g_pInstance->m_pSprite->Resize(0.5f);
				break;

			case 2703:
				g_pInstance->SetBackground(0);
				break;

			case 2704:
				g_pInstance->SetBackground(1);
				break;

			case 2705:
				g_pInstance->SetBackground(2);
				break;
			}
			break;

		default:
			break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

std::vector<unsigned char> ResUI::LoadImageFromFile(const char *filename, unsigned int &width, unsigned int &height) {
	std::vector<unsigned char> image;
	std::string fileName = filename;
	const char *ext = FileManager::GetFileExtFromPath(filename);
	if (stricmp(ext, "tga") == 0)
	{
		Targa *tga = Targa::LoadTargaImage(filename, SOURCE_FILE);
		if (tga)
		{
			width = tga->m_Header.width;
			height = tga->m_Header.height;
			image.resize(width * height * 4);
			tga->GetData(&image[0]);
			tga->Destroy();
		}
	}
	else
	{
		unsigned err = lodepng::decode(image, width, height, fileName);
		if (err)
		{
			MessageBox(g_pInstance->m_hMainWnd, lodepng_error_text(err), "Error", MB_OK);
			return image;
		}
	}

	image.shrink_to_fit();
	return image;
}

LRESULT CALLBACK ResUI::ControlsWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (iMsg)
	{
	case WM_CTLCOLORBTN:
		if (g_pInstance->m_pCurrentMaterial && g_pInstance->m_pCurrentMaterial->m_dwColorId >= 0)
		{
			if ((HWND)lParam == g_pInstance->m_pMaterialControls.m_btnColor.hWnd)
				return (LRESULT)g_pInstance->m_pPalette->m_pBrushes[g_pInstance->m_pCurrentMaterial->m_dwColorId];
		}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		if (wmEvent == CBN_SELCHANGE)
		{
			if (g_pInstance->m_pTextureControls.m_cbMipmap.dwID == wmId)
			{
				int mipid = UI_ComboBoxGetCurrentLine(g_pInstance->m_pTextureControls.m_cbMipmap.hWnd);
				if (g_pInstance->m_pCurrentTexture)
				{
					g_pInstance->ViewTextureMipmap(mipid);
				}
			}
			else
			{
				g_pInstance->ApplyUIChanges();
				g_pInstance->UpdateResourceView();
			}
		}
		else
		{
			if (wmId == g_pInstance->m_pTextureControls.m_btnApply.dwID) 
			{
				if (g_pInstance->m_pCurrentTexture)
				{
					ResTexture *tex = g_pInstance->m_pCurrentTexture;
					int format = UI_ComboBoxGetActiveLine(g_pInstance->m_pTextureControls.m_cbFormat.hWnd);
					if (format == 0)
						format = ResTextureFormat::Texture8;
					else if (format == 1)
						format = ResTextureFormat::Texture555;
					else if (format == 2)
						format = ResTextureFormat::Texture565;
					else if (format == 3)
						format = ResTextureFormat::Texture4444;
					else if (format == 4)
						format = ResTextureFormat::Texture8888;
					else if (format == 5)
						format = ResTextureFormat::TextureBump;

					void *data = malloc(tex->m_iWidth * tex->m_iHeight * sizeof(D3DCOLOR));
					Palette *plt = 0;
					if (g_pInstance->m_pCurrentModule->m_pPalettes.size() > 0)
						plt = g_pInstance->m_pCurrentModule->m_pPalettes[0];
					else
						plt = g_pInstance->m_pPalette;
					g_pInstance->m_pCurrentTexture->GetData(data, plt);
					g_pInstance->m_pCurrentTexture->SetData(data, format, plt);
					free(data);
					g_pInstance->UpdateResourceView();
				}
			}
			else if (wmId == g_pInstance->m_pTextureControls.m_btnImport.dwID)
			{
				char filename[512];
				char *ext = 0;
				if (UI_FileOpenDlg(hWnd, filename, "Image files (*.png, *.tga)\0*.png;*.tga\0\0", OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER, 0, 0, &ext))
				{
					unsigned width = 0, height = 0;
					auto image = g_pInstance->LoadImageFromFile(filename, width, height);
					if (image.size() > 0)
					{
						UI_ComboBoxSetActiveLine(g_pInstance->m_pTextureControls.m_cbFormat.hWnd, 4);
						g_pInstance->m_pCurrentTexture->SetData(image.data(), ResTextureFormat::Texture8888, 0, width, height);
						g_pInstance->UpdateResourceView();
					}
					break;
				}
			}
			else if (wmId == g_pInstance->m_pTextureControls.m_btnExport.dwID)
			{
				char path[512];
				strcpy(path, FileManager::GetFileNameWithoutExt(g_pInstance->m_pCurrentTexture->m_sName.c_str()));
				//strcat(path, ".png");
				char *ext = 0; int filterId = 1;
				if (UI_FileSaveDlg(hWnd, path, "Png files (*.png)\0*.png\0Tga files (*.tga)\0*.tga\0\0", OFN_PATHMUSTEXIST, 0, 0, &ext, &filterId))
				{
					int typeId = FixFilename(path, ext, filterId);
					ResTexture *tex = g_pInstance->m_pCurrentTexture;
					std::vector<unsigned char> pData;
					unsigned char *data = (unsigned char *)malloc(tex->m_iWidth * tex->m_iHeight * 4);
					Palette *plt = 0;
					if (g_pInstance->m_pCurrentModule->m_pPalettes.size() > 0)
						plt = g_pInstance->m_pCurrentModule->m_pPalettes[0];
					else
						plt = g_pInstance->m_pPalette;
					g_pInstance->m_pCurrentTexture->GetData(data, plt);
					//pData.reserve(tex->m_iWidth * tex->m_iHeight * tex->m_bDepth / 8);
					if (typeId == 2)
					{
						Targa *tga = Targa::LoadTargaImage(tex->m_iWidth, tex->m_iHeight, data);
						tga->SaveTargaImage(path);
						tga->Destroy();
					}
					else
					{
						for (int i = 0; i < tex->m_iWidth * tex->m_iHeight * 4; i++)
						{
							pData.push_back(data[i]);
						}
						unsigned err = lodepng::encode(path, pData, tex->m_iWidth, tex->m_iHeight, LodePNGColorType::LCT_RGBA);
						if (err)
						{
							MessageBox(hWnd, lodepng_error_text(err), "Error", MB_OK);
							break;
						}
					}
				}
			}
			else if (wmId == g_pInstance->m_pMaskControls.m_btnApply.dwID)
			{
				if (g_pInstance->m_pCurrentMask)
				{
					Mask *msk = g_pInstance->m_pCurrentMask;
					int format = UI_ComboBoxGetActiveLine(g_pInstance->m_pMaskControls.m_cbFormat.hWnd) + 1;

					void *data = malloc(msk->m_wWidth * msk->m_wHeight * sizeof(D3DCOLOR));
					Palette *plt = 0;
					if (g_pInstance->m_pCurrentModule->m_pPalettes.size() > 0)
						plt = g_pInstance->m_pCurrentModule->m_pPalettes[0];
					else
						plt = g_pInstance->m_pPalette;
					g_pInstance->m_pCurrentMask->GetData(data, plt);
					g_pInstance->m_pCurrentMask->SetData(data, format, plt);
					free(data);
					g_pInstance->UpdateResourceView();
				}
			}
			else if (wmId == g_pInstance->m_pMaskControls.m_btnImport.dwID)
			{
				char filename[512];
				char *ext = 0;
				if (UI_FileOpenDlg(hWnd, filename, "Image files (*.png, *.tga)\0*.png;*.tga\0\0", OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER, 0, 0, &ext))
				{
					unsigned width = 0, height = 0;
					auto image = g_pInstance->LoadImageFromFile(filename, width, height);
					if (image.size() > 0)
					{
						UI_ComboBoxSetActiveLine(g_pInstance->m_pMaskControls.m_cbFormat.hWnd, 1);
						g_pInstance->m_pCurrentMask->SetData(image.data(), 2, 0, width, height);
						g_pInstance->UpdateResourceView();
					}
				}
			}
			else if (wmId == g_pInstance->m_pMaskControls.m_btnExport.dwID)
			{
				char path[512];
				strcpy(path, FileManager::GetFileNameWithoutExt(g_pInstance->m_pCurrentMask->m_sName.c_str()));
				//strcat(path, ".png");
				char *ext = 0; 
				int filterId = 1;
				if (UI_FileSaveDlg(hWnd, path, "Png files (*.png)\0*.png\0Tga files (*.tga)\0*.tga\0\0", OFN_PATHMUSTEXIST, 0, 0, &ext, &filterId))
				{
					int typeId = FixFilename(path, ext, filterId);
					Mask *msk = g_pInstance->m_pCurrentMask;
					std::vector<unsigned char> pData;
					unsigned char *data = (unsigned char *)malloc(msk->m_wWidth * msk->m_wHeight * 4);
					Palette *plt = 0;
					if (g_pInstance->m_pCurrentModule->m_pPalettes.size() > 0)
						plt = g_pInstance->m_pCurrentModule->m_pPalettes[0];
					else
						plt = g_pInstance->m_pPalette;
					g_pInstance->m_pCurrentMask->GetData(data, plt);
					//pData.reserve(tex->m_iWidth * tex->m_iHeight * tex->m_bDepth / 8);
					for (int i = 0; i < msk->m_wWidth * msk->m_wHeight * 4; i++)
					{
						pData.push_back(data[i]);
					}
					unsigned err = lodepng::encode(path, pData, msk->m_wWidth, msk->m_wHeight, LodePNGColorType::LCT_RGBA, 8);
					if (err)
					{
						MessageBox(hWnd, lodepng_error_text(err), "Error", MB_OK);
						break;
					}
				}
			}
			else if (g_pInstance->m_pMaterialControls.m_btnColor.dwID == wmId)
			{
				int colorid = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOGCOLORS), g_pInstance->m_hMainWnd, ColorsWndProc);
				if (colorid >= 0)
				{
					g_pInstance->m_pCurrentMaterial->m_dwColorId = colorid;
					g_pInstance->UpdateResourceView();
				}
			}

			switch (wmId)
			{
			case ID_RESUI_BTN_SAVE:
				g_pInstance->SaveCurrentResource();
				break;

			case ID_RESUI_BTN_RESET:
				g_pInstance->ResetResource();
				break;

			default:
				return g_pInstance->ProcessUICommand(hWnd, wmId, wmEvent);
			}
		}
		
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
}

BOOL CALLBACK ResUI::ProgressWndProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndOwner;
	RECT rcOwner, rcDlg;
	RECT rc;
	switch (message)
	{
	case WM_SHOWWINDOW:

		// Get the owner window and dialog box rectangles. 

		if ((hwndOwner = GetParent(hwndDlg)) == NULL)
		{
			hwndOwner = GetDesktopWindow();
		}

		GetWindowRect(hwndOwner, &rcOwner);
		GetWindowRect(hwndDlg, &rcDlg);
		CopyRect(&rc, &rcOwner);

		// Offset the owner and dialog box rectangles so that right and bottom 
		// values represent the width and height, and then offset the owner again 
		// to discard space taken up by the dialog box. 

		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

		// The new position is the sum of half the remaining space and the owner's 
		// original position. 

		SetWindowPos(hwndDlg,
			HWND_TOP,
			rcOwner.left + (rc.right / 2),
			rcOwner.top + (rc.bottom / 2),
			0, 0,          // Ignores size arguments. 
			SWP_NOSIZE);

		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK ResUI::AboutWndProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndOwner;
	RECT rcOwner, rcDlg;
	RECT rc;
	DWORD wmId, wmEvent;
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CLICK:
		case NM_RETURN:
		{
			PNMLINK pNMLink = (PNMLINK)lParam;
			LITEM   item = pNMLink->item;

			ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);

			break;
		}
		}

		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{

		case IDOK:
			EndDialog(hwndDlg, 0);
			break;
		}
		break;
	case WM_SHOWWINDOW:

		// Get the owner window and dialog box rectangles. 

		if ((hwndOwner = GetParent(hwndDlg)) == NULL)
		{
			hwndOwner = GetDesktopWindow();
		}

		GetWindowRect(hwndOwner, &rcOwner);
		GetWindowRect(hwndDlg, &rcDlg);
		CopyRect(&rc, &rcOwner);

		// Offset the owner and dialog box rectangles so that right and bottom 
		// values represent the width and height, and then offset the owner again 
		// to discard space taken up by the dialog box. 

		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

		// The new position is the sum of half the remaining space and the owner's 
		// original position. 

		SetWindowPos(hwndDlg,
			HWND_TOP,
			rcOwner.left + (rc.right / 2),
			rcOwner.top + (rc.bottom / 2),
			0, 0,          // Ignores size arguments. 
			SWP_NOSIZE);

		return TRUE;
	}
	return FALSE;
}

BOOL CALLBACK ResUI::ColorsWndProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH selectedBrush = 0;
	static HWND buttons[256];
	static HWND selectedButton;
	static HWND hTip;
	HWND hwndOwner;
	RECT rcOwner, rcDlg;
	RECT rc;
	int wmId, wmEvent;
	static int selectedColor = 0;
	selectedColor = g_pInstance->m_pCurrentMaterial ? g_pInstance->m_pCurrentMaterial->m_dwColorId : 0;
	TOOLINFO ti = { 0 };
	switch (message)
	{
	case WM_CLOSE:
		DeleteObject(selectedBrush);
		EndDialog(hwndDlg, -1);
		break;
	case WM_CTLCOLORBTN:
		if (g_pInstance->m_pPalette)
		{
			if ((HWND)lParam == selectedButton)
				return (LRESULT)selectedBrush;
			int id = 0;
			for (int i = 0; i < 256; i++)
			{
				if (buttons[i] == (HWND)lParam)
				{
					id = i;
					break;
				}
			}
			return (LRESULT)g_pInstance->m_pPalette->m_pBrushes[id];
		}
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		if (wmId == ID_RESUI_BTN_COLORS + 256)
			break;
		EndDialog(hwndDlg, wmId - ID_RESUI_BTN_COLORS);
		break;
	case WM_INITDIALOG:
		hTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
			WS_POPUP | TTS_ALWAYSTIP,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			hwndDlg, NULL,
			(HINSTANCE)GetModuleHandle(NULL), NULL);

		ti.cbSize = sizeof(TOOLINFO);
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		ti.hwnd = hwndDlg;
		//ti.hinst = (HINSTANCE)GetModuleHandle(NULL);

		selectedBrush = CreateSolidBrush(RGB(255, 255, 0));
		selectedButton = UI_CreateButton(hwndDlg, "", 3 + (selectedColor % 16) * 28, 3 + (selectedColor / 16) * 28, 29, 29, BS_OWNERDRAW, ID_RESUI_BTN_COLORS + 256);
		for (int i = 0; i < 256; i++)
		{
			static char buf[512];
			auto &plt = g_pInstance->m_pPalette->m_pPalette;
			if (i == selectedColor)
				buttons[i] = UI_CreateButton(hwndDlg, "", 6 + (i % 16) * 28, 6 + (i / 16) * 28, 23, 23, BS_OWNERDRAW, ID_RESUI_BTN_COLORS + i);
			else
				buttons[i] = UI_CreateButton(hwndDlg, "", 5 + (i % 16) * 28, 5 + (i / 16) * 28, 25, 25, BS_OWNERDRAW | WS_BORDER, ID_RESUI_BTN_COLORS + i);

			sprintf(buf, "RGB: %d %d %d\r\nWeb: #%02X%02X%02X", plt[i].R, plt[i].G, plt[i].B, plt[i].R, plt[i].G, plt[i].B);
			ti.uId = i == selectedColor ? (UINT_PTR)selectedButton : (UINT_PTR)buttons[i];
			ti.lpszText = buf;
			SendMessage(hTip, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
		}
		SendMessage(hTip, TTM_SETMAXTIPWIDTH, 0, 150);
		//BringWindowToTop(buttons[selectedColor]);
		//SetWindowPos(buttons[selectedColor], HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		break;
	case WM_SHOWWINDOW:

		// Get the owner window and dialog box rectangles. 

		if ((hwndOwner = GetParent(hwndDlg)) == NULL)
		{
			hwndOwner = GetDesktopWindow();
		}

		GetWindowRect(hwndOwner, &rcOwner);
		GetWindowRect(hwndDlg, &rcDlg);
		CopyRect(&rc, &rcOwner);

		// Offset the owner and dialog box rectangles so that right and bottom 
		// values represent the width and height, and then offset the owner again 
		// to discard space taken up by the dialog box. 

		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

		// The new position is the sum of half the remaining space and the owner's 
		// original position. 

		SetWindowPos(hwndDlg,
			HWND_TOP,
			rcOwner.left + (rc.right / 2),
			rcOwner.top + (rc.bottom / 2),
			0, 0,          // Ignores size arguments. 
			SWP_NOSIZE);

		return TRUE;
	}
	return FALSE;
}

void CALLBACK ResUI::LoadModuleCB(GameModule *gm, int progress, int total)
{
	if (progress == 0) 
	{
		UI_ProgressBarSetRange(g_pInstance->m_hProgressBar, total);
	}

	if (progress == total)
	{
		UI_ProgressBarSetRange(g_pInstance->m_hProgressBar, total + 1);
	}
	UI_ProgressBarSetProgress(g_pInstance->m_hProgressBar, progress+1); // fix slow progress bar
	UI_ProgressBarSetProgress(g_pInstance->m_hProgressBar, progress);
	UpdateWindow(g_pInstance->m_hProgressBar);
	UpdateWindow(g_pInstance->m_hProgressWnd);
}

void CALLBACK ResUI::GenerateMipmapsCB(int progress, int total)
{
	if (progress == 0)
	{
		UI_ProgressBarSetRange(g_pInstance->m_hProgressBar, total);
	}

	if (progress == total)
	{
		UI_ProgressBarSetRange(g_pInstance->m_hProgressBar, total + 1);
	}
	UI_ProgressBarSetProgress(g_pInstance->m_hProgressBar, progress + 1); // fix slow progress bar
	UI_ProgressBarSetProgress(g_pInstance->m_hProgressBar, progress);
	UpdateWindow(g_pInstance->m_hProgressBar);
	UpdateWindow(g_pInstance->m_hProgressWnd);
}

void ResUI::Render()
{
	float color[4] = {1.0, 0.5, 0, 1.0f};
	m_pD3D->Begin(color, 1.0f, 0);
	if (m_pBackSprite)
		m_pD3D->DrawSprite(m_pBackSprite);

	bool state = m_pD3D->GetLinearSampler();
	if (m_eCurrentView == ResUIViewType::ViewPalette)
		m_pD3D->SetLinearSampler(false);

	if (m_pSprite)
		m_pD3D->DrawSprite(m_pSprite);

	m_pD3D->SetLinearSampler(state);
	m_pD3D->End();
}

void CALLBACK ResUI::PaletteClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type)
{
	auto root = item->GetRoot();
	GameModule *mod = (GameModule*)root->GetData();
	g_pInstance->UpdateCurrentModule(mod);
	if (msg == TVN_SELCHANGED)
	{
		Palette *tex = (Palette *)item->GetData();
		if (tex)
		{
			g_pInstance->ViewPalette(tex);
		}
	}
}

void CALLBACK ResUI::TextureClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type)
{
	auto root = item->GetRoot();
	GameModule *mod = (GameModule*)root->GetData();
	g_pInstance->UpdateCurrentModule(mod);
	if (msg == TVN_SELCHANGED)
	{
		ResTexture *tex = (ResTexture *)item->GetData();
		if (tex)
		{
			g_pInstance->ViewTexture(tex);
		}
	}
}

void CALLBACK ResUI::FontClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type)
{
	if (msg == TVN_SELCHANGED)
	{
		ResTexture *tex = (ResTexture *)item->GetData();
		if (tex)
		{
			g_pInstance->ViewFont(tex);
		}
	}
}

void CALLBACK ResUI::MaskClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type)
{
	auto root = item->GetRoot();
	GameModule *mod = (GameModule*)root->GetData();
	g_pInstance->UpdateCurrentModule(mod);
	if (msg == TVN_SELCHANGED)
	{
		Mask *tex = (Mask *)item->GetData();
		if (tex)
		{
			g_pInstance->ViewMask(tex);
		}
	}
}

void CALLBACK ResUI::MaterialClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type)
{
	auto root = item->GetRoot();
	GameModule *mod = (GameModule*)root->GetData();
	g_pInstance->UpdateCurrentModule(mod);
	if (msg == TVN_SELCHANGED)
	{
		Material *mat = (Material *)item->GetData();
		if (mat)
		{
			g_pInstance->ViewMaterial(mat);
		}
	}
}

void ResUI::SetBackground(int id)
{
	m_dwActiveBackTexture = id;
	m_pBackSprite->SetTexture(g_pInstance->m_pBackTexture[g_pInstance->m_dwActiveBackTexture]);
	
	CheckMenuItem(m_hViewWndMenu, 2703, id == 0 ? MF_CHECKED : 0);
	CheckMenuItem(m_hViewWndMenu, 2704, id == 1 ? MF_CHECKED : 0);
	CheckMenuItem(m_hViewWndMenu, 2705, id == 2 ? MF_CHECKED : 0);
	CheckMenuItem(m_hMainWndMenu, 40019, id == 0 ? MF_CHECKED : 0);
	CheckMenuItem(m_hMainWndMenu, 40020, id == 1 ? MF_CHECKED : 0);
	CheckMenuItem(m_hMainWndMenu, 40021, id == 2 ? MF_CHECKED : 0);
}

void ResUI::ViewPalette(Palette *plm)
{
	ResetViewUI();

	if (!plm)
		return;

	D3DCOLOR *data = (D3DCOLOR *)malloc(256 * sizeof(D3DCOLOR));
	for (int i = 0; i < 256; i++)
	{
		data[i] = D3DCOLOR_XRGB(plm->m_pPalette[i].R, plm->m_pPalette[i].G, plm->m_pPalette[i].B);
	}

	m_pSpriteTexture = m_pD3D->CreateTexture(data, 16, 16);
	free(data);
	m_pSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)m_dwViewWidth, (float)m_dwViewHeight }, { 0, 0 }, { 1.f, 1.f }, D3DCOLOR_XRGB(255, 255, 255), m_pSpriteTexture);
	//ResetViewUI();
	ShowUI(ResUIViewType::ViewPalette);
}

void ResUI::ViewTexture(ResTexture *tex)
{
	ResetViewUI();

	if (!tex)
		return;

	m_bSetupUINow = true;

	m_pCurrentTexture = new ResTexture(*tex);
	m_pCurrentTextureReal = tex;
	tex = m_pCurrentTexture;
	
	D3DCOLOR *data = (D3DCOLOR *)malloc(tex->m_iWidth * tex->m_iHeight * sizeof(D3DCOLOR));
	Palette *plt = 0;
	if (m_pCurrentModule->m_pPalettes.size() > 0)
		plt = m_pCurrentModule->m_pPalettes[0];
	else
		plt = m_pPalette;
	tex->GetData(data, plt);
	m_pSpriteTexture = m_pD3D->CreateTexture(data, tex->m_iWidth, tex->m_iHeight);
	free(data);
	m_pSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)tex->m_iWidth, (float)tex->m_iHeight }, { 0, 0 }, {1.f, 1.f}, D3DCOLOR_XRGB(255, 255, 255), m_pSpriteTexture);

	int format = 0;
	if (tex->m_bFormat == 2)
		format = 2;
	else if (tex->m_bFormat == 3)
		format = 1;
	else if (tex->m_bFormat == 5)
		format = 3;
	else if (tex->m_bFormat == 6)
		format = 4;
	else if (tex->m_bFormat == 7)
		format = 4;
	else if (tex->m_bFormat == 8)
		format = 5;

	UI_ComboBoxSetActiveLine(m_pTextureControls.m_cbFormat.hWnd, format);
	UI_SetLabelText(m_pTextureControls.m_lblSizeValue.hWnd, std::string(std::to_string(tex->m_iWidth) + "x" + std::to_string(tex->m_iHeight)).c_str());

	UI_SetCheckBoxState(m_pTextureControls.m_chkNoLoad.hWnd, tex->m_bNoLoad);
	UI_SetCheckBoxState(m_pTextureControls.m_chkMemfix.hWnd, tex->m_bMemfix);
	UI_SetCheckBoxState(m_pTextureControls.m_chkMipmap.hWnd, tex->m_dwMipmapCount > 0);

	UI_ResetComboBox(m_pTextureControls.m_cbMipmap.hWnd);
	if (tex->m_dwMipmapCount)
	{
		ComboBox_Enable(m_pTextureControls.m_cbMipmap.hWnd, 1);
		for (int i = 0; i <= tex->m_dwMipmapCount; i++)
		{
			UI_ComboBoxAddLine(m_pTextureControls.m_cbMipmap.hWnd, std::to_string(i).c_str());
		}
		UI_ComboBoxSetActiveLine(m_pTextureControls.m_cbMipmap.hWnd, 0);
	}
	else
	{
		ComboBox_Enable(m_pTextureControls.m_cbMipmap.hWnd, 0);
	}

	SetWindowText(m_hBoxItemName, tex->m_sName.c_str());
	ShowUI(ResUIViewType::ViewTexture);

	m_bSetupUINow = false;
}

void ResUI::ViewFont(ResTexture *tex)
{
	ResetViewUI();

	if (!tex)
		return;

	m_bSetupUINow = true;

	m_pCurrentTexture = new ResTexture(*tex);
	m_pCurrentTextureReal = tex;
	tex = m_pCurrentTexture;

	D3DCOLOR *data = (D3DCOLOR *)malloc(tex->m_iWidth * tex->m_iHeight * sizeof(D3DCOLOR));
	Palette *plt = 0;
	if (m_pCurrentModule && m_pCurrentModule->m_pPalettes.size() > 0)
		plt = m_pCurrentModule->m_pPalettes[0];
	else
		plt = m_pPalette;
	tex->GetData(data, plt);
	m_pSpriteTexture = m_pD3D->CreateTexture(data, tex->m_iWidth, tex->m_iHeight);
	free(data);
	m_pSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)tex->m_iWidth, (float)tex->m_iHeight }, { 0, 0 }, { 1.f, 1.f }, D3DCOLOR_XRGB(255, 255, 255), m_pSpriteTexture);

	SetWindowText(m_hBoxItemName, tex->m_sName.c_str());
	ShowUI(ResUIViewType::ViewFont);

	m_bSetupUINow = false;
}

void ResUI::ViewTextureMipmap(int id)
{
	if (!id)
	{
		ViewTexture(m_pCurrentTextureReal);
		return;
	}
	id--;

	if (m_pSprite)
	{
		delete m_pSprite;
		m_pSprite = 0;
	}
	SafeRelease(m_pSpriteTexture);

	int width, height;
	m_pCurrentTexture->GetMipmapData(id, 0, width, height);

	D3DCOLOR *data = (D3DCOLOR *)malloc(width * height * sizeof(D3DCOLOR));
	m_pCurrentTexture->GetMipmapData(id, data, width, height);
	m_pSpriteTexture = m_pD3D->CreateTexture(data, width, height);
	free(data);
	m_pSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)width, (float)height }, { 0, 0 }, { 1.f, 1.f }, D3DCOLOR_XRGB(255, 255, 255), m_pSpriteTexture);
}

void ResUI::ViewMaterial(Material *mat)
{
	ResetViewUI();

	if (!mat)
		return;

	m_bSetupUINow = true;

	m_pCurrentMaterial = new Material(*mat);
	m_pCurrentMaterialReal = mat;
	mat = m_pCurrentMaterial;

	int width = 200;
	int height = 200;
	DWORD flags = mat->m_dwFlags;

	if (flags & MATERIAL_HAS_TEXTURE)
	{
		ResTexture *tex = m_pCurrentModule->m_pTextures[mat->m_dwTextureId];
		D3DCOLOR *data = (D3DCOLOR *)malloc(tex->m_iWidth * tex->m_iHeight * sizeof(D3DCOLOR));
		Palette *plt = 0;
		if (m_pPalette)
			plt = m_pPalette;
		tex->GetData(data, plt);
		m_pSpriteTexture = m_pD3D->CreateTexture(data, tex->m_iWidth, tex->m_iHeight);
		free(data);
		width = tex->m_iWidth;
		height = tex->m_iHeight;
	}

	BYTE r = 255, g = 255, b = 255, a = 255;

	char buf[128];
	if (flags & MATERIAL_USE_COL && !(flags & MATERIAL_HAS_TTX))
	{
		if (m_pPalette)
		{
			r = m_pPalette->m_pPalette[mat->m_dwColorId].R;
			g = m_pPalette->m_pPalette[mat->m_dwColorId].G;
			b = m_pPalette->m_pPalette[mat->m_dwColorId].B;
		}
	}
	if (flags & MATERIAL_IS_TRANSPARENT)
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkTransparent.hWnd, 1);
		Edit_Enable(m_pMaterialControls.m_boxTransparent.hWnd, 1);
		sprintf(buf, "%g", mat->m_fTransparency);
		SetWindowText(m_pMaterialControls.m_boxTransparent.hWnd, buf);
		a = (BYTE)(mat->m_fTransparency * 255.f);
	}
	else
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkTransparent.hWnd, 0);
		Edit_Enable(m_pMaterialControls.m_boxTransparent.hWnd, 0);
		SetWindowText(m_pMaterialControls.m_boxTransparent.hWnd, "");

	}

	m_pSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)width, (float)height }, { 0, 0 }, { 1.f, 1.f }, D3DCOLOR_ARGB(a, r, g, b), m_pSpriteTexture);

	if (flags & MATERIAL_HAS_TEXTURE)
	{
		ComboBox_Enable(m_pMaterialControls.m_cbTexture.hWnd, 1);
		UI_ComboBoxSetActiveLine(m_pMaterialControls.m_cbTexture.hWnd, mat->m_dwTextureId);
		UI_SetCheckBoxState(m_pMaterialControls.m_chkTexture.hWnd, 1);
	}
	else
	{
		ComboBox_Enable(m_pMaterialControls.m_cbTexture.hWnd, 0);
		UI_ComboBoxSetActiveLine(m_pMaterialControls.m_cbTexture.hWnd, -1);
		UI_SetCheckBoxState(m_pMaterialControls.m_chkTexture.hWnd, 0);
	}

	if (flags & MATERIAL_ATT)
	{
		ComboBox_Enable(m_pMaterialControls.m_cbMaterial.hWnd, 1);
		UI_ComboBoxSetActiveLine(m_pMaterialControls.m_cbMaterial.hWnd, mat->m_dwInternalId);
		UI_SetCheckBoxState(m_pMaterialControls.m_chkMaterial.hWnd, 1);
	}
	else
	{
		ComboBox_Enable(m_pMaterialControls.m_cbMaterial.hWnd, 0);
		UI_ComboBoxSetActiveLine(m_pMaterialControls.m_cbMaterial.hWnd, -1);
		UI_SetCheckBoxState(m_pMaterialControls.m_chkMaterial.hWnd, 0);
	}

	if (mat->m_dwParent >= 0)
	{
		ComboBox_Enable(m_pMaterialControls.m_cbParMaterial.hWnd, 1);
		UI_ComboBoxSetActiveLine(m_pMaterialControls.m_cbParMaterial.hWnd, mat->m_dwParent);
		UI_SetCheckBoxState(m_pMaterialControls.m_chkParMaterial.hWnd, 1);
	}
	else
	{
		ComboBox_Enable(m_pMaterialControls.m_cbParMaterial.hWnd, 0);
		UI_ComboBoxSetActiveLine(m_pMaterialControls.m_cbParMaterial.hWnd, -1);
		UI_SetCheckBoxState(m_pMaterialControls.m_chkParMaterial.hWnd, 0);
	}

	if (flags & MATERIAL_MOVE)
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkMove.hWnd, 1);
		Edit_Enable(m_pMaterialControls.m_boxMoveX.hWnd, 1);
		Edit_Enable(m_pMaterialControls.m_boxMoveY.hWnd, 1);
		sprintf(buf, "%g", mat->m_fMoveX);
		SetWindowText(m_pMaterialControls.m_boxMoveX.hWnd, buf);
		sprintf(buf, "%g", mat->m_fMoveY);
		Edit_SetText(m_pMaterialControls.m_boxMoveY.hWnd, buf);
	}
	else
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkMove.hWnd, 0);
		Edit_Enable(m_pMaterialControls.m_boxMoveX.hWnd, 0);
		Edit_Enable(m_pMaterialControls.m_boxMoveY.hWnd, 0);
		Edit_SetText(m_pMaterialControls.m_boxMoveX.hWnd, "");
		Edit_SetText(m_pMaterialControls.m_boxMoveY.hWnd, "");
	}

	if (flags & MATERIAL_HAS_ROTATION)
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkRotation.hWnd, 1);
		Edit_Enable(m_pMaterialControls.m_boxRotationX.hWnd, 1);
		Edit_Enable(m_pMaterialControls.m_boxRotationY.hWnd, 1);
		sprintf(buf, "%g", mat->m_fRotationPointX);
		Edit_SetText(m_pMaterialControls.m_boxRotationX.hWnd, buf);
		sprintf(buf, "%g", mat->m_fRotationPointY);
		Edit_SetText(m_pMaterialControls.m_boxRotationY.hWnd, buf);
	}
	else
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkRotation.hWnd, 0);
		Edit_Enable(m_pMaterialControls.m_boxRotationX.hWnd, 0);
		Edit_Enable(m_pMaterialControls.m_boxRotationY.hWnd, 0);
		Edit_SetText(m_pMaterialControls.m_boxRotationX.hWnd, "");
		Edit_SetText(m_pMaterialControls.m_boxRotationY.hWnd, "");
	}

	if (flags & MATERIAL_ENV)
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkEnv.hWnd, 1);
		EnableWindow(m_pMaterialControls.m_boxEnv1.hWnd, 1);
		EnableWindow(m_pMaterialControls.m_boxEnv2.hWnd, 1);
		EnableWindow(m_pMaterialControls.m_boxEnvId.hWnd, 1);
		sprintf(buf, "%g", mat->m_fEnv1);
		SetWindowText(m_pMaterialControls.m_boxEnv1.hWnd, buf);
		sprintf(buf, "%g", mat->m_fEnv2);
		SetWindowText(m_pMaterialControls.m_boxEnv2.hWnd, buf);
		if (mat->m_dwEnvId > 0) 
		{
			sprintf(buf, "%d", mat->m_dwEnvId);
			SetWindowText(m_pMaterialControls.m_boxEnvId.hWnd, buf);
		}
	}
	else
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkEnv.hWnd, 0);
		EnableWindow(m_pMaterialControls.m_boxEnv1.hWnd, 0);
		EnableWindow(m_pMaterialControls.m_boxEnv2.hWnd, 0);
		EnableWindow(m_pMaterialControls.m_boxEnvId.hWnd, 0);
		SetWindowText(m_pMaterialControls.m_boxEnv1.hWnd, "");
		SetWindowText(m_pMaterialControls.m_boxEnv2.hWnd, "");
		SetWindowText(m_pMaterialControls.m_boxEnvId.hWnd, "");
	}

	if (flags & MATERIAL_SPECULAR)
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkSpecular.hWnd, 1);
		EnableWindow(m_pMaterialControls.m_boxSpecular.hWnd, 1);
		sprintf(buf, "%g", mat->m_fSpecular);
		SetWindowText(m_pMaterialControls.m_boxSpecular.hWnd, buf);

		EnableWindow(m_pMaterialControls.m_boxPower.hWnd, 1);
		sprintf(buf, "%d", mat->m_dwPower);
		SetWindowText(m_pMaterialControls.m_boxPower.hWnd, buf);
	}
	else
	{
		UI_SetCheckBoxState(m_pMaterialControls.m_chkSpecular.hWnd, 0);
		EnableWindow(m_pMaterialControls.m_boxSpecular.hWnd, 0);
		SetWindowText(m_pMaterialControls.m_boxSpecular.hWnd, "");

		EnableWindow(m_pMaterialControls.m_boxPower.hWnd, 0);
		SetWindowText(m_pMaterialControls.m_boxPower.hWnd, "");
	}

	sprintf(buf, "%d", mat->m_dwInternalTexCoordsId);
	SetWindowText(m_pMaterialControls.m_boxCoord.hWnd, buf);

	UI_SetCheckBoxState(m_pMaterialControls.m_chkNoF.hWnd, flags & MATERIAL_NOF);
	UI_SetCheckBoxState(m_pMaterialControls.m_chkNoZ.hWnd, flags & MATERIAL_NOZ);
	UI_SetCheckBoxState(m_pMaterialControls.m_chkIsTransparent.hWnd, flags & MATERIAL_HAS_TTX);
	EnableWindow(m_pMaterialControls.m_chkIsTransparent.hWnd, (flags & MATERIAL_HAS_TEXTURE) && !(flags & MATERIAL_USE_COL));
	UI_SetCheckBoxState(m_pMaterialControls.m_chkNoTile.hWnd, flags & MATERIAL_NOTILE);
	UI_SetCheckBoxState(m_pMaterialControls.m_chkColor.hWnd, flags & MATERIAL_USE_COL);
	EnableWindow(m_pMaterialControls.m_chkColor.hWnd, !(flags & MATERIAL_HAS_TTX));

	SetWindowText(m_hBoxItemName, mat->m_sName.c_str());
	InvalidateRect(m_pMaterialControls.m_btnColor.hWnd, 0, TRUE);
	ShowUI(ResUIViewType::ViewMaterial);
	m_bSetupUINow = false;
}

void ResUI::ViewMask(Mask *msk)
{
	ResetViewUI();

	if (!msk)
		return;

	m_bSetupUINow = true;

	m_pCurrentMask = new Mask(*msk);
	m_pCurrentMaskReal = msk;
	msk = m_pCurrentMask;

	D3DCOLOR *data = (D3DCOLOR *)malloc(msk->m_wWidth * msk->m_wHeight * sizeof(D3DCOLOR));
	Palette *plt = 0;
	if (m_pCurrentModule->m_pPalettes.size() > 0)
		plt = m_pCurrentModule->m_pPalettes[0];
	else
		plt = m_pPalette;
	msk->GetData(data, plt);
	m_pSpriteTexture = m_pD3D->CreateTexture(data, msk->m_wWidth, msk->m_wHeight);
	free(data);
	m_pSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)msk->m_wWidth, (float)msk->m_wHeight }, { 0, 0 }, { 1.f, 1.f }, D3DCOLOR_XRGB(255, 255, 255), m_pSpriteTexture);
	
	UI_ComboBoxSetActiveLine(m_pMaskControls.m_cbFormat.hWnd, msk->m_bFormat - 1);
	UI_SetLabelText(m_pMaskControls.m_lblSizeValue.hWnd, std::string(std::to_string(msk->m_wWidth) + "x" + std::to_string(msk->m_wHeight)).c_str());

	UI_SetCheckBoxState(m_pMaskControls.m_chkNoLoad.hWnd, msk->m_bNoLoad);
	SetWindowText(m_hBoxItemName, msk->m_sName.c_str());
	ShowUI(ResUIViewType::ViewMask);
	m_bSetupUINow = false;
}

void ResUI::ShowUI(ResUIViewType eType)
{
	for (int i = 0; i < ARRAYSIZE(m_pTextureControls.m_pAll); i++)
		ShowWindow(m_pTextureControls.m_pAll[i].hWnd, 0);

	for (int i = 0; i < ARRAYSIZE(m_pMaskControls.m_pAll); i++)
		ShowWindow(m_pMaskControls.m_pAll[i].hWnd, 0);

	for (int i = 0; i < ARRAYSIZE(m_pMaterialControls.m_pAll); i++)
		ShowWindow(m_pMaterialControls.m_pAll[i].hWnd, 0);

	switch (eType)
	{
	case ResUIViewType::ViewTexture:
		for (int i = 0; i < ARRAYSIZE(m_pTextureControls.m_pAll); i++)
			ShowWindow(m_pTextureControls.m_pAll[i].hWnd, 1);
		break;

	case ResUIViewType::ViewMask:
		for (int i = 0; i < ARRAYSIZE(m_pMaskControls.m_pAll); i++)
			ShowWindow(m_pMaskControls.m_pAll[i].hWnd, 1);
		break;

	case ResUIViewType::ViewMaterial:
		for (int i = 0; i < ARRAYSIZE(m_pMaterialControls.m_pAll); i++)
			ShowWindow(m_pMaterialControls.m_pAll[i].hWnd, 1);
		break;
	}
	m_eCurrentView = eType;
	ResetUISize();
}

void ResUI::ResetViewUI()
{
	if (m_pCurrentMaterial)
		delete m_pCurrentMaterial;
	if (m_pCurrentTexture)
		delete m_pCurrentTexture;
	if (m_pCurrentMask)
		delete m_pCurrentMask;
	m_pCurrentMaterial = 0;
	m_pCurrentTexture = 0;
	m_pCurrentMask = 0;
	m_pCurrentTextureReal = 0;
	m_pCurrentMaskReal = 0;
	m_pCurrentMaterialReal = 0;

	for (int i = 0; i < ARRAYSIZE(m_pTextureControls.m_pAll); i++)
		ShowWindow(m_pTextureControls.m_pAll[i].hWnd, 0);

	for (int i = 0; i < ARRAYSIZE(m_pMaskControls.m_pAll); i++)
		ShowWindow(m_pMaskControls.m_pAll[i].hWnd, 0);

	for (int i = 0; i < ARRAYSIZE(m_pMaterialControls.m_pAll); i++)
		ShowWindow(m_pMaterialControls.m_pAll[i].hWnd, 0);

	if (m_pSprite)
	{
		delete m_pSprite;
		m_pSprite = 0;
	}
	m_eCurrentView = ResUIViewType::ViewNone;
	SafeRelease(m_pSpriteTexture);
	ResetUISize();
}

void ResUI::ResetUISize()
{
	RECT rect;
	int controlsHeight = 0;
	GetClientRect(m_hMainWnd, &rect);
	SetWindowPos(m_pTree->m_hWnd, 0, 0, 0, 300, rect.bottom - rect.top, SWP_NOOWNERZORDER | SWP_NOREPOSITION);
	if (m_eCurrentView == ResUIViewType::ViewTexture)
		controlsHeight = 170;
	else if (m_eCurrentView == ResUIViewType::ViewMask)
		controlsHeight = 170;
	else if (m_eCurrentView == ResUIViewType::ViewMaterial)
		controlsHeight = 295;
	SetWindowPos(m_hViewWnd, 0, 300, 0, rect.right - rect.left - 300, rect.bottom - rect.top - controlsHeight, SWP_NOOWNERZORDER | SWP_NOREPOSITION);
	SetWindowPos(m_hControlsWnd, 0, 300, rect.bottom - rect.top - controlsHeight, rect.right - rect.left - 300, controlsHeight, SWP_NOOWNERZORDER | SWP_NOREPOSITION);
	GetClientRect(m_hViewWnd, &rect);
	if ((rect.right - rect.left) >= 1)
	{
		m_dwViewWidth = rect.right - rect.left;
		m_dwViewHeight = rect.bottom - rect.top;
		m_pD3D->Resize(rect.right - rect.left, rect.bottom - rect.top);
		if (m_pSprite)
			m_pSprite->Reset();
		InitBgSprite();
	}
}

void ResUI::UpdateCurrentModule(GameModule *mod, bool reload)
{
	if (mod && (m_pCurrentModule != mod || reload))
	{
		m_pCurrentModule = mod;

		UI_ResetComboBox(m_pMaterialControls.m_cbTexture.hWnd);
		UI_ResetComboBox(m_pMaterialControls.m_cbMaterial.hWnd);
		UI_ResetComboBox(m_pMaterialControls.m_cbParMaterial.hWnd);

		int i = 1;
		for (auto it = m_pCurrentModule->m_pTextures.begin(); it != m_pCurrentModule->m_pTextures.end(); it++)
		{
			UI_ComboBoxAddLine(m_pMaterialControls.m_cbTexture.hWnd, ("[" + std::to_string(i++) + "] " + (*it)->m_sName).c_str());
		}
		i = 1;
		for (auto it = m_pCurrentModule->m_pMaterials.begin(); it != m_pCurrentModule->m_pMaterials.end(); it++)
		{
			UI_ComboBoxAddLine(m_pMaterialControls.m_cbMaterial.hWnd, ("[" + std::to_string(i) + "] " + (*it)->m_sName).c_str());
			UI_ComboBoxAddLine(m_pMaterialControls.m_cbParMaterial.hWnd, ("[" + std::to_string(i++) + "] " + (*it)->m_sName).c_str());
		}
	}
}

void ResUI::UpdateResourceView()
{
	if (m_pSprite)
	{
		delete m_pSprite;
		m_pSprite = 0;
	}
	SafeRelease(m_pSpriteTexture);

	if (m_pCurrentMaterial)
	{
		Material *mat = m_pCurrentMaterial;
		int width = 200;
		int height = 200;
		DWORD flags = mat->m_dwFlags;

		if (flags & MATERIAL_HAS_TEXTURE)
		{
			ResTexture *tex = m_pCurrentModule->m_pTextures[mat->m_dwTextureId];
			D3DCOLOR *data = (D3DCOLOR *)malloc(tex->m_iWidth * tex->m_iHeight * sizeof(D3DCOLOR));
			Palette *plt = 0;
			if (m_pCurrentModule->m_pPalettes.size() > 0)
				plt = m_pCurrentModule->m_pPalettes[0];
			tex->GetData(data, plt);
			m_pSpriteTexture = m_pD3D->CreateTexture(data, tex->m_iWidth, tex->m_iHeight);
			free(data);
			width = tex->m_iWidth;
			height = tex->m_iHeight;
		}

		BYTE r = 255, g = 255, b = 255, a = 255;

		if (flags & MATERIAL_USE_COL && !(flags & MATERIAL_HAS_TTX))
		{
			if (m_pPalette)
			{
				r = m_pPalette->m_pPalette[mat->m_dwColorId].R;
				g = m_pPalette->m_pPalette[mat->m_dwColorId].G;
				b = m_pPalette->m_pPalette[mat->m_dwColorId].B;
			}
		}
		if (flags & MATERIAL_IS_TRANSPARENT)
		{
			a = (BYTE)(mat->m_fTransparency * 255.f);
		}

		m_pSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)width, (float)height }, { 0, 0 }, { 1.f, 1.f }, D3DCOLOR_ARGB(a, r, g, b), m_pSpriteTexture);

		InvalidateRect(m_pMaterialControls.m_btnColor.hWnd, 0, TRUE);
	}
	else if (m_pCurrentTexture)
	{
		ResTexture *tex = m_pCurrentTexture;
		D3DCOLOR *data = (D3DCOLOR *)malloc(tex->m_iWidth * tex->m_iHeight * sizeof(D3DCOLOR));
		Palette *plt = 0;
		if (m_pCurrentModule->m_pPalettes.size() > 0)
			plt = m_pCurrentModule->m_pPalettes[0];
		else
			plt = m_pPalette;
		tex->GetData(data, plt);
		m_pSpriteTexture = m_pD3D->CreateTexture(data, tex->m_iWidth, tex->m_iHeight);
		free(data);
		m_pSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)tex->m_iWidth, (float)tex->m_iHeight }, { 0, 0 }, { 1.f, 1.f }, D3DCOLOR_XRGB(255, 255, 255), m_pSpriteTexture);
		UI_SetLabelText(m_pTextureControls.m_lblSizeValue.hWnd, std::string(std::to_string(tex->m_iWidth) + "x" + std::to_string(tex->m_iHeight)).c_str());
	}
	else if (m_pCurrentMask)
	{
		Mask *msk = m_pCurrentMask;
		D3DCOLOR *data = (D3DCOLOR *)malloc(msk->m_wWidth * msk->m_wHeight * sizeof(D3DCOLOR));
		Palette *plt = 0;
		if (m_pCurrentModule->m_pPalettes.size() > 0)
			plt = m_pCurrentModule->m_pPalettes[0];
		else
			plt = m_pPalette;
		msk->GetData(data, plt);
		m_pSpriteTexture = m_pD3D->CreateTexture(data, msk->m_wWidth, msk->m_wHeight);
		free(data);
		m_pSprite = m_pD3D->CreateSprite({ 0, 0 }, { (float)msk->m_wWidth, (float)msk->m_wHeight }, { 0, 0 }, { 1.f, 1.f }, D3DCOLOR_XRGB(255, 255, 255), m_pSpriteTexture);
		UI_SetLabelText(m_pTextureControls.m_lblSizeValue.hWnd, std::string(std::to_string(msk->m_wWidth) + "x" + std::to_string(msk->m_wHeight)).c_str());
	}
}

void ResUI::SaveCurrentResource()
{
	static char bufName[512];
	GetWindowText(m_hBoxItemName, bufName, 512);
	if (m_pCurrentTexture)
	{
		m_pCurrentTexture->m_sName = bufName;
		if (UI_GetCheckBoxState(m_pTextureControls.m_chkMipmap.hWnd)) 
		{
			UI_ProgressBarSetProgress(m_hProgressBar, 0);
			ShowWindow(m_hProgressWnd, SW_SHOW);
			SetWindowText(m_hProgressWnd, "Генерация мипмапов");
			m_pCurrentTexture->GenerateMipmap(GenerateMipmapsCB);
			ShowWindow(m_hProgressWnd, SW_HIDE);
		}
		else
		{
			m_pCurrentTexture->RemoveMipmap();
		}
		*m_pCurrentTextureReal = *m_pCurrentTexture;
		auto tree = m_pCurrentTextureReal->GetTreeItem();
		
		tree->SetText((std::string("[") + std::to_string(m_pCurrentModule->GetTextureId(m_pCurrentTextureReal)+1) + "] " + m_pCurrentTexture->m_sName).c_str());
		ViewTexture(m_pCurrentTextureReal);
	}
	else if (m_pCurrentMaterial)
	{
		m_pCurrentMaterial->m_sName = bufName;
		*m_pCurrentMaterialReal = *m_pCurrentMaterial;
		auto tree = m_pCurrentMaterialReal->GetTreeItem();

		tree->SetText((std::string("[") + std::to_string(m_pCurrentModule->GetMaterialId(m_pCurrentMaterialReal) + 1) + "] " + m_pCurrentMaterial->m_sName).c_str());
		ViewMaterial(m_pCurrentMaterialReal);
	}
	else if (m_pCurrentMask)
	{
		m_pCurrentMask->m_sName = bufName;
		*m_pCurrentMaskReal = *m_pCurrentMask;
		auto tree = m_pCurrentMaskReal->GetTreeItem();

		tree->SetText((std::string("[") + std::to_string(m_pCurrentModule->GetMaskId(m_pCurrentMaskReal) + 1) + "] " + m_pCurrentMask->m_sName).c_str());
		ViewMask(m_pCurrentMaskReal);
	}
}

char *ResUI::GetValidFloat(char *value, float *fRetValue, float from, float to)
{
	bool changed = false;
	static char newValue[512];
	char *val = value;
	char *newVal = newValue;
	while (*val)
	{
		if (*val == '.' || (*val >= '0' && *val <= '9') || (*val == '-' && val == value))
		{
			*newVal = *val;
			newVal++;
			val++;
			continue;
		}
		changed = true;
		val++;
	}
	*newVal = 0;

	float fValue = atof(newValue);
	if (fValue < from)
	{
		changed = true;
		fValue = from;
	}
	else if (fValue > to)
	{
		changed = true;
		fValue = to;
	}
	sprintf(newValue, "%g", fValue);

	if (fRetValue)
		*fRetValue = fValue;

	if (changed)
		return newValue;
	
	return 0;
}

char *ResUI::GetValidInt(char *value, int *dwRetValue, int from, int to)
{
	bool changed = false;
	static char newValue[512];
	char *val = value;
	char *newVal = newValue;
	while (*val)
	{
		if ((*val >= '0' && *val <= '9') || (*val == '-' && val == value))
		{
			*newVal = *val;
			newVal++;
			val++;
			continue;
		}
		changed = true;
		val++;
	}
	*newVal = 0;

	int dwValue = atoi(newValue);
	if (dwValue < from)
	{
		changed = true;
		dwValue = from;
	}
	else if (dwValue > to)
	{
		changed = true;
		dwValue = to;
	}
	sprintf(newValue, "%d", dwValue);

	if (dwRetValue)
		*dwRetValue = dwValue;

	if (changed)
		return newValue;

	return 0;
}

void ResUI::ReplaceTextBoxValue(HWND hwnd, char *value)
{
	int len = strlen(value);
	SetWindowText(hwnd, value);
	SendMessage(hwnd, EM_SETSEL, len, len);
}

void ResUI::ApplyUIChanges()
{
	m_bSetupUINow = true;
	if (m_eCurrentView == ResUIViewType::ViewMaterial)
	{
		auto &c = m_pMaterialControls;
		auto &m = *m_pCurrentMaterial;
		int colorid = m.m_dwColorId;
		m.Reset();
		m.m_dwColorId = colorid;
		if (UI_GetCheckBoxState(c.m_chkTexture.hWnd))
		{
			EnableWindow(c.m_chkIsTransparent.hWnd, 1);
			EnableWindow(c.m_cbTexture.hWnd, 1);
			int texid = UI_ComboBoxGetCurrentLine(c.m_cbTexture.hWnd);
			if (texid >= 0)
			{
				m.m_dwFlags |= MATERIAL_HAS_TEXTURE;
				m.m_dwTextureId = texid;
			}
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_HAS_TEXTURE);
			m.m_dwTextureId = -1;
			UI_SetCheckBoxState(c.m_chkIsTransparent.hWnd, 0);
			EnableWindow(c.m_chkIsTransparent.hWnd, 0);
			EnableWindow(c.m_cbTexture.hWnd, 0);
		}
		if (UI_GetCheckBoxState(c.m_chkMaterial.hWnd))
		{
			EnableWindow(c.m_cbMaterial.hWnd, 1);
			int matid = UI_ComboBoxGetCurrentLine(c.m_cbMaterial.hWnd);
			if (matid >= 0)
			{
				m.m_dwFlags |= MATERIAL_ATT;
				m.m_dwInternalId = matid;
			}
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_ATT);
			m.m_dwInternalId = -1;
			EnableWindow(c.m_cbMaterial.hWnd, 0);
		}
		if (UI_GetCheckBoxState(c.m_chkParMaterial.hWnd))
		{
			EnableWindow(c.m_cbParMaterial.hWnd, 1);
			int matid = UI_ComboBoxGetCurrentLine(c.m_cbParMaterial.hWnd);
			if (matid >= 0)
			{
				m.m_dwParent = matid;
			}
		}
		else
		{
			m.m_dwParent = -1;
			EnableWindow(c.m_cbParMaterial.hWnd, 0);
		}
		if (UI_GetCheckBoxState(c.m_chkIsTransparent.hWnd))
		{
			m.m_dwFlags |= MATERIAL_HAS_TTX;
			UI_SetCheckBoxState(c.m_chkColor.hWnd, 0);
			EnableWindow(c.m_chkColor.hWnd, 0);
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_HAS_TTX);
			EnableWindow(c.m_chkColor.hWnd, 1);
		}

		if (UI_GetCheckBoxState(c.m_chkColor.hWnd))
		{
			m.m_dwFlags |= MATERIAL_USE_COL;
			UI_SetCheckBoxState(c.m_chkIsTransparent.hWnd, 0);
			EnableWindow(c.m_chkIsTransparent.hWnd, 0);
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_USE_COL);
			if (UI_GetCheckBoxState(c.m_chkTexture.hWnd))
				EnableWindow(c.m_chkIsTransparent.hWnd, 1);
		}

		if (UI_GetCheckBoxState(c.m_chkTransparent.hWnd))
		{
			char opacityText[512];
			m.m_dwFlags |= MATERIAL_IS_TRANSPARENT;
			GetWindowText(c.m_boxTransparent.hWnd, opacityText, 512);
			char *newOpacityText = GetValidFloat(opacityText, &m.m_fTransparency, 0.0f, 1.0f);
			if (newOpacityText)
				ReplaceTextBoxValue(c.m_boxTransparent.hWnd, newOpacityText);

			EnableWindow(c.m_boxTransparent.hWnd, 1);
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_IS_TRANSPARENT);
			EnableWindow(c.m_boxTransparent.hWnd, 0);
			SetWindowText(c.m_boxTransparent.hWnd, "");
		}

		if (UI_GetCheckBoxState(c.m_chkMove.hWnd))
		{
			char moveX[64], moveY[64];
			m.m_dwFlags |= MATERIAL_MOVE;
			GetWindowText(c.m_boxMoveX.hWnd, moveX, 64);
			GetWindowText(c.m_boxMoveY.hWnd, moveY, 64);
			char *newMoveX = GetValidFloat(moveX, &m.m_fMoveX, -1000.0f, 1000.0f);
			if (newMoveX)
				ReplaceTextBoxValue(c.m_boxMoveX.hWnd, newMoveX);
			char *newMoveY = GetValidFloat(moveY, &m.m_fMoveY, -1000.0f, 1000.0f);
			if (newMoveY)
				ReplaceTextBoxValue(c.m_boxMoveY.hWnd, newMoveY);

			EnableWindow(c.m_boxMoveX.hWnd, 1);
			EnableWindow(c.m_boxMoveY.hWnd, 1);
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_MOVE);
			EnableWindow(c.m_boxMoveX.hWnd, 0);
			EnableWindow(c.m_boxMoveY.hWnd, 0);
			SetWindowText(c.m_boxMoveX.hWnd, "");
			SetWindowText(c.m_boxMoveY.hWnd, "");
		}

		if (UI_GetCheckBoxState(c.m_chkRotationPoint.hWnd))
		{
			char rotX[64], rotY[64];
			m.m_dwFlags |= MATERIAL_HAS_ROTATION_POINT;
			GetWindowText(c.m_boxRotationX.hWnd, rotX, 64);
			GetWindowText(c.m_boxRotationY.hWnd, rotY, 64);
			char *newRotX = GetValidFloat(rotX, &m.m_fRotationPointX, -10000.0f, 10000.0f);
			if (newRotX)
				ReplaceTextBoxValue(c.m_boxRotationX.hWnd, newRotX);
			char *newRotY = GetValidFloat(rotY, &m.m_fRotationPointY, -10000.0f, 10000.0f);
			if (newRotY)
				ReplaceTextBoxValue(c.m_boxRotationY.hWnd, newRotY);

			EnableWindow(c.m_boxRotationX.hWnd, 1);
			EnableWindow(c.m_boxRotationY.hWnd, 1);
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_HAS_ROTATION_POINT);
			EnableWindow(c.m_boxRotationX.hWnd, 0);
			EnableWindow(c.m_boxRotationY.hWnd, 0);
			SetWindowText(c.m_boxRotationX.hWnd, "");
			SetWindowText(c.m_boxRotationY.hWnd, "");
		}

		if (UI_GetCheckBoxState(c.m_chkRotation.hWnd))
		{
			char rot[64];
			m.m_dwFlags |= MATERIAL_HAS_ROTATION;
			GetWindowText(c.m_boxRotation.hWnd, rot, 64);
			char *newRot = GetValidFloat(rot, &m.m_fRotation, -10000.0f, 10000.0f);
			if (newRot)
				ReplaceTextBoxValue(c.m_boxRotation.hWnd, newRot);

			EnableWindow(c.m_boxRotation.hWnd, 1);
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_HAS_ROTATION);
			EnableWindow(c.m_boxRotation.hWnd, 0);
			SetWindowText(c.m_boxRotation.hWnd, "");
		}
		if (UI_GetCheckBoxState(m_pMaterialControls.m_chkEnv.hWnd))
		{
			m.m_dwFlags |= MATERIAL_ENV;
			char env1[64];
			char env2[64];
			char envId[64];
			GetWindowText(c.m_boxEnv1.hWnd, env1, 64);
			GetWindowText(c.m_boxEnv2.hWnd, env2, 64);
			GetWindowText(c.m_boxEnvId.hWnd, envId, 64);
			char *newEnv1 = GetValidFloat(env1, &m.m_fEnv1, 0.0f, 100.0f);
			if (newEnv1)
				ReplaceTextBoxValue(c.m_boxEnv1.hWnd, newEnv1);
			char *newEnv2 = GetValidFloat(env2, &m.m_fEnv2, 0.0f, 100.0f);
			if (newEnv2)
				ReplaceTextBoxValue(c.m_boxEnv2.hWnd, newEnv2);

			char *newEnvId = GetValidInt(envId, &m.m_dwEnvId, 0, 5);
			if (newEnvId)
				ReplaceTextBoxValue(c.m_boxEnvId.hWnd, newEnvId);

			EnableWindow(c.m_boxEnv1.hWnd, 1);
			EnableWindow(c.m_boxEnv2.hWnd, 1);
			EnableWindow(c.m_boxEnvId.hWnd, 1);
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_ENV);
			EnableWindow(c.m_boxEnv1.hWnd, 0);
			EnableWindow(c.m_boxEnv2.hWnd, 0);
			EnableWindow(c.m_boxEnvId.hWnd, 0);
			SetWindowText(c.m_boxEnv1.hWnd, "");
			SetWindowText(c.m_boxEnv2.hWnd, "");
			SetWindowText(c.m_boxEnvId.hWnd, "");
		}
		if (UI_GetCheckBoxState(c.m_chkSpecular.hWnd))
		{
			char spec[64];
			m.m_dwFlags |= MATERIAL_SPECULAR | MATERIAL_POWER;
			GetWindowText(c.m_boxSpecular.hWnd, spec, 64);
			char *newSpec = GetValidFloat(spec, &m.m_fSpecular, 0.0f, 1.0f);
			if (newSpec)
				ReplaceTextBoxValue(c.m_boxSpecular.hWnd, newSpec);

			EnableWindow(c.m_boxSpecular.hWnd, 1);
			char power[64];
			GetWindowText(c.m_boxPower.hWnd, power, 64);
			char *newPow = GetValidInt(power, &m.m_dwPower, 0, 1000);
			if (newPow)
				ReplaceTextBoxValue(c.m_boxPower.hWnd, newPow);

			EnableWindow(c.m_boxPower.hWnd, 1);
		}
		else
		{
			m.m_dwFlags &= (~MATERIAL_SPECULAR | MATERIAL_POWER);
			EnableWindow(c.m_boxSpecular.hWnd, 0);
			SetWindowText(c.m_boxSpecular.hWnd, "");

			EnableWindow(c.m_boxPower.hWnd, 0);
			SetWindowText(c.m_boxPower.hWnd, "");
		}

		if (UI_GetCheckBoxState(c.m_chkNoZ.hWnd))
			m.m_dwFlags |= MATERIAL_NOZ;
		else
			m.m_dwFlags &= (~MATERIAL_NOZ);

		if (UI_GetCheckBoxState(c.m_chkNoF.hWnd))
			m.m_dwFlags |= MATERIAL_NOF;
		else
			m.m_dwFlags &= (~MATERIAL_NOF);

		if (UI_GetCheckBoxState(c.m_chkNoTile.hWnd))
			m.m_dwFlags |= MATERIAL_NOTILE;
		else
			m.m_dwFlags &= (~MATERIAL_NOTILE);

		// layer id
		char layer[64];
		GetWindowText(c.m_boxCoord.hWnd, layer, 64);
		char *newLayer = GetValidInt(layer, &m.m_dwInternalTexCoordsId, 0, 5);
		if (newLayer)
			ReplaceTextBoxValue(c.m_boxCoord.hWnd, newLayer);
	}
	else if (m_eCurrentView == ResUIViewType::ViewTexture)
	{
		m_pCurrentTexture->m_bMemfix = (bool)UI_GetCheckBoxState(m_pTextureControls.m_chkMemfix.hWnd);
		m_pCurrentTexture->m_bNoLoad = (bool)UI_GetCheckBoxState(m_pTextureControls.m_chkNoLoad.hWnd);
	}
	m_bSetupUINow = false;
}

void ResUI::ResetResource()
{
	switch (m_eCurrentView)
	{
	case ResUIViewType::ViewTexture:
		ViewTexture(m_pCurrentTextureReal);
		break;

	case ResUIViewType::ViewMaterial:
		ViewMaterial(m_pCurrentMaterialReal);
		break;

	case ResUIViewType::ViewMask:
		ViewMask(m_pCurrentMaskReal);
		break;
	}	
}

void ResUI::OpenModules(std::vector<std::string> &files)
{
	UI_ProgressBarSetProgress(m_hProgressBar, 0);
	ShowWindow(m_hProgressWnd, SW_SHOW);
	char title[512];
	int id = 1;
	for (auto it = files.begin(); it != files.end(); it++)
	{
		sprintf(title, "Открытие файлов (%d/%d)", id++, files.size());
		SetWindowText(m_hProgressWnd, title);
		if (!OpenModule((*it).c_str()))
		{
			MessageBox(m_hMainWnd, "Невозможно открыть файл!\nВозможно он занят другим процессом, или не является ресурсом", "Ошибка", MB_OK | MB_ICONERROR);
		}
	}
	ShowWindow(m_hProgressWnd, SW_HIDE);
}