// SegmentDownloader.cpp: implementation of the CSegmentDownloader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SegmentDownloader.h"
#include "SegmentInfoMap.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

size_t CSegmentDownloader::HeaderCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	CCallbackParam* lpParam = (CCallbackParam*)userdata;
	return lpParam->pDownloader->ProcessHeader(ptr, size, nmemb, lpParam->nIndex);
}

size_t CSegmentDownloader::DataCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	CCallbackParam* lpParam = (CCallbackParam*)userdata;
	return lpParam->pDownloader->ProcessData(ptr, size, nmemb, lpParam->nIndex);
}

int CSegmentDownloader::ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CCallbackParam* lpParam = (CCallbackParam*)clientp;
	return lpParam->pDownloader->ProcessProgress(dltotal, dlnow, ultotal, ulnow, lpParam->nIndex);
}

UINT CSegmentDownloader::DownloadProc(LPVOID lpvData)
{
	CSegmentDownloader* lpDownloder = (CSegmentDownloader*)lpvData;

	lpDownloder->Download();

	return 0;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSegmentDownloader::CSegmentDownloader() : m_curlm(NULL), m_pSegmentInfoArray(NULL)
{
}

CSegmentDownloader::~CSegmentDownloader()
{
	m_pSegmentInfoArray = NULL;
}

void CSegmentDownloader::Init(const CDownloadParam& param)
{
	m_dlParam = param;

	m_pSegmentInfoArray = new CSegmentInfoArray();
	CSegmentInfoMap::GetInstance()->AddSegmentInfoArray(m_dlParam.m_szUrl, m_pSegmentInfoArray);

	m_curlm = curl_multi_init();
	curl_multi_setopt(m_curlm, CURLMOPT_MAXCONNECTS, (long)MAX_WORKER_SESSION);

	//Init Multi CURL
	CArray<CRange, CRange&> sizeArray;
	SplitFileRange(m_dlParam.m_nFileSize, sizeArray);
	
	int i, nSize;	
	for(i = 0, nSize = sizeArray.GetSize(); i < nSize; i++)
	{
		CRange& segment = sizeArray.GetAt(i);
		CURL* easy_handle = InitEasyHandle(segment.cx, segment.cy, i);
		
		curl_multi_add_handle(m_curlm, easy_handle); 
	}
}

CURL* CSegmentDownloader::InitEasyHandle(int nStartPos, int nFinishPos, int nIndex)
{
	CURL* easy_handle = curl_easy_init();

	//URL address
	curl_easy_setopt(easy_handle, CURLOPT_URL, (LPCTSTR)m_dlParam.m_szUrl);

	//agent: IE8
	curl_easy_setopt(easy_handle, CURLOPT_USERAGENT, USER_AGENT_IE8);
	//redirect
	curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
	//no verbose
	curl_easy_setopt(easy_handle, CURLOPT_VERBOSE, 0L);
	//connect timeout: 10s
	curl_easy_setopt(easy_handle, CURLOPT_CONNECTTIMEOUT, 10L);

	curl_easy_setopt(easy_handle, CURLOPT_LOW_SPEED_LIMIT, 1L);
	curl_easy_setopt(easy_handle, CURLOPT_LOW_SPEED_TIME, 15L);

	//segment download, range setting
	CString szRange;
	szRange.Format("%d-%d", nStartPos, nFinishPos);
	curl_easy_setopt(easy_handle, CURLOPT_RANGE, (LPCTSTR)szRange);
	
	CCallbackParam* pCallbackParam = new CCallbackParam(nIndex, this);
	//header
	curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, CSegmentDownloader::HeaderCallback);
	curl_easy_setopt(easy_handle, CURLOPT_HEADERDATA, pCallbackParam);
	
	//data
	curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, CSegmentDownloader::DataCallback);
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, pCallbackParam);
	
	//progress
	curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(easy_handle, CURLOPT_PROGRESSFUNCTION, CSegmentDownloader::ProgressCallback);
	curl_easy_setopt(easy_handle, CURLOPT_PROGRESSDATA, pCallbackParam);
	
	//private
	curl_easy_setopt(easy_handle, CURLOPT_PRIVATE, pCallbackParam);
	
	//Init parameters
	CSegmentInfoEx* pSegmentInfo = NULL;
	if(nIndex < m_pSegmentInfoArray->GetSize())
	{
		pSegmentInfo = GetSegmentInfo(nIndex);

		ASSERT(pSegmentInfo->m_nIndex == nIndex);
		pSegmentInfo->m_nDlBefore += pSegmentInfo->m_nDlNow;
		pSegmentInfo->m_nDlNow = 0;
		pSegmentInfo->m_nRetry++;

		pSegmentInfo->m_range.cx = nStartPos;
		pSegmentInfo->m_range.cy = nFinishPos;

		char buf[32];
		
		sprintf(buf, "header_%d(%d).txt", nIndex, pSegmentInfo->m_nRetry);
		pSegmentInfo->m_lpFileHeader = fopen(buf, "wb");
		
		sprintf(buf, "data_%d", nIndex);
		pSegmentInfo->m_lpFileData = fopen(buf, "ab");

		pSegmentInfo->m_curl = easy_handle;
	}
	else
	{
		pSegmentInfo = new CSegmentInfoEx();
		AddSegmentInfo(pSegmentInfo);

		pSegmentInfo->m_nIndex = nIndex;
		pSegmentInfo->m_nDlBefore = 0;
		pSegmentInfo->m_nDlNow = 0;
		pSegmentInfo->m_nRetry = 0;

		pSegmentInfo->m_range.cx = nStartPos;
		pSegmentInfo->m_range.cy = nFinishPos;

		char buf[32];
		
		sprintf(buf, "header_%d.txt", nIndex);
		pSegmentInfo->m_lpFileHeader = fopen(buf, "wb");
		
		sprintf(buf, "data_%d", nIndex);
		pSegmentInfo->m_lpFileData = fopen(buf, "wb");

		pSegmentInfo->m_curl = easy_handle;
	}

	/*
	//exist already, retry
	if(nIndex < m_segInfos.GetSize())
	{
		ASSERT(nIndex >= 0 && nIndex < m_segInfos.GetSize());
		CSegmentInfoEx& segmentInfo = m_segInfos.ElementAt(nIndex);

		ASSERT(segmentInfo.m_nIndex == nIndex);
		segmentInfo.m_curl = easy_handle;

		segmentInfo.m_nDlBefore += segmentInfo.m_nDlNow;
		segmentInfo.m_nDlNow = 0;
		segmentInfo.m_nRetry++;

		segmentInfo.m_range.cx = nStartPos;
		segmentInfo.m_range.cy = nFinishPos;

		char buf[32];
		
		sprintf(buf, "header_%d(%d).txt", nIndex, segmentInfo.m_nRetry);
		segmentInfo.m_lpFileHeader = fopen(buf, "wb");
		
		sprintf(buf, "data_%d", nIndex);
		segmentInfo.m_lpFileData = fopen(buf, "ab");
	}
	//new task
	else
	{
		CSegmentInfoEx segmentInfo;
		segmentInfo.m_nIndex = nIndex;
		segmentInfo.m_curl = easy_handle;

		segmentInfo.m_nDlBefore = 0;
		segmentInfo.m_nDlNow = 0;
		segmentInfo.m_nRetry = 0;

		segmentInfo.m_range.cx = nStartPos;
		segmentInfo.m_range.cy = nFinishPos;

		char buf[32];

		sprintf(buf, "header_%d.txt", nIndex);
		segmentInfo.m_lpFileHeader = fopen(buf, "wb");
		
		sprintf(buf, "data_%d", nIndex);
		segmentInfo.m_lpFileData = fopen(buf, "wb");

		m_segInfos.Add(segmentInfo);
	}
	*/

	return easy_handle;
}

