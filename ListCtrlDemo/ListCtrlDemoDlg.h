// ListCtrlDemoDlg.h : header file
//

#if !defined(AFX_LISTCTRLDEMODLG_H__4D53D2D3_8FBA_48C7_8ACD_7520E8E9B13D__INCLUDED_)
#define AFX_LISTCTRLDEMODLG_H__4D53D2D3_8FBA_48C7_8ACD_7520E8E9B13D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CListCtrlDemoDlg dialog
#include "ThirdParty/MultiColumnSortListView.h"
#include "ProgressDlg.h"
#include "Counter.h"

class CListCtrlDemoDlg : public CResizableDialog
{
// Construction
public:
	CListCtrlDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CListCtrlDemoDlg)
	enum { IDD = IDD_LISTCTRLDEMO_DIALOG };
	CComboBox	m_filterComboBox;
	CListBox	m_srcDirListBox;
	CMultiColumnSortListCtrl m_resultListCtrl;
	BOOL	m_recursiveSubBtn;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ResetResult();
	void DeleteAllListItems();
	int AddRow(const CFileInfo& fi);
	// Generated message map functions
	//{{AFX_MSG(CListCtrlDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	afx_msg void OnButtonStart();
	afx_msg LRESULT OnStartCount(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndCount(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProgressUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSummaryUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg void OnButtonStop();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void InitResultListCtrl();
	void InitResizableDlgAnchor();
	void RefreshFilterArrays();
	//Member variables
protected:
	HICON m_hIcon;
	CStringArray m_sFilterArray;
private:
	CProgressDlg* m_pProgressDlg;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLDEMODLG_H__4D53D2D3_8FBA_48C7_8ACD_7520E8E9B13D__INCLUDED_)
