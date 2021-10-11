
// AutoMosaicTool.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "AutoMosaicTool.h"
#include "AutoMosaicToolDlg.h"

#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAutoMosaicToolApp

BEGIN_MESSAGE_MAP(CAutoMosaicToolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAutoMosaicToolApp 생성

CAutoMosaicToolApp::CAutoMosaicToolApp()
{
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);

	m_nTextureWidth = 640;
	m_nTextureHeight = 480;

	m_DialogSize.SetSize(m_nTextureWidth + 385, m_nTextureHeight + 65);

	// opencv face, eye info 불러오기
	if (!m_faceInfo.load("haarcascade_frontalface_alt.xml"))
		printf("--(!)Error loading face cascade\n");

	if (!m_eyeInfo.load("haarcascade_eye_tree_eyeglasses.xml"))
		printf("--(!)Error loading eyes cascade\n");

	m_nWebCamIndex = 0;
	m_nMainMaskIndex = 0;

	m_bDataLoad = FALSE;
	m_bDetectedFace = FALSE;

	m_nVisionShiftGap = 0;

	m_bShowMainMask = FALSE;
	m_bShowAnotherMosaic = TRUE;
	m_bShowMainMosaic = FALSE;

	m_nNoiseFaceWidth = 10;
}


// 유일한 CAutoMosaicToolApp 개체입니다.

CAutoMosaicToolApp theApp;


// CAutoMosaicToolApp 초기화

BOOL CAutoMosaicToolApp::InitInstance()
{
	CWinApp::InitInstance();


	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	CAutoMosaicToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 응용 프로그램이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}


void CAutoMosaicToolApp::ReadResource()
{
	HRESULT h;
	CString strFilePath;

	wchar_t chThisPath[256];
	GetCurrentDirectoryW(256, chThisPath);

	m_texMainMask0.width = 440;
	m_texMainMask0.height = 440;
	strFilePath = chThisPath;
	strFilePath += L"\\Res\\image1.png";

	h = D3DXCreateTextureFromFileEx(theApp.Device, strFilePath,
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0,
		D3DFMT_FROM_FILE, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &m_texMainMask0.tex);

	if (h == E_FAIL)
	{
		AfxMessageBox(L"failed image load");
	}

	m_texMainMask1.width = 200;
	m_texMainMask1.height = 200;
	strFilePath = chThisPath;
	strFilePath += L"\\Res\\image2.png";

	h = D3DXCreateTextureFromFileEx(theApp.Device, strFilePath,
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0,
		D3DFMT_FROM_FILE, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &m_texMainMask1.tex);

	if (h == E_FAIL)
	{
		AfxMessageBox(L"failed image load");
	}

	m_texMainMask2.width = 200;
	m_texMainMask2.height = 200;
	strFilePath = chThisPath;
	strFilePath += L"\\Res\\image3.png";

	h = D3DXCreateTextureFromFileEx(theApp.Device, strFilePath,
		D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, D3DX_FROM_FILE, 0,
		D3DFMT_FROM_FILE, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &m_texMainMask2.tex);

	if (h == E_FAIL)
	{
		AfxMessageBox(L"failed image load");
	}
}

HRESULT CAutoMosaicToolApp::InitD3D()
{
	this->D3D = Direct3DCreate9(D3D_SDK_VERSION);

	RELEASE(this->Device);
	HRESULT h;

	this->D3DPP.BackBufferCount = 1;
	this->D3DPP.BackBufferWidth = 1920;
	this->D3DPP.BackBufferHeight = 1080;
	this->D3DPP.BackBufferFormat = D3DFMT_X8R8G8B8;
	this->D3DPP.MultiSampleType = D3DMULTISAMPLE_NONE;
	//this->D3DPP.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
	this->D3DPP.MultiSampleQuality = 0;
	this->D3DPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
	this->D3DPP.hDeviceWindow = 0;
	this->D3DPP.Windowed = 1;
	this->D3DPP.EnableAutoDepthStencil = 1;
	this->D3DPP.AutoDepthStencilFormat = D3DFMT_D16;
	//    this->D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	this->D3DPP.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	this->D3DPP.Flags = 0;//D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	h = this->D3D->CreateDevice(0,
		D3DDEVTYPE_HAL,
		0,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
		&(this->D3DPP),
		&(this->Device));

	h = theApp.Device->CreateTexture(m_nTextureWidth, m_nTextureHeight, 1, 0,
		D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pWebCamTexture, NULL);

	return h;
}