void CSegmentDownloader::Start()
{
	AfxBeginThread(CSegmentDownloader::DownloadProc, (LPVOID)this);
//	Download();
}
void CSegmentDownloader::Download()
{
	int still_running = -1; /* keep number of running handles */

	fd_set fdread;
    fd_set fdwrite;
    fd_set fdexcep;
	int maxfd = -1;
	long curl_timeout = -1;
	struct timeval timeout;

	CURLMsg *msg; /* for picking up messages with the transfer status */
	int msgs_left; /* how many messages are left */
	int rc = 0;	//return value

	CString szLog;
	CResultCode result_code;

	while (still_running) 
	{
		result_code.m_nMajor = TASK_STATUS_OK;
		result_code.m_nMinor = INTERNAL_OK;

		curl_multi_perform(m_curlm, &still_running);
		
		if (still_running) 
		{
			FD_ZERO(&fdread);
			FD_ZERO(&fdwrite);
			FD_ZERO(&fdexcep);
			
			/* get file descriptors from the transfers */
			rc = curl_multi_fdset(m_curlm, &fdread, &fdwrite, &fdexcep, &maxfd);
			if(rc != CURLM_OK)
			{
				szLog.Format("[curl_multi_fdset] error: (%d)", rc);
				LOG4CPLUS_ERROR_STR(ROOT_LOGGER, (LPCSTR)szLog)

				result_code.m_nMajor = TASK_STATUS_TERMINATED;
				result_code.m_nMinor = INTERNAL_MULTI_FDSET_ERROR;
				break;
			}
			
			rc = curl_multi_timeout(m_curlm, &curl_timeout);
			if(rc != CURLM_OK)
			{
				szLog.Format("[curl_multi_timeout] error: (%d)", rc);
				LOG4CPLUS_ERROR_STR(ROOT_LOGGER, (LPCSTR)szLog)	
				
				result_code.m_nMajor = TASK_STATUS_TERMINATED;
				result_code.m_nMinor = INTERNAL_MULTI_TIMEOUT_ERROR;
				break;
			}
			if (curl_timeout == -1)
			{
				curl_timeout = 100;
			}			
			if (maxfd == -1) 
			{
				Sleep(curl_timeout);
			} 
			else 
			{
				timeout.tv_sec = curl_timeout / 1000;
				timeout.tv_usec = (curl_timeout % 1000) * 1000;
				
				/* In a real-world program you OF COURSE check the return code of the
				function calls.  On success, the value of maxfd is guaranteed to be
				greater or equal than -1.  We call select(maxfd + 1, ...), specially in
				case of (maxfd == -1), we call select(0, ...), which is basically equal
				to sleep. */
				rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);

				switch(rc) 
				{
				case -1:
					/* select error */
					{
						szLog.Format("[select] error: maxfd = %d, curl_timeout = %ld, %d - %s", maxfd, curl_timeout, 
							errno, strerror(errno));
						LOG4CPLUS_ERROR_STR(ROOT_LOGGER, (LPCSTR)szLog)	

						result_code.m_nMajor = TASK_STATUS_TERMINATED;
						result_code.m_nMinor = INTERNAL_SELECT_ERROR;
					}
					break;
				case 0: /* timeout */
					{
						if(IS_LOG_ENABLED(ROOT_LOGGER, log4cplus::TRACE_LOG_LEVEL))
						{
							szLog.Format("[select] timeout: maxfd = %d, curl_timeout = %ld", maxfd, curl_timeout);
							LOG4CPLUS_TRACE_STR(ROOT_LOGGER, (LPCSTR)szLog)	
						}
					}
				default: /* action */
					{
						//do nothing
					}
				}
			}
		}
		rc = 0;
		
		/* See how the transfers went */
		while( (msg = curl_multi_info_read(m_curlm, &msgs_left)) )
		{
			//Done
			if (msg->msg == CURLMSG_DONE) 
			{
				//Get the current connection information
				CCallbackParam* pCallbackParam = NULL;
				curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &pCallbackParam);

				ASSERT(pCallbackParam != NULL);

				//Data backup already
				int nIndex = pCallbackParam->nIndex;
				CURL* easy_handle = msg->easy_handle;
				CURLcode resCode = msg->data.result;

				CSegmentInfoEx* pSegmentInfo = GetSegmentInfo(nIndex);
				
				//log
				szLog.Format("(%d) - Transfer result: %d - %s", nIndex, resCode, curl_easy_strerror(resCode));
				LOG4CPLUS_INFO_STR(ROOT_LOGGER, (LPCTSTR)szLog)

				switch(resCode)
				{
				case CURLE_OK:
					{
						//Just stop connection
						StopConnection(nIndex, DLE_OK);
					}
					break;
				case CURLE_ABORTED_BY_CALLBACK:
					{
						//(1). Stop
						if(m_controlInfo.isStopped)
						{
							szLog.Format("(%d) - Connection stopped", nIndex);
							LOG4CPLUS_INFO_STR(ROOT_LOGGER, (LPCTSTR)szLog)

							result_code.m_nMajor = TASK_STATUS_USER_STOPPED;

							StopConnection(nIndex, DLE_STOP);
						}
						//(2). Pause
						else if(m_controlInfo.isPaused)
						{
							szLog.Format("(%d) - Connection paused", nIndex);
							LOG4CPLUS_INFO_STR(ROOT_LOGGER, (LPCTSTR)szLog)

							result_code.m_nMajor = TASK_STATUS_USER_PAUSED;

							StopConnection(nIndex, DLE_PAUSE);
						}
					}
					break;
				case CURLE_OPERATION_TIMEDOUT:
					{
						StopConnection(nIndex, DLE_OTHER);

						CURL* retry_handle = InitEasyHandle(pSegmentInfo->m_range.cx + pSegmentInfo->m_nDlNow, 
							pSegmentInfo->m_range.cy, nIndex);
						curl_multi_add_handle(m_curlm, retry_handle);
						still_running++; //just to prevent it from remaining at 0 if there are more URLs to get
					}
					break;
				//ohter cases: should be actual error, retry
				default:
					{
						StopConnection(nIndex, DLE_OTHER);

						if(pSegmentInfo->m_nRetry < MAX_RETRY_TIMES)
						{							 
							CURL* retry_handle = InitEasyHandle(pSegmentInfo->m_range.cx + pSegmentInfo->m_nDlNow, 
								pSegmentInfo->m_range.cy, nIndex);
							curl_multi_add_handle(m_curlm, retry_handle);
							still_running++; //just to prevent it from remaining at 0 if there are more URLs to get
						}
						else
						{
							rc = resCode;
							szLog.Format("(%d) - Retry times more than %d, abort download", nIndex, MAX_RETRY_TIMES);
							LOG4CPLUS_ERROR_STR(ROOT_LOGGER, (LPCTSTR)szLog)

							result_code.m_nMajor = TASK_STATUS_TERMINATED_CURL_CODE;
							result_code.m_nMinor = resCode;
						}
					}
					break;
				}
			}
			//error
			else 
			{
				szLog.Format("[curl_multi_info_read] error: msg->msg = %d", msg->msg);
				LOG4CPLUS_ERROR_STR(ROOT_LOGGER, (LPCTSTR)szLog)
			}
		}

		if(rc != 0)
		{
			break;
		}
	}
	
	curl_multi_cleanup(m_curlm);

	CString szErrorMsg;
	BOOL bResult = FormatErrorMsg(result_code, szErrorMsg);

	if(bResult)
	{
		szLog.Format("Download successfully");
		LOG4CPLUS_INFO_STR(ROOT_LOGGER, (LPCTSTR)szLog)

		RemoveSegmentInfoArray();
	}
	else
	{
		szLog.Format("Download failed: %s", szErrorMsg);
		LOG4CPLUS_ERROR_STR(ROOT_LOGGER, (LPCTSTR)szLog)
	}

	::SendMessage(m_dlParam.m_hWnd, WM_DOWNLOAD_COMPLETE, 0, 0);

	/*
	if(rc != 0)
	{
		szLog.Format("Download failed");
		LOG4CPLUS_ERROR_STR(ROOT_LOGGER, (LPCTSTR)szLog)
	}
	else
	{
		szLog.Format("Download successfully");
		LOG4CPLUS_INFO_STR(ROOT_LOGGER, (LPCTSTR)szLog)
	}
	*/
}

