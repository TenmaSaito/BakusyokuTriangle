//==================================================================================
// 
// ランキングのクライアントクラスのヘッダーファイル [rankingClient.h]
// Author : TENMA SAITO
// Date   : 2026/10/7
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "network/rankingClient.h"
#include "network/tcp_client.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CRankingClient::CRankingClient()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CRankingClient::~CRankingClient()
{
}

//==================================================================================
// --- 初期化処理 ---
//==================================================================================
HRESULT CRankingClient::Init(std::string_view sIPAddress, const u_short nPortID)
{ // 引数を保存
	m_sIPAddress = sIPAddress;
	m_nPortID = nPortID;

	// WSA初期化
	WSADATA wsaData = {};		// WSAデータ
	if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0)
	{ // 初期化失敗
		return E_FAIL;
	}

	return S_OK;
}

//==================================================================================
// --- 終了処理 ---
//==================================================================================
void CRankingClient::Uninit(void)
{ // WSA終了
	WSACleanup();
}

//==================================================================================
// --- スコア送信処理 ---
//==================================================================================
int CRankingClient::Send(const int nScore)
{
	std::unique_ptr pClient = std::make_unique<CTcpClient>();
	if (pClient == nullptr) return UNRANKING;			// インスタンス生成失敗

	// サーバーと接続
	int nResult = pClient->Init(m_sIPAddress.data(), m_nPortID);
	if(CLIENT_TCP_FAILED(nResult)) return UNRANKING;	// サーバーとの接続に失敗

	char aStr[sizeof(char) + sizeof(int)] = {};			// サーバーに送信する文字列
	int nScoreEndian = htonl(nScore);		// エンディアン変換後のスコア

	// メッセージを作成
	aStr[0] = static_cast<char>(COMMAND_TYPE::SET_RANKING);		// ランキングの送信を指定
	memcpy(&aStr[1], &nScoreEndian, sizeof(nScoreEndian));		// エンディアン変換後のスコアをバイトデータ化

	// メッセージを送信
	pClient->SendWithSize(aStr, sizeof(aStr));
	
	// 順位を受信
	int nByte = 0;	// サイズ
	char cRank;		// 順位
	nResult = pClient->RecvAll(&nByte, 1);
	nResult = pClient->RecvAll(&cRank, 1);
	if (CLIENT_TCP_FAILED(nResult))
	{ // インスタンス破棄
		SafeUniqueUninit(pClient);
		return UNRANKING;	// 順位の受信に失敗
	}

	// インスタンス破棄
	SafeUniqueUninit(pClient);

	// 順位を返す
	return static_cast<int>(cRank);
}

//==================================================================================
// --- スコア受信処理 ---
//==================================================================================
std::array<int, MAX_RANKING> CRankingClient::Get(void)
{
	std::array<int, MAX_RANKING> aScore = {};		// 受け取った順位
	std::unique_ptr pClient = std::make_unique<CTcpClient>();
	if (pClient == nullptr) return aScore;			// インスタンス生成失敗

	// サーバーと接続
	int nResult = pClient->Init(m_sIPAddress.data(), m_nPortID);
	if (CLIENT_TCP_FAILED(nResult)) return aScore;	// サーバーとの接続に失敗

	// メッセージを送信
	char cCommand = static_cast<char>(COMMAND_TYPE::GET_RANKING);	// ランキングの取得を指定
	pClient->SendWithSize(&cCommand, 1);

	// サイズを受信
	int nByte = 0;				// 受信予定サイズ
	nResult = pClient->RecvAll(&nByte, 1);
	if (CLIENT_TCP_FAILED(nResult))
	{ // サイズの受信に失敗
		// インスタンス破棄
		SafeUniqueUninit(pClient);
		return aScore;
	}

	// データを受信
	std::unique_ptr pByte = std::make_unique<char[]>(nByte);		// 受け取ったバイトデータ
	nResult = pClient->RecvAll(pByte.get(), nByte);
	if (CLIENT_TCP_FAILED(nResult))
	{ // 順位の受信に失敗
		// インスタンス破棄
		SafeUniqueUninit(pClient);
		return aScore;
	}

	// インスタンス破棄
	SafeUniqueUninit(pClient);

	// バイトデータをランキングに代入
	memcpy(&aScore[0], pByte.get(), nByte);

	for (auto &score : aScore)
	{ // 各スコアをエンディアン変換
		score = ntohl(score); 
	}

	// スコアを返す
	return aScore;
}