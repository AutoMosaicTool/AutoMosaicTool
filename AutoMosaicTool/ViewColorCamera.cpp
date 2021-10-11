#include "stdafx.h"
#include "AutoMosaicTool.h"
#include "ViewColorCamera.h"

IMPLEMENT_DYNAMIC(CViewColorCamera, CWnd)

CViewColorCamera::CViewColorCamera()
{
	SwapChain = NULL;
	m_pEffect = NULL;
	m_pDXLine = NULL;

}


CViewColorCamera::~CViewColorCamera()
{
}


BEGIN_MESSAGE_MAP(CViewColorCamera, CWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()


int CViewColorCamera::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitD3D();

	return 0;
}

void CViewColorCamera::InitD3D()
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
	m_pDXLine->SetWidth(2);
	m_pDXLine->SetPattern(0xFFFFFFFF);
	m_pDXLine->SetAntialias(TRUE);


	CFont font1;
	font1.CreatePointFont(250, _T("맑은 고딕"));

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
}

void CViewColorCamera::DrawMainFaceInfo()
{
	if (!theApp.m_bDetectedFace)
		return;

	//m_pDXLine->Draw(theApp.m_mainFaceRange.box, 5, D3DCOLOR_ARGB(255, 255, 0, 0));


	theApp.Device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	theApp.Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pEffect->SetTechnique("Tech");

	if (theApp.m_bShowMainMask)
	{
		int idx = theApp.m_nMainMaskIndex;

		if (1 == idx)
			m_pEffect->SetTexture("TexColor", theApp.m_texMainMask0.tex);
		else if (2 == idx)
			m_pEffect->SetTexture("TexColor", theApp.m_texMainMask1.tex);
		else
			m_pEffect->SetTexture("TexColor", theApp.m_texMainMask2.tex);


		struct SVertex
		{
			float Pos[3];
			float Tex[2];
		};

		float fHalfWidth = fabs(theApp.m_mainFaceRange.box[1].x - theApp.m_mainFaceRange.box[0].x) * 0.5;
		float fHalfHeight = fabs(theApp.m_mainFaceRange.box[2].y - theApp.m_mainFaceRange.box[1].y) * 0.5;

		D3DXVECTOR2 center;
		center.x = (theApp.m_mainFaceRange.box[0].x + theApp.m_mainFaceRange.box[1].x) * 0.5;
		center.y = (theApp.m_mainFaceRange.box[1].y + theApp.m_mainFaceRange.box[2].y) * 0.5;
		center.y = m_rectClient.Height() - center.y;
		SVertex v[4] =
		{
			{ { center.x - fHalfWidth, center.y + fHalfHeight, 0.f },{ 0, 0 } },
			{ { center.x + fHalfWidth, center.y + fHalfHeight, 0.f },{ 1, 0 } },
			{ { center.x + fHalfWidth, center.y - fHalfHeight, 0.f },{ 1, 1 } },
			{ { center.x - fHalfWidth, center.y - fHalfHeight, 0.f },{ 0, 1 } },
		};

		m_pEffect->Begin(0, 0);
		m_pEffect->BeginPass(0);

		theApp.Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(SVertex));

		m_pEffect->EndPass();
		m_pEffect->End();

		return;
	}






	if (theApp.m_bShowMainMosaic)
	{
		float fBoxWidth = theApp.m_mainFaceMosaicRange.width;


		for (int y = 0; y < nMosaicDetail; y++)
		{
			for (int x = 0; x < nMosaicDetail; x++)
			{


				D3DXVECTOR2 pos;
				pos.x = theApp.m_mainFaceMosaicRange.center[y][x].x;
				pos.y = theApp.m_mainFaceMosaicRange.center[y][x].y;

				D3DXVECTOR3 color;
				color.x = theApp.m_mainFaceMosaicRange.color[y][x].x;
				color.y = theApp.m_mainFaceMosaicRange.color[y][x].y;
				color.z = theApp.m_mainFaceMosaicRange.color[y][x].z;


				D3DXVECTOR4 col(float(color.x) / 255.f,
					float(color.y) / 255.f,
					float(color.z) / 255.f, 1.f);

				m_pEffect->SetVector("LineColor", &col);

				D3DXVECTOR3 fillTri[3];
				fillTri[0].x = pos.x - fBoxWidth * 0.5f;
				fillTri[0].y = float(m_rectClient.Height()) - pos.y - fBoxWidth * 0.5f;
				fillTri[0].z = 0.f;

				fillTri[1].x = pos.x + fBoxWidth * 0.5f;
				fillTri[1].y = float(m_rectClient.Height()) - pos.y - fBoxWidth * 0.5f;
				fillTri[1].z = 0.f;

				fillTri[2].x = pos.x + fBoxWidth * 0.5f;
				fillTri[2].y = float(m_rectClient.Height()) - pos.y + fBoxWidth * 0.5f;
				fillTri[2].z = 0.f;

				m_pEffect->Begin(0, 0);
				m_pEffect->BeginPass(1);

				theApp.Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, &fillTri[0], sizeof(D3DXVECTOR3));

				m_pEffect->EndPass();
				m_pEffect->End();

				fillTri[0].x = pos.x - fBoxWidth * 0.5f;
				fillTri[0].y = float(m_rectClient.Height()) - pos.y - fBoxWidth * 0.5f;
				fillTri[0].z = 0.f;

				fillTri[1].x = pos.x + fBoxWidth * 0.5f;
				fillTri[1].y = float(m_rectClient.Height()) - pos.y + fBoxWidth * 0.5f;
				fillTri[1].z = 0.f;


				fillTri[2].x = pos.x - fBoxWidth * 0.5f;
				fillTri[2].y = float(m_rectClient.Height()) - pos.y + fBoxWidth * 0.5f;
				fillTri[2].z = 0.f;

				m_pEffect->Begin(0, 0);
				m_pEffect->BeginPass(1);

				theApp.Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, &fillTri[0], sizeof(D3DXVECTOR3));

				m_pEffect->EndPass();
				m_pEffect->End();

			}
		}
	}
}

