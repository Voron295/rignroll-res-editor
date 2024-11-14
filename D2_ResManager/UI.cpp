#include <Windows.h>
#include <windowsx.h>
#include "UI.h"
#include <uxtheme.h>

HWND CreateChildWindow(HWND hwndParent, DWORD style, int x, int y, int sizeX, int sizeY, int id, WNDPROC proc)
{
	static int reg = 0;
	char buf[512];
	sprintf(buf, "D2ResManagerChildWClass%d", reg++);
	WNDCLASS w;
	memset(&w, 0, sizeof(WNDCLASS));
	w.lpfnWndProc = proc;
	w.hInstance = hInst;
	w.hbrBackground = CreateSolidBrush(RGB(240, 240, 240));
	w.lpszClassName = buf;
	w.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&w);
	reg = true;
	
	HWND child;
	child = CreateWindowEx(0, buf, (LPCTSTR)NULL,
		WS_CHILD | WS_VISIBLE | style, x, y,
		sizeX, sizeY, hwndParent, (HMENU)id, hInst, NULL);

	return child;
}

BOOL UI_FileOpenDlg(HWND hWnd, LPTSTR pstrFileName, LPTSTR Filter, DWORD Flags, LPTSTR InitDir, LPTSTR pstrTitleName, char **extension, int *filterId)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 512;
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = InitDir;
	ofn.Flags = Flags;
	auto result = GetOpenFileName(&ofn);
	if (extension)
		*extension = &pstrFileName[ofn.nFileExtension];
	if (filterId)
		*filterId = ofn.nFilterIndex;
	return result;
}

BOOL UI_FileSaveDlg(HWND hWnd, LPTSTR pstrFileName, LPTSTR Filter, DWORD Flags, LPTSTR InitDir, LPTSTR pstrTitleName, char **extension, int *filterId)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pstrFileName;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 512;
	ofn.lpstrFilter = Filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = InitDir;
	ofn.Flags = Flags | OFN_OVERWRITEPROMPT;
	auto result = GetSaveFileName(&ofn);
	if (extension) 
		*extension = &pstrFileName[ofn.nFileExtension];
	if (filterId)
		*filterId = ofn.nFilterIndex;
	return result;
}

HWND UI_CreateComboBox(HWND hWnd, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID)
{
	HWND hBox;
	hBox = CreateWindow("COMBOBOX", 0, CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD | style, PosX, PosY, Width, Height, hWnd, (HMENU)ElementID, NULL, NULL);
	return hBox;
}

HWND UI_CreateButton(HWND hWnd, TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID)
{
	HWND hBtn;
	hBtn = CreateWindow(TEXT("BUTTON"), Text, BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | WS_TABSTOP | style, PosX, PosY, Width, Height, hWnd, (HMENU)ElementID, NULL, NULL);
	return hBtn;
}

HWND UI_CreateListBox(HWND hWnd, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID)
{
	HWND hBox;
	hBox = CreateWindow("LISTBOX", 0, WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | style, PosX, PosY, Width, Height, hWnd, (HMENU)ElementID, NULL, NULL);
	return hBox;
}

HWND UI_CreateTextBox(HWND hWnd, const TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID)
{
	HWND hEdit;
	hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", Text, WS_VISIBLE | WS_CHILD | style, PosX, PosY, Width, Height, hWnd, (HMENU)ElementID, NULL, NULL);
	return hEdit;
}

HWND UI_CreateCheckBox(HWND hWnd, const TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID)
{
	HWND hEdit;
	hEdit = CreateWindowEx(0, "BUTTON", Text, BS_CHECKBOX | WS_VISIBLE | WS_CHILD | WS_GROUP | style, PosX, PosY, Width, Height, hWnd, (HMENU)ElementID, NULL, NULL);
	return hEdit;
}

HWND UI_CreateGroupBox(HWND hWnd, TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID)
{
	HWND hWndGroupBox = UI_CreateButton(hWnd, Text, PosX, PosY, Width, Height, style | BS_GROUPBOX, ElementID);
	return hWndGroupBox;
}

UINT UI_GetCheckBoxState(HWND hWnd)
{
	return SendMessage(hWnd, BM_GETCHECK, 0, 0);
}

