
// seikatsu.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CseikatsuApp:
// このクラスの実装については、seikatsu.cpp を参照してください
//

class CseikatsuApp : public CWinApp
{
public:
	CseikatsuApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CseikatsuApp theApp;
