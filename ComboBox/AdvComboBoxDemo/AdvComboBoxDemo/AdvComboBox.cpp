/////////////////////////////////////////////////////////////////////////////
// AdvComboBox.cpp : implementation file
// 
// CAdvComboBox Control
// Version: 2.1
// Date: September 2002
// Author: Mathias Tunared
// Email: Mathias@inorbit.com
// Copyright (c) 2002. All Rights Reserved.
//
// This code, in compiled form or as source code, may be redistributed 
// unmodified PROVIDING it is not sold for profit without the authors 
// written consent, and providing that this notice and the authors name 
// and all copyright notices remains intact.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "AdvComboBox.h"
#include "DropWnd.h"
#include "DropListBox.h"
#include "VisualStylesXP.h"
#include <algorithm>
#include "AdvComboBoxDemo.h"
#include "cflmfc/gdi_utils.h"
#include "cflmfc/UiTheme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define IDC_COMBOEDIT 108


/////////////////////////////////////////////////////////////////////////////
// CAdvComboBox
IMPLEMENT_DYNAMIC(CAdvComboBox, CWnd) 

CAdvComboBox::CAdvComboBox() : m_pDropWnd(NULL), m_bDropListVisible(0)
{
	RegisterWindowClass();
	m_pEdit = NULL;
	m_zDelta = 0;
	m_nCurSel = -1;
	m_bHasSentFocus = false;
	m_bSelItem = false;
	m_bFirstPaint = true;
	m_nMinVisItems = 5;
	m_bCodeCreate = false;
	m_bAutoAppend = TRUE;
	m_bDropButtonHot = false;
	m_bTrackMouseLeave = false;
	m_nDefaultDropItems = -1;

	m_dwACBStyle = ACBS_STANDARD;

	m_pFont = NULL;
	m_pen.CreatePen(PS_SOLID, 1, RGB(16, 16, 16));
}



CAdvComboBox::~CAdvComboBox()
{
	delete m_pFont;
	if( m_pDropWnd )
	{
		m_pDropWnd->ShowWindow( SW_HIDE );
		m_bDropListVisible = FALSE;
		m_pDropWnd->DestroyWindow();
		delete m_pDropWnd;
		m_pDropWnd = NULL;
	}
	if( m_pEdit )
	{
		m_pEdit->DestroyWindow();
		delete m_pEdit;
	}
	DeleteAllItems();
	m_pen.DeleteObject();
}


BEGIN_MESSAGE_MAP(CAdvComboBox, CWnd)
	//{{AFX_MSG_MAP(CAdvComboBox)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_SHOWWINDOW()
	ON_WM_ENABLE()
	ON_WM_CHILDACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_SELECTED_ITEM, OnSelectedItem )
	ON_MESSAGE( WM_ON_DROPDOWN_BUTTON, OnDropdownButton )
	ON_MESSAGE( WM_DESTROY_DROPLIST, OnDestroyDropdownList )
	ON_EN_KILLFOCUS(IDC_COMBOEDIT, OnKillfocusEdit)
	ON_EN_SETFOCUS(IDC_COMBOEDIT, OnSetfocusEdit)
	ON_EN_CHANGE(IDC_COMBOEDIT, OnChangeEdit)
	ON_EN_UPDATE(IDC_COMBOEDIT, OnUpdateEdit)

	ON_MESSAGE( CB_ADDSTRING, OnAddString )
	ON_MESSAGE( CB_SETCURSEL, OnSetCurSel )
	ON_MESSAGE( CB_GETCURSEL, OnGetCurSel )
	ON_MESSAGE( CB_SELECTSTRING, OnSelectString )
	ON_MESSAGE( CB_GETCOUNT, OnGetCount )
	ON_MESSAGE( CB_RESETCONTENT, OnResetContent )
	ON_MESSAGE( CB_GETLBTEXT, OnGetLBText )
	ON_MESSAGE( CB_GETLBTEXTLEN, OnGetLBTextLen )
	ON_MESSAGE( CB_GETTOPINDEX, OnGetTopIndex )
	ON_MESSAGE( CB_SETTOPINDEX, OnSetTopIndex )

END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAdvComboBox message handlers

LONG CAdvComboBox::OnAddString( WPARAM wParam, LPARAM lString )
{
	char* pStr = (char*)lString;
	return AddString( pStr );
}

LONG CAdvComboBox::OnSetCurSel( WPARAM wIndex, LPARAM lParam )
{
	int nIndex = (int)wIndex;
	return SetCurSel( nIndex );
}

LONG CAdvComboBox::OnGetCurSel( WPARAM wParam, LPARAM lParam )
{
	return GetCurSel();
}

LONG CAdvComboBox::OnSelectString( WPARAM wItemStart, LPARAM lString )
{
	int nItem = (int)wItemStart;
	char* pStr = (char*)lString;
	int nIndex = FindStringExact( nItem, pStr );
	return SetCurSel( nIndex );
}

LONG CAdvComboBox::OnGetCount( WPARAM wParam, LPARAM lParam )
{
	return GetCount();
}

LONG CAdvComboBox::OnResetContent( WPARAM wParam, LPARAM lParam )
{
	DeleteAllItems();

	m_strEdit = "";
	m_nCurSel = -1;
	SetWindowText( "" );
	if( m_pEdit )
	{
		m_pEdit->SetWindowText( "" );
	}
	Invalidate();
	return CB_OKAY;
}

LONG CAdvComboBox::OnGetLBText( WPARAM wIndex, LPARAM lString )
{
	int nIndex = (int)wIndex;
	char* pOutStr = (char*)lString;
	return GetLBText( nIndex, pOutStr );
}


LONG CAdvComboBox::OnGetLBTextLen( WPARAM wIndex, LPARAM lParam )
{
	int nIndex = (int)wIndex;
	return GetLBTextLen( nIndex );
}

LONG CAdvComboBox::OnGetTopIndex( WPARAM wParam, LPARAM lParam )
{
	return GetTopIndex();
}

LONG CAdvComboBox::OnSetTopIndex( WPARAM wIndex, LPARAM lParam )
{
	return SetTopIndex(wIndex);
}


BOOL CAdvComboBox::RegisterWindowClass()
{
	WNDCLASS wndcls;
	HINSTANCE hInst;
	hInst = AfxGetInstanceHandle();

	ASSERT( hInst != 0 );

    if( !(::GetClassInfo(hInst, ADVCOMBOBOXCTRL_CLASSNAME, &wndcls)) )
    {
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = 0;
		wndcls.cbWndExtra		= 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
        wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_WINDOW);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = ADVCOMBOBOXCTRL_CLASSNAME;

        if( !AfxRegisterClass(&wndcls) )
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }
    return TRUE;
}



