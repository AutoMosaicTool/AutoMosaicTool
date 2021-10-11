
// AutoMosaicToolDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "AutoMosaicTool.h"
#include "AutoMosaicToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoMosaicToolDlg 대화 상자



CAutoMosaicToolDlg::CAutoMosaicToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_AUTOMOSAICTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAutoMosaicToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CAMERA_VIEW, m_group_camera_view);
	DDX_Control(pDX, IDC_STATIC_ANALYSIS_VIEW, m_group_analysis_view);

	DDX_Control(pDX, IDC_CHECK_ANOTHER_MOSAIC, m_check_another_mosaic);
	DDX_Control(pDX, IDC_CHECK_OWN_MOSAIC, m_check_own_mosaic);
	DDX_Control(pDX, IDOK, m_button_close);
	DDX_Control(pDX, IDC_COMBO_MASK, m_combo_mask);
}

BEGIN_MESSAGE_MAP(CAutoMosaicToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHECK_ANOTHER_MOSAIC, &CAutoMosaicToolDlg::OnBnClickedCheckAnotherMosaic)
	ON_BN_CLICKED(IDC_CHECK_OWN_MOSAIC, &CAutoMosaicToolDlg::OnBnClickedCheckOwnMosaic)
	ON_BN_CLICKED(IDOK, &CAutoMosaicToolDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_MASK, &CAutoMosaicToolDlg::OnCbnSelchangeComboMask)
END_MESSAGE_MAP()


// CAutoMosaicToolDlg 메시지 처리기

BOOL CAutoMosaicToolDlg::OnInitDialog()
{
	// 프로그램 맨처음 시작하면 여기 호출됨
	CDialogEx::OnInitDialog();

	// 대화상자 사이즈
	this->MoveWindow(0, 0, theApp.m_DialogSize.cx, theApp.m_DialogSize.cy);

	::GetClientRect(this->m_hWnd, theApp.m_rectClient);

	//그래픽카드 초기화
	theApp.InitD3D();

	// 이미지 가져오기 
	theApp.ReadResource();

	// 컨트롤러 배치
	SetPositionControls();


	if (!CheckWebCamStatus())
	{
		MessageBox(L"카메라 연결 실패\n프로그램을 종료합니다.!");
		exit(0);
	}

	// 카메라 그려주는 클래스 생성
	theApp.m_pViewColorCamera = new CViewColorCamera();
	theApp.m_pViewColorCamera->Create(NULL, L"", WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, theApp.m_rectCameraView, this, 1000);

	// 비전정보 그려주는 클래스 생성
	theApp.m_pViewAnalysis = new CViewAnalysis();
	theApp.m_pViewAnalysis->Create(NULL, L"", WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, theApp.m_rectAnalysisView, this, 1001);


	// thread 영상, 비전 다~
	m_ThreadMainView = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(MainViewThread), this, 0, NULL);


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CAutoMosaicToolDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CAutoMosaicToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoMosaicToolDlg::SetPositionControls()
{
	int nMargin = theApp.m_rectClient.Width() * 0.01;
	// 카메라 그룹박스
	CRect rectGroupBox0;
	rectGroupBox0.left = nMargin;
	rectGroupBox0.right = rectGroupBox0.left + theApp.m_nTextureWidth - nMargin;
	rectGroupBox0.top = nMargin;
	rectGroupBox0.bottom = rectGroupBox0.top + theApp.m_nTextureHeight + nMargin;
	m_group_camera_view.MoveWindow(rectGroupBox0);

	// Camera View
	CRect rectCameraView;
	rectCameraView.left = rectGroupBox0.left + nMargin;
	rectCameraView.right = rectGroupBox0.right - nMargin;
	rectCameraView.top = rectGroupBox0.top + nMargin + nMargin;
	rectCameraView.bottom = rectGroupBox0.bottom - nMargin;

	theApp.m_rectCameraView = rectCameraView;

	// Analysis 그룹박스
	CRect rectGroupBox1;
	rectGroupBox1.left = rectGroupBox0.right + nMargin;
	rectGroupBox1.right = rectGroupBox1.left + 350;
	rectGroupBox1.top = rectGroupBox0.top;
	rectGroupBox1.bottom = rectGroupBox0.bottom - 130;
	m_group_analysis_view.MoveWindow(rectGroupBox1);

	// Analysis View
	CRect rectAnalysisView;
	rectAnalysisView.left = rectGroupBox1.left + nMargin;
	rectAnalysisView.right = rectGroupBox1.right - nMargin;
	rectAnalysisView.top = rectGroupBox1.top + nMargin + nMargin;
	rectAnalysisView.bottom = rectGroupBox1.bottom - nMargin;

	theApp.m_rectAnalysisView = rectAnalysisView;

	// 버튼들 위치 
	CRect rectCheckBox0;
	{
		rectCheckBox0.left = rectGroupBox1.left;
		rectCheckBox0.right = rectCheckBox0.left + 150;
		rectCheckBox0.top = rectGroupBox1.bottom + nMargin;
		rectCheckBox0.bottom = rectCheckBox0.top + 20;

		m_check_another_mosaic.SetCheck(theApp.m_bShowAnotherMosaic);
		m_check_another_mosaic.MoveWindow(rectCheckBox0);
	}

	CRect rectCheckBox1;
	{
		rectCheckBox1.left = rectGroupBox1.left;
		rectCheckBox1.right = rectCheckBox1.left + 150;
		rectCheckBox1.top = rectCheckBox0.bottom + nMargin * 0.5;
		rectCheckBox1.bottom = rectCheckBox1.top + 20;

		m_check_own_mosaic.SetCheck(theApp.m_bShowMainMask);
		m_check_own_mosaic.MoveWindow(rectCheckBox1);
	}

	CRect rectComboBox0;
	{
		rectComboBox0.left = rectGroupBox1.left;
		rectComboBox0.right = rectComboBox0.left + 110;
		rectComboBox0.top = rectCheckBox1.bottom + nMargin * 0.5;
		rectComboBox0.bottom = rectComboBox0.top + 20;

		m_combo_mask.ResetContent();
		m_combo_mask.AddString(L"없음");
		m_combo_mask.AddString(L"트위치");
		m_combo_mask.AddString(L"유튜브");
		m_combo_mask.AddString(L"아프리카");
		m_combo_mask.SetCurSel(0);

		m_combo_mask.MoveWindow(rectComboBox0);
	}


	//// 버튼들 위치 
	//{
	//	CRect rectButton;
	//	rectButton.left = rectGroupBox1.left;
	//	rectButton.right = rectButton.left + rectGroupBox1.Width() * 0.48;
	//	rectButton.top = rectGroupBox0.bottom - 40;
	//	rectButton.bottom = rectGroupBox0.bottom;

	//	m_button_start.MoveWindow(rectButton);
	//}

	{
		CRect rectButton;
		rectButton.left = rectGroupBox1.left + rectGroupBox1.Width() * 0.5;
		rectButton.right = rectGroupBox1.right;
		rectButton.top = rectGroupBox0.bottom - 40;
		rectButton.bottom = rectGroupBox0.bottom;

		m_button_close.MoveWindow(rectButton);
	}
}