BOOL CAutoMosaicToolApp::ProcessColorTexture()
{
	HRESULT hr;

	cv::Mat capturedImage;
	m_cvCurrentVideo.read(capturedImage);

	if (capturedImage.empty())
		return FALSE;

	// 얼굴 양쪽 눈검출
	CalcFaceInfo(capturedImage);



	int nWidth = capturedImage.cols;
	int nHeight = capturedImage.rows;

	D3DLOCKED_RECT lock_rect;
	hr = m_pWebCamTexture->LockRect(0, &lock_rect, 0, D3DLOCK_DISCARD);
	if (FAILED(hr))
		return FALSE;

	int nChannel = capturedImage.channels();
	int idx = 0;
	for (int y = 0; y < m_nTextureHeight; y++)
	{
		for (int x = 0; x < m_nTextureWidth; x++)
		{
			BYTE* dst = ((BYTE*)(lock_rect.pBits)) + (lock_rect.Pitch * int(y) + sizeof(DWORD) * int(x));
			dst[0] = capturedImage.data[idx + 0];
			dst[1] = capturedImage.data[idx + 1];
			dst[2] = capturedImage.data[idx + 2];

			idx += 3;
		}
	}
	hr = m_pWebCamTexture->UnlockRect(0);


	if (FAILED(hr))
		return FALSE;


	return TRUE;
}

