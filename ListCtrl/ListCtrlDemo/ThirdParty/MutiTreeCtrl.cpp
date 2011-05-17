#include "stdafx.h"
#include "MutiTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT ID_TREE_ITEM_SELECTED_EVENT = ::RegisterWindowMessage(_T("ID_TREE_ITEM_SELECTED_EVENT"));

CMutiTreeCtrl::CMutiTreeCtrl()
{
	m_uFlags=0;
}

CMutiTreeCtrl::~CMutiTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CMutiTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CMutiTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_CLICK, OnStateIconClick)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMutiTreeCtrl message handlers

void CMutiTreeCtrl::OnStateIconClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	if(m_uFlags & TVHT_ONITEMSTATEICON) 
	{
		*pResult = 1;
		CWnd* pOwner = GetOwner();
		
		if(pOwner != NULL)
		{
			pOwner->SendMessage(ID_TREE_ITEM_SELECTED_EVENT, 0, m_uFlags);
		}
	}
}

void CMutiTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	HTREEITEM hItem =HitTest(point, &m_uFlags);
	if ( (m_uFlags&TVHT_ONITEMSTATEICON ))
	{
		//nState: 0->��ѡ��ť 1->û��ѡ�� 2->����ѡ�� 3->ȫ��ѡ��
		UINT nState = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
		nState = (nState == TVIS_IMAGE_STATE_FULL_CHECK) ? TVIS_IMAGE_STATE_UNCHECK : TVIS_IMAGE_STATE_FULL_CHECK;
		SetItemState( hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK );
		Select(hItem, TVGN_CARET);

		CList<HTREEITEM, HTREEITEM> hItemList;
		GetSelectedItems(hItemList);
	}
	
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CMutiTreeCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
 	//����ո��
 	if(nChar==0x20)
 	{
 		HTREEITEM hItem =GetSelectedItem();
 		UINT nState = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
 		if(nState!=0)
 		{
 			nState=(nState==3)?1:3;
 			SetItemState( hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK );
 		}
 	}
	else CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMutiTreeCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	
	*pResult = 0;
}

BOOL CMutiTreeCtrl::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearch)
{
	BOOL bReturn=CTreeCtrl::SetItemState( hItem, nState, nStateMask );

	UINT iState = nState >> 12;
	if(iState!=0)
	{
		if(bSearch) TravelChild(hItem, iState);
		TravelSiblingAndParent(hItem,iState);
	}
	return bReturn;
}

void CMutiTreeCtrl::TravelChild(HTREEITEM hItem, int nState)
{
	HTREEITEM hChildItem,hBrotherItem;
	
	//�����ӽڵ㣬û�оͽ���
	hChildItem=GetChildItem(hItem);
	if(hChildItem!=NULL)
	{
		//�����ӽڵ��״̬�뵱ǰ�ڵ��״̬һ��
		CTreeCtrl::SetItemState( hChildItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK );
		//�ٵݹ鴦���ӽڵ���ӽڵ���ֵܽڵ�
		TravelChild(hChildItem, nState);
		
		//�����ӽڵ���ֵܽڵ�����ӽڵ�
		hBrotherItem=GetNextSiblingItem(hChildItem);
		while (hBrotherItem)
		{
			//�����ӽڵ���ֵܽڵ�״̬�뵱ǰ�ڵ��״̬һ��
			int nState1 = GetItemState( hBrotherItem, TVIS_STATEIMAGEMASK ) >> 12;
			if(nState1!=0)
			{
				CTreeCtrl::SetItemState( hBrotherItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK );
			}
			//�ٵݹ鴦���ӽڵ���ֵܽڵ���ӽڵ���ֵܽڵ�
			TravelChild(hBrotherItem, nState);
			hBrotherItem=GetNextSiblingItem(hBrotherItem);
		}
	}
}