void CSegmentDownloader::Stop()
{
	m_ctritialSection.Lock();

	m_controlInfo.isStopped = TRUE;

	m_ctritialSection.Unlock();
}

void CSegmentDownloader::Pause()
{
	m_ctritialSection.Lock();

	m_controlInfo.isModified = TRUE;
	m_controlInfo.isPaused = TRUE;
	
	m_ctritialSection.Unlock();
}

void CSegmentDownloader::Resume()
{
	m_ctritialSection.Lock();
	
	m_controlInfo.isModified = TRUE;
	m_controlInfo.isPaused = FALSE;
	
	m_ctritialSection.Unlock();

	m_curlm = curl_multi_init();
	curl_multi_setopt(m_curlm, CURLMOPT_MAXCONNECTS, (long)MAX_WORKER_SESSION);
	
	int i, nSize;

	int nStartPos, nFinishPos;

	CString szLog;
	for(i = 0, nSize = m_pSegmentInfoArray->GetSize(); i < nSize; i++)
	{
		CSegmentInfoEx* pSegmentInfo = GetSegmentInfo(i);

		nStartPos = pSegmentInfo->m_range.cx + pSegmentInfo->m_nDlNow;
		nFinishPos = pSegmentInfo->m_range.cy;

		if(nStartPos > nFinishPos)
		{
			szLog.Format("(%d) - Transfer ended. %d - %d", i, nStartPos, nFinishPos);
			LOG4CPLUS_INFO_STR(ROOT_LOGGER, (LPCTSTR)szLog)

			continue;
		}
		CURL* easy_handle = InitEasyHandle(nStartPos, nFinishPos, i);
		
		curl_multi_add_handle(m_curlm, easy_handle); 
	}

	AfxBeginThread(CSegmentDownloader::DownloadProc, (LPVOID)this);
}