void CViewColorCamera::DrawAnotherFaceInfo()
{
	if (!theApp.m_bDetectedFace)
		return;

	//m_pDXLine->Draw(theApp.m_mainFaceRange.box, 5, D3DCOLOR_ARGB(255, 255, 0, 0));

	if (!theApp.m_bShowAnotherMosaic)
		return;

	if (0 == theApp.m_vecAnotherFaces.size())
		return;


	//if (theApp.m_bShowMainMosaic)
	{
		theApp.Device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
		theApp.Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_pEffect->SetTechnique("Tech");

		int nSize = int(theApp.m_vecAnotherFaces.size());

		for (int n = 0; n < nSize; n++)
		{
			float fBoxWidth = theApp.m_vecAnotherFaceMosaicRange[n].width;
			for (int y = 0; y < nMosaicDetail; y++)
			{
				for (int x = 0; x < nMosaicDetail; x++)
				{
					D3DXVECTOR2 pos;
					pos.x = theApp.m_vecAnotherFaceMosaicRange[n].center[y][x].x;
					pos.y = theApp.m_vecAnotherFaceMosaicRange[n].center[y][x].y;

					D3DXVECTOR3 color;
					color.x = theApp.m_vecAnotherFaceMosaicRange[n].color[y][x].x;
					color.y = theApp.m_vecAnotherFaceMosaicRange[n].color[y][x].y;
					color.z = theApp.m_vecAnotherFaceMosaicRange[n].color[y][x].z;


					D3DXVECTOR4 col(float(color.x) / 255.f,
						float(color.y) / 255.f,
						float(color.z) / 255.f, 1.f);

					m_pEffect->SetVector("LineColor", &col);

					D3DXVECTOR3 fillTri[3];
					fillTri[0].x = pos.x - fBoxWidth * 0.5f;
					fillTri[0].y = float(m_rectClient.Height()) - pos.y - fBoxWidth * 0.5f;
					fillTri[0].z = 0.f;

					fillTri[1].x = pos.x + fBoxWidth * 0.5f;
					fillTri[1].y = float(m_rectClient.Height()) - pos.y - fBoxWidth * 0.5f;
					fillTri[1].z = 0.f;

					fillTri[2].x = pos.x + fBoxWidth * 0.5f;
					fillTri[2].y = float(m_rectClient.Height()) - pos.y + fBoxWidth * 0.5f;
					fillTri[2].z = 0.f;

					m_pEffect->Begin(0, 0);
					m_pEffect->BeginPass(1);

					theApp.Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, &fillTri[0], sizeof(D3DXVECTOR3));

					m_pEffect->EndPass();
					m_pEffect->End();

					fillTri[0].x = pos.x - fBoxWidth * 0.5f;
					fillTri[0].y = float(m_rectClient.Height()) - pos.y - fBoxWidth * 0.5f;
					fillTri[0].z = 0.f;

					fillTri[1].x = pos.x + fBoxWidth * 0.5f;
					fillTri[1].y = float(m_rectClient.Height()) - pos.y + fBoxWidth * 0.5f;
					fillTri[1].z = 0.f;


					fillTri[2].x = pos.x - fBoxWidth * 0.5f;
					fillTri[2].y = float(m_rectClient.Height()) - pos.y + fBoxWidth * 0.5f;
					fillTri[2].z = 0.f;

					m_pEffect->Begin(0, 0);
					m_pEffect->BeginPass(1);

					theApp.Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, &fillTri[0], sizeof(D3DXVECTOR3));

					m_pEffect->EndPass();
					m_pEffect->End();

				}
			}

		}
	}

}