BOOL CAdvComboBox::Create( DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	m_rcCombo = rect;
	m_bCodeCreate = true;

	m_dwACBStyle |= ACBS_STANDARD;

	//LoadString( nID );
	
	return CWnd::Create(NULL, "", dwStyle, m_rcCombo, pParentWnd, nID );
}

void CAdvComboBox::LoadString( UINT nStringID )
{
	UINT nIDS;
	if( nStringID )
		nIDS = nStringID;
	else
	{
		nIDS = GetWindowLong( GetSafeHwnd(), GWL_ID );
	}

	//
	// Load string from resources
	CString strItems;
	if( !strItems.LoadString( nIDS ) )
	{
		return;
	}

	//
	// Clear the list
	m_list.clear();

	//
	// Go through the string and look after '\n'.
	char seps[] = "\n";
	char *token;
	int nLen = strItems.GetLength();
	char* szTok = new char[nLen+5];
	memset( szTok, 0, nLen+5 );
	strcpy( szTok, (LPCTSTR)strItems );

	token = strtok( szTok, seps );
	while( token != NULL )
	{
		AddString( token );
		token = strtok( NULL, seps );
	}

	delete [] szTok;
}

int CAdvComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	if( !m_pFont )
	{
		LOGFONT logFont;
		memset( &logFont, 0, sizeof(LOGFONT) );
		CFont* pF = GetParent()->GetFont();
		pF->GetLogFont( &logFont );
		m_pFont = new CFont;
		m_pFont->CreateFontIndirect(&logFont);
		SetFont( m_pFont );
	}
	
	return 0;
}

void CAdvComboBox::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	//LoadString();

	if( !m_pFont )
	{
		LOGFONT logFont;
		memset( &logFont, 0, sizeof(LOGFONT) );
		CFont* pF = GetParent()->GetFont();
		pF->GetLogFont( &logFont );
		m_pFont = new CFont;
		m_pFont->CreateFontIndirect(&logFont);
		SetFont( m_pFont );
	}

	if( !m_bCodeCreate )
	{
		//
		// Do we need to show an edit control. (CBS_DROPDOWN)
		if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
		{
			if( !m_pEdit )
			{
				CRect rect;
				GetClientRect(rect);
				rect.right = rect.right - ::GetSystemMetrics(SM_CXHSCROLL);
				if( m_dwACBStyle & ACBS_STANDARD )
				{
					rect.DeflateRect( 2, 2 );
					rect.left += 1;
					rect.top += 2;
					//rect.bottom += 2;
				}

				m_pEdit = new CEdit;
				DWORD dwStyle = 0;
				dwStyle = WS_VISIBLE | WS_CHILD;
				if( GetStyle() & CBS_AUTOHSCROLL )
				{
					dwStyle |= ES_AUTOHSCROLL;
				}
				m_pEdit->Create( dwStyle, rect, this, IDC_COMBOEDIT );
				m_pEdit->SetFont( m_pFont );
				m_pEdit->SetWindowText( m_strEdit );
			}
		}
	}
	CWnd::PreSubclassWindow();
}

void CAdvComboBox::OnChildActivate() 
{
	CWnd::OnChildActivate();
	
	// TODO: Add your message handler code here
	if( !m_pFont )
	{
		LOGFONT logFont;
		memset( &logFont, 0, sizeof(LOGFONT) );
		CFont* pF = GetParent()->GetFont();
		pF->GetLogFont( &logFont );
		m_pFont = new CFont;
		m_pFont->CreateFontIndirect(&logFont);
		SetFont( m_pFont );
	}
	if( m_bCodeCreate )
	{
		//
		// Do we need to show an edit control. (CBS_DROPDOWN)
		if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
		{
			if( !m_pEdit )
			{
				CRect rect;
				GetClientRect(rect);
				rect.right = rect.right - ::GetSystemMetrics(SM_CXHSCROLL);
				if( m_dwACBStyle & ACBS_STANDARD )
				{
					rect.DeflateRect( 2, 2 );
					rect.left += 1;
					rect.top += 2;
					//rect.bottom += 2;
				}

				m_pEdit = new CEdit;
				DWORD dwStyle = 0;
				dwStyle = WS_VISIBLE | WS_CHILD;
				if( GetStyle() & CBS_AUTOHSCROLL )
				{
					dwStyle |= ES_AUTOHSCROLL;
				}
				m_pEdit->Create( dwStyle, rect, this, IDC_COMBOEDIT );
				m_pEdit->SetFont( m_pFont );
				m_pEdit->SetWindowText( m_strEdit );
			}
		}
	}
}

void CAdvComboBox::DeleteAllItems()
{
	int i, nSize = m_list.size();
	for(i = 0; i < nSize; i++)
	{
		PLIST_ITEM pItem = m_list.at(i);
		delete pItem;
	}
	m_list.clear();
}

PLIST_ITEM CAdvComboBox::GetListItem(int nIndex)
{
	if(nIndex < 0 || nIndex >= m_list.size())
	{
		return NULL;
	}
	return m_list.at(nIndex);
}

void CAdvComboBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	PLIST_ITEM pItem = GetListItem(m_nCurSel);
	if(pItem)
	{
		m_strEdit = pItem->strText;
		if( m_bFirstPaint )
		{
			if( m_pEdit )
			{
				m_pEdit->SetWindowText( m_strEdit );
				m_bFirstPaint = false;
				m_pEdit->EnableWindow( IsWindowEnabled() );
			}
		}
	}
	
	// TODO: Add your message handler code here
	CRect rect;
	CRect rcText;

	GetClientRect(rect);
	rcText = rect;
	rect.left = rect.right - ::GetSystemMetrics(SM_CXHSCROLL);
	rcText.right = rect.left-1;

	m_rcDropButton = rect;
	GetClientRect(rect);

	BOOL bWndEnabled = IsWindowEnabled();

	COLORREF clrDisabledBkg = ::GetSysColor(COLOR_BTNFACE);
	COLORREF clrDisabledBorder = ::GetSysColor(COLOR_3DDKSHADOW);
	COLORREF clrDisabledText = ::GetSysColor(COLOR_GRAYTEXT);

	if( !bWndEnabled )
	{
		if( 1 ) // Draw disabled flat control with border? Change to '0'.
		{
			dc.FillSolidRect( rect, clrDisabledBkg );
		}
		else
		{
			CBrush brDisabled(clrDisabledBkg);
			CBrush* pOldBrush = dc.SelectObject(&brDisabled);
			CPen penDisabled( PS_SOLID, 0, clrDisabledBorder);
			CPen* pOldPen = dc.SelectObject(&penDisabled);
			dc.Rectangle(rect);
			dc.SelectObject(pOldBrush);
			dc.SelectObject(pOldPen);
		}
	}
	else
	{
		COLORREF clrEnabledBkg = ::GetSysColor(COLOR_WINDOW);
		dc.FillSolidRect( rect, clrEnabledBkg );
	}


	DWORD dwBtnStyle = 0;
	if( !bWndEnabled )
	{
		dwBtnStyle |= DFCS_INACTIVE;
	}
	dwBtnStyle |= m_bDropListVisible ? (DFCS_SCROLLDOWN|DFCS_PUSHED|DFCS_FLAT) : DFCS_SCROLLDOWN;


	BOOL bThemeActive = FALSE;
	HRESULT hr;

	bThemeActive = g_xpStyle.UseVisualStyles();

	HTHEME hTheme = NULL;
	if( bThemeActive )
		hTheme = g_xpStyle.OpenThemeData( m_hWnd, L"COMBOBOX" );

	// Theme drop btn style
	int nDropBtnThemeStyle = 0;
	if( m_bDropListVisible )
	{
		nDropBtnThemeStyle = CBXS_PRESSED;
	}
	else
	{
		nDropBtnThemeStyle = CBXS_NORMAL;
		if( m_bDropButtonHot )
			nDropBtnThemeStyle = CBXS_HOT;
		if( !bWndEnabled )
			nDropBtnThemeStyle = CBXS_DISABLED;
	}

	if( m_dwACBStyle & ACBS_FLAT )
	{
		if( bThemeActive )
		{
			hr = g_xpStyle.DrawThemeBackground( hTheme, dc.m_hDC, CP_DROPDOWNBUTTON, nDropBtnThemeStyle, &m_rcDropButton, NULL);
		}
		else
		{
			dc.DrawFrameControl(m_rcDropButton, DFC_SCROLL, dwBtnStyle );
		}
	}
	else
	if( m_dwACBStyle & ACBS_STANDARD )
	{
		if( bThemeActive )
		{
			COLORREF clrBorder;
			hr = g_xpStyle.GetThemeColor( hTheme, BP_PUSHBUTTON, bWndEnabled ? PBS_NORMAL : PBS_DISABLED, TMT_BORDERCOLOR, &clrBorder );
			if( FAILED( hr ) )
			{
				clrBorder = RGB(0,0,0);
			}
			CPen penBorder( PS_SOLID, 0, clrBorder );
			CPen* oldBorderPen = dc.SelectObject( &penBorder );
			dc.Rectangle( &rect );
			m_rcDropButton.DeflateRect(0,1,0,1);
			m_rcDropButton.left -= 1;
			m_rcDropButton.right -= 1;

			if( !bWndEnabled )
			{
				COLORREF clrDisabledLightBorder;
				COLORREF clrDisabledFill;
				hr = g_xpStyle.GetThemeColor( hTheme, BP_PUSHBUTTON, bWndEnabled ? PBS_NORMAL : PBS_DISABLED, TMT_FILLCOLOR, &clrDisabledLightBorder );
				if( FAILED( hr ) )
				{
					clrDisabledLightBorder = RGB(255,255,255);
				}
				hr = g_xpStyle.GetThemeColor( hTheme, WP_DIALOG, 0, TMT_FILLCOLOR, &clrDisabledFill );
				if( FAILED( hr ) )
				{
					clrDisabledFill = RGB(255,0,0);
				}
				CPen penDisBorder( PS_SOLID, 0, clrDisabledLightBorder );
				CBrush brFill( clrDisabledBkg );//clrDisabledFill );
				CRect rcl = rect;
				rcl.DeflateRect(1,1);
				rcl.right = m_rcDropButton.left;
				CBrush *oldBr = dc.SelectObject( &brFill );
				dc.SelectObject( &penDisBorder );
				dc.Rectangle( &rcl );
				dc.SelectObject( oldBr );
			}

			dc.SelectObject( &oldBorderPen );
			// Button
			hr = g_xpStyle.DrawThemeBackground( hTheme, dc.m_hDC, CP_DROPDOWNBUTTON, nDropBtnThemeStyle, &m_rcDropButton, NULL);
		}
		else
		{
			COLORREF clrTopLeft = ::GetSysColor(COLOR_3DSHADOW);
			COLORREF clrBottomRight = ::GetSysColor(COLOR_3DHILIGHT);
			dc.Draw3dRect( &rect, clrTopLeft, clrBottomRight );
			clrTopLeft = ::GetSysColor(COLOR_3DDKSHADOW);
			clrBottomRight = ::GetSysColor(COLOR_3DLIGHT);
			rect.DeflateRect(1,1);
			dc.Draw3dRect( &rect, clrTopLeft, clrBottomRight );
			m_rcDropButton.DeflateRect(0,2,0,2);
			m_rcDropButton.left -= 2;
			m_rcDropButton.right -= 2;
			// Button
			dc.DrawFrameControl(m_rcDropButton, DFC_SCROLL, dwBtnStyle );
		}

		//
		// Adjust rects
		rcText.DeflateRect(4,3,2,3);
	}

	if( bThemeActive )
		hr = g_xpStyle.CloseThemeData( hTheme );



	if( (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWNLIST
	{
		/*
		COLORREF clrBackground;
		COLORREF clrOldBkColor;
		COLORREF clrOldTextColor;
		clrBackground = ::GetSysColor(COLOR_HIGHLIGHT);
		clrOldBkColor = dc.SetBkColor( clrBackground );
		int nOldBkMode = dc.SetBkMode( TRANSPARENT );
		CFont* pOldFont = dc.SelectObject( m_pFont );

		//dc.FillSolidRect( rcText, bWndEnabled ? clrBackground : clrDisabledBkg );
		if( m_bDropListVisible )
		{
			clrOldTextColor = dc.SetTextColor( bWndEnabled ? ::GetSysColor(COLOR_HIGHLIGHTTEXT) : clrDisabledText );
		}
		else
		{
			clrOldTextColor = dc.SetTextColor( bWndEnabled ? ::GetSysColor(COLOR_BTNTEXT) : clrDisabledText );
		}

		CString changedText;
		changedText.Format(_T("Changed Text: %s"), (LPCTSTR)m_strEdit);
		dc.DrawText( (LPCTSTR)changedText, &rcText, DT_SINGLELINE|DT_VCENTER);

		dc.SetTextColor(clrOldTextColor);
		dc.SelectObject( pOldFont );
		dc.SetBkMode( nOldBkMode );
		dc.SetBkColor(clrOldBkColor);
		*/

		int nSavedDC = dc.SaveDC();
		GetClientRect(&rect);

		cfl::UiTheme* pTheme = cfl::GetSysThemeMgr()->GetThemeData(cfl::UTI_BLUE);

		int nStateID = cfl::UTSI_NORMAL;
		if(m_bDropListVisible)
		{
			nStateID = cfl::UTSI_PRESSED;
		}
		else if(m_bDropButtonHot)
		{
			nStateID = cfl::UTSI_HOT;
		}
		pTheme->DrawThemeBackground(&dc, 0, nStateID, &rect);		

		dc.SelectObject(&m_pen);

		POINT ptOrigin;
		ptOrigin.x = ptOrigin.y = 0;
		CRect rcBox;
		cfl::DrawTriangle(&dc, ptOrigin, cfl::GD_DOWN, 5, &rcBox, cfl::GDTF_CALCRECT);

		ptOrigin.y = rect.top + (rect.Height() - rcBox.Height()) / 2;
		ptOrigin.x = rect.right - rcBox.Width() - 5;
		if(m_bDropListVisible)
		{
			ptOrigin.y += 1;
		}
		cfl::DrawTriangle(&dc, ptOrigin, cfl::GD_DOWN, 5);
		rect.right -= rcBox.Width() + 15;

		int nSel = GetCurSel();
		PLIST_ITEM pItem = (PLIST_ITEM)GetItemDataPtr(nSel);
		if(pItem != NULL)
		{
			dc.SetBkMode( TRANSPARENT );
			dc.SelectObject( m_pFont );
			rect.left += 7;
			dc.DrawText( (LPCTSTR)pItem->strText, &rect, DT_SINGLELINE|DT_VCENTER);
		}
		
		dc.RestoreDC(nSavedDC);
	}
	else
	{
		if( m_pEdit )
		{
			m_pEdit->SetFont( m_pFont );
		}
	}
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CAdvComboBox::OnEraseBkgnd(CDC* pDC) 
{
	return CWnd::OnEraseBkgnd(pDC);
//	return TRUE;	
}


int CAdvComboBox::SetItemHeight(int nIndex, int nHeight)
{
	if( nIndex == -1 )
	{
		if( nHeight < 10 || nHeight > 50 )
		{
			return CB_ERR;
		}
		else
		{
			//
			// Button rect
			GetClientRect(m_rcDropButton);
			m_rcDropButton.left = m_rcDropButton.right - ::GetSystemMetrics(SM_CXHSCROLL);

			return 0;
		}
	}
	return CB_ERR;
}

void CAdvComboBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( GetFocus() != this )
	{
		SetFocus();
	}

	//
	// Is mouse over drop button?
	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
	{
		if( m_rcDropButton.PtInRect( point ) )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			InvalidateRect( m_rcDropButton );
			Invalidate();
		}
	}
	else
	if( (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWNLIST
	{
		CRect rc = m_rcCombo;
		GetClientRect( &rc );
		if( rc.PtInRect( point ) )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			Invalidate();
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}


LONG CAdvComboBox::OnSelectedItem( WPARAM wParam, LPARAM lParam )
{
	int nPos = (int)wParam;
	PLIST_ITEM pItem = m_pDropWnd->GetListItem(nPos);
	ASSERT(pItem != NULL);

	m_strEdit = pItem->strText;
	m_nCurSel = FindStringExact( 0, m_strEdit );

	//SetWindowText( m_strEdit.c_str() );
	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
	{
		if( m_pEdit )
		{
			m_pEdit->SetWindowText( m_strEdit );
			m_pEdit->SetFocus();
			m_pEdit->SetSel( 0, -1, TRUE );
		}
	}
	// Send message to parent(dialog)
	m_bSelItem = true;
	int nId = GetDlgCtrlID();
	GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDOK), (LPARAM)m_hWnd );

	Invalidate();
	OnDestroyDropdownList(0,0);

	//
	// See to it that the drop button is redrawn
	InvalidateRect( /*m_rcDropButton*/ NULL);

	GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELCHANGE), (LPARAM)m_hWnd );

	return TRUE;
}