BOOL CAutoMosaicToolDlg::CheckWebCamStatus()
{
	int nIndex = theApp.m_nWebCamIndex;

	cv::VideoCapture cap(nIndex);

	if (cap.isOpened())
	{
		theApp.m_cvCurrentVideo = cap;

		theApp.m_bDataLoad = TRUE;
	}
	else
		theApp.m_bDataLoad = FALSE;

	BOOL bRet = theApp.m_bDataLoad;

	return bRet;
}

DWORD WINAPI CAutoMosaicToolDlg::MainViewThread(CAutoMosaicToolDlg* This)
{
	while (NULL != This->m_hWnd)
	{
		if (!theApp.m_bDataLoad)
			continue;

		if (!theApp.ProcessColorTexture())
			continue;

		//printf("1");

		theApp.m_pViewColorCamera->Render();

		theApp.m_pViewAnalysis->Render();


		Sleep(1);
	}

	return 0;

}


void CAutoMosaicToolDlg::OnBnClickedCheckAnotherMosaic()
{
	theApp.m_bShowAnotherMosaic = !theApp.m_bShowAnotherMosaic;
}


void CAutoMosaicToolDlg::OnBnClickedCheckOwnMosaic()
{
	theApp.m_bShowMainMosaic = !theApp.m_bShowMainMosaic;
}


void CAutoMosaicToolDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();

	//exit(0);
}


void CAutoMosaicToolDlg::OnCbnSelchangeComboMask()
{
	theApp.m_nMainMaskIndex = m_combo_mask.GetCurSel();

	if (0 == theApp.m_nMainMaskIndex)
		theApp.m_bShowMainMask = FALSE;
	else
		theApp.m_bShowMainMask = TRUE;

}
