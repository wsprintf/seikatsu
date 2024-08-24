
// seikatsuDlg.cpp : 実装ファイル
//

//	#include <process.h>
//	#include <stdlib.h>
//	#include <processthreadsapi.h>
//	#include <windows.h>
//	#include <iostream>
#include <string>
//	#include <wchar.h>


#include "pch.h"
#include "framework.h"
#include "seikatsu.h"
#include "seikatsuDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <windows.h>

HWND	exh = NULL;					// MessadeBox HWND

char	*bslist[8] = { "空腹時\n", "随　時\n", "食　後\n" };
int		bsno = 1;

char	rirekibuff[128][64][64] = {NULL};
int		rirekino = 0;




// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CseikatsuDlg ダイアログ



CseikatsuDlg::CseikatsuDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SEIKATSU_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CseikatsuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_cDatePicker);
}

BEGIN_MESSAGE_MAP(CseikatsuDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CseikatsuDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CseikatsuDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CseikatsuDlg::OnBnClickedButton2)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, &CseikatsuDlg::OnDtnDatetimechangeDatetimepicker1)
	ON_BN_CLICKED(IDC_BUTTON3, &CseikatsuDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDCANCEL, &CseikatsuDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CseikatsuDlg::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON4, &CseikatsuDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_CHECK1, &CseikatsuDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON5, &CseikatsuDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CseikatsuDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// CseikatsuDlg メッセージ ハンドラー

BOOL CseikatsuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。


	// 終了時に保存したウィンドウ位置の読み込み
	CWinApp* pApp = AfxGetApp();
	RECT rc = { pApp->GetProfileInt(_T("Window"), _T("left"), 0),
				pApp->GetProfileInt(_T("Window"), _T("top"), 0),
				pApp->GetProfileInt(_T("Window"), _T("right"), 0),
				pApp->GetProfileInt(_T("Window"), _T("bottom"), 0) };

	// ウィンドウ位置の変更
	if (!::IsRectEmpty(&rc))
		MoveWindow(&rc, FALSE);



	exh = CseikatsuDlg::GetSafeHwnd();							// ハンドルを取得


	CString str;

	CComboBox* pcmb_bs = (CComboBox*)GetDlgItem(IDC_COMBO1);		// 血糖値
	str.Format(_T("1. 空腹時"));
	pcmb_bs->AddString(str);
	str.Format(_T("2. 随　時"));
	pcmb_bs->AddString(str);
	str.Format(_T("3. 食　後"));
	pcmb_bs->AddString(str);

	pcmb_bs->SetMinVisibleItems(3);									//　選択肢数：３
	pcmb_bs->SetCueBanner(_T("3. 食　後"));							//　選択肢項目：名称
	pcmb_bs->SetCurSel(2);											//　選択肢番号

	pcmb_bs->EnableWindow(TRUE);




	CMFCButton* pMBtn_Edit = (CMFCButton*)GetDlgItem(IDC_CHECK1);
//	pMBtn_Edit->EnableWindow(FALSE);										// 新患処理		無効

	CEdit* myEditName = (CEdit*)GetDlgItem(IDC_EDIT2);						// 患者氏名		編集無効
	CEdit* myEditBirth = (CEdit*)GetDlgItem(IDC_EDIT3);						// 患者生年月日	編集無効
	CEdit* myEditSei = (CEdit*)GetDlgItem(IDC_EDIT4);						// 患者性別		編集無効
	myEditName->EnableWindow(FALSE);
	myEditBirth->EnableWindow(FALSE);
	myEditSei->EnableWindow(FALSE);


	CMFCButton* pMBtn_csv = (CMFCButton*)GetDlgItem(IDC_BUTTON6);			// 新規登録ボタン
	pMBtn_csv->EnableWindow(FALSE);




	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CseikatsuDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CseikatsuDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CseikatsuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CseikatsuDlg::OnBnClickedOk()										// Read処理
{

	// TODO: ここにコントロール通知ハンドラー コードを追加します。


	CString	ss[32];

	int		i;
	int		tbl = 0;

	char	idbuff[64] = { NULL };
	char	fname[64] = { NULL };

	char	readbuffer[256] = { NULL };

	FILE* rfp;

	errno_t error;

	CEdit* myEdit1 = (CEdit*)GetDlgItem(IDC_EDIT1);			// 患者ID
	CEdit* myEdit2 = (CEdit*)GetDlgItem(IDC_EDIT2);			// 氏名
	CEdit* myEdit3 = (CEdit*)GetDlgItem(IDC_EDIT3);			// 生年月日
	CEdit* myEdit4 = (CEdit*)GetDlgItem(IDC_EDIT4);			// 性別
	CEdit* myEdit5 = (CEdit*)GetDlgItem(IDC_EDIT5);			// 身長
	CEdit* myEdit6 = (CEdit*)GetDlgItem(IDC_EDIT6);			// 体重
	CEdit* myEdit7 = (CEdit*)GetDlgItem(IDC_EDIT7);			// BMI
	CEdit* myEdit8 = (CEdit*)GetDlgItem(IDC_EDIT8);			// 腹囲
	CEdit* myEdit9 = (CEdit*)GetDlgItem(IDC_EDIT9);			// 収縮期血圧
	CEdit* myEdit10 = (CEdit*)GetDlgItem(IDC_EDIT10);		// 拡張期血圧
	CEdit* myEdit11 = (CEdit*)GetDlgItem(IDC_EDIT11);		// 採血日
	CEdit* myEdit12 = (CEdit*)GetDlgItem(IDC_EDIT12);		// 血糖値
	CEdit* myEdit13 = (CEdit*)GetDlgItem(IDC_EDIT13);		// HbA1c
	CEdit* myEdit14 = (CEdit*)GetDlgItem(IDC_EDIT14);		// 総コレステロール
	CEdit* myEdit15 = (CEdit*)GetDlgItem(IDC_EDIT15);		// 中性脂肪
	CEdit* myEdit16 = (CEdit*)GetDlgItem(IDC_EDIT16);		// HDLコレステロール
	CEdit* myEdit17 = (CEdit*)GetDlgItem(IDC_EDIT17);		// LDLコレステロール

	CEdit* myEdit18 = (CEdit*)GetDlgItem(IDC_EDIT18);		// 食後時間

	CEdit* myEdit19 = (CEdit*)GetDlgItem(IDC_EDIT19);		// 患者データ件数
	CEdit* myEdit20 = (CEdit*)GetDlgItem(IDC_EDIT20);		// 前回登録日

	CEdit* myEdit21 = (CEdit*)GetDlgItem(IDC_EDIT21);		// 目標値HbA1c


	myEdit1->GetWindowText(ss[0]);
	myEdit2->GetWindowText(ss[1]);
	myEdit3->GetWindowText(ss[2]);
	myEdit4->GetWindowText(ss[3]);
	myEdit5->GetWindowText(ss[4]);
	myEdit6->GetWindowText(ss[5]);
	myEdit7->GetWindowText(ss[6]);
	myEdit8->GetWindowText(ss[7]);
	myEdit9->GetWindowText(ss[8]);
	myEdit10->GetWindowText(ss[9]);
	myEdit11->GetWindowText(ss[10]);
	myEdit12->GetWindowText(ss[11]);
	myEdit13->GetWindowText(ss[12]);
	myEdit14->GetWindowText(ss[13]);
	myEdit15->GetWindowText(ss[14]);
//	myEdit16->GetWindowText(ss[15]);
	myEdit17->GetWindowText(ss[16]);

	myEdit18->GetWindowText(ss[20]);						// 食後時間
	myEdit19->GetWindowText(ss[19]);						// 患者データ件数
	myEdit20->GetWindowText(ss[20]);						// 患者データ件数

	myEdit21->GetWindowText(ss[15]);						// 目標値HbA1c



	memcpy(idbuff, CStringA(ss[0]).GetBuffer(), sizeof(CStringA(ss[0]).GetBuffer()));						//	CString	→	char *
//	MessageBoxA(NULL, (const char*)idbuff, (const char*)"Debug Message", MB_OK);
	sprintf_s(fname, "./data/ID-%s.csv", (const char*)idbuff);
//	MessageBoxA(NULL, (const char*)fname, (const char*)"Debug Message", MB_OK);


	error = fopen_s(&rfp, (const char*)fname, "rt");

	if (error != 0) {
		MessageBoxA(exh, (const char*)"特定疾患療養管理加算対象外の患者です。", (const char*)"Error Message", MB_OK);
		//		int re = AfxMessageBox(_T("特定疾患療養管理加算対象外の患者です。"), MB_YESNO | MB_ICONQUESTION);
		//		if (re == IDYES) {
		//		}
	}
	else {
		for (i = 0; i < 32; i++) {
			if (fgets(readbuffer, sizeof(readbuffer), rfp) == NULL)
				break;
			else {
				if ( i == 19 ){
					if (memcmp(readbuffer, "空腹時", 6) == NULL) {
						bsno = 0;
					}
					if (memcmp(readbuffer, "随　時", 6) == NULL) {
						bsno = 1;
					}
					if (memcmp(readbuffer, "食　後", 6) == NULL) {
						bsno = 2;
					}
//					MessageBoxA(NULL, (const char*)readbuffer, (const char*)"Debug Message", MB_OK);
				}
				ss[tbl++] = CString(readbuffer);														//	char *	→　CString
//				MessageBoxA(NULL, (const char*)readbuffer, (const char*)"Debug Message", MB_OK);
			}

		}

		fclose(rfp);


		myEdit2->SetWindowText(ss[0]);		// 氏名
		myEdit3->SetWindowText(ss[1]);		// 生年月日
		myEdit4->SetWindowText(ss[2]);		// 性別

		myEdit5->SetWindowText(ss[9]);		// 身長
		myEdit6->SetWindowText(ss[10]);		// 体重
		myEdit7->SetWindowText(ss[11]);		// BMI
		myEdit8->SetWindowText(ss[12]);		// 腹囲
		myEdit9->SetWindowText(ss[13]);		// 収縮期
		myEdit10->SetWindowText(ss[14]);	// 拡張期

		myEdit11->SetWindowText(ss[18]);	// 検査日
		myEdit12->SetWindowText(ss[21]);	// 血糖値
		myEdit13->SetWindowText(ss[22]);	// HbA1c
		myEdit14->SetWindowText(ss[23]);	// 総コレステロール
		myEdit15->SetWindowText(ss[24]);	// 中性脂肪
		myEdit16->SetWindowText(ss[25]);	// HDLコレステロール
		myEdit17->SetWindowText(ss[26]);	// LDLコレステロール

		myEdit18->SetWindowText(ss[20]);	// 食後時間

		myEdit21->SetWindowText(ss[15]);	// 目標値HbA1c
		myEdit19->SetWindowText(ss[4]);		// 目標値HbA1c

		myEdit20->SetWindowText(ss[5]);		// 前回登録日付

		CComboBox* pcmb_bs = (CComboBox*)GetDlgItem(IDC_COMBO1);		// 空腹時／随時／食後
		pcmb_bs->SetCurSel(bsno);

		rirekino = 0;



		char	rirekifname[64] = { NULL };
		CTime NowTime = CTime::GetCurrentTime();
		int Year = NowTime.GetYear();
		int	Month = NowTime.GetMonth();
		sprintf_s(rirekifname, "./data/履歴データ/履歴%04d%02d.txt", Year, Month);
//		MessageBoxA(NULL, (const char*)rirekifname, (const char*)"Debug Message", MB_OK);
//		if (error = fopen_s(&rfp, (const char*)"./data/履歴.txt", "rt") == 0) {

/*
		if(error = fopen_s(&rfp, (const char*)rirekifname, "rt") == 0 ) {
			while( 1 ) {
				if (fgets(readbuffer, sizeof(readbuffer), rfp) == NULL)
					break;
				else {
					if (atol(idbuff) == atol(readbuffer)) {
//						MessageBoxA(NULL, (const char*)idbuff, (const char*)"Debug Message", MB_OK);
						for (i = 0; i < 32; i ++) {
							if (fgets(readbuffer, sizeof(readbuffer), rfp) == NULL)
								break;
							else { 	
								memcpy(&rirekibuff[rirekino][i][0], readbuffer, 64);
								if (memcmp(readbuffer, "-----END-----", 10) == 0)
									break;
							}
						}
//						MessageBoxA(NULL, (const char*)&rirekibuff[rirekino][1][0], (const char*)"Debug Message", MB_OK);
						rirekino++;
					}
				}
			}
			fclose(rfp);


			ss[21].Format(_T("%d"), rirekino);
			myEdit19->SetWindowText(ss[21]);								// 登録件数
			if (rirekino > 0) {
				ss[22] = CString(&rirekibuff[rirekino - 1][0][0]);
			}
			else {
				ss[22].Format(_T("%d"), rirekino);
				ss[22] = CString("　初回登録");
			}
			myEdit20->SetWindowText(ss[22]);							// 前回登録日付

		}
*/


	}





//	CDialogEx::OnOK();
}


void CseikatsuDlg::OnBnClickedButton1()
{

	MessageBoxA(exh, (const char*)"現在、この機能を使用できません。", (const char*)"Error Message", MB_OK);
	return;		//処理しない

	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	FILE*	rfp;
	FILE*	wfp;
	char	buffer[256];
	char	writebuff[256] = { NULL };
	char	fbuff[256] = { NULL };
	char*	fname = fbuff;

	errno_t error;

	int		i;
	int		top;
	char	idno[64] = { NULL } ;
	char	kname[64] = { NULL };
	char	yname[64] = { NULL };
	char	init[64] = { NULL };
	char	birth[64] = { NULL };
	char	yold[64] = { NULL };
	char	seib[64] = { NULL };


	error = fopen_s(&rfp, "特定疾患療養管理料リスト.csv", "rt");

	if ( error != 0) {
		int re = AfxMessageBox(_T("ファイルが見つからない"), MB_YESNO | MB_ICONQUESTION);
		if (re == IDYES) {
		}
	}
	else {
		int re = AfxMessageBox(_T("変換処理を実行します"), MB_YESNO | MB_ICONQUESTION);
		if (re == IDYES) {
			while (1) {
				if (fgets(buffer, sizeof(buffer), rfp) == NULL)
					break;
				else {

					top = 0;

					for (i = 0; i < sizeof(buffer); i++) {

						if (buffer[i] == ',') {
							buffer[i] = NULL;
							memcpy(idno, &buffer[top], i - top + 1);
							top = i + 1;
							//							MessageBoxA(NULL, (const char*)idno, (const char*)"File Append mode", MB_OK);
							break;
						}
					}
					for (; i < sizeof(buffer); i++) {
						if (buffer[i] == ',') {
							buffer[i] = NULL;
							memcpy(kname, &buffer[top], i - top + 1);
							top = i + 1;
							//							MessageBoxA(NULL, (const char*)kname, (const char*)"File Append mode", MB_OK);
							break;
						}

					}
					for (; i < sizeof(buffer); i++) {
						if (buffer[i] == ',') {
							buffer[i] = NULL;
							memcpy(yname, &buffer[top], i - top + 1);
							top = i + 1;
							//							MessageBoxA(NULL, (const char*)yname, (const char*)"File Append mode", MB_OK);
							break;
						}

					}
					for (; i < sizeof(buffer); i++) {
						if (buffer[i] == ',') {
							buffer[i] = NULL;
							memcpy(init, &buffer[top], i - top + 1);
							top = i + 1;
							//							MessageBoxA(NULL, (const char*)init, (const char*)"File Append mode", MB_OK);
							break;
						}

					}
					for (; i < sizeof(buffer); i++) {
						if (buffer[i] == ',') {
							buffer[i] = NULL;
							memcpy(birth, &buffer[top], i - top + 1);
							top = i + 1;
							//							MessageBoxA(NULL, (const char*)birth, (const char*)"File Append mode", MB_OK);
							break;
						}

					}
					for (; i < sizeof(buffer); i++) {
						if (buffer[i] == ',') {
							buffer[i] = NULL;
							memcpy(yold, &buffer[top], i - top + 1);
							top = i + 1;
							//							MessageBoxA(NULL, (const char*)yold, (const char*)"File Append mode", MB_OK);
							break;
						}

					}
					for (; i < sizeof(buffer); i++) {
						if (buffer[i] == ',') {
							buffer[i] = NULL;
							memcpy(seib, &buffer[top], i - top + 1);
							top = i + 1;
							//							MessageBoxA(NULL, (const char*)seib, (const char*)"File Append mode", MB_OK);
							break;
						}
					}
					sprintf_s(fbuff, "./data/ID-%s.csv", idno);
					//					MessageBoxA(NULL, (const char*)fname, (const char*)"File Append mode", MB_OK);
					error = fopen_s(&wfp, fname, "wt");
//					sprintf_s(writebuff, "%s\n%s\n%s\n%s\n0\n\n\n\n\n身長\n体重\nBMI\n腹囲\n収縮期\n拡張期\n目HbA1c\n\n\n2024/1/1\n空腹時\n0\n血糖値\nHbA1c\n総コレステロール\n中性脂肪\nHDLコレステロール\nLDLコレステロール\n", kname, birth, seib, idno );
					sprintf_s(writebuff, "%s\n%s\n%s\n%s\n0\n\n\n\n\n0\n0\n0\n0\n0\n0\n0\n\n\n未登録\n空腹時\n0\n0\n0\n0\n0\n0\n0\n", kname, birth, seib, idno);
					fputs(writebuff, wfp);
					fclose(wfp);

				}
			}

			fclose(rfp);


		}
	}

}



void CseikatsuDlg::OnBnClickedButton2()								// 保存・登録処理
{
	
	// TODO: ここにコントロール通知ハンドラー コードを追加します。


	CString	ss[64];

	int		i;
	int		tbl = 0;

	char	idbuff[64] = { NULL };
	char	fname[64] = { NULL };

	char	wtbl[32][64] = { NULL };

	char	writebuffer[256] = { NULL };
	char	readbuffer[256] = { NULL };
	char	dm_status[8] = { NULL };						// 糖尿病	チェック状態
	char	ht_status[8] = { NULL };						// 高血圧症	チェック状態
	char	hc_status[8] = { NULL };						// 脂質異常	チェック状態
	int		bsts = 0;


	FILE*	wfp;
	FILE*	afp;

	errno_t error;

	CTime	NowTM = CTime::GetCurrentTime();


	CEdit* myEdit1 = (CEdit*)GetDlgItem(IDC_EDIT1);			// 患者ID
	CEdit* myEdit2 = (CEdit*)GetDlgItem(IDC_EDIT2);			// 氏名
	CEdit* myEdit3 = (CEdit*)GetDlgItem(IDC_EDIT3);			// 生年月日
	CEdit* myEdit4 = (CEdit*)GetDlgItem(IDC_EDIT4);			// 性別
	CEdit* myEdit5 = (CEdit*)GetDlgItem(IDC_EDIT5);			// 身長
	CEdit* myEdit6 = (CEdit*)GetDlgItem(IDC_EDIT6);			// 体重
	CEdit* myEdit7 = (CEdit*)GetDlgItem(IDC_EDIT7);			// BMI
	CEdit* myEdit8 = (CEdit*)GetDlgItem(IDC_EDIT8);			// 腹囲
	CEdit* myEdit9 = (CEdit*)GetDlgItem(IDC_EDIT9);			// 収縮期血圧
	CEdit* myEdit10 = (CEdit*)GetDlgItem(IDC_EDIT10);		// 拡張期血圧
	CEdit* myEdit11 = (CEdit*)GetDlgItem(IDC_EDIT11);		// 採血日
	CEdit* myEdit12 = (CEdit*)GetDlgItem(IDC_EDIT12);		// 血糖値
	CEdit* myEdit13 = (CEdit*)GetDlgItem(IDC_EDIT13);		// HbA1c
	CEdit* myEdit14 = (CEdit*)GetDlgItem(IDC_EDIT14);		// 総コレステロール
	CEdit* myEdit15 = (CEdit*)GetDlgItem(IDC_EDIT15);		// 中性脂肪
	CEdit* myEdit16 = (CEdit*)GetDlgItem(IDC_EDIT16);		// HDLコレステロール
	CEdit* myEdit17 = (CEdit*)GetDlgItem(IDC_EDIT17);		// LDLコレステロール

	CEdit* myEdit18 = (CEdit*)GetDlgItem(IDC_EDIT18);		// 食後時間

	CEdit* myEdit19 = (CEdit*)GetDlgItem(IDC_EDIT19);		// 患者データ件数

	CEdit* myEdit21 = (CEdit*)GetDlgItem(IDC_EDIT21);		// 目標値HbA1c


	myEdit1->GetWindowText(ss[0]);
	myEdit2->GetWindowText(ss[1]);
	myEdit3->GetWindowText(ss[2]);
	myEdit4->GetWindowText(ss[3]);
	myEdit5->GetWindowText(ss[4]);
	myEdit6->GetWindowText(ss[5]);
	myEdit7->GetWindowText(ss[6]);
	myEdit8->GetWindowText(ss[7]);
	myEdit9->GetWindowText(ss[8]);
	myEdit10->GetWindowText(ss[9]);
	myEdit11->GetWindowText(ss[10]);
	myEdit12->GetWindowText(ss[11]);
	myEdit13->GetWindowText(ss[12]);
	myEdit14->GetWindowText(ss[13]);
	myEdit15->GetWindowText(ss[14]);
	myEdit16->GetWindowText(ss[15]);
	myEdit17->GetWindowText(ss[16]);

	myEdit18->GetWindowText(ss[17]);						// 食後時間
	myEdit19->GetWindowText(ss[19]);						// 患者データ件数

	myEdit21->GetWindowText(ss[21]);						// 目標値HbA1c


	CComboBox* pcmb_bs = (CComboBox*)GetDlgItem(IDC_COMBO1);			// 食後の選択肢の取得
	if ((bsno = pcmb_bs->GetCurSel()) != CB_ERR) {
//		MessageBoxA(exh, (const char*)bslist[bsno], (const char*)"Check Message", MB_OK);
	}



	if (MessageBoxA(exh, (const char*)"患者情報を登録しますか？", (const char*)"Check Message", MB_OKCANCEL) == IDOK) {

		memcpy(idbuff, CStringA(ss[0]).GetBuffer(), sizeof(CStringA(ss[0]).GetBuffer()));						//	CString	→	char *
//		MessageBoxA(NULL, (const char*)idbuff, (const char*)"Debug Message", MB_OK);
		sprintf_s(fname, "./data/ID-%s.csv", (const char*)idbuff);

		if (error = fopen_s(&afp, (const char*)fname, "rt") == 0) {
			bsts = 0;
			memcpy(dm_status, "FALSE", 5);
			memcpy(ht_status, "FALSE", 5);
			memcpy(hc_status, "FALSE", 5);
			while (1) {
				memset(readbuffer, NULL, sizeof(readbuffer));
				if (fgets(readbuffer, sizeof(readbuffer), afp) == NULL)
					break;
				else {
					if ((memcmp(readbuffer, "TRUE", 4) == NULL) || (memcmp(readbuffer, "FALSE", 5) == NULL)) {
//						MessageBoxA(NULL, (const char*)readbuffer, (const char*)"Debug Message", MB_OK);
						switch (bsts++) {
						case 0:
							memset(dm_status, NULL, sizeof(dm_status));
							memcpy(dm_status, readbuffer, strlen(readbuffer) - 1);
							break;
						case 1:
							memset(ht_status, NULL, sizeof(ht_status));
							memcpy(ht_status, readbuffer, strlen(readbuffer) - 1);
							break;
						case 2:
							memset(hc_status, NULL, sizeof(hc_status));
							memcpy(hc_status, readbuffer, strlen(readbuffer) - 1);
							break;
						}

					}

				}
			}
			fclose(afp);
		}


		if (error = fopen_s(&wfp, (const char*)fname, "wt") != 0)
			MessageBoxA(exh, (const char*)"登録に失敗しました。", (const char*)"Check Message", MB_OK);
		else {

			char	rirekifname[64] = { NULL };
			CTime NowTime = CTime::GetCurrentTime();
			int Year = NowTime.GetYear();
			int	Month = NowTime.GetMonth();
			int Days = NowTime.GetDay();
			int Hour = NowTime.GetHour();
			int Min = NowTime.GetMinute();
			sprintf_s(rirekifname, "./data/履歴データ/履歴%04d%02d.txt", Year, Month);

//			MessageBoxA(exh, (const char*)rirekifname, (const char*)"Check Message", MB_OK);

			if ( error = fopen_s(&afp, (const char*)rirekifname, "at+") != 0 )
				MessageBoxA(exh, (const char*)"履歴ファイルの生成に失敗しました・", (const char*)"Check Message", MB_OK);
			else {
				for (i = 1; i < 27; i++) {
					memcpy(&wtbl[i][0], CStringA(ss[i]).GetBuffer(), CStringA(ss[i]).GetLength());
					if (wtbl[i][CStringA(ss[i]).GetLength() - 1] != '\n') {
						wtbl[i][CStringA(ss[i]).GetLength()] = '\n';
						wtbl[i][CStringA(ss[i]).GetLength() + 1] = NULL;
						wtbl[i][CStringA(ss[i]).GetLength() + 1] = NULL;
					}
				}

				int xx = atoi(&wtbl[19][0]) + 1;


				sprintf_s(writebuffer, "%s%s%s%s\n%d\n%04d/%02d/%02d %02d:%02d\n\n\n\n%s%s%s%s%s%s%s\n\n%s%s%s%s%s%s%s%s%s\n\n%s\n%s\n%s\n",
					&wtbl[1][0], &wtbl[2][0], &wtbl[3][0], idbuff, xx, Year, Month, Days, Hour, Min,
					&wtbl[4][0], &wtbl[5][0], &wtbl[6][0], &wtbl[7][0], &wtbl[8][0], &wtbl[9][0], &wtbl[21][0],
					&wtbl[10][0], bslist[bsno], &wtbl[17],
					&wtbl[11][0], &wtbl[12][0], &wtbl[13][0], &wtbl[14][0], &wtbl[15][0], &wtbl[16][0],
					dm_status, ht_status, hc_status );

//				MessageBoxA(NULL, (const char*)writebuffer, (const char*)"Debug Message", MB_OK);


				fputs(writebuffer, wfp);

				sprintf_s(writebuffer, "%s\n%d-%02d-%02d %02d:%02d:%02d\n%s%s%s\n\n\n\n\n\n%s%s%s%s%s%s\n\n\n%s%s%s%s%s%s%s%s%s\n-----END-----\n", idbuff, NowTM.GetYear(), NowTM.GetMonth(), NowTM.GetDay(), NowTM.GetHour(), NowTM.GetMinute(), NowTM.GetSecond(),
					&wtbl[1][0], &wtbl[2][0], &wtbl[3][0],
					&wtbl[4][0], &wtbl[5][0], &wtbl[6][0], &wtbl[7][0], &wtbl[8][0], &wtbl[9][0],
					&wtbl[10][0], bslist[bsno], &wtbl[17],
					&wtbl[11][0], &wtbl[12][0], &wtbl[13][0], &wtbl[14][0], &wtbl[15][0], &wtbl[16][0]);

				fputs(writebuffer, afp);

				fclose(wfp);
				fclose(afp);

				MessageBoxA(exh, (const char*)"登録が完了しました。", (const char*)"Check Message", MB_OK);
			}
		}

	}


}




void CseikatsuDlg::OnDtnDatetimechangeDatetimepicker1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: ここにコントロール通知ハンドラー コードを追加します。



	int		Year, Month, Days;
	char	kensadate[16] = { NULL };
	CString	wkbuff;


	CEdit* myEdit11 = (CEdit*)GetDlgItem(IDC_EDIT11);									// 採血日
	myEdit11->GetWindowText(wkbuff);




	CTime date;
	this->m_cDatePicker.GetTime(date);

	Year = date.GetYear();
	Month = date.GetMonth();
	Days = date.GetDay();

	sprintf_s(kensadate, "%04d/%02d/%02d", Year, Month, Days);

//	MessageBoxA(NULL, (const char*)kensadate, (const char*)"Debug Message", MB_OK);

	wkbuff = CString(kensadate);														//	char *	→　CString
	myEdit11->SetWindowText(wkbuff);													// 採血日


}


