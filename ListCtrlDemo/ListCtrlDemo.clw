; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CListCtrlDemoDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ListCtrlDemo.h"

ClassCount=3
Class1=CListCtrlDemoApp
Class2=CListCtrlDemoDlg
Class3=CAboutDlg

ResourceCount=4
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDR_RESULT_MENU (Neutral)
Resource4=IDD_LISTCTRLDEMO_DIALOG

[CLS:CListCtrlDemoApp]
Type=0
HeaderFile=ListCtrlDemo.h
ImplementationFile=ListCtrlDemo.cpp
Filter=N

[CLS:CListCtrlDemoDlg]
Type=0
HeaderFile=ListCtrlDemoDlg.h
ImplementationFile=ListCtrlDemoDlg.cpp
Filter=D
LastObject=IDC_RESULT_LIST
BaseClass=CResizableDialog
VirtualFilter=dWC

[CLS:CAboutDlg]
Type=0
HeaderFile=ListCtrlDemoDlg.h
ImplementationFile=ListCtrlDemoDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_LISTCTRLDEMO_DIALOG]
Type=1
Class=CListCtrlDemoDlg
ControlCount=18
Control1=IDC_MENU_SEP,static,1342177296
Control2=IDC_SOURCE_DIR_FRAME,button,1342177287
Control3=IDC_SOURCE_DIR_LIST,listbox,1352728835
Control4=IDC_BUTTON_ADD,button,1342242816
Control5=IDC_BUTTON_DEL,button,1342242816
Control6=IDC_FILTER_COMBO,combobox,1344340226
Control7=IDC_RECURSIVE_SUB_CHECK,button,1342242819
Control8=IDC_RESULT_FRAME,button,1342177287
Control9=IDC_RESULT_LIST,SysListView32,1350631425
Control10=IDC_BUTTON_START,button,1342242816
Control11=IDC_BUTTON_STOP,button,1342242816
Control12=IDC_SUMMARY_FRAME,button,1342177287
Control13=IDC_TXT_TOTAL,static,1342308354
Control14=IDC_TXT_FILE_NUMBER,static,1342308352
Control15=IDC_TXT_LINES,static,1342308352
Control16=IDC_TXT_SHARP,static,1342308354
Control17=IDC_NUMBER,static,1342312448
Control18=IDC_LINES,static,1342312448

[MNU:IDR_RESULT_MENU (Neutral)]
Type=1
Class=?
Command1=IDM_FILE_ADD
Command2=IDM_FILE_DEL
Command3=IDM_FILE_START
Command4=IDM_FILE_STOP
Command5=IDM_FILE_EXPORT
Command6=IDM_FILE_EXIT
Command7=IDM_SETTING_FILTER
Command8=IDM_HELP_ABOUT
CommandCount=8