LONG CAdvComboBox::OnDropdownButton( WPARAM wParam, LPARAM lParam )
{
	//
	//
	if( !m_bDropListVisible )
	{
		//
		// Button is pressed
		//
		// Create list
		if( !m_pDropWnd )
		{
			CreateDropList( m_list );
		}
		m_pDropWnd->ShowWindow( SW_SHOW );
		m_bDropListVisible = TRUE;
	}
	else
	{
		OnDestroyDropdownList(0,0);
	}

	// Return TRUE if OK to go back, else return FALSE.
	return TRUE;
}


LONG CAdvComboBox::OnDestroyDropdownList( WPARAM wParam, LPARAM lParam )
{
	//
	// 
	if( m_pDropWnd )
	{
		m_pDropWnd->ShowWindow( SW_HIDE );
		m_bDropListVisible = FALSE;
		m_pDropWnd->DestroyWindow();
		delete m_pDropWnd;
		m_pDropWnd = NULL;

		InvalidateRect( /*&m_rcDropButton*/NULL );
		int nId = GetDlgCtrlID();
		if( !m_bSelItem )
		{
			GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDCANCEL), (LPARAM)m_hWnd );
		}
		else
		{
			m_bSelItem = false;
		}
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_CLOSEUP), (LPARAM)m_hWnd );
	}
	return TRUE;
}

void CAdvComboBox::CreateDropList( std::vector<PLIST_ITEM> &droplist)
{
	CRect rc;
	if( m_pDropWnd )
		ASSERT(0);
	m_pDropWnd = new CDropWnd( this, droplist );
	GetWindowRect( &rc );
	rc.top = rc.bottom ;

	// Get height by calc items in list times itemheight
	int nCount = m_list.size();
	//
	// Get screen size
	CRect rcWorkArea;
	SystemParametersInfo( SPI_GETWORKAREA, 0, (LPRECT)rcWorkArea, 0) ;
	if( rc.bottom >= rcWorkArea.bottom )
	{
		rc.bottom = rcWorkArea.bottom;
	}
	else
	{
	}
	
	int nStyle = WS_CHILD|WS_BORDER|LBS_DISABLENOSCROLL|LBS_NOTIFY;
	m_pDropWnd->Create( nStyle , rc, 1 ? GetDesktopWindow() : this, 6 );
	m_pDropWnd->GetListBoxPtr()->SetCurSel( m_nCurSel );

	m_pDropWnd->SetFont( m_pFont );

	// Send message to parent(dialog)
	int nId = GetDlgCtrlID();
	GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_DROPDOWN), (LPARAM)m_hWnd );
}