void CseikatsuDlg::OnBnClickedButton3()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。


	CEdit* myEdit1 = (CEdit*)GetDlgItem(IDC_EDIT5);
	CEdit* myEdit2 = (CEdit*)GetDlgItem(IDC_EDIT6);
	CEdit* myEdit3 = (CEdit*)GetDlgItem(IDC_EDIT7);

	CString ss1, ss2, ss3;
	long	wait, tall, bmi;
	double	bmi1 = 0, weit1;

	ss3.Format(_T("%.1f"), bmi1);
	myEdit3->SetWindowText(ss3);

	myEdit1->GetWindowText(ss1);							  //文字列をssに格納
	bmi1 = _ttof(ss1);
	myEdit2->GetWindowText(ss2);							  //文字列をssに格納
	weit1 = _ttof(ss2);
	bmi1 = bmi1 / 100;
	bmi1 = bmi1 * bmi1;
	bmi1 = weit1 / bmi1;


	tall = _ttol(ss1);
	if ((tall >= 50 && tall <= 200)) {
		tall = tall * tall;
		//	tall = tall / 10000; 
		//	ss1.Format(_T("%d"), tall);
		//	myEdit1->SetWindowText(ss1);

		myEdit2->GetWindowText(ss2);						  //文字列をssに格納
		wait = _ttol(ss2);
		if ((wait >= 30) && (wait <= 200)) {					// 30 ～ 200Kg
			wait = wait * 100000;

			bmi = wait / tall;
			//	bmi1 = bmi;
			//	bmi1 = bmi1 / 10;

			//	ss3.Format(_T("%d"), bmi);
			ss3.Format(_T("%.1f"), bmi1);
			myEdit3->SetWindowText(ss3);
		}
		else
			MessageBoxA(exh, (const char*)"体重は、30Kg～200Kgの範囲で入力して下さい。", (const char*)"Error Message", MB_OK);
	}
	else
		MessageBoxA(exh, (const char*)"身長は、50cm～200cmの範囲で入力して下さい。", (const char*)"Error Message", MB_OK);


}


