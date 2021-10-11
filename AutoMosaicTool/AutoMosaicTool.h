
// AutoMosaicTool.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.

#include "ViewColorCamera.h"
#include "ViewAnalysis.h"

// CAutoMosaicToolApp:
// 이 클래스의 구현에 대해서는 AutoMosaicTool.cpp을 참조하십시오.
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

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()


public:
	CSize					m_DialogSize; // 대화상자 사각형
	CRect					m_rectClient; // 대화상자 사각형
	CRect					m_rectCameraView; // 카메라 영상 사각형
	CRect					m_rectAnalysisView; // 비전정보 사각형

	BOOL					m_bDataLoad;
	BOOL					m_bDetectedFace; // 얼굴 1개이상을 찾으면 true 아님 false
	BOOL					m_bShowMainMask; // 아이유 얼굴볼래?
	int						m_nMainMaskIndex;
	BOOL					m_bShowMainMosaic; // 메인 모자이크 볼래?
	BOOL					m_bShowAnotherMosaic; //타인 모자이크 볼래?

	int						m_nTextureWidth; // 640
	int						m_nTextureHeight; // 480

	int						m_nWebCamIndex; // 0 (카메라 index)
	int						m_nNoiseFaceWidth;

public:
	IDirect3D9*             D3D; // rendering
	IDirect3DDevice9*       Device; // rendering
	D3DPRESENT_PARAMETERS   D3DPP; // rendering

	IDirect3DTexture9*		m_pWebCamTexture; // opencv -> mfc -> 화면에 그려주는 이미지영상 변수 (directx)

	cv::VideoCapture        m_cvCurrentVideo; //opencv video value

	cv::CascadeClassifier	m_faceInfo; // opencv face, eyes
	cv::CascadeClassifier	m_eyeInfo;

	CViewColorCamera*		m_pViewColorCamera; // 카메라 영상 클래스 (그려주는 클래스)
	CViewAnalysis*			m_pViewAnalysis; //  비전정보 출력 클래스 (그려주는 클래스)

	sTexture				m_texMainMask0; // 트위치 이미지 구조체
	sTexture				m_texMainMask1; // 유투브 이미지 구조체
	sTexture				m_texMainMask2; // 아프리카 이미지 구조체


	sRenderBox				m_mainFaceRange; // 메인 얼굴영역 (점 4개, 사각형)
	sMosaicRange			m_mainFaceMosaicRange;

	std::vector<sRenderBox>	m_vecAnotherFaces; // 타인들 얼굴영역 
	std::vector<sMosaicRange> m_vecAnotherFaceMosaicRange;
	int						m_nVisionShiftGap; // 비젼 shift 이건 노트북마다 최적화필요

public:
	HRESULT					InitD3D(); // 랜더링 초기화 GPU
	BOOL					ProcessColorTexture(); // 영상, 비전처리
	void					CalcFaceInfo(cv::Mat capturedImage); // 비전처리
	void					ReadResource(); // 아이유랑 모자이크 이미지 메모리화

};

extern CAutoMosaicToolApp theApp;