int CAdvComboBox::GetLBText(int nIndex, LPTSTR lpszText)
{
	PLIST_ITEM pItem = GetListItem(nIndex);
	if(pItem == NULL)
	{
		return CB_ERR;
	}

	_tcscpy(lpszText, (LPCTSTR)(pItem->strText));
	return pItem->strText.GetLength() + 1;
}

void CAdvComboBox::GetLBText(int nIndex, CString &rString)
{
	PLIST_ITEM pItem = GetListItem(nIndex);
	if(pItem == NULL)
	{
		rString = _T("");
		return;
	}
	rString = pItem->strText;
}

int CAdvComboBox::GetLBTextLen(int nIndex )
{
	PLIST_ITEM pItem = GetListItem(nIndex);
	if(pItem == NULL)
	{
		return CB_ERR;
	}

	return pItem->strText.GetLength()+1;
}

static bool CompareListItemPtr(PLIST_ITEM p1, PLIST_ITEM p2) 
{
	if(p1 == NULL)
	{
		return true;
	}
	if(p2 == NULL)
	{
		return false;
	}
	return p1->strText < p2->strText;
}

int CAdvComboBox::AddString(LPCTSTR lpszString)
{
	LIST_ITEM* pItem = new LIST_ITEM();
	pItem->strText = lpszString;
	m_list.push_back(pItem);
	
	if( GetStyle() & CBS_SORT )
	{		
		std::sort(m_list.begin(), m_list.end(), CompareListItemPtr);
		// Find new item
		return FindString( -1, pItem->strText );
	}
	else
		return m_list.size()-1;
}

PLIST_ITEM CAdvComboBox::AddItem(const LIST_ITEM* pListItem, PLIST_ITEM pParent)
{
	LIST_ITEM* pItem = new LIST_ITEM();
	*pItem = *pListItem;
	if(pParent)
	{
		pItem->parent = pParent;

		pParent->AddChild(pItem);
	}
	m_list.push_back(pItem);

// 	if( GetStyle() & CBS_SORT )
// 	{
// 		std::sort(m_list.begin(), m_list.end(), CompareListItemPtr);
// 	}

	return pItem;
}

int CAdvComboBox::DeleteItem(UINT nIndex)
{
	return DeleteString(nIndex);
}

int CAdvComboBox::GetText(LPTSTR lpszText)
{
	if( m_pEdit )
	{	
		CString str;
		m_pEdit->GetWindowText( str );
		_tcscpy( lpszText, (LPCTSTR)str );
		return str.GetLength();
	}
	else
	{
		_tcscpy( lpszText, m_strEdit );
		return m_strEdit.GetLength();
	}
}

void CAdvComboBox::GetText(CString &rString)
{
	if( m_pEdit )
	{	
		m_pEdit->GetWindowText( rString );
	}
	else
	{
		rString = m_strEdit;
	}
}

void CAdvComboBox::SetText(LPCTSTR lpszText)
{
	if( m_pEdit )
	{	
		m_pEdit->SetWindowText( lpszText );
	}
	m_strEdit = lpszText;
}

BOOL CAdvComboBox::PointInWindow(CPoint ptScreenPoint)
{
	CRect rc;
	GetWindowRect( &rc );
	return rc.PtInRect( ptScreenPoint );
}

BOOL CAdvComboBox::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	if( !m_bDropListVisible )
	{
		//
		// Select another string from the map
		m_zDelta += zDelta;
		if( m_zDelta >= WHEEL_DELTA )
		{
			//
			// Select item upwards
			m_zDelta = 0;
			SelPrevItem();
		}
		else
		if( m_zDelta <= -WHEEL_DELTA )
		{
			//
			// Select item downwards
			m_zDelta = 0;
			SelNextItem();
		}
	}
	else
	{
		//
		// Handle mousewheel for the droplist here
		//
		// Select another string from the map
		m_zDelta += zDelta;
		if( m_zDelta >= WHEEL_DELTA )
		{
			//
			// Scroll list upwards
			m_zDelta = 0;
			int nTop = m_pDropWnd->GetListBoxPtr()->GetTopIndex();
			nTop -= 3;
			nTop = nTop < 0 ? 0 : nTop;
			m_pDropWnd->GetListBoxPtr()->SetTopIdx( nTop, TRUE );
		}
		else
		if( m_zDelta <= -WHEEL_DELTA )
		{
			//
			// Scroll list downwards
			m_zDelta = 0;
			int nTop = m_pDropWnd->GetListBoxPtr()->GetTopIndex();
			nTop += 3;
			nTop = nTop > m_pDropWnd->GetListBoxPtr()->GetCount() ? m_pDropWnd->GetListBoxPtr()->GetCount() : nTop;
			m_pDropWnd->GetListBoxPtr()->SetTopIdx( nTop, TRUE );
		}
	}
	return TRUE;
//	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CAdvComboBox::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	//
	// Move Dropdown?
//	SetWindowPos(
}


void CAdvComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	// TODO: Add your message handler code here
	Invalidate();
//	OutputDebugString( "AdvComboBox got Focus.\n" );
	//
	// Set focus to the edit control? (CBS_DROPDOWN)
	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )	// == CBS_DROPDOWN
	{
		if( m_pEdit )
		{
			m_pEdit->SetFocus();
		}
	}
	BOOL bDropdownList = (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE);
	if( bDropdownList )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SETFOCUS), (LPARAM)m_hWnd );
	}
}

void CAdvComboBox::OnSetfocusEdit() 
{
	CWnd* pWnd = GetFocus();
	if( !m_bHasSentFocus )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SETFOCUS), (LPARAM)m_hWnd );
		m_bHasSentFocus = true;
	}
}

void CAdvComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

	// Needed for keydown's like 'Alt-C'("&Cancel" button)
	if( m_pDropWnd ) 
	{
		OnDestroyDropdownList(0,0);
	}
	Invalidate();

	BOOL bDropdownList = (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE);
	if( bDropdownList && !m_pDropWnd )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDCANCEL), (LPARAM)m_hWnd );
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_KILLFOCUS), (LPARAM)m_hWnd );
	}

}

void CAdvComboBox::OnKillfocusEdit() 
{
	Invalidate();

	CWnd* pWnd = GetFocus();
	if( !m_pDropWnd && pWnd != this )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDCANCEL), (LPARAM)m_hWnd );
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_KILLFOCUS), (LPARAM)m_hWnd );
		m_bHasSentFocus = false;
		m_pEdit->SetSel(0,0);
	}
}

