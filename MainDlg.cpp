// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"
#include "CefClientImpl.h"
#include "resource.h"

extern CefClientImpl* g_sh;

#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited = FALSE;
}

CMainDlg::~CMainDlg()
{
}

HWND hMessageWnd = NULL;
HWND CreateMessageWindow(HINSTANCE hInstance);
LRESULT CALLBACK MessageWndProc(HWND, UINT, WPARAM, LPARAM);
int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// 		MARGINS mar = {5,5,30,5};
	// 		DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	SetMsgHandled(FALSE);
	hMessageWnd = CreateMessageWindow(SApplication::getSingleton().GetInstance());
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	return 0;
}


//TODO:消息映射
void CMainDlg::OnClose()
{
	if (g_sh && !g_sh->IsClosing()) {
		CefRefPtr<CefBrowser> browser = g_sh->GetBrowser();
		if (browser.get()) {
			// Notify the browser window that we would like to close it. This
			// will result in a call to ClientHandler::DoClose() if the
			// JavaScript 'onbeforeunload' event handler allows it.
			browser->GetHost()->CloseBrowser(false);
		}
	}
	SetMsgHandled(FALSE);
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void CMainDlg::OnDestroy()
{
   SetMsgHandled(TRUE);
   if(m_pScriptModule)
   {//脚本独有的事件
	   EventExit evt(this);
	   FireEvent(evt);
   }

   SWindow::SSendMessage(WM_DESTROY);

   if(m_pTipCtrl)
   {
	   GetMsgLoop()->RemoveMessageFilter(m_pTipCtrl);
	   GETTOOLTIPFACTORY->DestroyToolTip(m_pTipCtrl);
	   m_pTipCtrl = NULL;
   }
   if(m_hostAttr.m_bTranslucent && m_dummyWnd.IsWindow())
   {
	   m_dummyWnd.DestroyWindow();
   }
}


HWND CreateMessageWindow(HINSTANCE hInstance) {
	static const wchar_t kWndClass[] = L"ClientMessageWindow";

	WNDCLASSEX wc = {0};
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = MessageWndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = kWndClass;
	RegisterClassEx(&wc);

	return CreateWindow(kWndClass, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0,
		hInstance, 0);
}

LRESULT CALLBACK MessageWndProc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam) 
{
	switch (message) 
	{
	    case WM_COMMAND: 
		{
		   int wmId = LOWORD(wParam);
		   switch (wmId) 
		   {
	         case ID_QUIT:
				 {
                    PostQuitMessage(0);
				 }
		      return 0;
		   }
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}