size_t CSegmentDownloader::ProcessHeader(char *ptr, size_t size, size_t nmemb, int index)
{
	if( IS_LOG_ENABLED(ROOT_LOGGER, log4cplus::DEBUG_LOG_LEVEL) )
	{
		CString szLine(ptr, size * nmemb);
		szLine.Replace(_T("\r"), _T("[0x0D]"));
		szLine.Replace(_T("\n"), _T("[0x0A]"));

		CString szMsg;
		szMsg.Format("%d - %s", index, szLine);
		LOG4CPLUS_DEBUG_STR(ROOT_LOGGER, (LPCTSTR)szMsg)
	}
	
	CSegmentInfoEx* pSegmentInfo = GetSegmentInfo(index);
	fwrite(ptr, size, nmemb, pSegmentInfo->m_lpFileHeader);
	
	return (size_t)(size * nmemb);

	return size * nmemb;
}

size_t CSegmentDownloader::ProcessData(char *ptr, size_t size, size_t nmemb, int index)
{
	CSegmentInfoEx* pSegmentInfo = GetSegmentInfo(index);
	fwrite(ptr, size, nmemb, pSegmentInfo->m_lpFileData);

	return size * nmemb;
}

int CSegmentDownloader::ProcessProgress(double dltotal, double dlnow, double ultotal, double ulnow, int nIndex)
{
	ASSERT(nIndex >= 0 && nIndex < m_pSegmentInfoArray->GetSize());
	CSegmentInfoEx* pSegmentInfo = GetSegmentInfo(nIndex);

	pSegmentInfo->m_nDlNow = (DWORD64)dlnow;

	DWORD64 nDlNow = GetTotalDownloadNow();

	//Send progress notification
	CProgressInfo progressInfo;
	progressInfo.dltotal = (DWORD64)m_dlParam.m_nFileSize;
	progressInfo.dlnow = (DWORD64)nDlNow;
	progressInfo.ultotal = (DWORD64)ultotal;
	progressInfo.ulnow = (DWORD64)ulnow;
	progressInfo.retCode = -1;
	progressInfo.szReason = "";
	progressInfo.index = m_dlParam.m_nIndex;
	
	::SendMessage(m_dlParam.m_hWnd, WM_DOWNLOAD_PROGRESS, (WPARAM)&progressInfo, (LPARAM)0);

	//Pause
	if(m_controlInfo.IsModified() && m_controlInfo.IsPaused())
	{
		return 5;
	}
	//Stop
	if(m_controlInfo.IsStopped())
	{
		return 5;
	}

	return 0;
}