void CseikatsuDlg::OnBnClickedCancel()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	if (MessageBoxA(exh, (const char*)"終了しますか？", (const char*)"Check Message", MB_OKCANCEL) == IDOK) {
		CDialogEx::OnCancel();

		// ウィンドウ位置等の取得
		WINDOWPLACEMENT wpl;
		GetWindowPlacement(&wpl);

		// 保存
		CWinApp* pApp = AfxGetApp();
		pApp->WriteProfileInt(_T("Window"), _T("left"), wpl.rcNormalPosition.left);
		pApp->WriteProfileInt(_T("Window"), _T("top"), wpl.rcNormalPosition.top);
		pApp->WriteProfileInt(_T("Window"), _T("right"), wpl.rcNormalPosition.right);
		pApp->WriteProfileInt(_T("Window"), _T("bottom"), wpl.rcNormalPosition.bottom);
		pApp->WriteProfileInt(_T("Window"), _T("show"), wpl.showCmd);

//		CFrameWnd::OnDestroy();


	}

}


void CseikatsuDlg::OnCbnSelchangeCombo1()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
}


void CseikatsuDlg::OnBnClickedButton4()										// 保存のみ　処理
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。



	CString	ss[64];

	int		i;
	int		tbl = 0;

	char	idbuff[64] = { NULL };
	char	fname[64] = { NULL };
	char	rirekifname[64] = { "./data/履歴.txt" };

	char	wtbl[32][64] = { NULL };

	char	writebuffer[256] = { NULL };
	char	readbuffer[256] = { NULL };
	char	dm_status[8] = { NULL };						// 糖尿病	チェック状態
	char	ht_status[8] = { NULL };						// 高血圧症	チェック状態
	char	hc_status[8] = { NULL };						// 脂質異常	チェック状態
	int		bsts = 0;

	FILE* wfp;
	FILE* afp;

	errno_t error;

	CTime	NowTM = CTime::GetCurrentTime();


	CEdit* myEdit1 = (CEdit*)GetDlgItem(IDC_EDIT1);			// 患者ID
	CEdit* myEdit2 = (CEdit*)GetDlgItem(IDC_EDIT2);			// 氏名
	CEdit* myEdit3 = (CEdit*)GetDlgItem(IDC_EDIT3);			// 生年月日
	CEdit* myEdit4 = (CEdit*)GetDlgItem(IDC_EDIT4);			// 性別
	CEdit* myEdit5 = (CEdit*)GetDlgItem(IDC_EDIT5);			// 身長
	CEdit* myEdit6 = (CEdit*)GetDlgItem(IDC_EDIT6);			// 体重
	CEdit* myEdit7 = (CEdit*)GetDlgItem(IDC_EDIT7);			// BMI
	CEdit* myEdit8 = (CEdit*)GetDlgItem(IDC_EDIT8);			// 腹囲
	CEdit* myEdit9 = (CEdit*)GetDlgItem(IDC_EDIT9);			// 収縮期血圧
	CEdit* myEdit10 = (CEdit*)GetDlgItem(IDC_EDIT10);		// 拡張期血圧
	CEdit* myEdit11 = (CEdit*)GetDlgItem(IDC_EDIT11);		// 採血日
	CEdit* myEdit12 = (CEdit*)GetDlgItem(IDC_EDIT12);		// 血糖値
	CEdit* myEdit13 = (CEdit*)GetDlgItem(IDC_EDIT13);		// HbA1c
	CEdit* myEdit14 = (CEdit*)GetDlgItem(IDC_EDIT14);		// 総コレステロール
	CEdit* myEdit15 = (CEdit*)GetDlgItem(IDC_EDIT15);		// 中性脂肪
	CEdit* myEdit16 = (CEdit*)GetDlgItem(IDC_EDIT16);		// HDLコレステロール
	CEdit* myEdit17 = (CEdit*)GetDlgItem(IDC_EDIT17);		// LDLコレステロール

	CEdit* myEdit18 = (CEdit*)GetDlgItem(IDC_EDIT18);		// 食後時間

	CEdit* myEdit19 = (CEdit*)GetDlgItem(IDC_EDIT19);		// 患者データ件数
	CEdit* myEdit20 = (CEdit*)GetDlgItem(IDC_EDIT20);		// 患者データ件数


	CEdit* myEdit21 = (CEdit*)GetDlgItem(IDC_EDIT21);		// 目標値HbA1c


	myEdit1->GetWindowText(ss[0]);
	myEdit2->GetWindowText(ss[1]);
	myEdit3->GetWindowText(ss[2]);
	myEdit4->GetWindowText(ss[3]);
	myEdit5->GetWindowText(ss[4]);
	myEdit6->GetWindowText(ss[5]);
	myEdit7->GetWindowText(ss[6]);
	myEdit8->GetWindowText(ss[7]);
	myEdit9->GetWindowText(ss[8]);
	myEdit10->GetWindowText(ss[9]);
	myEdit11->GetWindowText(ss[10]);
	myEdit12->GetWindowText(ss[11]);
	myEdit13->GetWindowText(ss[12]);
	myEdit14->GetWindowText(ss[13]);
	myEdit15->GetWindowText(ss[14]);
	myEdit16->GetWindowText(ss[15]);
	myEdit17->GetWindowText(ss[16]);

	myEdit18->GetWindowText(ss[17]);						// 食後時間
	myEdit19->GetWindowText(ss[19]);						// 患者データ件数
	myEdit20->GetWindowText(ss[20]);						// 患者データ件数

	myEdit21->GetWindowText(ss[21]);						// 目標値HbA1c


	CComboBox* pcmb_bs = (CComboBox*)GetDlgItem(IDC_COMBO1);			// 食後の選択肢の取得
	if ((bsno = pcmb_bs->GetCurSel()) != CB_ERR) {
//		MessageBoxA(exh, (const char*)bslist[bsno], (const char*)"Check Message", MB_OK);
	}



	if (MessageBoxA(exh, (const char*)"患者情報を保存しますか？", (const char*)"Check Message", MB_OKCANCEL) == IDOK) {

		memcpy(idbuff, CStringA(ss[0]).GetBuffer(), sizeof(CStringA(ss[0]).GetBuffer()));						//	CString	→	char *
//		MessageBoxA(NULL, (const char*)idbuff, (const char*)"Debug Message", MB_OK);
		sprintf_s(fname, "./data/ID-%s.csv", (const char*)idbuff);



		if (error = fopen_s(&afp, (const char*)fname, "rt") == 0) {
			bsts = 0;
			memcpy(dm_status, "FALSE", 5);
			memcpy(ht_status, "FALSE", 5);
			memcpy(hc_status, "FALSE", 5);
			while (1) {
				memset(readbuffer, NULL, sizeof(readbuffer));
				if (fgets(readbuffer, sizeof(readbuffer), afp) == NULL)
					break;
				else {
					if ((memcmp(readbuffer, "TRUE", 4) == NULL) || (memcmp(readbuffer, "FALSE", 5) == NULL)) {
//						MessageBoxA(NULL, (const char*)readbuffer, (const char*)"Debug Message", MB_OK);
						switch (bsts++) {
						case 0:
							memset(dm_status, NULL, sizeof(dm_status));
							memcpy(dm_status, readbuffer, strlen(readbuffer)-1);
							break;
						case 1:
							memset(ht_status, NULL, sizeof(ht_status));
							memcpy(ht_status, readbuffer, strlen(readbuffer)-1);
							break;
						case 2:
							memset(hc_status, NULL, sizeof(hc_status));
							memcpy(hc_status, readbuffer, strlen(readbuffer)-1);
							break;
						}

					}

				}
			}
			fclose(afp);

		}

		if (error = fopen_s(&wfp, (const char*)fname, "wt") != 0)
			MessageBoxA(exh, (const char*)"登録に失敗しました。", (const char*)"Check Message", MB_OK);
		else {

//			if (error = fopen_s(&afp, (const char*)rirekifname, "at+") != 0)
//				MessageBoxA(exh, (const char*)"履歴ファイルの生成に失敗しました・", (const char*)"Check Message", MB_OK);
//			else {
				for (i = 1; i < 27; i++) {
					memcpy(&wtbl[i][0], CStringA(ss[i]).GetBuffer(), CStringA(ss[i]).GetLength());
					if (wtbl[i][CStringA(ss[i]).GetLength() - 1] != '\n') {
						wtbl[i][CStringA(ss[i]).GetLength()] = '\n';
						wtbl[i][CStringA(ss[i]).GetLength() + 1] = NULL;
						wtbl[i][CStringA(ss[i]).GetLength() + 1] = NULL;
					}
				}

/*
				sprintf_s(writebuffer, "%s%s%s%s\n%d\n%04d/%02d/%02d %02d:%02d\n\n\n\n%s%s%s%s%s%s%s\n\n%s%s%s%s%s%s%s%s%s",
					&wtbl[1][0], &wtbl[2][0], &wtbl[3][0], idbuff, xx, Year, Month, Days, Hour, Min,
					&wtbl[4][0], &wtbl[5][0], &wtbl[6][0], &wtbl[7][0], &wtbl[8][0], &wtbl[9][0], &wtbl[21][0],
					&wtbl[10][0], bslist[bsno], &wtbl[17],
					&wtbl[11][0], &wtbl[12][0], &wtbl[13][0], &wtbl[14][0], &wtbl[15][0], &wtbl[16][0]);
*/

				sprintf_s(writebuffer, "%s%s%s%s\n%s%s\n\n\n%s%s%s%s%s%s%s\n\n%s%s%s%s%s%s%s%s%s\n\n%s\n%s\n%s\n",
					&wtbl[1][0], &wtbl[2][0], &wtbl[3][0], idbuff, &wtbl[19][0], &wtbl[20][0],
					&wtbl[4][0], &wtbl[5][0], &wtbl[6][0], &wtbl[7][0], &wtbl[8][0], &wtbl[9][0], &wtbl[21][0],
					&wtbl[10][0], bslist[bsno], &wtbl[17],
					&wtbl[11][0], &wtbl[12][0], &wtbl[13][0], &wtbl[14][0], &wtbl[15][0], &wtbl[16][0],
					dm_status, ht_status, hc_status);

//					MessageBoxA(NULL, (const char*)writebuffer, (const char*)"Debug Message", MB_OK);


				fputs(writebuffer, wfp);

//				sprintf_s(writebuffer, "%s\n%d-%02d-%02d %02d:%02d:%02d\n%s%s%s\n\n\n\n\n\n%s%s%s%s%s%s\n\n\n%s%s%s%s%s%s%s%s%s\n-----END-----\n", idbuff, NowTM.GetYear(), NowTM.GetMonth(), NowTM.GetDay(), NowTM.GetHour(), NowTM.GetMinute(), NowTM.GetSecond(),
//					&wtbl[1][0], &wtbl[2][0], &wtbl[3][0],
//					&wtbl[4][0], &wtbl[5][0], &wtbl[6][0], &wtbl[7][0], &wtbl[8][0], &wtbl[9][0],
//					&wtbl[10][0], bslist[bsno], &wtbl[17],
//					&wtbl[11][0], &wtbl[12][0], &wtbl[13][0], &wtbl[14][0], &wtbl[15][0], &wtbl[16][0]);
//
//				fputs(writebuffer, afp);

				fclose(wfp);
//				fclose(afp);

				MessageBoxA(exh, (const char*)"患者情報の保存のみ実行しました。", (const char*)"Check Message", MB_OK);
//			}
		}

	}


}


