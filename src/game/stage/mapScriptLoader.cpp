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
#include "util/stringUtil.h"
#include "core/manager.h"
#include "render/texture.h"
#include "ui/timer.h"
#include "game/player/player.h"
#include "game/item/item.h"
#include "game/stage/stage.h"
#include "game/stage/wall.h"
#include "game/player/playerCamera.h"
#include "util/util.h"

namespace
{
	//==================================================================================
	// --- コメント・タブスペースを消去した文字列の一列読み込み処理 ---
	//==================================================================================
	std::optional<std::string> ReadCleanLine(CFileStream &file)
	{ // 文字列を一列読み込み、読み込めなかった場合は無効値を返す
		std::optional<std::string> line = file.ReadLine();
		if (line.has_value() == false) return std::nullopt;

		// コメント消去
		auto comment = line->find_last_of('#');
		if (comment != std::string::npos)
		{ // コメントが見つかった場合、そこから後ろを切り捨て
			line->erase(comment);
		}

		// タブスペース消去
		std::erase(*line, '\t');
		return line;
	}
}

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
	std::optional<std::string> line;		// 読み込んだ文字列
	CManager *pManager = CManager::GetInstance();			// マネージャへのポインタ
	CTexture *pTexture = CTexture::GetInstance();			// テクスチャへのポインタ
	std::unique_ptr pFile = std::make_unique<CFileStream>();		// ファイルストリームへのポインタ
	std::vector<IParentable*> vpParentable;		// 親マトリックスの設定可能なオブジェクト
	CStage *pStage = nullptr;				// ステージへのポインタ

	if (pFile->Open(path, CFileStream::MODE::READ) == false)
	{ // ファイルオープン失敗時
		hr = E_FAIL;
		return hr;
	}

	while (1)
	{ // SCRIPTキーワードを検索
		// 文字列を一列読み込み
		line = pFile->ReadLine();
		if (line.has_value() == false)
		{ // 見つからないままファイルが終わった場合
			hr = S_FALSE;
			return hr;
		}
		else if (StringUtil::Contains(*line, "SCRIPT") == true)
		{ // 見つかった場合
			break;
		}
	}

	while (1)
	{ // 各オブジェクトを配置
		// 文字列を一列読み込み
		line = pFile->ReadLine();
		if (line.has_value() == false || StringUtil::Contains(*line, "END_SCRIPT"))
		{ // 読み込み終了
			break;
		}
		else if (StringUtil::Contains(*line, "TIMERSET"))
		{ // タイマーの設置キーワードの場合
			std::string sTexturePath;		// テクスチャのパス
			Vector3 pos = VECTOR3_NULL;		// 位置
			Vector2 size = VECTOR2_NULL;	// サイズ
			int nTime = 0;					// 制限時間
			int nIdxTexture = IResourceable::INVALID_RESOURCE_ID;	// テクスチャ無効値

			while (1)
			{ // プレイヤーの設置に必要な各情報を読み込み
				// コメント・タブスペースを消去した文字列を一列読み込み
				std::optional<std::string> data = ReadCleanLine(*pFile);
				if (data.has_value() == false)
				{ // 終了キーワードが見つからないままファイルが終わった場合、中断
					break;
				}
				std::string &sLine = data.value();		// 読み込んだ文字列

				if (StringUtil::Contains(sLine, "END_TIMERSET"))
				{ // タイマーの情報が終了した場合、設置
					CTimer::Create(pos, size, Util::GetNumInt(nTime), nTime, pTexture->Register(sTexturePath));
					break;
				}
				else if (auto value = StringUtil::GetValue(sLine, "POS = "))
				{ // 位置の情報だった場合、読み込み
					pos = StringUtil::ToVector3(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "SIZE = "))
				{ // 位置の情報だった場合、読み込み
					size = StringUtil::ToVector2(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "TIME = "))
				{ // 制限時間の情報だった場合、読み込み
					nTime = StringUtil::ToInt(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "TEXTURE_FILENAME = "))
				{ // テクスチャパス情報だった場合、コメント消去後読み込み
					std::string_view filename = *value;		// 見つかった文字列
					sTexturePath = filename;		// モデルのパスを保存
				}
			}
		}
		else if (StringUtil::Contains(*line, "PLAYERSET"))
		{ // プレイヤーの設置キーワードの場合
			std::string sModelPath;			// モデルのパス
			Vector3 pos = VECTOR3_NULL;		// 位置
			float fRadius = 0.0f;			// 当たり判定の半径

			while (1)
			{ // プレイヤーの設置に必要な各情報を読み込み
				// コメント・タブスペースを消去した文字列を一列読み込み
				std::optional<std::string> data = ReadCleanLine(*pFile);
				if (data.has_value() == false)
				{ // 終了キーワードが見つからないままファイルが終わった場合、中断
					break;
				}
				std::string &sLine = data.value();		// 読み込んだ文字列

				if (StringUtil::Contains(sLine, "END_PLAYERSET"))
				{ // プレイヤーの情報が終了した場合、設置
					CPlayer *pPlayer = CPlayer::Create(sModelPath.c_str(), pos, fRadius);
					vpParentable.push_back(pPlayer);
					break;
				}
				else if (auto value = StringUtil::GetValue(sLine, "POS = "))
				{ // 位置の情報だった場合、読み込み
					pos = StringUtil::ToVector3(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "RADIUS = "))
				{ // 位置の情報だった場合、読み込み
					fRadius = StringUtil::ToFloat(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "MODEL_FILENAME = "))
				{ // モデルパス情報だった場合、コメント消去後読み込み
					std::string_view filename = *value;		// 見つかった文字列
					sModelPath = filename;		// モデルのパスを保存
				}
			}
		}
		else if (StringUtil::Contains(*line, "FIELDSET"))
		{ // 床の設置キーワードの場合
			std::string sTexturePath;		// テクスチャのパス
			Vector3 pos = VECTOR3_NULL;		// 位置
			Vector2 size = VECTOR2_NULL;	// サイズ

			while (1)
			{ // 床の設置に必要な各情報を読み込み
				// コメント・タブスペースを消去した文字列を一列読み込み
				std::optional<std::string> data = ReadCleanLine(*pFile);
				if (data.has_value() == false)
				{ // 終了キーワードが見つからないままファイルが終わった場合、中断
					break;
				}
				std::string &sLine = data.value();		// 読み込んだ文字列

				if (StringUtil::Contains(sLine, "END_FIELDSET"))
				{ // 床の情報が終了した場合、設置
					pStage = CStage::Create(pos, VECTOR3_NULL, size);
					pStage->BindTexture(pTexture->Register(sTexturePath));
					break;
				}
				else if (auto value = StringUtil::GetValue(sLine, "POS = "))
				{ // 位置の情報だった場合、読み込み
					pos = StringUtil::ToVector3(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "SIZE = "))
				{ // サイズの情報だった場合、読み込み
					size = StringUtil::ToVector2(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "TEXTURE_FILENAME = "))
				{ // テクスチャパス情報だった場合、コメント消去後読み込み
					std::string_view filename = *value;		// 見つかった文字列
					sTexturePath = filename;		// テクスチャのパスを保存
				}
			}
		}
		else if (StringUtil::Contains(*line, "WALLSET"))
		{ // 壁の設置キーワードの場合
			std::string sTexturePath;		// テクスチャのパス
			Vector3 pos = VECTOR3_NULL;		// 位置
			Vector3 rot = VECTOR3_NULL;		// 角度
			Vector2 size = VECTOR2_NULL;	// サイズ

			while (1)
			{ // 壁の設置に必要な各情報を読み込み
				// コメント・タブスペースを消去した文字列を一列読み込み
				std::optional<std::string> data = ReadCleanLine(*pFile);
				if (data.has_value() == false)
				{ // 終了キーワードが見つからないままファイルが終わった場合、中断
					break;
				}
				std::string &sLine = data.value();		// 読み込んだ文字列

				if (StringUtil::Contains(sLine, "END_WALLSET"))
				{ // 壁の情報が終了した場合、設置
					CWall *pWall = CWall::Create(pos, rot, size);
					pWall->BindTexture(pTexture->Register(sTexturePath));
					vpParentable.push_back(pWall);
					break;
				}
				else if (auto value = StringUtil::GetValue(sLine, "POS = "))
				{ // 位置の情報だった場合、読み込み
					pos = StringUtil::ToVector3(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "ROT = "))
				{ // 位置の情報だった場合、読み込み
					rot = StringUtil::ToVector3(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "SIZE = "))
				{ // サイズの情報だった場合、読み込み
					size = StringUtil::ToVector2(*value);
				}
				else if (auto value = StringUtil::GetValue(sLine, "TEXTURE_FILENAME = "))
				{ // テクスチャパス情報だった場合、コメント消去後読み込み
					std::string_view filename = *value;		// 見つかった文字列
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