//==================================================================================
// 
// Colorの色関連をまとめたヘッダーファイル [color.h]
// Author : TENMA SAITO
// Date   : 2026/5/16
// 
//==================================================================================
#ifndef _COLOR_H_
#define _COLOR_H_

//**********************************************************************************
// *** インクルードファイル ***
//**********************************************************************************
#include "core/main.h"
#include "util/enumUtil.h"

//**********************************************************************************
// *** 色関連名前空間 ***
//**********************************************************************************
namespace Colors
{
	//******************************************************************************
	// *** 色の種類 ***
	//******************************************************************************
	enum class COLOR
	{
		ALICEBLUE,						// アリスブルー（薄い水色）
		ANTIQUEWHITE,					// アンティークホワイト（くすんだ真珠色）
		AQUA,							// アクア（水色）
		AQUAMARINE,						// アクアマリン（藍玉色）
		AZURE,							// アジュール（薄い空色）
		BEIGE,							// ベージュ
		BISQUE,							// ビスキー（薄い茶色）
		BLACK,							// 黒
		BLANCHEDALMOND,					// ブランシェドアマンド（薄い杏色）
		BLUE,							// 青
		BLUEVIOLET,						// ブルーバイオレット（青紫）
		BROWN,							// 茶色
		BURLYWOOD,						// バーリーウッド（濃いベージュ）
		CADETBLUE,						// カデットブルー（くすんだ青）
		CHARTREUSE,						// シャルトルーズ（黄緑）
		CHOCOLATE,						// チョコレート
		CORAL,							// コーラル（珊瑚色）
		CORNFLOWERBLUE,					// コーンフラワーブルー（矢車菊の青）
		CORNSILK,						// コーンシルク（トウモロコシのひげのようなくすんだ白）
		CRIMSON,						// クリムゾン（深紅）
		CYAN,							// シアン（水色）
		DARKBLUE,						// ダークブルー（濃い青）
		DARKCYAN,						// ダークシアン（濃い水色）
		DARKGOLDENROD,					// ダークゴールデンロド（濃い金紅花色）
		DARKGRAY,						// ダークグレー（深灰色）
		DARKGREEN,						// ダークグリーン（深緑）
		DARKKHAKI,						// ダークカーキ
		DARKMAGENTA,					// ダークマゼンタ（濃い赤紫）
		DARKOLIVEGREEN,					// ダークオリーブグリーン（暗オリーブ色）
		DARKORANGE,						// ダークオレンジ
		DARKORCHID,						// ダークオーキッド（濃い蘭色）
		DARKRED,						// ダークレッド（濃い赤）
		DARKSALMON,						// ダークサーモン
		DARKSEAGREEN,					// ダークシーグリーン（くすんだ薄緑）
		DARKSLATEBLUE,					// ダークスレートブルー（暗青灰色）
		DARKSLATEGRAY,					// ダークスレートグレー（暗石板色）
		DARKTURQUOISE,					// ダークターコイズ（濃いトルコ石色）
		DARKVIOLET,						// ダークバイオレット（濃い紫）
		DEEPPINK,						// ディープピンク
		DEEPSKYBLUE,					// ディープスカイブルー
		DIMGRAY,						// ディムグレー（薄暗い灰色）
		DODGERBLUE,						// ドジャーブルー（明るい青）
		FIREBRICK,						// ファイアブリック（煉瓦色）
		FLORALWHITE,					// フローラルホワイト（花の白）
		FORESTGREEN,					// フォレストグリーン（深い森の緑）
		FUCHSIA,						// フクシャ（鮮やかな赤紫）
		GAINSBORO,						// ゲインズボロ（明るい灰色）
		GHOSTWHITE,						// ゴーストホワイト（幽霊の白）
		GOLD,							// 金色
		GOLDENROD,						// ゴールデンロド（金紅花色）
		GRAY,							// 灰色
		GREEN,							// 緑
		GREENYELLOW,					// グリーンイエロー（黄緑）
		HONEYDEW,						// ハニーデュー（甘露蜜の白）
		HOTPINK,						// ホットピンク（鮮やかなピンク）
		INDIANRED,						// インディアンレッド（インド赤）
		INDIGO,							// インディゴ（藍色）
		IVORY,							// アイボリー（象牙色）
		KHAKI,							// カーキ
		LAVENDER,						// ラベンダー
		LAVENDERBLUSH,					// ラベンダーブラッシュ（薄いラベンダーピンク）
		LAWNGREEN,						// ローングリーン（芝生の緑）
		LEMONCHIFFON,					// レモンシフォン
		LIGHTBLUE,						// ライトブルー（薄い青）
		LIGHTCORAL,						// ライトコーラル（薄い珊瑚色）
		LIGHTCYAN,						// ライトシアン（薄い水色）
		LIGHTGOLDENRODYELLOW,			// ライトゴールデンロドイエロー（薄い金紅花黄）
		LIGHTGREEN,						// ライトグリーン（薄い緑）
		LIGHTGRAY,						// ライトグレー（薄い灰色）
		LIGHTPINK,						// ライトピンク（薄いピンク）
		LIGHTSALMON,					// ライトサーモン（薄い鮭色）
		LIGHTSEAGREEN,					// ライトシーグリーン（明るい海洋緑）
		LIGHTSKYBLUE,					// ライトスカイブルー（明るい空色）
		LIGHTSLATEGRAY,					// ライトスレートグレー（明るい石板色）
		LIGHTSTEELBLUE,					// ライトスチールブルー（明るい鋼色）
		LIGHTYELLOW,					// ライトイエロー（薄い黄色）
		LIME,							// ライム（鮮やかな黄緑）
		LIMEGREEN,						// ライムグリーン
		LINEN,							// リネン（亜麻色）
		MAGENTA,						// マゼンタ（赤紫）
		MAROON,							// マルーン（栗色）
		MEDIUMAQUAMARINE,				// ミディアムアクアマリン
		MEDIUMBLUE,						// ミディアムブルー
		MEDIUMORCHID,					// ミディアムオーキッド（中間の蘭色）
		MEDIUMPURPLE,					// ミディアムパープル（中間の紫）
		MEDIUMSEAGREEN,					// ミディアムシーグリーン（中間の海洋緑）
		MEDIUMSLATEBLUE,				// ミディアムスレートブルー（中間の石板青）
		MEDIUMSPRINGGREEN,				// ミディアムスプリンググリーン
		MEDIUMTURQUOISE,				// ミディアムターコイズ（中間のトルコ石色）
		MEDIUMVIOLETRED,				// ミディアムバイオレットレッド（中間の赤紫）
		MIDNIGHTBLUE,					// ミッドナイトブルー（夜が更けた真夜中の濃い青）
		MINTCREAM,						// ミントクリーム
		MISTYROSE,						// ミスティローズ（霧がかったバラ色）
		MOCCASIN,						// モカシン（淡い黄茶色）
		NAVAJOWHITE,					// ナバホホワイト
		NAVY,							// ネイビー（紺色）
		OLDLACE,						// オールドレース（くすんだ白）
		OLIVE,							// オリーブ
		OLIVEDRAB,						// オリーブドラブ（暗い黄緑）
		ORANGE,							// オレンジ
		ORANGERED,						// オレンジレッド（朱色）
		ORCHID,							// オーキッド（蘭色）
		PALEGOLDENROD,					// ペールゴールデンロド（淡い金紅花色）
		PALEGREEN,						// ペールグリーン（淡い緑）
		PALETURQUOISE,					// ペールターコイズ（淡いトルコ石色）
		PALEVIOLETRED,					// ペールバイオレットレッド（淡い赤紫）
		PAPAYAWHIP,						// パパイヤホイップ
		PEACHPUFF,						// ピーチパフ（桃のうぶ毛のような色）
		PERU,							// ペルー（ペルー茶色）
		PINK,							// ピンク
		PLUM,							// プラム（西洋すもも色）
		POWDERBLUE,						// パウダーブルー（おしろいの青）
		PURPLE,							// 紫
		RED,							// 赤
		ROSYBROWN,						// ロージーブラン（バラ色の茶色）
		ROYALBLUE,						// ロイヤルブルー（英国王室の青）
		SADDLEBROWN,					// サドルブラウン（革の鞍の茶色）
		SALMON,							// サーモン（鮭色）
		SANDYBROWN,						// サンディブラウン（砂茶色）
		SEAGREEN,						// シーグリーン（海洋緑）
		SEASHELL,						// シーシェル（貝殻の白）
		SIENNA,							// シエナ（黄褐色）
		SILVER,							// シルバー（銀色）
		SKYBLUE,						// スカイブルー（空色）
		SLATEBLUE,						// スレートブルー（石板の青）
		SLATEGRAY,						// スレートグレー（石板の灰色）
		SNOW,							// スノー（雪の白）
		SPRINGGREEN,					// スプリンググリーン（新緑の緑）
		STEELBLUE,						// スチールブルー（鋼色）
		TAN, 							// タン（日焼け色）
		TEAL,							// ティール（鴨の羽の青緑）
		THISTLE,						// シッスル（あざみ色）
		TOMATO,							// トマト（トマトの赤）
		CLEAR,							// 透明（カラーなし）
		TURQUOISE,						// ターコイズ（トルコ石色）
		VIOLET,							// バイオレット（すみれ色）
		WHEAT,							// ウィート（小麦色）
		WHITE,							// 白
		WHITESMOKE,						// ホワイトスモーク（白い煙色）
		YELLOW,							// 黄色
		YELLOWGREEN,					// イエローグリーン（黄緑）
		MAX
	};

	// 有効値かどうかの判定
	constexpr bool IsValid(const COLOR value) noexcept
	{
		return EnumUtil::InRange(value, COLOR::ALICEBLUE, COLOR::MAX);
	}

	//******************************************************************************
	// *** プロトタイプ宣言 ***
	//******************************************************************************
	Color GetColor(const COLOR col);
	Color Random(const bool bAlpha);
	Color Lerp(const Color &start, const Color &end, const float t);
}
#endif