
// MessageHelperTestDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "MessageHelperTest.h"
#include "MessageHelperTestDlg.h"
#include "afxdialogex.h"
#include "resource.h"

#include "../ScheduleLib/WindowsMessageHelperThread.h"
#include "../ScheduleLib/WindowsMessageParameter.h"
#include "../ScheduleLib/AutoRecvWMP.h"
#define g_WindowsMessageHelper	CWindowsMessageHelperThread::GetSingleton()
#define WMH_TARGET_WND			_T("dialog")

#include "TestObj1.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMessageHelperTestDlg 대화 상자



CMessageHelperTestDlg::CMessageHelperTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MESSAGEHELPERTEST_DIALOG, pParent)
	, m_nThreadCount(10)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMessageHelperTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editLog);
	DDX_Text(pDX, IDC_EDIT_THREAD_COUNT, m_nThreadCount);
	DDV_MinMaxInt(pDX, m_nThreadCount, 1, 100000);
}

BEGIN_MESSAGE_MAP(CMessageHelperTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_TEST, &CMessageHelperTestDlg::OnBnClickedButtonTest)
	ON_WM_CLOSE()

	ON_MESSAGE(WMH_ID_TEST, &CMessageHelperTestDlg::OnWindowsMessageHelperTest)

	ON_BN_CLICKED(IDC_BUTTON_TEST_STOP, &CMessageHelperTestDlg::OnBnClickedButtonTestStop)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMessageHelperTestDlg 메시지 처리기

BOOL CMessageHelperTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_lRecvCount = 0;
	
	CWindowsMessageHelperThread* pWMHelperThread = static_cast<CWindowsMessageHelperThread*>(AfxBeginThread(RUNTIME_CLASS(CWindowsMessageHelperThread), THREAD_PRIORITY_HIGHEST, 0, CREATE_SUSPENDED));
	pWMHelperThread->AddTargetMsgWnd(WMH_TARGET_WND, (CWnd*)this);
	pWMHelperThread->ResumeThread();
	pWMHelperThread->SetSingleton(pWMHelperThread);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMessageHelperTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMessageHelperTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMessageHelperTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



LRESULT CMessageHelperTestDlg::OnWindowsMessageHelperTest(WPARAM wParam, LPARAM lParam)
{
	CAutoRecvWMP WMP(wParam);

	int nVar1 = WMP.GetData<int>();
	int nVar2 = WMP.GetData<int>();
	
	ASSERT(nVar1 == 1);
	ASSERT(nVar2 == 2);
	
	float fVar1 = WMP.GetData<float>();
	float fVar2 = WMP.GetData<float>();

	ASSERT(fVar1 == 0.1f);
	ASSERT(fVar2 == 0.2f);


	CString strVar1 = WMP.GetData<CString>();
	CString strVar2 = WMP.GetData<CString>();

	ASSERT(strVar1 == _T("String Test1"));
	ASSERT(strVar2 == _T("String Test2"));


	pair<char*, int> buffer = WMP.GetData< pair<char*, int> >();
	delete buffer.first;

	CTestObj1 obj1 = WMP.GetData<CTestObj1>();
	ASSERT(obj1.m_nA == 10);
	ASSERT(obj1.m_strB == _T("Class String Test1"));

	CTestObj1* pObj1 = WMP.GetData<CTestObj1*>();
	ASSERT(pObj1->m_nA == 11);
	ASSERT(pObj1->m_strB == _T("Class String Test2"));
	delete pObj1;


	m_llLastElapsedMicroSeconds = WMP.GetElapsedMicroseconds();
	m_lRecvIdx = WMP.GetUniqueIndex();
	m_lRecvCount++;
	return 1;
}

void CMessageHelperTestDlg::OnBnClickedButtonTest()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	GetDlgItem(IDC_EDIT_THREAD_COUNT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_TEST_STOP)->EnableWindow(TRUE);
	

	auto implThread = [&]()
	{
		while (false == m_bExitThread)
		{
			CWindowsMessage wmp(WMH_TARGET_WND, CWindowsMessage::eMSG_Type::eType_Post, WMH_ID_TEST);
			//wmp.m_eJobType = eType_Reserve;
			wmp.GetParameter().SetData<int>(1);
			wmp.GetParameter().SetData<int>(2);
			wmp.GetParameter().SetData<float>(0.1f);
			wmp.GetParameter().SetData<float>(0.2f);

			wmp.GetParameter().SetData<CString>(_T("String Test1"));
			wmp.GetParameter().SetData<CString>(_T("String Test2"));
			wmp.GetParameter().SetData< pair<char*,int> >( pair<char*, int>(new char[1024], 1024 ) );
			
			CTestObj1 obj1;
			obj1.m_nA = 10;
			obj1.m_strB = _T("Class String Test1");
			wmp.GetParameter().SetData<CTestObj1>(obj1);

			CTestObj1* pObj1 = new CTestObj1();
			pObj1->m_nA = 11;
			pObj1->m_strB = _T("Class String Test2");
			wmp.GetParameter().SetData<CTestObj1*>(pObj1);
			
			wmp.Execute(true);
			Sleep(1);
		}

	};

	m_lRecvCount = 0;
	m_bExitThread = false;

	m_vThreadList.reserve(m_nThreadCount);
	for (int i = 0; i < m_nThreadCount; i++)
	{
		m_vThreadList.push_back( new std::thread(implThread));
	}
	SetTimer(TIMER_THROUGHT, 1000, nullptr);
}


void CMessageHelperTestDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	KillTimer(TIMER_THROUGHT);
	m_bExitThread = true;
	for (auto &it : m_vThreadList)
	{
		std::thread* pThread = it;
		pThread->join();
		delete pThread;
	}
	m_vThreadList.clear();

	// Delete
	if (g_WindowsMessageHelper != NULL)
	{
		g_WindowsMessageHelper->SetExitThread();
		WaitForSingleObject(g_WindowsMessageHelper->m_hThread, INFINITE);
	}

	CDialog::OnClose();
}


void CMessageHelperTestDlg::OnBnClickedButtonTestStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	KillTimer(TIMER_THROUGHT);
	m_bExitThread = true;
	for (auto &it : m_vThreadList)
	{
		std::thread* pThread = it;
		pThread->join();
		delete pThread;
	}
	m_vThreadList.clear();


	GetDlgItem(IDC_EDIT_THREAD_COUNT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_TEST)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_TEST_STOP)->EnableWindow(FALSE);

	AfxMessageBox(_T("Test Complete"));


}


void CMessageHelperTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	switch (nIDEvent)
	{
	case TIMER_THROUGHT:
		{
			//g_WindowsMessageHelper->SetReserveJobLock(!g_WindowsMessageHelper->GetReserveJobLock());
			bool bLock = g_WindowsMessageHelper->GetReserveJobLock();
			int nReadyQueueSize = g_WindowsMessageHelper->GetReadyQueueSize();
			int nMsgQueueSize = g_WindowsMessageHelper->GetMsgQueueSize();
			int nReserveQueueSize = g_WindowsMessageHelper->GetReserveQueueSize();
			CString strTmp;
			strTmp.Format(_T("Lock[%d], Throughtput : %ld rv/s, E.Micro. : %lld, Ready.Q[%d], Msg.Q[%d], Reserve.Q[%d], U.Idx : %ld"), bLock, m_lRecvCount, m_llLastElapsedMicroSeconds, nReadyQueueSize, nMsgQueueSize, nReserveQueueSize, m_lRecvIdx);
			m_editLog.SetWindowText(strTmp);
			m_lRecvCount = 0;
		}
		break;
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}
