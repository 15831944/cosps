// Downloader.cpp: implementation of the CDownloader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Downloader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDownloader::CDownloader()
{
}

CDownloader::~CDownloader()
{
}

CDownloader* CDownloader::GetNext()
{
	return NULL;
}

LPCTSTR CDownloader::GetName()
{
	return _T("");
}


