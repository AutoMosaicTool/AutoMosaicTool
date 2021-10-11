#include "stdafx.h"
#include "AutoMosaicTool.h"
#include "ViewAnalysis.h"

IMPLEMENT_DYNAMIC(CViewAnalysis, CWnd)

CViewAnalysis::CViewAnalysis()
{
    SwapChain = NULL;
    m_pEffect = NULL;
    m_pDXLine = NULL;

}


CViewAnalysis::~CViewAnalysis()
{
}


BEGIN_MESSAGE_MAP(CViewAnalysis, CWnd)
    ON_WM_CREATE()
END_MESSAGE_MAP()


int CViewAnalysis::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    InitD3D();

    return 0;
}

void CViewAnalysis::InitD3D()
{
    CRect r;
    ::GetClientRect(this->m_hWnd, &r);

    ID3DXBuffer* err = 0;
    D3DXCreateEffectFromFile(theApp.Device,
        _T("newSaga.fx"),
        0,
        0,
        0,
        0,
        &m_pEffect,
        &err);

    D3DPRESENT_PARAMETERS pp = theApp.D3DPP;
    pp.BackBufferWidth = r.right;
    pp.BackBufferHeight = r.bottom;
    pp.hDeviceWindow = this->m_hWnd;
    theApp.Device->CreateAdditionalSwapChain(&pp, &(this->SwapChain));

	D3DXCreateLine(theApp.Device, &m_pDXLine);
	m_pDXLine->SetWidth(1);
	m_pDXLine->SetPattern(0xFFFFFFFF);
	m_pDXLine->SetAntialias(TRUE);


	CFont font1;
	font1.CreatePointFont(180, _T("맑은 고딕"));

	LOGFONT lf1;
	font1.GetLogFont(&lf1);
	lf1.lfWeight = FW_THIN;
	D3DXCreateFont(theApp.Device,
		lf1.lfHeight, lf1.lfWidth, lf1.lfWeight,
		0,
		lf1.lfItalic,
		lf1.lfCharSet,
		lf1.lfOutPrecision,
		lf1.lfQuality,
		lf1.lfPitchAndFamily,
		lf1.lfFaceName,
		&m_pDXFont0);

	CFont font2;
	font2.CreatePointFont(150, _T("맑은 고딕"));

	LOGFONT lf2;
	font2.GetLogFont(&lf2);
	lf2.lfWeight = FW_THIN;
	D3DXCreateFont(theApp.Device,
		lf2.lfHeight, lf2.lfWidth, lf2.lfWeight,
		0,
		lf2.lfItalic,
		lf2.lfCharSet,
		lf2.lfOutPrecision,
		lf2.lfQuality,
		lf2.lfPitchAndFamily,
		lf2.lfFaceName,
		&m_pDXFont1);
}