void CMutiTreeCtrl::TravelSiblingAndParent(HTREEITEM hItem, int nState)
{
	HTREEITEM hNextSiblingItem,hPrevSiblingItem,hParentItem;
	
	//���Ҹ��ڵ㣬û�оͽ���
	hParentItem=GetParentItem(hItem);
	if(hParentItem!=NULL)
	{
		int nState1=nState;//���ʼֵ����ֹû���ֵܽڵ�ʱ����
		
		//���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
		hNextSiblingItem=GetNextSiblingItem(hItem);
		while(hNextSiblingItem!=NULL)
		{
			nState1 = GetItemState( hNextSiblingItem, TVIS_STATEIMAGEMASK ) >> 12;
			if(nState1!=nState && nState1!=0) break;
			else hNextSiblingItem=GetNextSiblingItem(hNextSiblingItem);
		}
		
		if(nState1==nState)
		{
			//���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
			hPrevSiblingItem=GetPrevSiblingItem(hItem);
			while(hPrevSiblingItem!=NULL)
			{
				nState1 = GetItemState( hPrevSiblingItem, TVIS_STATEIMAGEMASK ) >> 12;
				if(nState1!=nState && nState1!=0) break;
				else hPrevSiblingItem=GetPrevSiblingItem(hPrevSiblingItem);
			}
		}
		
		if(nState1==nState || nState1==0)
		{
			nState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK ) >> 12;
			if(nState1!=0)
			{
				//���״̬һ�£��򸸽ڵ��״̬�뵱ǰ�ڵ��״̬һ��
				CTreeCtrl::SetItemState( hParentItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK );
			}
			//�ٵݹ鴦���ڵ���ֵܽڵ���丸�ڵ�
			TravelSiblingAndParent(hParentItem,nState);
		}
		else
		{
			//״̬��һ�£���ǰ�ڵ�ĸ��ڵ㡢���ڵ�ĸ��ڵ㡭��״̬��Ϊ����̬
			hParentItem=GetParentItem(hItem);
			while(hParentItem!=NULL)
			{
				nState1 = GetItemState( hParentItem, TVIS_STATEIMAGEMASK ) >> 12;
				if(nState1!=0)
				{
					CTreeCtrl::SetItemState( hParentItem, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK );
				}
				hParentItem=GetParentItem(hParentItem);
			}
		}
	}	
}

UINT CMutiTreeCtrl::GetItemImageState(HTREEITEM hItem)
{
	UINT nState = GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;
	return nState;
}

BOOL CMutiTreeCtrl::GetSelectedItems(CList<HTREEITEM, HTREEITEM>& hItemList)
{
	// Look at all of the root-level items
	HTREEITEM hCurrent = /*GetNextItem(TVI_ROOT, TVGN_NEXT);*/GetRootItem();
	if(hCurrent == NULL)
	{
		return FALSE;
	}

	//Iterator all the selected leaf nodes.
	CList<HTREEITEM, HTREEITEM> htiParentList;
	htiParentList.AddTail(hCurrent);
	UINT nState;
	while(!htiParentList.IsEmpty())
	{
		hCurrent = htiParentList.RemoveHead();
		nState = GetItemImageState(hCurrent);
		if(nState != TVIS_IMAGE_STATE_FULL_CHECK && nState != TVIS_IMAGE_STATE_PARTIAL_CHECK)
		{
			continue;
		}
		//Not a leaf node
		if(ItemHasChildren(hCurrent))
		{
			HTREEITEM hChildItem = GetChildItem(hCurrent);
			while (hChildItem != NULL)
			{
				htiParentList.AddTail(hChildItem);
				hChildItem = GetNextItem(hChildItem, TVGN_NEXT);
			}
		}
		//leaf node can't have a partial check state
		else
		{
			hItemList.AddTail(hCurrent);
// 			// Get the text for the item. Notice we use TVIF_TEXT because
// 			// we want to retrieve only the text, but also specify TVIF_HANDLE
// 			// because we're getting the item by its handle.
// 			TVITEM item;
// 			TCHAR szText[1024];
// 			item.hItem = hCurrent;
// 			item.mask = TVIF_TEXT | TVIF_HANDLE;
// 			item.pszText = szText;
// 			item.cchTextMax = 1024;
// 			
// 			BOOL bWorked = GetItem(&item);
// 			if(bWorked)
// 			{
// 				hItemList.AddTail(hCurrent);
// 				AfxTrace("Selected Item: %s\n", szText);
// 			}
		}
	}
	return TRUE;
}