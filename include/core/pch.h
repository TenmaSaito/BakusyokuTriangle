//==================================================================================
// 
// プリコンパイル済みヘッダーファイル [pch.h]
// Author : TENMA SAITO
// 
//==================================================================================
#ifndef _PCH_H_
#define _PCH_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#define _WINSOCK_DEPRECATED_NO_WARNINGS		// WinSock2の警告対処マクロ
#include <WinSock2.h>						// ランキング接続に必要
#include <Windows.h>						// Windowsシステムに必要
#include <imm.h>							// IME無効化に必要
#include <assert.h>							// アサーションに必要
#include <time.h>							// 時刻取得に必要
#include <array>							// 固定長配列
#include <vector>							// 可変長配列
#include <list>								// リスト構造
#include <memory>							// スマートポインタ
#include <algorithm>						// アルゴリズム
#include <string>							// 文字列
#include <string_view>						// 文字列参照
#include <span>								// 配列参照
#include "d3dx9.h"							// 描画処理に必要
#define DIRECTINPUT_VERSION		(0x0800)	// dinputの警告対処マクロ
#include "dinput.h"							// 入力処理に必要(先に上のマクロを定義する)
#include "xinput.h"							// ジョイパッド処理に必要
#include "xaudio2.h"						// サウンド処理に必要

//**********************************************************************************
// *** ライブラリのリンク ***
//**********************************************************************************
#pragma comment(lib, "ws2_32.lib")			// winsockへのリンカー設定
#pragma comment(lib, "winmm.lib")			// システムの時刻取得・サーバー接続に必要
#pragma comment(lib, "imm32.lib")			// IME無効化に必要
#pragma comment(lib, "d3d9.lib")			// 描画処理に必要
#pragma comment(lib, "d3dx9.lib")			// d3d9.libの拡張ライブラリ
#pragma comment(lib, "dxguid.lib")			// DirectXコンポーネント(部品)使用に必要
#pragma comment(lib, "dinput8.lib")			// 入力処理に必要
#pragma comment(lib, "xinput.lib")			// ジョイパッド処理に必要
#pragma comment(lib, "xaudio2.lib")			// 3Dオーディオに必要

//**********************************************************************************
// *** エイリアス宣言 ***
//**********************************************************************************
using Vector2 = D3DXVECTOR2;
using Vector3 = D3DXVECTOR3;
using Vector4 = D3DXVECTOR4;
using Color = D3DXCOLOR;
using Quaternion = D3DXQUATERNION;
using Matrix = D3DXMATRIX;
#include "math/constVector.h"		// constexpr対応のベクトル・色

//**********************************************************************************
// *** 定数宣言 ***
//**********************************************************************************
constexpr int SCREEN_WIDTH = 1280;			// ウィンドウの横幅
constexpr int SCREEN_HEIGHT = 720;			// ウィンドウの横幅
constexpr float PI = D3DX_PI;				// π
constexpr float DOUBLE_PI = PI * 2.0f;		// 2π
constexpr float HALF_PI = PI * 0.5f;		// 1/2π
constexpr float QUARTER_PI = PI * 0.25f;	// 1/4π
constexpr int DEFAULT_VERTEX_NUM = 4;		// 板ポリのデフォルトの頂点数
constexpr int FVF_VERTEX_2D = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;					// 2D頂点フォーマット
constexpr int FVF_VERTEX_3D = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;	// 3D頂点フォーマット
constexpr int FVF_VERTEX_3D_MULTI = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 | D3DFVF_TEX2;	// マルチテクスチャ3D頂点フォーマット
inline constexpr ConstVector2 VECTOR2_NULL = ConstVector2(0.0f, 0.0f);			// Vector2の0クリア
inline constexpr ConstVector2 VECTOR2_ONE = ConstVector2(1.0f, 1.0f);				// Vector2の1クリア
inline constexpr ConstVector3 VECTOR3_NULL = ConstVector3(0.0f, 0.0f, 0.0f);		// Vector3の0クリア
inline constexpr ConstVector3 VECTOR3_ONE = ConstVector3(1.0f, 1.0f, 1.0f);		// Vector3の1クリア
inline constexpr ConstColor COLOR_NULL = ConstColor(0.0f, 0.0f, 0.0f, 0.0f);		// Colorの0クリア
inline constexpr ConstColor COLOR_ONE = ConstColor(1.0f, 1.0f, 1.0f, 1.0f);		// Colorの1クリア
inline constexpr ConstVector2 SCREEN_SIZE = ConstVector2(SCREEN_WIDTH, SCREEN_HEIGHT);			// ウィンドウのサイズ
inline constexpr ConstVector3 SCREEN_MIDDLE = ConstVector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f);	// ウィンドウの真ん中
constexpr int DECIMAL_BASE = 10;			// 10進数の基数
constexpr float DECIMAL_BASE_F = static_cast<float>(DECIMAL_BASE);	// 10進数の基数 (float)
constexpr int HEX_BASE = 16;				// 16進数の基数
constexpr DWORD MILLISECONDS_PER_SECOND = 1000;	// 1秒あたりのミリ秒
constexpr DWORD DEFAULT_ALPHA_REF = 30;	// αテストの基準値
inline constexpr ConstVector3 SHADOW_PLANE_POS = ConstVector3(0.0f, 0.1f, 0.0f);	// 影を落とす平面の位置 (床とのZファイティング防止)
inline constexpr ConstColor SHADOW_COLOR = ConstColor(0.0f, 0.0f, 0.0f, 0.8f);		// 影の色

//**********************************************************************************
// *** 頂点情報(2D)の構造体を定義 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// 頂点情報(x,y,z)
	float rhw;				// 座標変換用係数(1.0fで固定)
	D3DCOLOR col;			// 頂点カラー(R,G,B,a)
	D3DXVECTOR2 tex;		// テクスチャ座標(x,y)
} VERTEX_2D;

//**********************************************************************************
// *** 頂点情報(3D)の構造体を定義 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// 頂点情報(x,y,z)
	D3DXVECTOR3 nor;		// 法線ベクトル
	D3DCOLOR col;			// 頂点カラー(R,G,B,a)
	D3DXVECTOR2 tex;		// テクスチャ座標(x,y)
} VERTEX_3D;

//**********************************************************************************
// *** 頂点情報(3D MULTI)の構造体を定義 ***
//**********************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// 頂点情報(x,y,z)
	D3DXVECTOR3 nor;		// 法線ベクトル
	D3DCOLOR col;			// 頂点カラー(R,G,B,a)
	D3DXVECTOR2 tex;		// テクスチャ座標(x,y)
	D3DXVECTOR2 texM;		// マルチテクスチャ座標(x,y)
} VERTEX_3D_MULTI;
#endif