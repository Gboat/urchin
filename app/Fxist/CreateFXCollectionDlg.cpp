// CreateFXCollectionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Fxist.h"
#include "CreateFXCollectionDlg.h"
#include ".\createfxcollectiondlg.h"
#include "MainFrm.h"
#include "FxistView.h"
#include "FxistDoc.h"

#include "utility/dirent.h"


#define  WM_CREATECOMPLETE WM_USER + 1300
// CCreateFXCollectionDlg 对话框

IMPLEMENT_DYNAMIC(CCreateFXCollectionDlg, CDialog)
CCreateFXCollectionDlg::CCreateFXCollectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreateFXCollectionDlg::IDD, pParent)
	, m_strSrcCol(_T(""))
	, m_bMergeToOne(FALSE)
	, m_nSpliteMode(0)
	, m_nMinSize(30)
	, m_nMaxSize(30)
	, m_strBeginTag(_T(""))
	, m_strEndTag(_T(""))
	, m_bStopCreate(FALSE)
	, m_bCreating(FALSE)
	, m_strMergeFile(_T(""))
{
}

CCreateFXCollectionDlg::~CCreateFXCollectionDlg()
{
}

void CCreateFXCollectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FXCOLSRC, m_strSrcCol);
	DDX_Check(pDX, IDC_CHECK_MERGETOONE, m_bMergeToOne);
	DDX_Radio(pDX, IDC_RADIO_NOSPLITE, m_nSpliteMode);
	DDX_Text(pDX, IDC_EDIT_MINSIZE, m_nMinSize);
	DDV_MinMaxInt(pDX, m_nMinSize, 1, 100000);
	DDX_Text(pDX, IDC_EDIT_MAXSIZE, m_nMaxSize);
	DDV_MinMaxInt(pDX, m_nMaxSize, 1, 10000);
	DDX_Text(pDX, IDC_EDIT_BEGINTAG, m_strBeginTag);
	DDX_Text(pDX, IDC_EDIT_ENDTAG, m_strEndTag);
	DDX_Control(pDX, IDC_PROGRESS_CREATE, m_ctrlCreateProg);
	DDX_Text(pDX, IDC_EDIT_MERGEFILE, m_strMergeFile);
}


BEGIN_MESSAGE_MAP(CCreateFXCollectionDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_FXCOLBROWSE, OnBnClickedButtonFxcolbrowse)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_RADIO_NOSPLITE, OnBnClickedRadioNosplite)
	ON_BN_CLICKED(IDC_RADIO_RANDSPLITE, OnBnClickedRadioRandsplite)
	ON_BN_CLICKED(IDC_RADIO_TAGSPLITE, OnBnClickedRadioTagsplite)
	ON_MESSAGE(WM_CREATECOMPLETE,OnCreateComplete)
	ON_BN_CLICKED(IDC_BUTTON_MERGEFILEBROWSE, OnBnClickedButtonMergefilebrowse)
	ON_BN_CLICKED(IDC_CHECK_MERGETOONE, OnBnClickedCheckMergetoone)
END_MESSAGE_MAP()


// CCreateFXCollectionDlg 消息处理程序

void CCreateFXCollectionDlg::OnBnClickedButtonFxcolbrowse()
{
	CFxistDoc*	pDoc = (CFxistDoc*)((CFxistView*)((CMainFrame*)AfxGetApp()->GetMainWnd())->GetActiveView() )->GetDocument();
	m_strSrcCol = pDoc->BrowseForFolder();

	UpdateData(FALSE);
}

