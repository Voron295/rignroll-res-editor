#include "UI.h"
#include "GameModule.h"
#include "DirectX.h"

#define ID_RESUI_START 3500
#define ID_RESUI_BOX_NAME 2800
#define ID_RESUI_BTN_SAVE 2801
#define ID_RESUI_BTN_RESET 2802

#define ID_RESUI_BTN_COLORS 13000

enum FileType
{
	FileNone,
	FileRes,
	FileFon
};

struct ResUIElement
{
	HWND hWnd;
	int dwID;
};

enum ResUIViewType 
{
	ViewNone,
	ViewPalette,
	ViewTexture,
	ViewMask,
	ViewMaterial,
	ViewFont
};

class ResUI
{
private:
	D3D *m_pD3D;
	HWND m_hMainWnd;
	HWND m_hViewWnd;
	HWND m_hControlsWnd;
	HWND m_hProgressWnd;
	HWND m_hProgressBar;
	HWND m_hColorsWnd;
	WinTree *m_pTree;
	std::vector<GameModule *>m_pModules;
	GameModule *m_pCurrentModule;
	Palette *m_pPalette;
	std::string m_sCurrentFile;
	FileType m_eFileType;
	int m_dwViewWidth;
	int m_dwViewHeight;

	ResUIViewType m_eCurrentView;

	ResTexture *m_pCurrentTexture;
	ResTexture *m_pCurrentTextureReal;
	Mask *m_pCurrentMask;
	Mask *m_pCurrentMaskReal;
	Material *m_pCurrentMaterial;
	Material *m_pCurrentMaterialReal;

	HMENU m_hMainWndMenu;
	HMENU m_hViewWndMenu;
	HMENU m_hModuleMenu;

	ResUIElement m_boxName;
	ResUIElement m_btnSave;

	// textures controls
	union
	{
		struct{
			ResUIElement m_chkNoLoad;
			ResUIElement m_chkMemfix;
			ResUIElement m_chkMipmap;
			ResUIElement m_cbMipmap;
			ResUIElement m_lblFormat;
			ResUIElement m_cbFormat;
			ResUIElement m_btnApply;
			ResUIElement m_btnImport;
			ResUIElement m_btnExport;
			ResUIElement m_lblSize;
			ResUIElement m_lblSizeValue;
		};
		ResUIElement m_pAll[12];
	} m_pTextureControls;
	HMENU m_hTextureMenu;
	HMENU m_hTexturesMenu;

	union
	{
		struct{
			ResUIElement m_chkNoLoad;
			ResUIElement m_lblFormat;
			ResUIElement m_cbFormat;
			ResUIElement m_btnApply;
			ResUIElement m_btnImport;
			ResUIElement m_btnExport;
			ResUIElement m_lblSize;
			ResUIElement m_lblSizeValue;
		};
		ResUIElement m_pAll[8];
	} m_pMaskControls;
	HMENU m_hMaskMenu;
	HMENU m_hMasksMenu;

	union
	{
		struct
		{
			ResUIElement m_chkTexture;
			ResUIElement m_cbTexture;
			ResUIElement m_chkMove;
			ResUIElement m_boxMoveX;
			ResUIElement m_boxMoveY;
			ResUIElement m_chkRotationPoint;
			ResUIElement m_boxRotationX;
			ResUIElement m_boxRotationY;
			ResUIElement m_chkRotation;
			ResUIElement m_boxRotation;
			ResUIElement m_chkMaterial;
			ResUIElement m_cbMaterial;
			ResUIElement m_chkParMaterial;
			ResUIElement m_cbParMaterial;
			ResUIElement m_chkTransparent;
			ResUIElement m_boxTransparent;
			ResUIElement m_chkNoZ;
			ResUIElement m_chkNoF;
			ResUIElement m_chkNoTile;
			ResUIElement m_chkNoTileU;
			ResUIElement m_chkEnv;
			ResUIElement m_boxEnv1;
			ResUIElement m_boxEnv2;
			ResUIElement m_lblEnvId;
			ResUIElement m_boxEnvId;
			ResUIElement m_lblCoord;
			ResUIElement m_boxCoord;
			ResUIElement m_lblSpecular;
			ResUIElement m_chkSpecular;
			ResUIElement m_boxSpecular;
			ResUIElement m_lblPower;
			ResUIElement m_boxPower;
			ResUIElement m_chkIsTransparent;
			ResUIElement m_grpColorGroup;
			ResUIElement m_grpFlags;
			ResUIElement m_grpConnection;
			ResUIElement m_grpMove;
			ResUIElement m_chkColor;
			ResUIElement m_btnColor;
		};
		ResUIElement m_pAll[39];
	} m_pMaterialControls;
	HMENU m_hMaterialMenu;
	HMENU m_hMaterialsMenu;

