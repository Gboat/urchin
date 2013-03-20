// PluginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "PluginDlg.h"
#include ".\plugindlg.h"
#include "plugin/FirteXCOM.h"


// CPluginDlg 对话框

IMPLEMENT_DYNCREATE(CPluginDlg, CChildDialog)
CPluginDlg::CPluginDlg(CWnd* pParent /*=NULL*/)
	: CChildDialog(CPluginDlg::IDD, pParent)
{
}

CPluginDlg::~CPluginDlg()
{
}

void CPluginDlg::DoDataExchange(CDataExchange* pDX)
{
	CChildDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PLUGIN, m_ctrlistPlugin);
}


BEGIN_MESSAGE_MAP(CPluginDlg, CChildDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PLUGIN, OnLvnItemchangedListPlugin)
	ON_BN_CLICKED(IDC_BUTTON_SETUP, OnBnClickedButtonSetup)
	ON_BN_CLICKED(IDC_BUTTON_GETMORE, OnBnClickedButtonGetmore)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PLUGIN, OnNMClickListPlugin)
END_MESSAGE_MAP()


// CPluginDlg 消息处理程序
void CPluginDlg::OnOpenIndex()
{

}
void CPluginDlg::OnCloseIndex()
{

}

BOOL CPluginDlg::OnInitDialog()
{
	CChildDialog::OnInitDialog();

	AddAnchor(IDC_LIST_PLUGIN,TOP_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDC_GROUP71,BOTTOM_LEFT,BOTTOM_RIGHT);
	AddAnchor(IDC_STATIC71,BOTTOM_LEFT);	
	AddAnchor(IDC_STATIC73,BOTTOM_LEFT);
	AddAnchor(IDC_STATIC74,BOTTOM_LEFT);
	AddAnchor(IDC_STATIC75,BOTTOM_LEFT);
	AddAnchor(IDC_BUTTON_SETUP,BOTTOM_RIGHT);
	AddAnchor(IDC_BUTTON_GETMORE,BOTTOM_RIGHT);

	AddAnchor(IDC_STATIC_PLUGNAME,BOTTOM_LEFT);	
	AddAnchor(IDC_STATIC_PLUGID,BOTTOM_LEFT);
	AddAnchor(IDC_STATIC_PLUGCLSID,BOTTOM_LEFT);
	AddAnchor(IDC_STATIC_PLUGENABLE,BOTTOM_LEFT);

	m_ctrlistPlugin.SetExtendedStyle(m_ctrlistPlugin.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	m_ctrlistPlugin.SetExtendedStyle(m_ctrlistPlugin.GetExtendedStyle()|LVS_EX_CHECKBOXES);
	m_ctrlistPlugin.InsertColumn(0,"插件",LVCFMT_LEFT,300);

	LoadPlugins();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CPluginDlg::LoadPlugins()
{
	int iCount = 0;

	CString sPath = GlobalConfig.General.PluginPath.c_str();	
	iCount = FindFiles(sPath);
	
}
int CPluginDlg::FindFiles(CString strDir)
{
	int iCount = 0;

	WIN32_FIND_DATA FindFileData;
	CString sPath = strDir;
	CString strExt = "*.dll";
	sPath += "\\";

	CString sTmp = sPath + strExt;

	CString strFind;
	int nDot;

	HANDLE hFind;
	hFind = FindFirstFile((LPCSTR)sTmp, &FindFileData);
	while( hFind != INVALID_HANDLE_VALUE)
	{
		if(!strcmp(FindFileData.cFileName,".") ||!strcmp(FindFileData.cFileName,"..") )
		{
			if(FindNextFile(hFind,&FindFileData) != TRUE)
				break;
			continue;
		}
		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			/*CString sTmp2 = sPath;
			sTmp2 += FindFileData.cFileName;			
			iCount += FindFiles(sTmp2);*/
			continue;
		}
		else 
		{			
			strFind = FindFileData.cFileName;

			nDot = strFind.ReverseFind('.');
			if(nDot >= 0)
			{
				strFind = strFind.Mid(0,nDot);	
				m_ctrlistPlugin.InsertItem(m_ctrlistPlugin.GetItemCount(),strFind);
			}			
		}


		if(FindNextFile(hFind,&FindFileData) != TRUE)
			break;
	}

	FindClose(hFind);
	return iCount;
}

void CPluginDlg::displayPluginInfo(firtex::plugin::CPlugin* pPlugin)
{
	if(pPlugin)
	{
		GetDlgItem(IDC_STATIC_PLUGNAME)->SetWindowText(pPlugin->getName().c_str());		
		GetDlgItem(IDC_STATIC_PLUGID)->SetWindowText(pPlugin->getIdentifier().c_str());
		GetDlgItem(IDC_STATIC_PLUGCLSID)->SetWindowText(pPlugin->getCLSIDAsString().c_str());
		GetDlgItem(IDC_STATIC_PLUGENABLE)->SetWindowText(pPlugin->isEnabled()?_T("是"):_T("否"));
		if(pPlugin->isInstalled())
		{
			GetDlgItem(IDC_BUTTON_SETUP)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SETUP)->SetWindowText(_T("卸载"));			
		}
		else
		{
			GetDlgItem(IDC_BUTTON_SETUP)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SETUP)->SetWindowText(_T("安装"));			
		}
	}
	else 
	{
		GetDlgItem(IDC_STATIC_PLUGNAME)->SetWindowText(_T("?"));		
		GetDlgItem(IDC_STATIC_PLUGID)->SetWindowText(_T("?"));
		GetDlgItem(IDC_STATIC_PLUGCLSID)->SetWindowText(_T("?"));
		GetDlgItem(IDC_STATIC_PLUGENABLE)->SetWindowText(_T("?"));
		GetDlgItem(IDC_BUTTON_SETUP)->EnableWindow(FALSE);
	}
}