void CAdvComboBox::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
}

void CAdvComboBox::ModifyACBStyle(UINT nRemoveStyle, UINT nAddStyle)
{
	if( nAddStyle & ACBS_FLAT )
	{
		nRemoveStyle |= ACBS_STANDARD;
	}
	else
	if( nAddStyle & ACBS_STANDARD )
	{
		nRemoveStyle |= ACBS_FLAT;
	}
	m_dwACBStyle &= ~nRemoveStyle;
	m_dwACBStyle |= nAddStyle;
	Invalidate();
}

int CAdvComboBox::GetCount()
{
	return m_list.size();
}

int CAdvComboBox::GetCurSel()
{
	CString str;
	GetText( str );
	return FindStringExact( -1, str );
}

int CAdvComboBox::SetCurSel(int nSelect)
{
	if( nSelect == -1 )
	{
		m_nCurSel = nSelect;
		m_strEdit = "";
		Invalidate();
		return CB_ERR;
	}
	else
	if( m_list.size() == 0 )
	{
		m_nCurSel = nSelect;
		return CB_ERR;
	}
	else
	if( nSelect < -1 || nSelect > m_list.size()-1 )
	{
		return CB_ERR;
	}
	else
	{
		m_nCurSel = nSelect;
		PLIST_ITEM pItem = GetListItem(m_nCurSel);
		m_strEdit = pItem->strText;
		Invalidate();
		return m_nCurSel;
	}
}

int CAdvComboBox::FindString(int nStartAfter, LPCTSTR lpszString)
{
	int nPos = 0, nSize = m_list.size();
	if(nStartAfter >= 0)
	{
		nPos = nStartAfter;
	}
	for( ; nPos < nSize; nPos++)
	{
		if(m_list.at(nPos)->strText == lpszString)
		{
			return nPos;
		}
	}
	return CB_ERR;
}

int CAdvComboBox::FindStringExact(int nIndexStart, LPCTSTR lpszFind)
{
	return FindString(nIndexStart, lpszFind);
}

int CAdvComboBox::SelectString(int nStartAfter, LPCTSTR lpszString)
{
	int nPos = FindString(nStartAfter, lpszString);
	if(nPos == CB_ERR)
	{
		return CB_ERR;
	}

	PLIST_ITEM pItem = m_list.at(nPos);
	ASSERT(pItem);

	m_strEdit = pItem->strText;
	Invalidate();
	return nPos;
}

int CAdvComboBox::SetItemData(int nIndex, DWORD dwItemData)
{
	PLIST_ITEM pItem = GetListItem(nIndex);
	if(pItem == NULL)
	{
		return CB_ERR;
	}
	pItem->vpItemData = (void*)dwItemData;
	return CB_OKAY;
}

DWORD CAdvComboBox::GetItemData(int nIndex)
{
	PLIST_ITEM pItem = GetListItem(nIndex);
	if(pItem == NULL)
	{
		return CB_ERR;
	}
	return (DWORD)pItem->vpItemData;
}

int CAdvComboBox::SetItemDataPtr(int nIndex, void *pData)
{
	return SetItemData(nIndex, (DWORD)pData);
}

void* CAdvComboBox::GetItemDataPtr(int nIndex)
{
	return (void*)GetListItem(nIndex);
}

void CAdvComboBox::ResetContent()
{
	DeleteAllItems();
	m_strEdit = "";
	Invalidate();
}

void AFXAPI DDX_ACBIndex( CDataExchange* pDX, int nIDC, int& index )
{
	HWND hWnd = pDX->PrepareCtrl( nIDC );
	if( pDX->m_bSaveAndValidate )
	{
		CAdvComboBox* pACB = (CAdvComboBox*)pDX->m_pDlgWnd->GetDlgItem( nIDC );
		index = pACB->GetCurSel();
	}
	else
	{
		CAdvComboBox* pACB = (CAdvComboBox*)pDX->m_pDlgWnd->GetDlgItem( nIDC );
		pACB->SetCurSel( index );
	}
}

void AFXAPI DDX_ACBString( CDataExchange* pDX, int nIDC, CString& value )
{
	HWND hWnd = pDX->PrepareCtrl( nIDC );
	if( pDX->m_bSaveAndValidate )
	{
		CAdvComboBox* pACB = (CAdvComboBox*)pDX->m_pDlgWnd->GetDlgItem( nIDC );
		pACB->GetText( value );
	}
	else
	{
		CAdvComboBox* pACB = (CAdvComboBox*)pDX->m_pDlgWnd->GetDlgItem( nIDC );
		pACB->SetText( value );
	}
}

BOOL CAdvComboBox::GetItemDisabled( int nIndex )
{
	PLIST_ITEM pItem = GetListItem(nIndex);
	if(pItem == NULL)
	{
		return CB_ERR;
	}

	return (pItem->state & ACBIS_DISABLED);
}

void CAdvComboBox::SetItemDisabled(int nIndex, BOOL bDisabled)
{
	PLIST_ITEM pItem = GetListItem(nIndex);
	if(pItem == NULL)
	{
		return;
	}

	if(bDisabled)
	{
		pItem->state |= ACBIS_DISABLED;
	}
	else
	{
		pItem->state &= ~ACBIS_DISABLED;
	}
}

BOOL CAdvComboBox::GetItemChecked( int nIndex )
{
	PLIST_ITEM pItem = GetListItem(nIndex);
	if(pItem == NULL)
	{
		return CB_ERR;
	}
	
	return (pItem->state & ACBIS_CHECKED);
}

void CAdvComboBox::SetItemChecked(int nIndex, BOOL bChecked)
{
	PLIST_ITEM pItem = GetListItem(nIndex);
	if(pItem == NULL)
	{
		return;
	}
	
	if(bChecked)
	{
		pItem->state |= ACBIS_CHECKED;
	}
	else
	{
		pItem->state &= ~ACBIS_CHECKED;
	}
}