void CseikatsuDlg::OnBnClickedCheck1()					// 新規患者登録処理
{

//	MessageBoxA(exh, (const char*)"現在、開発中です。", (const char*)"Check Message", MB_OK);

	CMFCButton* pMBtn_Edit = (CMFCButton*)GetDlgItem(IDC_CHECK1);
	CMFCButton* pMBtn_csv = (CMFCButton*)GetDlgItem(IDC_BUTTON6);			// CSVファイル出力ボタン


	CEdit* myEditName = (CEdit*)GetDlgItem(IDC_EDIT2);						// 患者氏名		編集無効
	CEdit* myEditBirth = (CEdit*)GetDlgItem(IDC_EDIT3);						// 患者生年月日	編集無効
	CEdit* myEditSei = (CEdit*)GetDlgItem(IDC_EDIT4);						// 患者性別		編集無効

	if (pMBtn_Edit->GetCheck() == BST_CHECKED) {
//		MessageBoxA(exh, (const char*)"編集可能", (const char*)"Check Message", MB_OK);
		myEditName->EnableWindow(TRUE);
		myEditBirth->EnableWindow(TRUE);
		myEditSei->EnableWindow(TRUE);

		pMBtn_csv->EnableWindow(TRUE);
	}
	else {
		myEditName->EnableWindow(FALSE);
		myEditBirth->EnableWindow(FALSE);
		myEditSei->EnableWindow(FALSE);

		pMBtn_csv->EnableWindow(FALSE);
	}

}