DWORD64 CSegmentDownloader::GetTotalDownloadNow()
{	
	DWORD64 nDlNow = 0;
	int i, nSize;	
	for(i = 0, nSize = m_pSegmentInfoArray->GetSize(); i < nSize; i++)
	{
		CSegmentInfoEx* pSegmentInfo = GetSegmentInfo(i);
		nDlNow += pSegmentInfo->m_nDlNow + pSegmentInfo->m_nDlBefore;
	}

	return nDlNow;
}

void CSegmentDownloader::SplitFileRange(UINT nFileSize, CArray<CRange, CRange&>& sizeArray)
{
	int i;
	CRange segment;
	int nMinSize = nFileSize / MAX_WORKER_SESSION;
	if(nMinSize >= MIN_SEGMENT_SIZE)
	{
		for(i = 0; i < MAX_WORKER_SESSION - 1; i++)
		{
			segment.cx = i * nMinSize;
			segment.cy = segment.cx + nMinSize - 1;
			sizeArray.Add(segment);
		}
		segment.cx = i * nMinSize;
		segment.cy = nFileSize - 1;
		sizeArray.Add(segment);
	}
	else
	{
		int nSessionNumber = nFileSize / MIN_SEGMENT_SIZE;
		if(nSessionNumber > 0)
		{
			for(i = 0; i < nSessionNumber; i++)
			{
				segment.cx = i * MIN_SEGMENT_SIZE;
				segment.cy = segment.cx + MIN_SEGMENT_SIZE - 1;
				sizeArray.Add(segment);
			}
			if(nFileSize % MIN_SEGMENT_SIZE != 0)
			{
				segment.cx = nSessionNumber * MIN_SEGMENT_SIZE;
				segment.cy = nFileSize - 1;
				sizeArray.Add(segment);
			}			
		}
		//total size is less than MIN_SEGMENT_SIZE
		else
		{
			segment.cx = 0;
			segment.cy = nFileSize - 1;
			sizeArray.Add(segment);
		}		
	}
}