void CAutoMosaicToolApp::CalcFaceInfo(cv::Mat capturedImage)
{
	using namespace cv;
	Mat gray;
	// 행렬 생성    

	cvtColor(capturedImage, gray, COLOR_RGB2GRAY);
	// 이미지를 행렬로 표현해서 RGB로 쓰겟다!! 
	std::vector<cv::Rect> originFacePos;
	std::vector<cv::Rect> tuningFacePos;
	// 얼굴위치저장    

	//theApp.m_faceInfo.detectMultiScale(gray, originFacePos, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
	theApp.m_faceInfo.detectMultiScale(gray, originFacePos);
	//얼굴검출     
	//얼굴영역표시 

	// 얼굴영역이 검출이 안되면 눈이검출이 안되도 그건 눈을 감은게 아니다.
	// 얼굴이 검출된 상태에서 눈이 검출안되야 감은거로 판단

	if (0 == originFacePos.size())
	{
		m_bDetectedFace = FALSE;

		m_mainFaceRange.box[0].x = 0.f;
		m_mainFaceRange.box[0].y = 0.f;
		m_mainFaceRange.box[1].x = 0.f;
		m_mainFaceRange.box[1].y = 0.f;
		m_mainFaceRange.box[2].x = 0.f;
		m_mainFaceRange.box[2].y = 0.f;
		m_mainFaceRange.box[3].x = 0.f;
		m_mainFaceRange.box[3].y = 0.f;
		m_mainFaceRange.box[4].x = 0.f;
		m_mainFaceRange.box[4].y = 0.f;

		m_vecAnotherFaces.clear();

		return;
	}
	else
	{
		// 노이즈 제거 (m_nNoiseFaceWidth 보다 작은 영역은 스킵해라)
		// originFacePos 에서 m_nNoiseFaceWidth 보다 영역은 버리고
		// tuningFacePos 다 넣어라
		for (auto item : originFacePos)
		{
			//if (m_nNoiseFaceWidth > item.width)
			//	continue;

			tuningFacePos.push_back(item);
		}

		if (0 < tuningFacePos.size())
			m_bDetectedFace = TRUE;
		else
			m_bDetectedFace = FALSE;


		//printf("human count : %d\n", tuningFacePos.size());

		uchar* pRaw = capturedImage.data;
		int nDataLength = capturedImage.size().width * capturedImage.size().height * 3;

		// 검색된 사람에서 가장 가까운 (얼굴영역이 제일 큰 사람이 main 이된다.
		int mainIndex = 0;
		{
			int nMaxWidth = 0;
			for (int m = 0; m < tuningFacePos.size(); m++)
			{
				int width = tuningFacePos[m].width;

				if (width > nMaxWidth)
				{// 가장 얼굴영역이 큰 idx 가 남는다.
					nMaxWidth = width;
					mainIndex = m;
				}
			}

			// 가장 얼굴영역이 큰 idx 의 영역을 메인으로 구성한다.
			D3DXVECTOR2 pos;
			pos.x = float(m_rectCameraView.Width()) - float(tuningFacePos[mainIndex].x - m_nVisionShiftGap);
			pos.y = float(tuningFacePos[mainIndex].y);

			m_mainFaceRange.box[0].x = pos.x;
			m_mainFaceRange.box[0].y = pos.y;

			m_mainFaceRange.box[1].x = pos.x - float(tuningFacePos[mainIndex].width);
			m_mainFaceRange.box[1].y = pos.y;

			m_mainFaceRange.box[2].x = pos.x - float(tuningFacePos[mainIndex].width);
			m_mainFaceRange.box[2].y = pos.y + float(tuningFacePos[mainIndex].height);

			m_mainFaceRange.box[3].x = pos.x;
			m_mainFaceRange.box[3].y = pos.y + float(tuningFacePos[mainIndex].height);

			m_mainFaceRange.box[4].x = pos.x;
			m_mainFaceRange.box[4].y = pos.y;

			int width = tuningFacePos[mainIndex].width;
			int height = tuningFacePos[mainIndex].height;

			float fBoxWidth = (float(width) / float(nMosaicDetail));
			float fHalfBoxWidth = fBoxWidth * 0.5f;

			m_mainFaceMosaicRange.width = fBoxWidth;

			for (int y = 0; y < nMosaicDetail; y++)
			{
				float sy = float(y) / float(nMosaicDetail - 1);

				for (int x = 0; x < nMosaicDetail; x++)
				{
					float sx = float(x) / float(nMosaicDetail - 1);

					D3DXVECTOR3 mPos;
					mPos.x = pos.x - (x * fBoxWidth) + fHalfBoxWidth;
					mPos.y = pos.y + (y * fBoxWidth) + fHalfBoxWidth;
					mPos.z = 0.f;

					m_mainFaceMosaicRange.center[y][x] = mPos;

					int idx = int(mPos.y) * m_nTextureWidth * 3 + int(m_nTextureWidth - mPos.x - fBoxWidth) * 3;

					if (idx < 0)
						idx = 0;

					if (idx > nDataLength - 3)
						idx = nDataLength - 3;

					int red = pRaw[idx + 0];
					int green = pRaw[idx + 1];
					int blue = pRaw[idx + 2];

					m_mainFaceMosaicRange.color[y][x].x = float(blue);
					m_mainFaceMosaicRange.color[y][x].y = float(green);
					m_mainFaceMosaicRange.color[y][x].z = float(red);
				}
			}
		}


		{// main 을 제외하고 나머지 검색된 얼굴을 main 은 제외하고 vector에 담는다.

			m_vecAnotherFaces.clear();
			m_vecAnotherFaceMosaicRange.clear();
			for (int m = 0; m < tuningFacePos.size(); m++)
			{
				if (m == mainIndex)
					continue;

				std::vector<cv::Rect> eye_pos;
				Mat roi = gray(tuningFacePos[m]);
				//theApp.m_eyeInfo.detectMultiScale(roi, eye_pos, 1.1, 5, 0 | CASCADE_SCALE_IMAGE, Size(49, 49));
				theApp.m_eyeInfo.detectMultiScale(roi, eye_pos);

				//if (0 == eye_pos.size())
				//	continue;

				D3DXVECTOR2 pos;
				pos.x = float(m_rectCameraView.Width()) - float(tuningFacePos[m].x - m_nVisionShiftGap);
				pos.y = float(tuningFacePos[m].y);

				sRenderBox renderBox;

				renderBox.box[0].x = pos.x;
				renderBox.box[0].y = pos.y;

				renderBox.box[1].x = pos.x - float(tuningFacePos[m].width);
				renderBox.box[1].y = pos.y;

				renderBox.box[2].x = pos.x - float(tuningFacePos[m].width);
				renderBox.box[2].y = pos.y + float(tuningFacePos[m].height);

				renderBox.box[3].x = pos.x;
				renderBox.box[3].y = pos.y + float(tuningFacePos[m].height);

				renderBox.box[4].x = pos.x;
				renderBox.box[4].y = pos.y;

				m_vecAnotherFaces.push_back(renderBox);


				int width = tuningFacePos[m].width;
				int height = tuningFacePos[m].height;

				float fBoxWidth = (float(width) / float(nMosaicDetail));
				float fHalfBoxWidth = fBoxWidth * 0.5f;

				sMosaicRange mosaicRange;

				mosaicRange.width = fBoxWidth;

				for (int y = 0; y < nMosaicDetail; y++)
				{
					float sy = float(y) / float(nMosaicDetail - 1);

					for (int x = 0; x < nMosaicDetail; x++)
					{
						float sx = float(x) / float(nMosaicDetail - 1);

						D3DXVECTOR3 mPos;
						mPos.x = pos.x - (x * fBoxWidth) + fHalfBoxWidth;
						mPos.y = pos.y + (y * fBoxWidth) + fHalfBoxWidth;
						mPos.z = 0.f;

						mosaicRange.center[y][x] = mPos;

						int idx = int(mPos.y) * theApp.m_nTextureWidth * 3 + int(m_nTextureWidth - mPos.x - fBoxWidth) * 3;

						if (idx < 0)
							idx = 0;

						if (idx > nDataLength - 3)
							idx = nDataLength - 3;

						int red = pRaw[idx + 0];
						int green = pRaw[idx + 1];
						int blue = pRaw[idx + 2];

						mosaicRange.color[y][x].x = float(blue);
						mosaicRange.color[y][x].y = float(green);
						mosaicRange.color[y][x].z = float(red);

					}

				}

				m_vecAnotherFaceMosaicRange.push_back(mosaicRange);
			}
		}
	}
}
