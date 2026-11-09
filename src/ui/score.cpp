//==================================================================================
// 
// スコアクラスのソースファイル [score.cpp]
// Author : TENMA SAITO
// Date   : 2026/5/18
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "ui/score.h"
#include "object/2d/number.h"
#include "core/manager.h"
#include "render/renderer.h"

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
namespace
{
	constexpr int SCORE_ADD_STEP = 50;		// 1フレームで加算するスコア
}

//**********************************************************************************
// *** マクロ定義 ***
//**********************************************************************************
constexpr ConstVector3 INIT_POS = VECTOR3_NULL;		// 初期位置
constexpr ConstVector2 INIT_SIZE = ConstVector2(50, 65);	// 初期サイズ
constexpr int INIT_SCORE = 0;					// 初期スコア

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CScore *CScore::Create(const int nIdxTexture, const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber)
{
	CScore *pScore;		// 生成したスコアへのポインタ
	// スコアを生成
	pScore = new CScore;
	if (pScore == nullptr)
	{ // 生成失敗
		return nullptr;
	}

	// スコアを初期化
	pScore->Init(nIdxTexture, pos, size, nNumNumber);

	return pScore;
}

//==================================================================================
// --- 生成処理 ---
//==================================================================================
CScore *CScore::Create(const int nIdxTexture, const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber, const int nScore)
{
	CScore *pScore;		// 生成したスコアへのポインタ

	// スコアを生成
	pScore = new CScore;
	if (pScore == nullptr)
	{ // 生成失敗
		return nullptr;
	}

	// スコアを初期化
	pScore->Init(nIdxTexture, pos, size, nNumNumber, nScore);

	return pScore;
}

