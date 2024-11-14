#ifndef UI_HEADER
#define UI_HEADER

#include <Windows.h>
#include <ShellAPI.h>
#include <shlobj.h>
#include <atlbase.h>
#include <Commdlg.h>
#include "CNode.h"
extern HINSTANCE hInst;
extern HFONT hMainFont;

HWND CreateChildWindow(HWND hwndParent, DWORD style, int x, int y, int sizeX, int sizeY, int id, WNDPROC proc);
BOOL UI_FileOpenDlg(HWND hWnd, LPTSTR pstrFileName, LPTSTR Filter, DWORD Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, LPTSTR InitDir = NULL, LPTSTR pstrTitleName = NULL, char **ext = NULL, int *filterId = NULL);
BOOL UI_FileSaveDlg(HWND hWnd, LPTSTR pstrFileName, LPTSTR Filter, DWORD Flags = OFN_PATHMUSTEXIST, LPTSTR InitDir = NULL, LPTSTR pstrTitleName = NULL, char **ext = NULL, int *filterId = NULL);
HWND UI_CreateButton(HWND hWnd, TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID);
HWND UI_CreateComboBox(HWND hWnd, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID);
HWND UI_CreateListBox(HWND hWnd, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID);
HWND UI_CreateTextBox(HWND hWnd, const TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID);
HWND UI_CreateCheckBox(HWND hWnd, const TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID);
HWND UI_CreateGroupBox(HWND hWnd, TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID);
UINT UI_GetCheckBoxState(HWND hWnd);
void UI_SetCheckBoxState(HWND hWnd, BOOL checked);
DWORD UI_ComboBoxAddLine(HWND hWnd, const TCHAR *Text);
void UI_ComboBoxSetActiveLine(HWND hWnd, DWORD id);
DWORD UI_ComboBoxGetActiveLine(HWND hWnd);
void UI_ListBoxAddLine(HWND hWnd, const TCHAR *Text);
void UI_ListBoxInsertLine(HWND hWnd, const TCHAR *Text, int line = -1);
DWORD UI_ComboBoxGetCurrentLine(HWND hWnd);
DWORD UI_ListBoxGetCurrentLine(HWND hWnd);
DWORD UI_ListBoxGetCurrentLineText(HWND hWnd, TCHAR *buf);
HWND UI_CreateLabel(HWND hWnd, const TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID);
void UI_SetLabelText(HWND hLabel, const TCHAR *Text);
HFONT UI_CreateFont(TCHAR *Family, int size);
void UI_SetElementFont(HWND hWnd, HFONT hFont);
void UI_ResetListBox(HWND hWnd);
void UI_ResetComboBox(HWND hWnd);
HWND UI_CreateProgressBar(HWND hWnd, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID, int Range);
void UI_ProgressBarSetMarque(HWND hProgBar, BOOL state, int time);
void UI_ProgressBarSetProgress(HWND hProgBar, int Progress);
void UI_ProgressBarSetRange(HWND hProgBar, int Range);
bool UI_ColorPicker(HWND hWnd, COLORREF *color);

typedef class WinTree WINTREE;
typedef class WinTreeItem WINTREEITEM;
typedef void (CALLBACK *LPWINTREECALLBACK)(WINTREE *, WINTREEITEM *, DWORD, DWORD);
typedef void (CALLBACK *LPWINTREEFORALLCALLBACK)(WINTREEITEM *);

class WinTreeItem
{
private:
	WinTreeItem *m_pParent;
	WinTreeItem *m_pNext;
	WinTreeItem *m_pChild;
	int			m_dLevel;
	LPTSTR		m_sName;
	DWORD		m_dData;
	DWORD		m_dDataType;
	HMENU		m_hMenu;
	LPWINTREECALLBACK m_pCallback;

	WinTreeItem();
	~WinTreeItem();

public:
	WinTree		*m_pTree;
	HTREEITEM	m_hItem;
	static WinTreeItem *Create(WinTree *winTree, LPCTSTR lpszItem, DWORD data, LPWINTREECALLBACK callback = 0);
	void Destroy();
	WinTreeItem *GetRoot();
	WinTreeItem *AddChild(LPCTSTR lpszItem, DWORD data, LPWINTREECALLBACK callback = 0);
	WinTreeItem *AddNext(LPCTSTR lpszItem, DWORD data, LPWINTREECALLBACK callback = 0);
	WinTreeItem *AddToLast(LPCTSTR lpszItem, DWORD data, LPWINTREECALLBACK callback = 0);
	void SetCallback(LPWINTREECALLBACK callback);
	void SetMenu(HMENU menu);
	void ForAll(LPWINTREEFORALLCALLBACK callback);
	void ForAllChildren(LPWINTREEFORALLCALLBACK callback);

	WinTreeItem *GetChild();
	WinTreeItem *GetParent();
	WinTreeItem *GetNext();
	WinTreeItem *GetByHandle(HTREEITEM item);
	int GetSelection(Node<WinTreeItem> *node);
	DWORD GetData(DWORD *type = 0);
	void SetData(DWORD data, DWORD type = 0);
	int GetLevel();
	void Expand();
	void Collapse();
	void SetActive();
	void SetText(LPCTSTR name);

	friend WINTREE;

};

class WinTree
{
private:
	HWND	m_hWndParent;
	int		m_dId;
	WinTreeItem *m_pSelected;

	WinTree();
	~WinTree();

public:
	HWND	m_hWnd;
	WinTreeItem *m_pItems;

	static WinTree *Create(HWND hwndParent, int style, int x, int y, int sizeX, int sizeY, int id);
	void Destroy();
	WinTreeItem *AddItem(LPCTSTR lpszItem, DWORD data, DWORD dataType = 0);
	WinTreeItem *GetSelected();
	void Clear();
	bool ProcessEvent(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

	friend WINTREEITEM;
};

#endif