void UI_SetCheckBoxState(HWND hWnd, BOOL checked)
{
	SendMessage(hWnd, BM_SETCHECK, checked, 0);
}

DWORD UI_ComboBoxAddLine(HWND hWnd, const TCHAR *Text)
{
	return SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)Text);
}

void UI_ComboBoxSetActiveLine(HWND hWnd, DWORD id)
{
	SendMessage(hWnd, CB_SETCURSEL, id, 0);
}

DWORD UI_ComboBoxGetActiveLine(HWND hWnd)
{
	return SendMessage(hWnd, CB_GETCURSEL, 0, 0);
}

void UI_ListBoxAddLine(HWND hWnd, const TCHAR *Text)
{
	SendMessage(hWnd, LB_ADDSTRING, 0, (LPARAM)Text);
}

void UI_ListBoxInsertLine(HWND hWnd, const TCHAR *Text, int line)
{
	SendMessage(hWnd, LB_INSERTSTRING, (WPARAM)line, (LPARAM)Text);
}

DWORD UI_ComboBoxGetCurrentLine(HWND hWnd)
{
	return SendMessage(hWnd, CB_GETCURSEL, 0, 0);
}

DWORD UI_ListBoxGetCurrentLine(HWND hWnd)
{
	return SendMessage(hWnd, LB_GETCURSEL, 0, 0);
}

DWORD UI_ListBoxGetCurrentLineText(HWND hWnd, TCHAR *buf)
{
	if (!buf)
		return 0;

	return SendMessage(hWnd, LB_GETTEXT, SendMessage(hWnd, LB_GETCURSEL, 0, 0), (LPARAM)buf);
}

void UI_ResetListBox(HWND hWnd)
{
	SendMessage(hWnd, LB_SETCURSEL, -1, 0);
	int size = SendMessage(hWnd, LB_GETCOUNT, 0, 0);
	for (int i = 0; i < size; i++)
	{
		SendMessage(hWnd, LB_DELETESTRING, 0, 0);
	}
}

void UI_ResetComboBox(HWND hWnd)
{
	SendMessage(hWnd, CB_SETCURSEL, -1, 0);
	int size = SendMessage(hWnd, CB_GETCOUNT, 0, 0);
	for (int i = 0; i < size; i++)
	{
		SendMessage(hWnd, CB_DELETESTRING, 0, 0);
	}
}

HWND UI_CreateLabel(HWND hWnd, const TCHAR *Text, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID)
{
	HWND hLabel;
	hLabel = CreateWindow("STATIC", Text, WS_VISIBLE | WS_CHILD | SS_LEFT | style, PosX, PosY, Width, Height, hWnd, (HMENU)ElementID, NULL, NULL);
	return hLabel;
}

void UI_SetLabelText(HWND hLabel, const TCHAR *Text)
{
	//SendMessage(hLabel, , (WPARAM)Text, 0);
	SetWindowText(hLabel, Text);
}

HFONT UI_CreateFont(TCHAR *Family, int size)
{
	return CreateFont(size, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, Family);
}

void UI_SetElementFont(HWND hWnd, HFONT hFont)
{
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
}

HWND UI_CreateProgressBar(HWND hWnd, int PosX, int PosY, int Width, int Height, DWORD style, int ElementID, int Range)
{
	HWND hProgBar;
	hProgBar = CreateWindowEx(0, PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | style,
		PosX, PosY, Width, Height, hWnd, (HMENU)ElementID, NULL, NULL);
	SendMessage(hProgBar, PBM_SETRANGE, 0, (LPARAM)MAKELONG(0, Range));
	SendMessage(hProgBar, PBM_SETSTEP, (WPARAM)1, 0);
	return hProgBar;
}

void UI_ProgressBarSetMarque(HWND hProgBar, BOOL state, int time)
{
	if (state)
	{
		LONG style = GetWindowLong(hProgBar, GWL_STYLE);
		if (!(style & PBS_MARQUEE))
		{
			SetWindowLong(hProgBar, GWL_STYLE, style | PBS_MARQUEE);
		}
		SendMessage(hProgBar, PBM_SETMARQUEE, (WPARAM)1, time);
	}
	else
	{
		LONG style = GetWindowLong(hProgBar, GWL_STYLE);
		if (style & PBS_MARQUEE)
			SetWindowLong(hProgBar, GWL_STYLE, style ^ PBS_MARQUEE);
	}
}