BOOL CAdvComboBox::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		m_bAutoAppend = TRUE;

		if( pMsg->wParam == VK_RETURN )
		{
			if( m_pDropWnd )
			{
				int nPos = m_pDropWnd->GetListBoxPtr()->GetCurSel();
				SendMessage( WM_SELECTED_ITEM, (WPARAM)nPos );
			}
			else
			{
				return CWnd::PreTranslateMessage(pMsg);
			}
		}
		else
		if( pMsg->wParam == VK_ESCAPE )
		{
			if( m_pDropWnd )
			{
				OnDestroyDropdownList(0,0);
				Invalidate();
			}
		}
		else
		if( pMsg->wParam == VK_F4 )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			Invalidate();
		}
		else
		if( pMsg->wParam == VK_UP )
		{
			SelPrevItem();
		}
		else
		if( pMsg->wParam == VK_DOWN )
		{
			SelNextItem();
		}
		else
		if( pMsg->wParam == VK_DELETE || pMsg->wParam == VK_BACK )
		{
			m_bAutoAppend = FALSE;
			return CWnd::PreTranslateMessage(pMsg);
		}
		else
		if( pMsg->wParam == VK_RIGHT )
		{
			if( m_dwACBStyle & ACBS_AUTOAPPEND )
			{
				// If the cursor is at the end of the text, show auto suggest text
				if( m_pEdit )
				{
					int nS, nE;
					m_pEdit->GetSel( nS, nE );
					if( nS == nE && nS == m_pEdit->LineLength() )
					{
						OnUpdateEdit();
					}
					else
					{
						return CWnd::PreTranslateMessage(pMsg);
					}
				}
			}
			else
			{
				return CWnd::PreTranslateMessage(pMsg);
			}
		}
		else
		{
			return CWnd::PreTranslateMessage(pMsg);
		}
		return TRUE;
	}
	else
	if( pMsg->message == WM_SYSKEYDOWN )
	{
		if( pMsg->wParam == VK_DOWN ||
			pMsg->wParam == VK_UP )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			Invalidate();
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}
void CAdvComboBox::SelPrevItem()
{
	if( m_pDropWnd )
	{
		int nPos = m_pDropWnd->GetListBoxPtr()->GetCurSel();
		if( nPos > 0 )
		{
			m_pDropWnd->GetListBoxPtr()->SetCurSel( --nPos );
		}
	}
	else
	{
		int nOldSel = m_nCurSel, nPos = m_nCurSel - 1;
		for( ; nPos >= 0; nPos--)
		{
			PLIST_ITEM pItem = m_list.at(nPos);
			ASSERT(pItem);

			//make sure the item is enabled
			if( !(pItem->state & ACBIS_DISABLED) && (pItem->GetChildCount() <= 0) )
			{
				m_strEdit = pItem->strText;
				if(m_pEdit)
				{
					m_pEdit->SetWindowText(m_strEdit);
				}
				m_nCurSel = nPos;
				Invalidate();
				break;
			}
		}

		//selection changed
		if( nOldSel != m_nCurSel )
		{
			// Send message to parent(dialog)
			int nId = GetDlgCtrlID();
			GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDOK), (LPARAM)m_hWnd );
			GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELCHANGE), (LPARAM)m_hWnd );
		}
	}
}
void CAdvComboBox::SelNextItem()
{
	if( m_pDropWnd )
	{
		int nPos = m_pDropWnd->GetListBoxPtr()->GetCurSel();
		if( nPos < m_pDropWnd->GetListBoxPtr()->GetCount() )
		{
			m_pDropWnd->GetListBoxPtr()->SetCurSel( ++nPos );
		}
	}
	else
	{
		int nOldSel = m_nCurSel, nPos = m_nCurSel + 1;	
		for( ; nPos < m_list.size(); nPos++)
		{
			PLIST_ITEM pItem = m_list.at(nPos);
			ASSERT(pItem);
			
			//make sure the item is enabled
			if( !(pItem->state & ACBIS_DISABLED) && (pItem->GetChildCount() <= 0))
			{
				m_strEdit = pItem->strText;
				if(m_pEdit)
				{
					m_pEdit->SetWindowText(m_strEdit);
				}
				m_nCurSel = nPos;
				Invalidate();
				break;
			}
		}

		if( nOldSel != m_nCurSel )
		{
			// Send message to parent(dialog)
			int nId = GetDlgCtrlID();
			GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELENDOK), (LPARAM)m_hWnd );
			GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_SELCHANGE), (LPARAM)m_hWnd );
		}
	}
}
int CAdvComboBox::GetTopIndex()
{
	if( m_bDropListVisible )
	{
		if( m_pDropWnd )
		{
			return m_pDropWnd->GetListBoxPtr()->GetTopIndex();
		}
	}
	return CB_ERR;
}

int CAdvComboBox::SetTopIndex(int nIndex)
{
	if( m_bDropListVisible )
	{
		if( m_pDropWnd )
		{
			return m_pDropWnd->GetListBoxPtr()->SetTopIndex(nIndex);
		}
	}
	return CB_ERR;
}

//
// Will not allocate anything. I can't see the need in doing that.
// Everything is stored in a STL list.
int CAdvComboBox::InitStorage(int nItems, UINT nBytes)
{
	return nItems;
}

void CAdvComboBox::ShowDropDown(BOOL bShowIt)
{
	if( bShowIt )
	{
		if( !m_bDropListVisible )
		{
			SendMessage( WM_ON_DROPDOWN_BUTTON );
			Invalidate();
		}
	}
	else
	{
		if( m_bDropListVisible )
		{
			SendMessage( WM_DESTROY_DROPLIST );
			Invalidate();
		}
	}
}

void CAdvComboBox::GetDroppedControlRect(LPRECT lprect)
{
	if( m_bDropListVisible )
	{
		m_pDropWnd->GetWindowRect( lprect );
	}
}

BOOL CAdvComboBox::GetDroppedState()
{
	return m_bDropListVisible;
}

int CAdvComboBox::SetExtendedUI(BOOL bExtended)
{
	return CB_OKAY;
}

BOOL CAdvComboBox::GetExtendedUI()
{
	return FALSE;
}

int CAdvComboBox::DeleteString(UINT nIndex)
{
	if(nIndex < 0 || nIndex >= m_list.size())
	{
		return CB_ERR;
	}

	std::vector<PLIST_ITEM>::iterator iter = m_list.begin();
	std::advance(iter, nIndex);
	ASSERT(iter != m_list.end());

	PLIST_ITEM pItem = *iter;
	delete pItem;

	m_list.erase(iter);

	return m_list.size();
}

int CAdvComboBox::InsertString(int nIndex, LPCTSTR lpszString)
{
	LIST_ITEM* pItem = new LIST_ITEM;
	pItem->strText = lpszString;

	//insert at the end
	if(nIndex < 0 || nIndex >= m_list.size())
	{
		m_list.push_back(pItem);
		return m_list.size() - 1;
	}

	std::vector<PLIST_ITEM>::iterator iter = m_list.begin();
	std::advance(iter, nIndex);
	m_list.insert(iter, pItem);
	if(nIndex <= m_nCurSel)
	{
		m_nCurSel++;
	}
	return nIndex;
}