//	ss = L"/A C:\\療養計画書\\VisualStudio2022\\x64\\Release\\PDFデータ\\2024-04\\ID-1977(2024年04月01日)初回.pdf");

#include <string>

void CseikatsuDlg::OnBnClickedButton5()																// 直近の療養計画書のpdfを開く
{

	CString		datafname ;																			//

	CString		idno;																				//
	CEdit*		myEdit1 = (CEdit*)GetDlgItem(IDC_EDIT1);											// 患者ID

	CString		cnt;
	CEdit*		myEdit19 = (CEdit*)GetDlgItem(IDC_EDIT19);											// 患者データ件数

	CString		ss;
	CEdit*		myEdit20 = (CEdit*)GetDlgItem(IDC_EDIT20);											// 前回登録日


	myEdit1->GetWindowText(idno);																	// 患者ID取得
	myEdit19->GetWindowText(cnt);																	// 患者データ件数
	myEdit20->GetWindowText(ss);																	// 前回登録日取得


	const wchar_t* path = L"C:\\Program Files\\Adobe\\Acrobat DC\\Acrobat\\Acrobat.exe";			// Acrobat.exe path
//	const wchar_t* path = L"Acrobat.exe";															// Acrobat.exe path

//	CString pdffpath  = L"/A C:\\療養計画書\\VisualStudio2022\\x64\\Release\\PDFデータ\\";			// pdf-file path
	CString pdffpath  = L"/A .\\PDFデータ\\";														// pdf-file path
	CString	pdffname2 = L"初回.pdf\0" ;																// pdf-file name

	CString YY = ss.Mid(0, 4);																		// Year		先頭の0文字目から4文字
	CString MM = ss.Mid(5, 2);																		// Month	先頭の5文字目から2文字
	CString DD = ss.Mid(8, 2);																		// Days		先頭の8文字目から2文字

	CString pdfyymm = YY + L"-" + MM + L"\\ID-" ;													// YYYY-MM
	CString pdfymd = L"(" + YY + L"年" + MM + L"月" + DD + L"日)";									// (YYYY年MM月DD日)
	CString qty; // = cnt.Mid(0, 1);

	int hqty; 
	char rqty[8] = { NULL } ;
	memcpy(rqty, CStringA(cnt).GetBuffer(), sizeof(CStringA(cnt).GetBuffer()));					//	CString	→	char *
	hqty = atoi(rqty);
	sprintf_s(rqty, "%d", hqty - 1);
	qty = rqty;

	if ( atoi( CStringA(cnt.GetBuffer())) == 1 )													// pdf発行枚数
		pdffname2 = L"初回.pdf\0";
	else
		pdffname2 = L"継続" + qty + L"回目.pdf\0";

	datafname = pdffpath + pdfyymm + idno + pdfymd + pdffname2;										// pdf path + pdf file name

//	MessageBoxA(exh, (LPCSTR)CStringA(datafname).GetBuffer(), (const char*)"Check Message", MB_OK);

	std::wstring name = datafname.GetBuffer();
	const wchar_t* szName = name.c_str();


	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	if (CreateProcess( path, (LPWSTR)szName, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

}



void CseikatsuDlg::OnBnClickedButton6()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。

	CString		idbuff;
	CString		kname;
	CString		kbirth;
	CString		kseibetsu;

	FILE*		wfp;

	char	writebuff[256] = { NULL };
	char	fbuff[256] = { NULL };

	errno_t error;


	CMFCButton* pMBtn_Edit = (CMFCButton*)GetDlgItem(IDC_CHECK1);

	CEdit* myEditid = (CEdit*)GetDlgItem(IDC_EDIT1);						// 患者ID
	CEdit* myEditName = (CEdit*)GetDlgItem(IDC_EDIT2);						// 患者氏名		編集無効
	CEdit* myEditBirth = (CEdit*)GetDlgItem(IDC_EDIT3);						// 患者生年月日	編集無効
	CEdit* myEditSei = (CEdit*)GetDlgItem(IDC_EDIT4);						// 患者性別		編集無効

	myEditid->GetWindowText(idbuff);
	myEditName->GetWindowText(kname);
	myEditBirth->GetWindowText(kbirth);
	myEditSei->GetWindowText(kseibetsu);

	if (MessageBoxA(exh, (const char*)"新規患者情報を保存しますか？", (const char*)"Check Message", MB_OKCANCEL) == IDOK) {



		sprintf_s(fbuff, "./data/ID-%s.csv", CStringA(idbuff).GetBuffer());
		error = fopen_s(&wfp, fbuff, "wt");
		sprintf_s(writebuff, "%s\n%s\n%s\n%s\n0\n\n\n\n\n0\n0\n0\n0\n0\n0\n0\n\n\n未登録\n空腹時\n0\n0\n0\n0\n0\n0\n0\n", CStringA(kname).GetBuffer(), CStringA(kbirth).GetBuffer(), CStringA(kseibetsu).GetBuffer(), CStringA(idbuff).GetBuffer());
		fputs(writebuff, wfp);
		fclose(wfp);




		MessageBoxA(exh, (const char*)"保存しました。", (const char*)"Check Message", MB_OK);
	}

}
