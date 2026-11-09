//==================================================================================
// 
// マップスクリプト読み込みクラスのヘッダーファイル [mapScriptLoader.cpp]
// Author : TENMA SAITO
// 
//==================================================================================
//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "game/stage/mapScriptLoader.h"
#include "util/filestream.h"
#include "core/manager.h"
#include "render/texture.h"
#include "ui/timer.h"
#include "game/player/player.h"
#include "game/item/item.h"
#include "game/stage/stage.h"
#include "game/stage/wall.h"
#include "game/player/playerCamera.h"
#include "util/util.h"

//==================================================================================
// --- コンストラクタ ---
//==================================================================================
CMapScriptLoader::CMapScriptLoader()
{
}

//==================================================================================
// --- デストラクタ ---
//==================================================================================
CMapScriptLoader::~CMapScriptLoader()
{
}

//==================================================================================
// --- スクリプト読み込み処理 ---
//==================================================================================
HRESULT CMapScriptLoader::Load(std::string_view path)
{
	HRESULT hr = S_OK;		// 読み込み結果
	std::string line;		// 読み込んだ文字列
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	std::unique_ptr pFile = std::make_unique<CFileStream>();		// ファイルストリームへのポインタ
	std::vector<IParentable*> vpParentable;		// 親マトリックスの設定可能なオブジェクト
	CStage *pStage = nullptr;				// ステージへのポインタ

	if (pFile->Open(path, false) == false)
	{ // ファイルオープン失敗時
		hr = E_FAIL;
		return hr;
	}

	while (1)
	{ // SCRIPTキーワードを検索
		// 文字列を一列読み込み
		pFile->ReadString(line);
		if (pFile->FindString(line, "SCRIPT") == true)
		{ // 見つかった場合
			break;
		}
		else if (pFile->IsEoF() == true)
		{ // 見つからなかった場合
			hr = S_FALSE;
			return hr;
		}
	}

	while (1)
	{ // 各オブジェクトを配置
		// 文字列を一列読み込み
		pFile->ReadString(line);
		if (pFile->FindString(line, "END_SCRIPT") || pFile->IsEoF())
		{ // 読み込み終了
			break;
		}
		else if (pFile->FindString(line, "TIMERSET"))
		{ // タイマーの設置キーワードの場合
			std::string sTexturePath;		// テクスチャのパス
			Vector3 pos = VECTOR3_NULL;		// 位置
			Vector2 size = VECTOR2_NULL;	// サイズ
			int nTime = 0;					// 制限時間
			int nIdxTexture = INVALID_TEX_ID;	// テクスチャ無効値

			while (1)
			{ // プレイヤーの設置に必要な各情報を読み込み
				size_t find = 0U;		// 見つけた文字列の位置
				std::string_view after;	// コメント・タブスペース消去後の文字列

				// 文字列を一列読み込み
				pFile->ReadString(line);
				after = line;		// 文字列の参照を取得

				// コメント消去
				auto comment = line.find_last_of('#');
				if (comment != std::string_view::npos)
				{ // コメントが見つかった場合、そこを切り捨て
					line = line.substr(0U, comment);
				}

				while (1)
				{// タブスペース消去
					auto tab = std::ranges::find(line, '\t');
					if (tab != line.end())
					{ // タブスペースが見つかった場合、そこを切り捨て
						line.erase(tab);
					}
					else
					{ // 見つからなくなったら終了
						break;
					}
				}

				if (pFile->FindString(line, "END_TIMERSET"))
				{ // タイマーの情報が終了した場合、設置
					CTimer::Create(pos, size, Util::GetNumInt(nTime), nTime, pTexture->Register(sTexturePath));
					break;
				}
				else if (pFile->FindString(line, "POS = ", &find, true))
				{ // 位置の情報だった場合、読み込み
					pos = pFile->ToVector3(&line[find]);
				}
				else if (pFile->FindString(line, "SIZE = ", &find, true))
				{ // 位置の情報だった場合、読み込み
					size = pFile->ToVector2(&line[find]);
				}
				else if (pFile->FindString(line, "TIME = ", &find, true))
				{ // 制限時間の情報だった場合、読み込み
					nTime = pFile->ToInt(&line[find]);
				}
				else if (pFile->FindString(line, "TEXTURE_FILENAME = ", &find, true))
				{ // テクスチャパス情報だった場合、コメント消去後読み込み
					std::string_view filename = &line[find];		// 見つかった文字列
					sTexturePath = filename;		// モデルのパスを保存
				}
			}
		}
		else if (pFile->FindString(line, "PLAYERSET"))
		{ // プレイヤーの設置キーワードの場合
			std::string sModelPath;			// モデルのパス
			Vector3 pos = VECTOR3_NULL;		// 位置
			float fRadius = 0.0f;			// 当たり判定の半径

			while (1)
			{ // プレイヤーの設置に必要な各情報を読み込み
				size_t find = 0U;		// 見つけた文字列の位置
				std::string_view after;	// コメント・タブスペース消去後の文字列

				// 文字列を一列読み込み
				pFile->ReadString(line);
				after = line;		// 文字列の参照を取得

				// コメント消去
				auto comment = line.find_last_of('#');
				if (comment != std::string_view::npos)
				{ // コメントが見つかった場合、そこを切り捨て
					line = line.substr(0U, comment);
				}

				while (1)
				{// タブスペース消去
					auto tab = std::ranges::find(line, '\t');
					if (tab != line.end())
					{ // タブスペースが見つかった場合、そこを切り捨て
						line.erase(tab);
					}
					else
					{ // 見つからなくなったら終了
						break;
					}
				}

				if (pFile->FindString(line, "END_PLAYERSET"))
				{ // プレイヤーの情報が終了した場合、設置
					CPlayer *pPlayer = CPlayer::Create(sModelPath.c_str(), pos, fRadius);
					vpParentable.push_back(pPlayer);
					break;
				}
				else if (pFile->FindString(line, "POS = ", &find, true))
				{ // 位置の情報だった場合、読み込み
					pos = pFile->ToVector3(&line[find]);
				}
				else if (pFile->FindString(line, "RADIUS = ", &find, true))
				{ // 位置の情報だった場合、読み込み
					fRadius = pFile->ToFloat(&line[find]);
				}
				else if (pFile->FindString(line, "MODEL_FILENAME = ", &find, true))
				{ // モデルパス情報だった場合、コメント消去後読み込み
					std::string_view filename = &line[find];		// 見つかった文字列
					sModelPath = filename;		// モデルのパスを保存
				}
			}
		}
		else if (pFile->FindString(line, "FIELDSET"))
		{ // 床の設置キーワードの場合
			std::string sTexturePath;		// テクスチャのパス
			Vector3 pos = VECTOR3_NULL;		// 位置
			Vector2 size = VECTOR2_NULL;	// サイズ

			while (1)
			{ // 床の設置に必要な各情報を読み込み
				size_t find = 0U;		// 見つけた文字列の位置
				std::string_view after;	// コメント・タブスペース消去後の文字列

				// 文字列を一列読み込み
				pFile->ReadString(line);
				after = line;		// 文字列の参照を取得

				// コメント消去
				auto comment = line.find_last_of('#');
				if (comment != std::string_view::npos)
				{ // コメントが見つかった場合、そこを切り捨て
					line = line.substr(0U, comment);
				}

				while (1)
				{// タブスペース消去
					auto tab = std::ranges::find(line, '\t');
					if (tab != line.end())
					{ // タブスペースが見つかった場合、そこを切り捨て
						line.erase(tab);
					}
					else
					{ // 見つからなくなったら終了
						break;
					}
				}

				if (pFile->FindString(line, "END_FIELDSET"))
				{ // 床の情報が終了した場合、設置
					pStage = CStage::Create(pos, VECTOR3_NULL, size);
					pStage->BindTexture(pTexture->Register(sTexturePath));
					break;
				}
				else if (pFile->FindString(line, "POS = ", &find, true))
				{ // 位置の情報だった場合、読み込み
					pos = pFile->ToVector3(&line[find]);
				}
				else if (pFile->FindString(line, "SIZE = ", &find, true))
				{ // サイズの情報だった場合、読み込み
					size = pFile->ToVector2(&line[find]);
				}
				else if (pFile->FindString(line, "TEXTURE_FILENAME = ", &find, true))
				{ // テクスチャパス情報だった場合、コメント消去後読み込み
					std::string_view filename = &line[find];		// 見つかった文字列
					sTexturePath = filename;		// テクスチャのパスを保存
				}
			}
		}
		else if (pFile->FindString(line, "WALLSET"))
		{ // 壁の設置キーワードの場合
			std::string sTexturePath;		// テクスチャのパス
			Vector3 pos = VECTOR3_NULL;		// 位置
			Vector3 rot = VECTOR3_NULL;		// 角度
			Vector2 size = VECTOR2_NULL;	// サイズ

			while (1)
			{ // 壁の設置に必要な各情報を読み込み
				size_t find = 0U;		// 見つけた文字列の位置
				std::string_view after;	// コメント・タブスペース消去後の文字列

				// 文字列を一列読み込み
				pFile->ReadString(line);
				after = line;		// 文字列の参照を取得

				// コメント消去
				auto comment = line.find_last_of('#');
				if (comment != std::string_view::npos)
				{ // コメントが見つかった場合、そこを切り捨て
					line = line.substr(0U, comment);
				}

				while (1)
				{// タブスペース消去
					auto tab = std::ranges::find(line, '\t');
					if (tab != line.end())
					{ // タブスペースが見つかった場合、そこを切り捨て
						line.erase(tab);
					}
					else
					{ // 見つからなくなったら終了
						break;
					}
				}

				if (pFile->FindString(line, "END_WALLSET"))
				{ // 壁の情報が終了した場合、設置
					CWall *pWall = CWall::Create(pos, rot, size);
					pWall->BindTexture(pTexture->Register(sTexturePath));
					vpParentable.push_back(pWall);
					break;
				}
				else if (pFile->FindString(line, "POS = ", &find, true))
				{ // 位置の情報だった場合、読み込み
					pos = pFile->ToVector3(&line[find]);
				}
				else if (pFile->FindString(line, "ROT = ", &find, true))
				{ // 位置の情報だった場合、読み込み
					rot = pFile->ToVector3(&line[find]);
				}
				else if (pFile->FindString(line, "SIZE = ", &find, true))
				{ // サイズの情報だった場合、読み込み
					size = pFile->ToVector2(&line[find]);
				}
				else if (pFile->FindString(line, "TEXTURE_FILENAME = ", &find, true))
				{ // テクスチャパス情報だった場合、コメント消去後読み込み
					std::string_view filename = &line[find];		// 見つかった文字列
					sTexturePath = filename;		// テクスチャのパスを保存
				}
			}
		}
	}

	if (pStage != nullptr)
	{ // ステージがnullでなければ、各オブジェクトの親に設定
		for (auto &pParentable : vpParentable)
		{
			pParentable->SetParent(pStage->GetMatrix());
		}
	}

	// プレイヤーカメラも設置
	CPlayerCamera::Create(CObject::FindAnyObjectByType<CPlayer>());

	return hr;
}