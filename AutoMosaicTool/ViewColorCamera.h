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
    ID3DXEffect*				m_pEffect; // rendering (�̹��� gpu)
    ID3DXLine*					m_pDXLine; // rendering (���ߴ� ����)
	ID3DXFont*					m_pDXFont0; // rendering (�ؽ�Ʈ ����)

	CRect						m_rectClient; // ī�޶� �� �簢��

public:
    void		InitD3D(); // redering �ʱ�ȭ
    void        SetMatrix();

public:
    void		Render();
	void		DrawColorImageTexture(); // �÷��̹��� �׷����
	void		DrawMainFaceInfo(); // ���� ������ �׷���
	void		DrawAnotherFaceInfo(); // Ÿ�ε� ������ �׷���
	void		DrawText(ID3DXFont* pFont, LPCWSTR pString, LPRECT pRect, DWORD Format, D3DCOLOR Color); // �ؽ�Ʈ���

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

};

