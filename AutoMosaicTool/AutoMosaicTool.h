
// AutoMosaicTool.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.

#include "ViewColorCamera.h"
#include "ViewAnalysis.h"

// CAutoMosaicToolApp:
// �� Ŭ������ ������ ���ؼ��� AutoMosaicTool.cpp�� �����Ͻʽÿ�.
//
enum {nMosaicDetail = 15 };
struct sTexture
{
	IDirect3DTexture9*  tex;
	float width;
	float height;
	D3DXVECTOR3 center;

	sTexture()
	{
		tex = NULL;
		width = 0.f;
		height = 0.f;
	}

	void reset()
	{
		tex = NULL;
		width = 0.f;
		height = 0.f;
	}
};

struct sRenderBox
{
	D3DXVECTOR2 box[5];
};

struct sMosaicRange
{
	D3DXVECTOR3 color[nMosaicDetail][nMosaicDetail];
	D3DXVECTOR3 center[nMosaicDetail][nMosaicDetail];
	float width;
};

class CAutoMosaicToolApp : public CWinApp
{
public:
	CAutoMosaicToolApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()


public:
	CSize					m_DialogSize; // ��ȭ���� �簢��
	CRect					m_rectClient; // ��ȭ���� �簢��
	CRect					m_rectCameraView; // ī�޶� ���� �簢��
	CRect					m_rectAnalysisView; // �������� �簢��

	BOOL					m_bDataLoad;
	BOOL					m_bDetectedFace; // �� 1���̻��� ã���� true �ƴ� false
	BOOL					m_bShowMainMask; // ������ �󱼺���?
	int						m_nMainMaskIndex;
	BOOL					m_bShowMainMosaic; // ���� ������ũ ����?
	BOOL					m_bShowAnotherMosaic; //Ÿ�� ������ũ ����?

	int						m_nTextureWidth; // 640
	int						m_nTextureHeight; // 480

	int						m_nWebCamIndex; // 0 (ī�޶� index)
	int						m_nNoiseFaceWidth;

public:
	IDirect3D9*             D3D; // rendering
	IDirect3DDevice9*       Device; // rendering
	D3DPRESENT_PARAMETERS   D3DPP; // rendering

	IDirect3DTexture9*		m_pWebCamTexture; // opencv -> mfc -> ȭ�鿡 �׷��ִ� �̹������� ���� (directx)

	cv::VideoCapture        m_cvCurrentVideo; //opencv video value

	cv::CascadeClassifier	m_faceInfo; // opencv face, eyes
	cv::CascadeClassifier	m_eyeInfo;

	CViewColorCamera*		m_pViewColorCamera; // ī�޶� ���� Ŭ���� (�׷��ִ� Ŭ����)
	CViewAnalysis*			m_pViewAnalysis; //  �������� ��� Ŭ���� (�׷��ִ� Ŭ����)

	sTexture				m_texMainMask0; // Ʈ��ġ �̹��� ����ü
	sTexture				m_texMainMask1; // ������ �̹��� ����ü
	sTexture				m_texMainMask2; // ������ī �̹��� ����ü


	sRenderBox				m_mainFaceRange; // ���� �󱼿��� (�� 4��, �簢��)
	sMosaicRange			m_mainFaceMosaicRange;

	std::vector<sRenderBox>	m_vecAnotherFaces; // Ÿ�ε� �󱼿��� 
	std::vector<sMosaicRange> m_vecAnotherFaceMosaicRange;
	int						m_nVisionShiftGap; // ���� shift �̰� ��Ʈ�ϸ��� ����ȭ�ʿ�

public:
	HRESULT					InitD3D(); // ������ �ʱ�ȭ GPU
	BOOL					ProcessColorTexture(); // ����, ����ó��
	void					CalcFaceInfo(cv::Mat capturedImage); // ����ó��
	void					ReadResource(); // �������� ������ũ �̹��� �޸�ȭ

};

extern CAutoMosaicToolApp theApp;