void UI_ProgressBarSetProgress(HWND hProgBar, int Progress)
{
	SendMessage(hProgBar, PBM_SETPOS, Progress, 0);
}

void UI_ProgressBarSetRange(HWND hProgBar, int Range)
{
	SendMessage(hProgBar, PBM_SETRANGE, 0, (LPARAM)MAKELONG(0, Range));
}

bool UI_ColorPicker(HWND hWnd, COLORREF *color)
{
	CHOOSECOLORA chooseCol;
	static COLORREF colRef[16];
	memset(colRef, 0, sizeof(colRef));
	memset(&chooseCol, 0, sizeof(chooseCol));
	chooseCol.lStructSize = sizeof(chooseCol);
	chooseCol.hwndOwner = hWnd;
	chooseCol.Flags = CC_FULLOPEN;
	chooseCol.lpCustColors = colRef;
	if (ChooseColor(&chooseCol))
	{
		*color = chooseCol.rgbResult;
	}
	return false;
}

// -------------- WinTree Class --------------------

void CALLBACK CheckWinTreeItem(WinTreeItem *item);
void CALLBACK UnCheckWinTreeItem(WinTreeItem *item);

WinTreeItem::WinTreeItem()
{
	m_sName = 0;
}

WinTreeItem::~WinTreeItem()
{
	if (m_sName)
		delete[] m_sName;

	WinTreeItem *item = m_pChild;
	while (item)
	{
		WinTreeItem *next = item->GetNext();
		delete item;
		item = next;
	}
	WinTreeItem *items = m_pTree->m_pItems;
	if (m_pParent)
	{
		items = m_pParent->m_pChild;
	}
	WinTreeItem *prev = items;
	while (items && items != this)
	{
		prev = items;
		items = items->m_pNext;
	}
	if (prev == this)
	{
		(m_pParent ? m_pParent->m_pChild : m_pTree->m_pItems) = m_pNext;
	}
	else if (prev)
	{
		prev->m_pNext = m_pNext;
	}
	m_pChild = 0;
	TreeView_DeleteItem(m_pTree->m_hWnd, m_hItem);
}

WinTree::WinTree()
{
	memset(this, 0, sizeof(WinTree));
}

WinTree::~WinTree()
{

}

WinTreeItem *WinTreeItem::GetRoot()
{
	WinTreeItem *parent = this;
	while (parent)
	{
		if (!parent->m_pParent)
			break;

		parent = parent->m_pParent;
	}
	return parent;
}

WinTreeItem *WinTreeItem::GetNext()
{
	return m_pNext;
}

WinTreeItem *WinTreeItem::GetParent()
{
	return m_pParent;
}

void WinTreeItem::SetActive()
{
	TreeView_SelectItem(m_pTree->m_hWnd, m_hItem);
}

int WinTreeItem::GetSelection(Node<WinTreeItem> *node)
{
	int count = 0;
	WinTreeItem *child = m_pChild;
	while (child)
	{
		node = node->GetLast();
		if (TreeView_GetCheckState(m_pTree->m_hWnd, child->m_hItem))
		{
			node->data = child;
			node->next = new Node<WinTreeItem>(0, 0);
			node = node->next;
			count++;
		}
		count += child->GetSelection(node);
		child = child->m_pNext;
	}
	return count;
}

