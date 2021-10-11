#pragma once

class CViewColorCamera : public CWnd
{
    DECLARE_DYNAMIC(CViewColorCamera)

public:
	CViewColorCamera();
    virtual ~CViewColorCamera();

protected:
    DECLARE_MESSAGE_MAP()


public:
    IDirect3DSwapChain9*		SwapChain; // rendering (back buffer)
    ID3DXEffect*				m_pEffect; // rendering (이미지 gpu)
    ID3DXLine*					m_pDXLine; // rendering (선긋는 변수)
	ID3DXFont*					m_pDXFont0; // rendering (텍스트 변수)

	CRect						m_rectClient; // 카메라 뷰 사각형

public:
    void		InitD3D(); // redering 초기화
    void        SetMatrix();

public:
    void		Render();
	void		DrawColorImageTexture(); // 컬러이미지 그려줘라
	void		DrawMainFaceInfo(); // 메인 얼굴정보 그려라
	void		DrawAnotherFaceInfo(); // 타인들 얼굴정보 그려라
	void		DrawText(ID3DXFont* pFont, LPCWSTR pString, LPRECT pRect, DWORD Format, D3DCOLOR Color); // 텍스트써라

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

};

