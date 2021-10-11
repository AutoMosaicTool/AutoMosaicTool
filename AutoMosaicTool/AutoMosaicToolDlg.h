
// AutoMosaicToolDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"


// CAutoMosaicToolDlg ��ȭ ����
class CAutoMosaicToolDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CAutoMosaicToolDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTOMOSAICTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;


public:
	HANDLE		m_ThreadMainView;
	CStatic		m_group_camera_view; // �׷�ڽ�
	CStatic		m_group_analysis_view; // �׷�ڽ�

	CButton		m_check_another_mosaic; // üũ�ڽ� 
	CButton		m_check_own_mosaic; // üũ�ڽ�
	CButton		m_button_close; // �����ư
	CComboBox	m_combo_mask;


public:
	void	SetPositionControls(); // ��Ʈ�ѷ� ��ġ
	BOOL	CheckWebCamStatus(); //ķ�ִ���

public:
	static DWORD WINAPI MainViewThread(CAutoMosaicToolDlg* This);


	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckAnotherMosaic();
	afx_msg void OnBnClickedCheckOwnMosaic();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeComboMask();
};