WinTreeItem *WinTreeItem::Create(WinTree *winTree, LPCTSTR lpszItem, DWORD data, LPWINTREECALLBACK callback)
{
	WinTreeItem *item = new WinTreeItem();
	if (!item)
		return 0;

	int len = strlen(lpszItem) + 1;
	item->m_sName = new char[len];
	strcpy_s(item->m_sName, len, lpszItem);

	item->m_pParent = 0;
	item->m_pChild = 0;
	item->m_pNext = 0;
	item->m_dData = data;
	item->m_dLevel = 0;
	item->m_pTree = winTree;
	item->m_pCallback = callback;
	item->m_hMenu = 0;

	TVITEM tvi;
	TVINSERTSTRUCT tvins;

	tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
	tvi.stateMask = TVIS_STATEIMAGEMASK;

	// Set the text of the item. 
	tvi.pszText = item->m_sName;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

	// Assume the item is not a parent item, so give it a 
	// document image. 
	tvi.iImage = 0;
	tvi.iSelectedImage = 0;

	// Save the heading level in the item's application-defined 
	// data area. 
	tvi.lParam = (LPARAM)data;
	tvi.state = 0;
	tvins.item = tvi;
	tvins.hInsertAfter = TVI_FIRST;

	// Set the parent item based on the specified level. 
	tvins.hParent = TVI_ROOT;
	// Add the item to the tree-view control. 
	item->m_hItem = (HTREEITEM)SendMessage(winTree->m_hWnd, TVM_INSERTITEM,
		0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	if (item->m_hItem == NULL)
	{
		delete[] item->m_sName;
		item->~WinTreeItem();
		return NULL;
	}

	return item;
}

WinTreeItem *WinTreeItem::AddChild(LPCTSTR lpszItem, DWORD data, LPWINTREECALLBACK callback)
{
	if (m_pChild)
	{
		WinTreeItem *next = m_pChild;
		while (next->m_pNext)
			next = next->m_pNext;
		return next->AddNext(lpszItem, data, callback);
	}

	WinTreeItem *item = new WinTreeItem();
	if (!item)
		return 0;

	int len = strlen(lpszItem) + 1;
	item->m_sName = new char[len];
	strcpy_s(item->m_sName, len, lpszItem);

	item->m_pParent = this;
	item->m_pChild = 0;
	item->m_pNext = 0;
	item->m_dData = data;
	item->m_dLevel = m_dLevel + 1;
	item->m_pTree = m_pTree;
	item->m_pCallback = callback;
	item->m_hMenu = 0;

	TVITEM tvi;
	TVINSERTSTRUCT tvins;

	tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
	tvi.stateMask = TVIS_STATEIMAGEMASK;

	// Set the text of the item. 
	tvi.pszText = item->m_sName;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

	// Assume the item is not a parent item, so give it a 
	// document image. 
	tvi.iImage = 0;
	tvi.iSelectedImage = 0;

	// Save the heading level in the item's application-defined 
	// data area. 
	tvi.lParam = (LPARAM)data;
	tvi.state = 0;
	tvins.item = tvi;
	tvins.hInsertAfter = TVI_FIRST;

	// Set the parent item based on the specified level. 
	tvins.hParent = m_hItem;
	// Add the item to the tree-view control. 
	item->m_hItem = (HTREEITEM)SendMessage(m_pTree->m_hWnd, TVM_INSERTITEM,
		0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	if (item->m_hItem == NULL)
	{
		delete[] item->m_sName;
		item->~WinTreeItem();
		return NULL;
	}

	UnCheckWinTreeItem(item);

	m_pChild = item;

	return item;
}

WinTreeItem *WinTreeItem::AddNext(LPCTSTR lpszItem, DWORD data, LPWINTREECALLBACK callback)
{
	WinTreeItem *item = new WinTreeItem();
	if (!item)
		return 0;

	int len = strlen(lpszItem) + 1;
	item->m_sName = new char[len];
	strcpy_s(item->m_sName, len, lpszItem);

	item->m_pParent = m_pParent;
	item->m_pChild = 0;
	item->m_pNext = 0;
	item->m_dData = data;
	item->m_dLevel = m_dLevel;
	item->m_pTree = m_pTree;
	item->m_pCallback = callback;
	item->m_hMenu = 0;

	TVITEM tvi;
	TVINSERTSTRUCT tvins;

	tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
	tvi.stateMask = TVIS_STATEIMAGEMASK;

	// Set the text of the item. 
	tvi.pszText = item->m_sName;
	tvi.cchTextMax = sizeof(tvi.pszText) / sizeof(tvi.pszText[0]);

	// Assume the item is not a parent item, so give it a 
	// document image. 
	tvi.iImage = 0;
	tvi.iSelectedImage = 0;

	// Save the heading level in the item's application-defined 
	// data area. 
	tvi.lParam = (LPARAM)data;
	tvins.item = tvi;
	tvins.hInsertAfter = m_hItem;
	tvi.state = 0;

	// Set the parent item based on the specified level. 
	if (m_pParent)
		tvins.hParent = m_pParent->m_hItem;
	else
		tvins.hParent = TVI_ROOT;
	// Add the item to the tree-view control. 
	item->m_hItem = (HTREEITEM)SendMessage(m_pTree->m_hWnd, TVM_INSERTITEM,
		0, (LPARAM)(LPTVINSERTSTRUCT)&tvins);

	if (item->m_hItem == NULL)
	{
		delete[] item->m_sName;
		item->~WinTreeItem();
		return NULL;
	}

	m_pNext = item;
	UnCheckWinTreeItem(item);
	return item;
}

WinTreeItem *WinTreeItem::AddToLast(LPCTSTR lpszItem, DWORD data, LPWINTREECALLBACK callback)
{
	if (m_pNext)
	{
		WinTreeItem *next = m_pNext;
		while (next->m_pNext)
			next = next->m_pNext;
		return next->AddNext(lpszItem, data, callback);
	}
	return AddNext(lpszItem, data, callback);
}

void WinTreeItem::Expand()
{
	TreeView_Expand(m_pTree->m_hWnd, m_hItem, TVE_EXPAND);
}

void WinTreeItem::Collapse()
{
	TreeView_Expand(m_pTree->m_hWnd, m_hItem, TVE_COLLAPSE);
}

void WinTreeItem::SetText(LPCTSTR name)
{
	TV_ITEM tvi = {0};

	tvi.mask = TVIF_TEXT | TVIF_HANDLE;
	tvi.hItem = m_hItem;
	tvi.pszText = (LPSTR)name;

	SendMessage(m_pTree->m_hWnd, TVM_SETITEM, 0, (WPARAM)&tvi);
}

void WinTreeItem::Destroy()
{
	this->~WinTreeItem();
}

WinTree *WinTree::Create(HWND hwndParent, int style, int x, int y, int sizeX, int sizeY, int id)
{
	WinTree *tree = new WinTree();
	if (!tree)
		return 0;
	// Ensure that the common control DLL is loaded. 
	InitCommonControls();

	tree->m_hWndParent = hwndParent;
	tree->m_dId = id;
	tree->m_pItems = 0;
	tree->m_hWnd = CreateWindowEx(0,
		WC_TREEVIEW,
		TEXT("Tree View"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | style,
		x,
		y,
		sizeX,
		sizeY,
		hwndParent,
		(HMENU)id,
		0,
		NULL);
	if (!tree->m_hWnd)
	{
		tree->~WinTree();
		return 0;
	}
	SetWindowTheme(tree->m_hWnd, L"EXPLORER", NULL);
	ShowWindow(tree->m_hWnd, SW_NORMAL);
	UpdateWindow(tree->m_hWnd);

	return tree;
}

WinTreeItem *WinTree::AddItem(LPCTSTR lpszItem, DWORD data, DWORD dataType)
{
	WinTreeItem *add;
	if (!m_pItems)
	{
		add = WinTreeItem::Create(this, lpszItem, data);
		m_pItems = add;
	}
	else
	{
		add = m_pItems->AddToLast(lpszItem, data);
	}
	add->m_dDataType = dataType;
	return add;
}

void WinTree::Destroy()
{
	if (m_pItems)
		m_pItems->Destroy();

	DestroyWindow(m_hWnd);
}

void WinTree::Clear()
{
	WinTreeItem *item = m_pItems;
	m_pItems = 0;
	while (item)
	{
		WinTreeItem *next = item->GetNext();
		item->Destroy();
		item = next;
	}
}

WinTreeItem *WinTree::GetSelected()
{
	HTREEITEM hSelectedItem = TreeView_GetSelection(m_hWnd);
	if (hSelectedItem)
		return m_pItems->GetByHandle(hSelectedItem);

	return 0;
}

void CALLBACK CheckWinTreeItem(WinTreeItem *item)
{
	TreeView_SetCheckState(item->m_pTree->m_hWnd, item->m_hItem, 1);
}

void CALLBACK UnCheckWinTreeItem(WinTreeItem *item)
{
	TreeView_SetCheckState(item->m_pTree->m_hWnd, item->m_hItem, 0);
}

bool WinTree::ProcessEvent(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//if(m_hWnd != hWnd)
	//return 0;
	if (!m_pItems)
		return 0;

	if (iMsg == WM_CONTEXTMENU)
	{
		TVHITTESTINFO test;
		test.pt.x = GET_X_LPARAM(lParam);
		test.pt.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hWnd, &test.pt);
		TreeView_HitTest(m_hWnd, &test);
		if (test.flags & TVHT_ONITEM && !(test.flags & TVHT_ONITEMSTATEICON))
		{
			WinTreeItem *item = m_pItems->GetByHandle(test.hItem);
			if (item->m_hMenu)
			{
				TreeView_SelectItem(m_hWnd, test.hItem);
				SetForegroundWindow(hWnd);
				TrackPopupMenu(item->m_hMenu, TPM_TOPALIGN | TPM_LEFTALIGN, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), 0, hWnd, NULL);
			}
		}
	}
	else
	{
		LPNMHDR pNMHDR = reinterpret_cast<LPNMHDR>(lParam);
		if (pNMHDR->hwndFrom == m_hWnd && (pNMHDR->code == NM_DBLCLK || pNMHDR->code == NM_CLICK || pNMHDR->code == TVN_SELCHANGED))
		{
			if (pNMHDR->code == TVN_SELCHANGED)
			{
				WinTreeItem *item = m_pItems->GetByHandle(((LPNMTREEVIEW)lParam)->itemNew.hItem);
				m_pSelected = item;

				if (item->m_pCallback)
					item->m_pCallback(this, item, pNMHDR->code, 0);
			}
			else
			{
				TVHITTESTINFO test;
				GetCursorPos(&test.pt);
				ScreenToClient(hWnd, &test.pt);
				TreeView_HitTest(pNMHDR->hwndFrom, &test);
				if (test.flags & TVHT_ONITEM || test.flags & TVHT_ONITEMSTATEICON)
				{
					WinTreeItem *item = m_pItems->GetByHandle(test.hItem);
					if (test.flags & TVHT_ONITEMSTATEICON)
					{
						if (!TreeView_GetCheckState(m_hWnd, item->m_hItem))
						{
							item->ForAllChildren(CheckWinTreeItem);
						}
						else
						{
							item->ForAllChildren(UnCheckWinTreeItem);
						}
					}
					if (item->m_pCallback)
						item->m_pCallback(this, item, pNMHDR->code, test.flags);
				}
			}
		}
	}
	
	return 1;
}

WinTreeItem *WinTreeItem::GetByHandle(HTREEITEM item)
{
	//if(m_hItem == item)
	//return this;

	WinTreeItem *result = 0;
	WinTreeItem *next = m_pNext;

	for (next = this; next; next = next->m_pNext)
	{
		if (next->m_hItem == item)
			return next;

		if (next->m_pChild)
		{
			result = next->m_pChild->GetByHandle(item);
			if (result)
				return result;
		}
	}
	return result;
}

DWORD WinTreeItem::GetData(DWORD *type)
{
	if (type)
		*type = m_dDataType;
	return m_dData;
}

int WinTreeItem::GetLevel()
{
	return m_dLevel;
}

void WinTreeItem::SetCallback(LPWINTREECALLBACK callback)
{
	m_pCallback = callback;
}

void WinTreeItem::SetMenu(HMENU menu)
{
	m_hMenu = menu;
}

void WinTreeItem::SetData(DWORD data, DWORD type)
{
	m_dData = data;
	m_dDataType = type;
}

void WinTreeItem::ForAll(LPWINTREEFORALLCALLBACK callback)
{
	callback(this);
	if (m_pChild)
	{
		WinTreeItem *next = m_pChild;
		while (next)
		{
			next->ForAll(callback);
			next = next->m_pNext;
		}
	}
}

void WinTreeItem::ForAllChildren(LPWINTREEFORALLCALLBACK callback)
{
	if (m_pChild)
	{
		WinTreeItem *next = m_pChild;
		while (next)
		{
			next->ForAll(callback);
			next = next->m_pNext;
		}
	}
}