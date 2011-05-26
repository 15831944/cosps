// MainReBar.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MainReBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainReBar

CMainReBarCtrl::CMainReBarCtrl()
{
	m_bkBitmap.LoadBitmap(IDB_MAIN_TOOLBAR_BKG);
}

CMainReBarCtrl::~CMainReBarCtrl()
{
}


BEGIN_MESSAGE_MAP(CMainReBarCtrl, CReBarCtrl)
	//{{AFX_MSG_MAP(CMainReBar)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainReBar message handlers

BOOL CMainReBarCtrl::OnEraseBkgnd(CDC* pDC) 
{
	CRect rect;
	GetClientRect(&rect);

	BITMAP bmp;
	m_bkBitmap.GetBitmap(&bmp);
	
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	CBitmap* pOldBitmap = dc.SelectObject(&m_bkBitmap);	
	
	pDC->StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), 
		&dc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	dc.SelectObject(pOldBitmap);

	//Release
	dc.DeleteDC();

	return TRUE;
}