void CPluginDlg::OnLvnItemchangedListPlugin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	POSITION pos = m_ctrlistPlugin.GetFirstSelectedItemPosition();
	int curSel = m_ctrlistPlugin.GetNextSelectedItem(pos);
	if(curSel >= 0)
	{
		CString str = m_ctrlistPlugin.GetItemText(curSel,0);		
		firtex::plugin::CPlugin* pPlugin = firtex::plugin::GlobalPlugins.findByFileName((LPCTSTR)str);
		displayPluginInfo(pPlugin);
	}
	*pResult = 0;
}
void CPluginDlg::OnNMClickListPlugin(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_ctrlistPlugin.GetFirstSelectedItemPosition();
	int curSel = m_ctrlistPlugin.GetNextSelectedItem(pos);
	if(curSel >= 0)
	{
		CString str = m_ctrlistPlugin.GetItemText(curSel,0);		
		firtex::plugin::CPlugin* pPlugin = firtex::plugin::GlobalPlugins.findByFileName((LPCTSTR)str);
		displayPluginInfo(pPlugin);
	}
	*pResult = 0;
}
void CPluginDlg::OnBnClickedButtonSetup()
{
	POSITION pos = m_ctrlistPlugin.GetFirstSelectedItemPosition();
	int curSel = m_ctrlistPlugin.GetNextSelectedItem(pos);
	if(curSel >= 0)
	{
		CString str = m_ctrlistPlugin.GetItemText(curSel,0);		
		firtex::plugin::CPlugin* pPlugin = firtex::plugin::GlobalPlugins.findByFileName((LPCTSTR)str);
		if(pPlugin)
		{
			if(pPlugin->isInstalled())
			{
				CString str;
				str.Format(_T("你真的要卸载组件： %s 吗？"),pPlugin->getName().c_str());
				if(AfxMessageBox(str,MB_YESNO)==IDYES)
				{
					if(pPlugin->uninstall())
					{						
						GetDlgItem(IDC_BUTTON_SETUP)->SetWindowText(_T("安装"));
					}
				}
			}
			else 
			{
				if(pPlugin->install())
					GetDlgItem(IDC_BUTTON_SETUP)->SetWindowText(_T("卸载"));			
			}
		}	
	}
}

void CPluginDlg::OnBnClickedButtonGetmore()
{
	TCHAR key[MAX_PATH + MAX_PATH];

	CString url = _T("http://www.firtex.org");

	HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, SW_HIDE);

	if ((UINT)result <= HINSTANCE_ERROR) 
	{

		if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) {
			lstrcat(key, _T("\\shell\\open\\command"));

			if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) {
				TCHAR *pos;
				pos = _tcsstr(key, _T("\"%1\""));
				if (pos == NULL) {                     // No quotes found
					pos = _tcsstr(key, _T("%1"));      // Check for %1, without quotes 
					if (pos == NULL)                   // No parameter at all...
						pos = key+lstrlen(key)-1;
					else
						*pos = '\0';                   // Remove the parameter
				}
				else
					*pos = '\0';                       // Remove the parameter

				lstrcat(pos, _T(" "));
				lstrcat(pos, url);

				USES_CONVERSION;
				result = (HINSTANCE) WinExec(T2A(key),SW_HIDE);
			}
		}
	}
}
LONG CPluginDlg::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hkey;
	LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

	if (retval == ERROR_SUCCESS) {
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue(hkey, NULL, data, &datasize);
		lstrcpy(retdata,data);
		RegCloseKey(hkey);
	}

	return retval;
}


