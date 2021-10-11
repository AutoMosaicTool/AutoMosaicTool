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
	CRect						m_recAnalysisTitle; // ���� ��� ���� �簢��
	CRect						m_rectMainInfoTitle; // ���� ���� ���� �簢��
	CRect						m_rectMainInfo; // �������� �߽���ġ �簢��
	CRect						m_rectAnotherInfoTitle; // Ÿ�� ���� ���� �簢��
	CRect						m_rectAnotherInfo; // Ÿ�� ���� �߽���ġ �簢��

public:
    void		InitD3D();
    void        SetMatrix();

public:
    void		Render();

	void		DrawAnalysisTitle(); // ���
	void		DrawAnotherInfoTitle();// ���
	void		DrawAnotherInfo();// ���
	void		DrawMainInfoTitle();// ���
	void		DrawMainInfo();// ���
	void		DrawAnalysisInfo();// ���

	void		DrawText(ID3DXFont* pFont, LPCWSTR pString, LPRECT pRect, DWORD Format, D3DCOLOR Color);

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

};

