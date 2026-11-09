//==================================================================================
// 
// サウンドクラスのヘッダーファイル [sound.h]
// Author : TENMA SAITO
// Date   : 2026/5/17
// 
//==================================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
#define ENABLE_SOUND		// サウンド有効マクロ

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr float SOUND_VOLUME = 0.2f;	// 音量

//**********************************************************************************
// *** 前方宣言 ***
//**********************************************************************************
class CSoundCallbacker;		// XAudio2のコールバッククラス

//**********************************************************************************
// *** サウンドクラス ***
//**********************************************************************************
class CSound
{
public:
	//******************************************************************************
	// *** サウンドの種類 ***
	//******************************************************************************
	enum class LABEL
	{
		BGM_TITLE = 0,	// タイトル画面のBGM
		BGM_INGAME,		// ゲーム画面のBGM
		BGM_RESULT,		// リザルト画面のBGM
		SE_BITE,		// 咀嚼音
		SE_SUCCESS,	// 成功音
		SE_FAIL,		// 失敗音
		MAX
	};

	// 有効値かどうかの判定
	static constexpr bool IsValid(const LABEL value) noexcept
	{
		return EnumUtil::InRange(value, LABEL::BGM_TITLE, LABEL::MAX);
	}

	//*****************************************************************************
	// *** サウンド情報 ***
	//*****************************************************************************
	typedef struct
	{
		const char* pFilename;		// ファイル名
		int nCntLoop;				// ループカウント
	} INFO;

	CSound();
	~CSound();

	HRESULT Init(const HWND hWnd);
	void Uninit(void);
	void Play(const LABEL label);
	void Stop(const LABEL label);
	void Stop(void);
	void Pause(const bool bPause, const LABEL label);
	void Pause(const bool bPause);
	void SetPitch(const LABEL label, const float fPitch = 1.0f);
	bool GetPlay(const LABEL label) const;
	HRESULT GetError(const LABEL label) const;

private:
	HRESULT CheckChunk(const HANDLE hFile, const DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
	HRESULT ReadChunkData(const HANDLE hFile, void *pBuffer, const DWORD dwBuffersize, const DWORD dwBufferoffset);

	IXAudio2 *m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;			// マスターボイス
	IXAudio2SourceVoice *m_apSourceVoice[EnumUtil::ToIndex(LABEL::MAX)];	// ソースボイス
	CSoundCallbacker *m_apCallback[EnumUtil::ToIndex(LABEL::MAX)];			// コールバック
	BYTE *m_apDataAudio[EnumUtil::ToIndex(LABEL::MAX)];						// オーディオデータ
	DWORD m_aSizeAudio[EnumUtil::ToIndex(LABEL::MAX)];						// オーディオデータサイズ
	static const INFO m_aInfo[EnumUtil::ToIndex(LABEL::MAX)];				// サウンド情報
};
#endif