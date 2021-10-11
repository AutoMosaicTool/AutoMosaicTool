
// AutoMosaicTool.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
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


// CAutoMosaicToolApp ����

CAutoMosaicToolApp::CAutoMosaicToolApp()
{
	int nWidth = GetSystemMetrics(SM_CXSCREEN);
	int nHeight = GetSystemMetrics(SM_CYSCREEN);

	m_nTextureWidth = 640;
	m_nTextureHeight = 480;

	m_DialogSize.SetSize(m_nTextureWidth + 385, m_nTextureHeight + 65);

	// opencv face, eye info �ҷ�����
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


// ������ CAutoMosaicToolApp ��ü�Դϴ�.

CAutoMosaicToolApp theApp;


// CAutoMosaicToolApp �ʱ�ȭ

BOOL CAutoMosaicToolApp::InitInstance()
{
	CWinApp::InitInstance();


	// ��ȭ ���ڿ� �� Ʈ�� �� �Ǵ�
	// �� ��� �� ��Ʈ���� ���ԵǾ� �ִ� ��� �� �����ڸ� ����ϴ�.
	CShellManager *pShellManager = new CShellManager;

	// MFC ��Ʈ���� �׸��� ����ϱ� ���� "Windows ����" ���־� ������ Ȱ��ȭ
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	CAutoMosaicToolDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "���: ��ȭ ���ڸ� ������ �������Ƿ� ���� ���α׷��� ����ġ �ʰ� ����˴ϴ�.\n");
		TRACE(traceAppMsg, 0, "���: ��ȭ ���ڿ��� MFC ��Ʈ���� ����ϴ� ��� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS�� ������ �� �����ϴ�.\n");
	}

	// ������ ���� �� �����ڸ� �����մϴ�.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
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

	// �� ���� ������
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
	// ��� ����    

	cvtColor(capturedImage, gray, COLOR_RGB2GRAY);
	// �̹����� ��ķ� ǥ���ؼ� RGB�� ���ٴ�!! 
	std::vector<cv::Rect> originFacePos;
	std::vector<cv::Rect> tuningFacePos;
	// ����ġ����    

	//theApp.m_faceInfo.detectMultiScale(gray, originFacePos, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(10, 10));
	theApp.m_faceInfo.detectMultiScale(gray, originFacePos);
	//�󱼰���     
	//�󱼿���ǥ�� 

	// �󱼿����� ������ �ȵǸ� ���̰����� �ȵǵ� �װ� ���� ������ �ƴϴ�.
	// ���� ����� ���¿��� ���� ����ȵǾ� �����ŷ� �Ǵ�

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
		// ������ ���� (m_nNoiseFaceWidth ���� ���� ������ ��ŵ�ض�)
		// originFacePos ���� m_nNoiseFaceWidth ���� ������ ������
		// tuningFacePos �� �־��
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

		// �˻��� ������� ���� ����� (�󱼿����� ���� ū ����� main �̵ȴ�.
		int mainIndex = 0;
		{
			int nMaxWidth = 0;
			for (int m = 0; m < tuningFacePos.size(); m++)
			{
				int width = tuningFacePos[m].width;

				if (width > nMaxWidth)
				{// ���� �󱼿����� ū idx �� ���´�.
					nMaxWidth = width;
					mainIndex = m;
				}
			}

			// ���� �󱼿����� ū idx �� ������ �������� �����Ѵ�.
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


		{// main �� �����ϰ� ������ �˻��� ���� main �� �����ϰ� vector�� ��´�.

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