void CViewColorCamera::DrawColorImageTexture()
{
	if (!theApp.m_bDataLoad)
		return;

	if (NULL == theApp.m_pWebCamTexture)
		return;

	CRect rectClient = m_rectClient;

	theApp.Device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	theApp.Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pEffect->SetTechnique("Tech");

	m_pEffect->SetTexture("TexColor", theApp.m_pWebCamTexture);

	struct SVertex
	{
		float Pos[3];
		float Tex[2];
	};

	float fWidth = float(rectClient.Width());
	float fHeight = float(rectClient.Height());


	//SVertex v[4] =
	//{
	//	{ { 0.f, fHeight, 0.f },{ 0, 0 } },
	//	{ { fWidth, fHeight, 0.f },{ 1, 0 } },
	//	{ { fWidth, 0.f , 0.f },{ 1, 1 } },
	//	{ { 0.f, 0.f , 0.f },{ 0, 1 } },
	//};


	SVertex v[4] =
	{
		{ { 0.f, fHeight, 0.f },{ 1, 0 } },
		{ { fWidth, fHeight, 0.f },{ 0, 0 } },
		{ { fWidth, 0.f , 0.f },{ 0, 1 } },
		{ { 0.f, 0.f , 0.f },{ 1, 1 } },
	};


	m_pEffect->Begin(0, 0);
	m_pEffect->BeginPass(0);

	theApp.Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(SVertex));

	m_pEffect->EndPass();
	m_pEffect->End();
}



void CViewColorCamera::SetMatrix()
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


void CViewColorCamera::Render()
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

	DrawColorImageTexture(); // 컬려이미지 그려주고


	DrawMainFaceInfo(); // 메인 얼굴 그려주고

	DrawAnotherFaceInfo(); // 타인들 얼굴 그려주고

	theApp.Device->EndScene();

	HRESULT hr = S_OK;

	hr = this->SwapChain->Present(0, 0, this->m_hWnd, 0, D3DPRESENT_DONOTWAIT);

	srf->Release();
}

void CViewColorCamera::DrawText(ID3DXFont* pFont, LPCWSTR pString, LPRECT pRect, DWORD Format, D3DCOLOR Color)
{
	pFont->DrawText(NULL, pString, -1, pRect, Format, Color);
}