DWORD CAdvComboBox::GetEditSel()
{
	//CBS_DROPDOWN
	if( ((GetStyle() & CBS_DROPDOWN) == CBS_DROPDOWN) && m_pEdit != NULL )
	{
		return m_pEdit->GetSel();
	}
	return CB_ERR;
}

BOOL CAdvComboBox::SetEditSel(int nStartChar, int nEndChar)
{
	//CBS_DROPDOWN
	if( ((GetStyle() & CBS_DROPDOWN) == CBS_DROPDOWN) && m_pEdit != NULL )
	{
		m_pEdit->SetSel( nStartChar, nEndChar, TRUE );
		return TRUE;
	}
	return CB_ERR;
}


void CAdvComboBox::OnChangeEdit()
{
	if( !m_pDropWnd )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_EDITCHANGE), (LPARAM)m_hWnd );
	}
}

void CAdvComboBox::OnUpdateEdit()
{
	static bool bAutoAppendInProgress;
	CString strEdit;
	m_pEdit->GetWindowText( strEdit );
	if( GetFocus() == m_pEdit )
	{
		if( !bAutoAppendInProgress )
		{
			if( (m_dwACBStyle & ACBS_AUTOAPPEND) && m_bAutoAppend )
			{
				CString str = (LPCTSTR)strEdit;
				int nEditLen = str.GetLength();
				if( !nEditLen )
					return;
				int nStartSel;
				int nEndSel;

				m_pEdit->GetSel( nStartSel, nEndSel );

				PLIST_ITEM pItem;
				std::vector<PLIST_ITEM>::iterator iter = m_list.begin();
				for( ; iter != m_list.end(); iter++)
				{
					pItem = *iter;

					int nPos = pItem->strText.Find( str, 0 );
					if( nPos == 0 )
					{
						bAutoAppendInProgress = true;
						m_pEdit->SetWindowText( pItem->strText );
						m_pEdit->SetSel( nEditLen, pItem->strText.GetLength(), TRUE );
						bAutoAppendInProgress = false;
						break;
					}
				}
			}
		}

		if( !bAutoAppendInProgress )
		{
			if( m_dwACBStyle & ACBS_AUTOSUGGEST )
			{
				CString str = (LPCTSTR)strEdit;
				int nEditLen = str.GetLength();
				if( !nEditLen )
				{
					if( m_pDropWnd )
						SendMessage( WM_DESTROY_DROPLIST );
					return;
				}
				int nStartSel;
				int nEndSel;

				m_pEdit->GetSel( nStartSel, nEndSel );

				std::vector<PLIST_ITEM> suggestlist;

				PLIST_ITEM pItem;
				std::vector<PLIST_ITEM>::iterator iter = m_list.begin();
				for( ; iter != m_list.end(); iter++)
				{
					pItem = *iter;

					int nPos = pItem->strText.Find( str, 0 );
					if( nPos == 0 )
					{
						suggestlist.push_back( pItem );
					}
				}
				if( m_pDropWnd )
					SendMessage( WM_DESTROY_DROPLIST );
				if( suggestlist.size() != 0 )
					CreateDropList( suggestlist );
			}
		}
	}

	if( !m_pDropWnd )
	{
		// Send message to parent(dialog)
		int nId = GetDlgCtrlID();
		GetParent()->SendMessage( WM_COMMAND, MAKEWPARAM(nId,CBN_EDITUPDATE), (LPARAM)m_hWnd );
	}
}

void CAdvComboBox::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	
	Invalidate();
	if( m_pEdit )
	{
		m_pEdit->EnableWindow( bEnable );
	}
	
}

BOOL CAdvComboBox::LimitText(int nMaxChars)
{
	if( m_pEdit )
	{
		int nCh = !nMaxChars ? 65535 : nMaxChars;
		m_pEdit->SetLimitText( nCh );
		return TRUE;
	}
	else
	{
		return CB_ERR;
	}
}

void CAdvComboBox::SetMinVisibleItems(int nMinItems)
{
	m_nMinVisItems = nMinItems;
}

int CAdvComboBox::GetMinVisibleItems()
{
	return m_nMinVisItems;
}

void CAdvComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	bool bChange = m_bDropButtonHot;

	if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWN
	{
		if( m_rcDropButton.PtInRect( point ) )
		{
			m_bDropButtonHot = true;
		}
		else
		{
			m_bDropButtonHot = false;
		}
	}
	else
	if( (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWNLIST
	{
		GetClientRect( &m_rcCombo );
		if( m_rcCombo.PtInRect( point ) )
		{
			m_bDropButtonHot = true;
		}
		else
		{
			m_bDropButtonHot = false;
		}
	}


	if( bChange != m_bDropButtonHot )
	{
		if( !m_bTrackMouseLeave )
		{
			//
			// Could not use TrackMouseEvent() under Release. A bug appeared 
			// the second time the dropwindow was shown, and I did not 
			// have to strenght to go look for it. :)
/*			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			TrackMouseEvent(&tme);*/

			m_bTrackMouseLeave = true;
			SetTimer(1, 50, NULL);
		}

		InvalidateRect( /*&m_rcDropButton*/ NULL);
	}

	CWnd::OnMouseMove(nFlags, point);
}

void CAdvComboBox::OnMouseLeave() 
{
	// TODO: Add your message handler code here and/or call default
	bool bChange = m_bDropButtonHot;
	m_bDropButtonHot = false;
	m_bTrackMouseLeave = false;

	if( bChange != m_bDropButtonHot )
		InvalidateRect( /*&m_rcDropButton*/ NULL);
}


void CAdvComboBox::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == 1 )
	{
		CPoint point;
		GetCursorPos( &point );
		ScreenToClient( &point );
		if( (GetStyle() & CBS_DROPDOWN) && !(GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWN
		{
			if( !m_rcDropButton.PtInRect( point ) )
			{
				KillTimer( 1 );
				OnMouseLeave();
			}
		}
		else
		if( (GetStyle() & CBS_DROPDOWN) && (GetStyle() & CBS_SIMPLE) )  // == CBS_DROPDOWNLIST
		{
			GetClientRect( &m_rcCombo );
			if( !m_rcCombo.PtInRect( point ) )
			{
				KillTimer( 1 );
				OnMouseLeave();
			}
		}

	}
	CWnd::OnTimer(nIDEvent);
}


int CAdvComboBox::GetDefaultVisibleItems()
{
	return m_nDefaultDropItems;
}

void CAdvComboBox::SetDefaultVisibleItems(int nItems)
{
	m_nDefaultDropItems = nItems;
}