BOOL CCreateFXCollectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MINSIZE))->SetRange32(1,10000);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_MAXSIZE))->SetRange32(1,10000);

	OnBnClickedCheckMergetoone();
	OnBnClickedRadioNosplite();
	
	m_ctrlCreateProg.SetWindowText(_T(""));
	m_ctrlCreateProg.SetMarquee(FALSE, 40);	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCreateFXCollectionDlg::OnBnClickedButtonCreate()
{
	if(m_bCreating)
	{
		if(AfxMessageBox(_T("真的要终止创建进程吗？"),MB_YESNO) == IDYES)
		{
			m_bStopCreate = TRUE;
			GetDlgItem(IDC_BUTTON_CREATE)->EnableWindow(FALSE);
		}
	}
	else 
	{
		UpdateData(TRUE);
		if(m_strSrcCol == _T(""))
		{
			AfxMessageBox(_T("请选择要处理的文档集目录！"));
			return ;
		}

//		SpliteFiles(m_strSrcCol,1024*1024*1024);
//		return;
		//////////////////////////////////////////////////////////////////////////
		if(m_nSpliteMode == 1)
		{
			if(m_nMinSize > m_nMaxSize)
			{
				AfxMessageBox(_T("分割块最小值应该小于或等于最大值！"));
				return;
			}
		}
		else if(m_nSpliteMode == 2)
		{
			if(m_strBeginTag == _T("") && m_strEndTag == _T(""))
			{
				AfxMessageBox(_T("应该至少指定一个分割文件的标志标签！"));
				return;
			}
		}
		m_bCreating = TRUE;
		GetDlgItem(IDC_BUTTON_CREATE)->SetWindowText(_T("停止"));
		m_bStopCreate = FALSE;

		m_ctrlCreateProg.SetWindowText(_T("正在创建..."));
		m_ctrlCreateProg.SetMarquee(TRUE, 40);	

		AfxBeginThread((AFX_THREADPROC)CreateThreadProc,(LPVOID)this);
	}
}

void CCreateFXCollectionDlg::OnBnClickedRadioNosplite()
{
	GetDlgItem(IDC_EDIT_MINSIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MAXSIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_MINSIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_MAXSIZE)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_BEGINTAG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ENDTAG)->EnableWindow(FALSE);
}

void CCreateFXCollectionDlg::OnBnClickedRadioRandsplite()
{
	GetDlgItem(IDC_EDIT_MINSIZE)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_MAXSIZE)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_MINSIZE)->EnableWindow(TRUE);
	GetDlgItem(IDC_SPIN_MAXSIZE)->EnableWindow(TRUE);

	GetDlgItem(IDC_EDIT_BEGINTAG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_ENDTAG)->EnableWindow(FALSE);
}

void CCreateFXCollectionDlg::OnBnClickedRadioTagsplite()
{
	GetDlgItem(IDC_EDIT_MINSIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MAXSIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_MINSIZE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SPIN_MAXSIZE)->EnableWindow(FALSE);

	GetDlgItem(IDC_EDIT_BEGINTAG)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_ENDTAG)->EnableWindow(TRUE);
}
void CCreateFXCollectionDlg::EnableControls(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_FXCOLSRC)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_FXCOLBROWSE)->EnableWindow(bEnable);
	GetDlgItem(IDC_CHECK_MERGETOONE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_NOSPLITE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_RANDSPLITE)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_TAGSPLITE)->EnableWindow(bEnable);
	GetDlgItem(IDCANCEL)->EnableWindow(bEnable);	
	if(m_nSpliteMode == 0)
		OnBnClickedRadioNosplite();
	else if(m_nSpliteMode == 1)
		OnBnClickedRadioRandsplite();
	else OnBnClickedRadioTagsplite();
}
LONG CCreateFXCollectionDlg::CreateThreadProc(void* param)
{
	CCreateFXCollectionDlg* pThis = (CCreateFXCollectionDlg*)param;
	if(pThis->m_nSpliteMode == 0)
	{

	}
	else if(pThis->m_nSpliteMode == 1)
	{
		if(pThis->m_bMergeToOne)
            CreateIndexFiles(pThis->m_strSrcCol,_T("index.xml"),pThis->m_nMinSize,pThis->m_nMaxSize,&(pThis->m_bStopCreate),pThis->m_strMergeFile);
		else CreateIndexFiles(pThis->m_strSrcCol,_T("index.xml"),pThis->m_nMinSize,pThis->m_nMaxSize,&(pThis->m_bStopCreate));
	}
	else 
	{
	}

	pThis->SendMessage(WM_CREATECOMPLETE);
	return 0L;
}
LRESULT CCreateFXCollectionDlg::OnCreateComplete(WPARAM wParam,LPARAM lParam)
{
	EnableControls(TRUE);
	GetDlgItem(IDC_BUTTON_CREATE)->SetWindowText(_T("创建"));
	GetDlgItem(IDC_BUTTON_CREATE)->EnableWindow(TRUE);
	m_ctrlCreateProg.SetWindowText(_T(""));
	m_ctrlCreateProg.SetMarquee(FALSE, 40);
	if(m_bStopCreate)
		AfxMessageBox(_T("创建过程被用户中止！"));
	else AfxMessageBox(_T("创建成功！"));
	
	m_bCreating = FALSE;
	m_bStopCreate = FALSE;
	return 0L;
}