void CViewAnalysis::DrawAnotherInfoTitle()
{
	m_rectAnotherInfoTitle.left = m_rectClient.left;
	m_rectAnotherInfoTitle.right = m_rectClient.right;
	m_rectAnotherInfoTitle.top = m_rectMainInfo.bottom;
	m_rectAnotherInfoTitle.bottom = m_rectAnotherInfoTitle.top + 50;

	CString strText;
	strText.Format(L"  [타인 정보]");

	DrawText(m_pDXFont0, strText, m_rectAnotherInfoTitle, DT_LEFT | DT_VCENTER, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CViewAnalysis::DrawAnalysisInfo()
{
	//if (E_ANALYSIS != theApp.m_eCurrentMode)
	//	return;

	//m_rectAnalysisInfo.left = m_rectClient.left;
	//m_rectAnalysisInfo.right = m_rectClient.right;
	//m_rectAnalysisInfo.top = m_rectMainInfoTitle.bottom + 10;
	//m_rectAnalysisInfo.bottom = m_rectAnalysisInfo.top + 110;

	//int nDetectedFaceCount = theApp.m_nDetectedFaceCount;
	//int nDetectedEyesCount = theApp.m_nDetectedEyesCount;

	//if (0 == nDetectedFaceCount)
	//	nDetectedFaceCount = 1;

	//float fScale = float(nDetectedEyesCount) / float(nDetectedFaceCount); // 눈떠있는 비중 0 ~ 1
	//float fRevScale = 1.f - fScale; // 눈감은 비중 0 ~ 1

	//float fTotalTime = float(theApp.m_nAnalysisLimitTime) / 1000.f;
	//float fBlinkedTime = fRevScale * float(theApp.m_nAnalysisLimitTime) / 1000.f;

	//CString strText;
	//strText.Format(L" 분석 시간 총 : %.1f 초\n 눈 감은 시간 총 : %.1f 초", fTotalTime, fBlinkedTime);

	//DrawText(m_pDXFont0, strText, m_rectAnalysisInfo, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 255, 255, 255));

	//// 결과 출력 (적합/부적합)
	//CRect rectResult0;
	//rectResult0.left = m_rectAnalysisInfo.left;
	//rectResult0.right = m_rectAnalysisInfo.right;
	//rectResult0.top = m_rectAnalysisInfo.bottom;
	//rectResult0.bottom = rectResult0.top + 30;

	//if (0.f == fTotalTime)
	//	fTotalTime = 1.f;

	//float fCurrentResultRate = fBlinkedTime / fTotalTime * 100.f; // 100% 비율로 변환

	//if (fCurrentResultRate >= theApp.m_fResultLimitRate)
	//{// 적합
	//	float fLimit = fTotalTime * theApp.m_fResultLimitRate * 0.01f;

	//	strText.Format(L" 결과 : 적합 (%.1f초 이상)", fLimit);
	//	DrawText(m_pDXFont0, strText, rectResult0, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 0, 255, 0));
	//}
	//else
	//{// 부적합
	//	float fLimit = fTotalTime * theApp.m_fResultLimitRate * 0.01f;

	//	strText.Format(L" 결과 : 부적합 (%.1f초 미만)", fLimit);

	//	DrawText(m_pDXFont0, strText, rectResult0, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 255, 0, 0));
	//}

	//// 커트라인 정보 출력
}

void CViewAnalysis::DrawMainInfoTitle()
{
	m_rectMainInfoTitle.left = m_rectClient.left;
	m_rectMainInfoTitle.right = m_rectClient.right;
	m_rectMainInfoTitle.top = m_recAnalysisTitle.bottom;
	m_rectMainInfoTitle.bottom = m_rectMainInfoTitle.top + 50;

	CString strText;
	strText.Format(L"  [본인 정보]");

	DrawText(m_pDXFont0, strText, m_rectMainInfoTitle, DT_LEFT | DT_VCENTER, D3DCOLOR_ARGB(255, 255, 255, 255));

}

void CViewAnalysis::DrawMainInfo()
{
	m_rectMainInfo.left = m_rectClient.left;
	m_rectMainInfo.right = m_rectClient.right;
	m_rectMainInfo.top = m_rectMainInfoTitle.bottom;
	m_rectMainInfo.bottom = m_rectMainInfo.top + 40;

	CString strText;
	if (!theApp.m_bDetectedFace)
	{
		strText.Format(L"  검색실패!!!!!");
		DrawText(m_pDXFont1, strText, m_rectMainInfo, DT_LEFT | DT_VCENTER, D3DCOLOR_ARGB(255, 200, 0, 0));
	}
	else
	{
		D3DXVECTOR2 center = (theApp.m_mainFaceRange.box[0] + theApp.m_mainFaceRange.box[2]) * 0.5f;
		strText.Format(L"  중심위치 : %.0f, %.0f", center.x, center.y);
		DrawText(m_pDXFont1, strText, m_rectMainInfo, DT_LEFT | DT_VCENTER, D3DCOLOR_ARGB(255, 0, 200, 0));
	}



}

void CViewAnalysis::DrawAnotherInfo()
{
	m_rectAnotherInfo.left = m_rectClient.left;
	m_rectAnotherInfo.right = m_rectClient.right;
	m_rectAnotherInfo.top = m_rectAnotherInfoTitle.bottom;
	m_rectAnotherInfo.bottom = m_rectClient.bottom;

	if (!theApp.m_bDetectedFace || (0 == theApp.m_vecAnotherFaces.size()))
	{
		CString strText;

		strText.Format(L"  검색실패!!!!!");
		DrawText(m_pDXFont1, strText, m_rectAnotherInfo, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 200, 0, 0));
	}
	else
	{
		CString strText;

		int idx = 1;
		for (auto item : theApp.m_vecAnotherFaces)
		{
			CString str0;
			D3DXVECTOR2 center = (item.box[0] + item.box[2]) * 0.5f;
			str0.Format(L"  타인 %d   중심위치 : %.0f, %.0f\n", idx, center.x, center.y);
			strText += str0;

			idx++;
		}

		DrawText(m_pDXFont1, strText, m_rectAnotherInfo, DT_LEFT | DT_TOP, D3DCOLOR_ARGB(255, 0, 200, 0));
	}
}


void CViewAnalysis::DrawAnalysisTitle()
{
	m_recAnalysisTitle.left = m_rectClient.left;
	m_recAnalysisTitle.right = m_rectClient.right;
	m_recAnalysisTitle.top = m_rectClient.top;
	m_recAnalysisTitle.bottom = m_recAnalysisTitle.top + 40;

	CString strText = L"비전 결과";


	DrawText(m_pDXFont0, strText, m_recAnalysisTitle, DT_CENTER | DT_VCENTER, D3DCOLOR_ARGB(255, 0, 200, 0));

	// 하단에 선긋기 (seperator)
	{
		D3DXVECTOR2 line[2];

		line[0].x = float(m_recAnalysisTitle.left);
		line[0].y = float(m_recAnalysisTitle.bottom);
		line[1].x = float(m_recAnalysisTitle.right);
		line[1].y = float(m_recAnalysisTitle.bottom);
		m_pDXLine->Draw(line, 2, D3DCOLOR_ARGB(255, 0, 200, 255));

	}
}


void CViewAnalysis::DrawText(ID3DXFont* pFont, LPCWSTR pString, LPRECT pRect, DWORD Format, D3DCOLOR Color)
{
	pFont->DrawText(NULL, pString, -1, pRect, Format, Color);
}

void CViewAnalysis::SetMatrix()
{
    theApp.Device->SetRenderState(D3DRS_LIGHTING, FALSE);
    theApp.Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    theApp.Device->SetRenderState(D3DRS_ZENABLE, TRUE);
    theApp.Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    theApp.Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    theApp.Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    D3DXMATRIX mat;
    D3DXMatrixIdentity(&mat);

    m_pEffect->SetMatrix("matWorld", &mat);
    m_pEffect->SetMatrix("matView", &mat);
    m_pEffect->SetMatrix("matProj", &mat);

    float aspect = float(m_rectClient.Height()) / float(m_rectClient.Width());


    float x = 1.f / float(m_rectClient.Width()) * 2.f;
    float y = 1.f / float(m_rectClient.Height()) * 2.f;

    D3DXMATRIX matScale;
    D3DXMatrixScaling(&matScale, x, y, 1.f);
    D3DXMatrixMultiply(&mat, &matScale, &mat);

    float sx = float(m_rectClient.Width()) * 0.5f;
    float sy = float(m_rectClient.Height()) * 0.5f;
    D3DXMATRIX matTrans;
    D3DXMatrixTranslation(&matTrans, -sx, -sy, 0.0f);
    D3DXMatrixMultiply(&mat, &matTrans, &mat);

    // 	D3DXMatrixScaling(&mat, aspect, 1.0f, 1.0f);
    m_pEffect->SetMatrix("matViewProj", (D3DXMATRIX*)&mat);
}


void CViewAnalysis::Render()
{
    if (this->SwapChain == 0)
        return;

    IDirect3DSurface9* srf;
    this->SwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &srf);
    theApp.Device->SetRenderTarget(0, srf);
    theApp.Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
//     theApp.Device->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0xff3f4f5f, 1.0f, 0);

    D3DSURFACE_DESC sd;
    srf->GetDesc(&sd);

    D3DVIEWPORT9 vp = { 0, 0, sd.Width, sd.Height, 0, 1.0f };
    theApp.Device->SetViewport(&vp);

    theApp.Device->BeginScene();

	::GetClientRect(this->m_hWnd, &m_rectClient);

    SetMatrix();

	DrawAnalysisTitle();

	DrawMainInfoTitle();

	DrawMainInfo();

	DrawAnotherInfoTitle();

	DrawAnotherInfo();


	//DrawAnalysisInfo();

    theApp.Device->EndScene();

    HRESULT hr = S_OK;

    hr = this->SwapChain->Present(0, 0, this->m_hWnd, 0, D3DPRESENT_DONOTWAIT);

    srf->Release();
}

