
// AutoMosaicToolDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "AutoMosaicTool.h"
#include "AutoMosaicToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoMosaicToolDlg ��ȭ ����



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


// CAutoMosaicToolDlg �޽��� ó����

BOOL CAutoMosaicToolDlg::OnInitDialog()
{
	// ���α׷� ��ó�� �����ϸ� ���� ȣ���
	CDialogEx::OnInitDialog();

	// ��ȭ���� ������
	this->MoveWindow(0, 0, theApp.m_DialogSize.cx, theApp.m_DialogSize.cy);

	::GetClientRect(this->m_hWnd, theApp.m_rectClient);

	//�׷���ī�� �ʱ�ȭ
	theApp.InitD3D();

	// �̹��� �������� 
	theApp.ReadResource();

	// ��Ʈ�ѷ� ��ġ
	SetPositionControls();


	if (!CheckWebCamStatus())
	{
		MessageBox(L"ī�޶� ���� ����\n���α׷��� �����մϴ�.!");
		exit(0);
	}

	// ī�޶� �׷��ִ� Ŭ���� ����
	theApp.m_pViewColorCamera = new CViewColorCamera();
	theApp.m_pViewColorCamera->Create(NULL, L"", WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, theApp.m_rectCameraView, this, 1000);

	// �������� �׷��ִ� Ŭ���� ����
	theApp.m_pViewAnalysis = new CViewAnalysis();
	theApp.m_pViewAnalysis->Create(NULL, L"", WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, theApp.m_rectAnalysisView, this, 1001);


	// thread ����, ���� ��~
	m_ThreadMainView = CreateThread(NULL, 0, LPTHREAD_START_ROUTINE(MainViewThread), this, 0, NULL);


	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CAutoMosaicToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CAutoMosaicToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAutoMosaicToolDlg::SetPositionControls()
{
	int nMargin = theApp.m_rectClient.Width() * 0.01;
	// ī�޶� �׷�ڽ�
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

	// Analysis �׷�ڽ�
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

	// ��ư�� ��ġ 
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
		m_combo_mask.AddString(L"����");
		m_combo_mask.AddString(L"Ʈ��ġ");
		m_combo_mask.AddString(L"��Ʃ��");
		m_combo_mask.AddString(L"������ī");
		m_combo_mask.SetCurSel(0);

		m_combo_mask.MoveWindow(rectComboBox0);
	}


	//// ��ư�� ��ġ 
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