//////////////////////////////////////////////////////////////////////////
//
static char tags[][20] = 
{
	"<DOC>\n",
	"</DOC>\n",
	"\t<DOCNO>",
	"</DOCNO>\n",
	"\t<MAINFILE>",
	"</MAINFILE>\n",
	"\t<OFFSET>",
	"</OFFSET>\n",
	"\t<LENGTH>",
	"</LENGTH>\n"
};


int CCreateFXCollectionDlg::CreateIndexFiles(LPCTSTR lpszMainFileDir,LPCTSTR lpszIndexFile,int minSize,int maxSize,BOOL* bStop,LPCTSTR lpszMergeFile)
{
	int docNo = 0;
	DIR* dir = opendir(lpszMainFileDir);
	struct dirent* fl = readdir(dir);
	struct __stat64 buf;

	vector<tstring>files;
	tstring path;
	numbervar nb;
	nb.nt = CNumber::INT64;
	while ( fl != NULL )
	{
		path = lpszMainFileDir;
		path += PATH_DELIMITERA;
		path += fl->d_name;				
		int32_t ret = _tstat64(path.c_str(),&buf);
		if ( ret==0 && !(buf.st_mode & S_IFDIR) )
		{
			if ( (_tcscmp(fl->d_name, _T("."))) && (_tcscmp(fl->d_name, _T(".."))) && (_tcscmp(fl->d_name, _T("index.xml"))) )
			{
				files.push_back(path);
			}
		}
		fl = readdir(dir);
	}
	closedir(dir);			

	tstring sIndexFile;
	int hMainFile = -1;
	char* buffer = NULL;
	if(lpszMergeFile)
	{
		hMainFile = fileOpen(lpszMergeFile,O_BINARY | O_RDWR | O_RANDOM | O_CREAT, _S_IREAD | _S_IWRITE);
		buffer = new char[30*1024];

		tstring str = lpszMergeFile;
		tstring::size_type npos = -1;
		tstring::size_type pos = str.rfind(_T("\\"));
		if(pos != npos)
			str = str.substr(0,pos);
		sIndexFile = str;
		sIndexFile += _T("\\");
		sIndexFile += lpszIndexFile;
	}
	else 
	{
		sIndexFile = lpszMainFileDir;
		sIndexFile += _T("\\");
		sIndexFile += lpszIndexFile;
	}

	int hIndexFile = fileOpen(sIndexFile.c_str(),O_BINARY | O_RDWR | O_RANDOM | O_TRUNC, _S_IREAD | _S_IWRITE);	
	if(hIndexFile>=0)
		fileClose(hIndexFile);

	vector<tstring>::iterator iter = files.begin();
	while (iter != files.end())
	{
		if((*bStop) == TRUE)
			break;
		if(hMainFile >= 0)
		{
			int hSubMainFile = fileOpen((*iter).c_str(),O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
			if(hSubMainFile < 0)
			{
				CString str;
				str.Format(_T("打开文件失败：%s"),(*iter).c_str());
				AfxMessageBox(str);
			}
			else 
			{
				int64_t curPos = fileTell(hMainFile);
				size_t read = fileRead(hSubMainFile,buffer,30720);
				while(read > 0)
				{
					fileWrite(hMainFile,buffer,read);
					read = fileRead(hSubMainFile,buffer,30720);
				}
				fileSeek(hMainFile,curPos,SEEK_SET);
				CreateIndexFile(lpszMergeFile,sIndexFile.c_str(),minSize,maxSize,docNo,bStop,hMainFile);
				fileSeek(hMainFile,0,SEEK_END);
				fileClose(hSubMainFile);
			}
		}
		else 
		{
			docNo += CreateIndexFile((*iter).c_str(),sIndexFile.c_str(),minSize,maxSize,docNo,bStop);
		}	
		
		iter++;
	}
	if(hMainFile)
	{
		delete[] buffer;
		fileClose(hMainFile);
	}
	return docNo;
}

int CCreateFXCollectionDlg::CreateIndexFile(LPCTSTR lpszMainFile,LPCTSTR lpszIndexFile,int minSize,int maxSize,int startDocNo,BOOL* bStop,int hMainFile)
{
	int MainFile,IndexFile;
	char SmallFileName[50];
	if(hMainFile < 0)
	{
		MainFile=fileOpen(lpszMainFile,O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
		if(MainFile<0)
		{
			CString str;
			str.Format(_T("can not open file: %s\n"),lpszMainFile);		
			AfxMessageBox(str);
			return 0;
		}
	}	
	else MainFile = hMainFile;

	tstring strMainFileName = lpszMainFile;
	tstring::size_type pos = strMainFileName.rfind(_T("\\"));
	tstring::size_type npos = -1;
	if(pos != npos)
	{
		size_t len = strMainFileName.length();
		strMainFileName = strMainFileName.substr(pos+1,len-pos-1);
	}

	IndexFile = fileOpen(lpszIndexFile,O_BINARY | O_RDWR | O_RANDOM | _O_APPEND | O_CREAT, _S_IREAD | _S_IWRITE);
	if(IndexFile < 0)
	{
		CString str;
		str.Format(_T("can not open file: %s\n"),lpszIndexFile);		
		AfxMessageBox(str);
		return 0;
	}

	char *Buffer = new char[maxSize*1024];
	int cnt=-1;
	int64_t curpos = fileTell(MainFile);
	srand(NULL);
	int randSize = (maxSize==minSize)?maxSize: (minSize + rand()%(maxSize - minSize + 1));
	randSize *= 1024;
	int ReadSize = fileRead(MainFile,Buffer,randSize);
	while(ReadSize!=0)
	{
		if((*bStop) == TRUE)
			break;
		cnt ++;
		int Len;
		Len=ReadSize;
		while(Len>0 && Buffer[Len-1]!='\n')
		{
			Len--;
		}
		if(Len==0)
		{
			Len=ReadSize;
		}
		fileWrite(IndexFile,tags[0],strlen(tags[0]));//<DOC>
		fileWrite(IndexFile,tags[2],strlen(tags[2]));//<DOCNO>
		sprintf(SmallFileName,"%u",cnt + startDocNo);
		fileWrite(IndexFile,SmallFileName,strlen(SmallFileName));
		fileWrite(IndexFile,tags[3],strlen(tags[3]));//</DOCNO>
		fileWrite(IndexFile,tags[4],strlen(tags[4]));//<MAINFILE>
		fileWrite(IndexFile,strMainFileName.c_str(),strMainFileName.length());
		fileWrite(IndexFile,tags[5],strlen(tags[5]));//</MAINFILE>
		fileWrite(IndexFile,tags[6],strlen(tags[6]));//<OFFSET>
		sprintf(SmallFileName,"%I64d",curpos);
		fileWrite(IndexFile,SmallFileName,strlen(SmallFileName));
		fileWrite(IndexFile,tags[7],strlen(tags[7]));//</OFFSET>
		fileWrite(IndexFile,tags[8],strlen(tags[8]));//<LENGTH>
		sprintf(SmallFileName,"%u",Len);
		fileWrite(IndexFile,SmallFileName,strlen(SmallFileName));
		fileWrite(IndexFile,tags[9],strlen(tags[9]));//</LENGTH>
		fileWrite(IndexFile,tags[1],strlen(tags[1]));//</DOC>

		fileSeek(MainFile,Len-ReadSize,SEEK_CUR);
		curpos = fileTell(MainFile);
		randSize = (maxSize==minSize)?maxSize: (minSize + rand()%(maxSize - minSize + 1));
		randSize *= 1024;
		ReadSize=fileRead(MainFile,Buffer,randSize);
	}
	delete[] Buffer;
	fileClose(IndexFile);
	if(hMainFile < 0)
		fileClose(MainFile);
	return (cnt==-1)?0:cnt;
}
int CCreateFXCollectionDlg::SpliteFiles(LPCTSTR lpszMainFileDir,int64_t spliteSize)
{	
	int docNo = 0;
	DIR* dir = opendir(lpszMainFileDir);
	struct dirent* fl = readdir(dir);
	struct __stat64 buf;

	vector<tstring>files;
	tstring path;
	numbervar nb;
	nb.nt = CNumber::INT64;
	while ( fl != NULL )
	{
		path = lpszMainFileDir;
		path += PATH_DELIMITERA;
		path += fl->d_name;				
		int32_t ret = _tstat64(path.c_str(),&buf);
		if ( ret==0 && !(buf.st_mode & S_IFDIR) )
		{
			if ( (_tcscmp(fl->d_name, _T("."))) && (_tcscmp(fl->d_name, _T(".."))) && (_tcscmp(fl->d_name, _T("index.xml"))) )
			{
				files.push_back(path);
			}
		}
		fl = readdir(dir);
	}
	closedir(dir);			
	

	char	spliteFileName[256];
	int		order = 0;
	int64_t curWrite = 0;
	sprintf(spliteFileName,_T("%s\\splite_%d"),(LPCTSTR)lpszMainFileDir,order);
	FILE*	fileSplite = _tfopen(spliteFileName,_T("wb"));
	char	buffer[30720];

	vector<tstring>::iterator iter = files.begin();
	while (iter != files.end())
	{
		FILE* hMainFile = _tfopen((*iter).c_str(),_T("rb"));
		if(hMainFile == NULL)
		{
			CString str;
			str.Format(_T("打开文件失败：%s"),(*iter).c_str());
			AfxMessageBox(str);
		}
		else 
		{			
			size_t read = fread(buffer,1,30720,hMainFile);
			while (read > 0)
			{
				curWrite += read;
				if(curWrite >= spliteSize)
				{
					size_t Len;
					Len = read;
					while(Len>0 && buffer[Len-1]!='\n')
					{
						Len--;
					}
					if(Len==0)
					{
						Len = read;
					}
					fwrite(buffer,1,Len,fileSplite);
					if(read-Len > 0)
						fseek(hMainFile,(int)Len - (int)read,SEEK_CUR);
					fclose(fileSplite);
					order++;
					sprintf(spliteFileName,_T("%s\\splite_%d"),(LPCTSTR)lpszMainFileDir,order);
					fileSplite = _tfopen(spliteFileName,_T("wb"));
					curWrite = 0;
				}
				else 
					fwrite(buffer,1,read,fileSplite);				
				read = fread(buffer,1,30720,hMainFile);
			}
			fclose(hMainFile);	
		}				
		iter++;
	}
	fclose(fileSplite);		
	return order;
}

void CCreateFXCollectionDlg::OnBnClickedButtonMergefilebrowse()
{
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"列表文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	if(dlg.DoModal() == IDOK)
	{
		m_strMergeFile = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CCreateFXCollectionDlg::OnBnClickedCheckMergetoone()
{
	UpdateData(TRUE);
	if(m_bMergeToOne)
	{
		GetDlgItem(IDC_EDIT_MERGEFILE)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_MERGEFILEBROWSE)->EnableWindow(TRUE);
	}
	else 
	{
		GetDlgItem(IDC_EDIT_MERGEFILE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_MERGEFILEBROWSE)->EnableWindow(FALSE);
	}
}

