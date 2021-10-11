#pragma once

class CViewAnalysis : public CWnd
{
    DECLARE_DYNAMIC(CViewAnalysis)

public:
	CViewAnalysis();
    virtual ~CViewAnalysis();

protected:
    DECLARE_MESSAGE_MAP()


public:
    IDirect3DSwapChain9*		SwapChain;
    ID3DXEffect*				m_pEffect;
    ID3DXLine*					m_pDXLine;

	ID3DXFont*					m_pDXFont0;
	ID3DXFont*					m_pDXFont1;

	CRect						m_rectClient;
	CRect						m_recAnalysisTitle; // 비전 결과 문구 사각형
	CRect						m_rectMainInfoTitle; // 본인 정보 문구 사각형
	CRect						m_rectMainInfo; // 본인정보 중심위치 사각형
	CRect						m_rectAnotherInfoTitle; // 타인 정보 문구 사각형
	CRect						m_rectAnotherInfo; // 타인 정보 중심위치 사각형

public:
    void		InitD3D();
    void        SetMatrix();

public:
    void		Render();

	void		DrawAnalysisTitle(); // 써라
	void		DrawAnotherInfoTitle();// 써라
	void		DrawAnotherInfo();// 써라
	void		DrawMainInfoTitle();// 써라
	void		DrawMainInfo();// 써라
	void		DrawAnalysisInfo();// 써라

	void		DrawText(ID3DXFont* pFont, LPCWSTR pString, LPRECT pRect, DWORD Format, D3DCOLOR Color);

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

};

