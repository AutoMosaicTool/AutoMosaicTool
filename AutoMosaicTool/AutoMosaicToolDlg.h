
// AutoMosaicToolDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"


// CAutoMosaicToolDlg 대화 상자
class CAutoMosaicToolDlg : public CDialogEx
{
// 생성입니다.
public:
	CAutoMosaicToolDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUTOMOSAICTOOL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;


public:
	HANDLE		m_ThreadMainView;
	CStatic		m_group_camera_view; // 그룹박스
	CStatic		m_group_analysis_view; // 그룹박스

	CButton		m_check_another_mosaic; // 체크박스 
	CButton		m_check_own_mosaic; // 체크박스
	CButton		m_button_close; // 종료버튼
	CComboBox	m_combo_mask;


public:
	void	SetPositionControls(); // 컨트롤러 배치
	BOOL	CheckWebCamStatus(); //캠있는지

public:
	static DWORD WINAPI MainViewThread(CAutoMosaicToolDlg* This);


	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCheckAnotherMosaic();
	afx_msg void OnBnClickedCheckOwnMosaic();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeComboMask();
};
