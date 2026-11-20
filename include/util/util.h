//==================================================================================
// 
// ユーティリティ関数をまとめたヘッダーファイル [vec2math.h]
// Author : TENMA SAITO
// Date   : 2026/6/22
// 
//==================================================================================
#ifndef _UTIL_H_
#define _UTIL_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include <iostream>
#include <sstream>
#include <type_traits>

//**********************************************************************************
// *** ユーティリティ関数関連名前空間 ***
//**********************************************************************************
namespace Util
{
	int FindString(const char **ppStr, const int nNumString, const char *pFind);
	float FixedRotation(const float fRadian);
	float Lerp(const float fStart, const float fEnd, const float ft);
	void SetStringSetting(const int nNumPrecision);

	//******************************************************************************
	// *** 引数の桁数取得処理 ***
	//******************************************************************************
	constexpr int GetNumInt(const int nValue)
	{ // 引数の桁数取得処理
		int nValueWork = nValue;		// 計算用変数
		int nNumValue = 1;				// 引数の桁数

		while (nValueWork / DECIMAL_BASE > 0)
		{ // 10で割った値が0以下の場合ループ終了
			nNumValue++;
			nValueWork /= DECIMAL_BASE;
		}

		return nNumValue;
	}

	//******************************************************************************
	// *** 型が指定したうちのどれかであることを要求するコンセプト ***
	//******************************************************************************
	template<class T, class... Types>
	concept AnyOf = (std::same_as<T, Types> || ...);

	//******************************************************************************
	// *** Initによる初期化が可能であることを要求するコンセプト ***
	//******************************************************************************
	template<class T, typename... Args>
	concept HasInit = requires(T & t, Args... args)
	{
		{ t.Init(args...) } -> AnyOf<HRESULT, bool, void>;
	};

	//******************************************************************************
	// *** Uninitによる終了(破棄)が可能であることを要求するコンセプト ***
	//******************************************************************************
	template<class T, typename... Args>
	concept HasUninit = requires(T &t)
	{
		{ t.Uninit() } -> std::same_as<void>;
	};

	//******************************************************************************
	// *** Init, Uninitが可能であることを要求するコンセプト ***
	//******************************************************************************
	template<class T, typename... Args>
	concept Createable = HasInit<T, Args...> || HasUninit<T>;

	//******************************************************************************
	// *** 初期化処理 ***
	//******************************************************************************
	template<Createable T, typename... Args>
	T *Create(Args... args)
	{
		T *pCreate = new T;
		using ReturnType = decltype(pCreate->Init(args...));

		if constexpr (std::is_same_v<ReturnType, HRESULT>)
		{ // 返り値の型がHRESULTだった場合
			if (pCreate != nullptr && FAILED(pCreate->Init(args...)))
			{ // 初期化に失敗した場合
				pCreate->Uninit();
				return nullptr;
			}
		}
		else if constexpr(std::is_same_v<ReturnType, bool>)
		{ // 返り値の型がboolだった場合
			if (pCreate != nullptr && pCreate->Init(args...) == false)
			{ // 初期化に失敗した場合
				pCreate->Uninit();
				return nullptr;
			}
		}
		else
		{ // 返り値がない場合
			if (pCreate == nullptr) return nullptr;
			pCreate->Init(args...);
		}

		return pCreate;
	}
}
#endif