void CSegmentDownloader::StopConnection(int nIndex, int nCleanType)
{
	CSegmentInfoEx* pSegmentInfo = GetSegmentInfo(nIndex);

	//Get the current connection information
	CCallbackParam* pCallbackParam = NULL;
	curl_easy_getinfo(pSegmentInfo->m_curl, CURLINFO_PRIVATE, &pCallbackParam);
	ASSERT(pCallbackParam != NULL);
	
	//clean up
	curl_multi_remove_handle(m_curlm, pSegmentInfo->m_curl);
	curl_easy_cleanup(pSegmentInfo->m_curl);
	
	if(nCleanType == DLE_STOP)
	{
		pSegmentInfo->m_nRetry = 0;
	}
	pSegmentInfo->m_curl = NULL;
	
	fclose(pSegmentInfo->m_lpFileHeader);
	fclose(pSegmentInfo->m_lpFileData);
	if(pCallbackParam != NULL)
	{
		delete pCallbackParam;
		pCallbackParam = NULL;
	}
}

CSegmentInfoEx* CSegmentDownloader::GetSegmentInfo(int nIndex)
{
	ASSERT(m_pSegmentInfoArray != NULL);
	ASSERT(nIndex >= 0 && nIndex < m_pSegmentInfoArray->GetSize());

	return (CSegmentInfoEx*)m_pSegmentInfoArray->GetAt(nIndex);
}
void CSegmentDownloader::AddSegmentInfo(CSegmentInfoEx* pSegmentInfo)
{
	ASSERT(pSegmentInfo != NULL);
	m_pSegmentInfoArray->Add(pSegmentInfo);
}

void CSegmentDownloader::RemoveSegmentInfoArray()
{
	CSegmentInfoArray* pSegInfoArray = CSegmentInfoMap::GetInstance()->GetSegmentInfoArray(m_dlParam.m_szUrl);
	ASSERT(pSegInfoArray != NULL && m_pSegmentInfoArray == pSegInfoArray);

	CSegmentInfoMap::GetInstance()->RemoveSegmentInfoArray(m_dlParam.m_szUrl);

	m_pSegmentInfoArray = NULL;
}

BOOL CSegmentDownloader::FormatErrorMsg(CResultCode result_code, CString& szErrorMsg)
{
	BOOL bResult = FALSE;
	switch(result_code.m_nMajor)
	{
	case TASK_STATUS_OK:
		{
			szErrorMsg = "No Error";
			bResult = TRUE;
		}
		break;
	case TASK_STATUS_USER_PAUSED:
		{
			szErrorMsg = "Paused";
		}
		break;
	case TASK_STATUS_USER_STOPPED:
		{
			szErrorMsg = "Stopped";
		}
		break;
	case TASK_STATUS_TERMINATED:
		{
			FormatInternalErrorMsg(result_code.m_nMinor, szErrorMsg);
		}
		break;
	case TASK_STATUS_TERMINATED_CURL_CODE:
		{
			szErrorMsg.Format("[Transfer Error]: %d - %s", result_code.m_nMinor, curl_easy_strerror((CURLcode)result_code.m_nMinor));
		}
		break;
	default:
		{
			szErrorMsg.Format("Unkonwn error - %d", result_code.m_nMajor);
		}
		break;
	}

	return bResult;
}

void CSegmentDownloader::FormatInternalErrorMsg(int nCode, CString& szErrorMsg)
{
	switch(nCode)
	{
	case INTERNAL_OK:
		{
			szErrorMsg = "[Internal Error]: no error";
		}
		break;
	case INTERNAL_MULTI_FDSET_ERROR:
		{
			szErrorMsg = "[Internal Error]: multi_fdset error";
		}
		break;
	case INTERNAL_MULTI_TIMEOUT_ERROR:
		{
			szErrorMsg = "[Internal Error]: multi_timout error";
		}
		break;
	case INTERNAL_SELECT_ERROR:
		{
			szErrorMsg = "[Internal Error]: select error";
		}
		break;
	case INTERNAL_RETRY_OVER_ERROR:
		{
			szErrorMsg = "[Internal Error]: retry over error";
		}
		break;
	default:
		{
			szErrorMsg.Format("[Internal Error]: Unkonwn error - %d", nCode);
		}
		break;
	}
}