	HWND m_hBoxItemName;
	HWND m_hBtnSave;
	HWND m_hBtnReset;

	bool m_bSetupUINow;

	ID3D11Texture2D *m_pSpriteTexture;
	D3DSprite *m_pSprite;
	ID3D11Texture2D *m_pBackTexture[3];
	int m_dwActiveBackTexture;
	D3DSprite *m_pBackSprite;

	bool SetupModuleEnv(GameModule *gm);
	bool OpenModule(const char *path);
	bool SaveModule(GameModule *module, const char *path);
	bool CloseModule(GameModule *module);
	void InitBgSprite();

	static LRESULT CALLBACK ViewWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ControlsWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK ProgressWndProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK ColorsWndProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK AboutWndProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	static ResUI *g_pInstance;

	static void CALLBACK PaletteClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type);
	static void CALLBACK TextureClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type);
	static void CALLBACK MaterialClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type);
	static void CALLBACK MaskClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type);
	static void CALLBACK FontClick(WinTree *tree, WinTreeItem *item, DWORD msg, DWORD type);

	static void CALLBACK LoadModuleCB(GameModule *gm, int progress, int total);
	static void CALLBACK GenerateMipmapsCB(int progress, int total);

	static std::vector<std::string> ParseFilesInput(const char *filename);

	std::vector<unsigned char> ResUI::LoadImageFromFile(const char *filename, unsigned int &width, unsigned int &height);

	void ViewPalette(Palette *plm);
	void ViewTexture(ResTexture *tex);
	void ViewTextureMipmap(int id);
	void ViewMaterial(Material *tex);
	void ViewMask(Mask *msk);
	void ViewFont(ResTexture *msk);

	char *SearchForCommonRes(const char *path);

	void SetBackground(int id);

	void ShowUI(ResUIViewType eType);
	void ShowTextureUI();
	void ShowMaterialUI();
	void ResetViewUI();
	void ResetUISize();
	void ResetResource();
	void SaveCurrentResource();
	void UpdateResourceView();
	void ApplyUIChanges();

	void RemoveTexture(WinTreeItem *item);
	void RemoveMaterial(WinTreeItem *item);
	void RemoveMask(WinTreeItem *item);
	void AddTexture(WinTreeItem *item);
	void AddMaterial(WinTreeItem *item);
	void AddMask(WinTreeItem *item);

	static int FixFilename(char *path, char *ext, int extId);

	char *GetValidFloat(char *value, float *fRetValue, float from = 0.0f, float to = 1.0f);
	char *GetValidInt(char *value, int *dwRetValue, int from = 0, int to = 1000);
	void ReplaceTextBoxValue(HWND hwnd, char *value);

	void UpdateCurrentModule(GameModule *mod, bool reload = false);

public:
	ResUI(HWND hWnd);
	~ResUI();

	bool ProcessUICommand(HWND hWnd, int wmId, int wmCommand);
	bool ProcessEvents(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	void Render();

	void OpenModules(std::vector<std::string> &files);
};