//==================================================================================
// --- 検索処理 ---
//==================================================================================
CScore *CScore::Find(void)
{
	return FindAnyObjectByType<CScore>(SCORE_PRIORITY);
}

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CScore::CScore(const int nPriority) : CObject(nPriority)
{ // タイプを指定
	SetType(TYPE::SCORE);
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CScore::~CScore()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CScore::Init(const int nIdxTexture, const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber)
{
	D3DXVECTOR2 numSize;	// 数値1個当たりのサイズ

	// 引数を保存
	m_nScore = INIT_SCORE;
	m_nNumScore = nNumNumber;

	if (m_nNumScore > MAX_SCORENUM)
	{ // スコアの最大桁数以上の場合
		// 桁数を最大値に変更
		m_nNumScore = MAX_SCORENUM;
	}

	// 数値のサイズを計算
	numSize.x = size.x / static_cast<float>(nNumNumber);
	numSize.y = size.y;

	// 数値オブジェクトを作成
	for (int nCntNumber = 0; nCntNumber < m_nNumScore; nCntNumber++)
	{
		CNumber *pNumber = nullptr;		// 生成した数値オブジェクトへのポインタ
		D3DXVECTOR3 posCreate;			// 生成位置

		// 生成位置を計算
		posCreate.x = pos.x + (numSize.x * nCntNumber) + (numSize.x * 0.5f);
		posCreate.y = pos.y + (numSize.y * 0.5f);

		// 数値オブジェクトを生成
		pNumber = CNumber::Create(posCreate, numSize, 0);
		if (pNumber != NULL)
		{ // 生成成功時
			// テクスチャ割り当て
			pNumber->BindTexture(nIdxTexture);

			// 生成したポインタを保存 (CObjectによって解放されるためdeleteは禁止)
			m_apNumber[nCntNumber] = pNumber;
		}
		else
		{ // 生成失敗
			return S_FALSE;
		}
	}

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CScore::Init(const int nIdxTexture, const D3DXVECTOR3 &pos, const D3DXVECTOR2 &size, const int nNumNumber, const int nScore)
{
	D3DXVECTOR2 numSize;	// 数値1個当たりのサイズ
	int *pNumValue;			// 各桁の数値

	// 引数を保存
	m_nScore = nScore;
	m_nNumScore = nNumNumber;

	if (nNumNumber > MAX_SCORENUM)
	{ // スコアの最大桁数以上の場合
		// 桁数を最大値に変更
		m_nNumScore = MAX_SCORENUM;
	}

	// 数値のサイズを計算
	numSize.x = size.x / static_cast<float>(nNumNumber);
	numSize.y = size.y;

	// 桁数分だけメモリを確保
	pNumValue = new int[m_nNumScore];

	for (int nCntScore = 0; nCntScore < m_nNumScore; nCntScore++)
	{ // 各桁の数値を求める
		pNumValue[nCntScore] = nScore % (int)powf(DECIMAL_BASE_F, (float)(m_nNumScore - nCntScore)) / (int)powf(DECIMAL_BASE_F, (float)(m_nNumScore - nCntScore) - 1.0f);
	}

	// 数値オブジェクトを作成
	for (int nCntNumber = 0; nCntNumber < m_nNumScore; nCntNumber++)
	{
		CNumber* pNumber = nullptr;		// 生成した数値オブジェクトへのポインタ
		D3DXVECTOR3 posCreate;			// 生成位置

		// 生成位置(X座標)を修正
		posCreate.x = pos.x + (numSize.x * nCntNumber) + (numSize.x * 0.5f);
		posCreate.y = pos.y + (numSize.y * 0.5f);

		// 数値オブジェクトを生成
		pNumber = CNumber::Create(posCreate, numSize, pNumValue[nCntNumber]);
		if (pNumber != NULL)
		{ // 生成成功時
			// テクスチャ割り当て
			pNumber->BindTexture(nIdxTexture);

			// 生成したポインタを保存 (CObjectによって解放されるためdeleteは禁止)
			m_apNumber[nCntNumber] = pNumber;
		}
	}

	if (pNumValue != nullptr)
	{ // メモリ解放
		delete[] pNumValue;
		pNumValue = nullptr;
	}

	// 初期化結果を返す
	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CScore::Uninit(void)
{
	for (int nCntNumber = 0; nCntNumber < MAX_SCORENUM; nCntNumber++)
	{ // スコアが生成した数値オブジェクトを破棄
		if (m_apNumber[nCntNumber] != nullptr)
		{ // NULLではなかった場合、破棄 + 終了処理
			m_apNumber[nCntNumber]->Uninit();
			delete m_apNumber[nCntNumber];
			m_apNumber[nCntNumber] = nullptr;
		}
	}

	// 親クラスの終了処理
	CObject::Release();
}

//==================================================================================
// --- 更新処理 ---
//==================================================================================
void CScore::Update(void)
{
	if (m_nAdditionalScore > 0)
	{ // 追加予定スコアがまだ残っていたら5足す
		SetScore(m_nScore + SCORE_ADD_STEP);
		m_nAdditionalScore -= SCORE_ADD_STEP;
		if (m_nAdditionalScore < 0)
		{ // 0未満になった時、スコア修正
			SetScore(m_nScore + m_nAdditionalScore);
		}
	}

	for (int nCntNumber = 0; nCntNumber < MAX_SCORENUM; nCntNumber++)
	{ // スコアが生成した数値オブジェクトを更新
		if (m_apNumber[nCntNumber] != nullptr)
		{ // NULLではなかった場合、破棄 + 終了処理
			m_apNumber[nCntNumber]->Update();
		}
	}
}

//==================================================================================
// --- 描画処理 ---
//==================================================================================
void CScore::Draw(void)
{
	for (int nCntNumber = 0; nCntNumber < MAX_SCORENUM; nCntNumber++)
	{ // スコアが生成した数値オブジェクトを更新
		if (m_apNumber[nCntNumber] != nullptr)
		{ // NULLではなかった場合、破棄 + 終了処理
			m_apNumber[nCntNumber]->Draw();
		}
	}
}

//==================================================================================
// --- スコア設定処理 ---
//==================================================================================
void CScore::SetScore(const int nScore)
{
	int *pNumValue;			// 各桁の数値

	// スコアを加算
	m_nScore = nScore;

	// 桁数分だけメモリを確保
	pNumValue = new int[m_nNumScore];

	for (int nCntScore = 0; nCntScore < m_nNumScore; nCntScore++)
	{ // 各桁の数値を求める
		pNumValue[nCntScore] = m_nScore % (int)powf(DECIMAL_BASE_F, (float)(m_nNumScore - nCntScore)) / (int)powf(DECIMAL_BASE_F, (float)(m_nNumScore - nCntScore) - 1.0f);
	}

	for (int nCntScore = 0; nCntScore < m_nNumScore; nCntScore++)
	{ // 各桁の数値オブジェクトに適用
		if (m_apNumber[nCntScore] != nullptr)
		{ // NULLじゃなければ
			// 値を変更
			m_apNumber[nCntScore]->SetNumber(pNumValue[nCntScore]);
		}
	}

	if (pNumValue != nullptr)
	{ // メモリ解放
		delete[] pNumValue;
		pNumValue = nullptr;
	}
}

//==================================================================================
// --- スコア追加処理 ---
//==================================================================================
void CScore::AddScore(const int nValue)
{ // 追加予定スコアに追加
	m_nAdditionalScore += nValue;
}

//==================================================================================
// --- スコア取得処理 ---
//==================================================================================
int CScore::GetScore(void) const
{
	return m_nScore;
}