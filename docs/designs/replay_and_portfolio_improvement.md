# 爆食トライアングル 応募作品の改善とリプレイ機能

*   **Author:** TenmaSaito
*   **Status:** Draft
*   **Date:** 2026-11-09

--------------------------------------------------------------------------------

## Overview

完成済みの1人用ゲーム『爆食トライアングル』(C++20 / DirectX 9、約2万行) を、ゲーム会社へ応募する作品として改善する。期間は2週間である。

作業は3つに分かれる。

1.  既知のクラッシュとフリーズの経路を閉じる。
2.  オブジェクトの寿命の決まりを明確にし、具体的な問題を解決する箇所にデザインパターンを導入する。
3.  ゲームプレイを記録し、ローカルで再生できるようにする。オンラインへの移行は、ローカルの動作確認の後、他の改善を優先したうえで余力があれば検討する。

手書きの連結リストによる `CObject` の基本構造は残す。

## The Problem

採用担当者は、コードの設計、読みやすさ、技術力を見る。現在のコードには、その評価を下げる問題が3種類ある。

**動作の問題:** サーバーの応答を検査せずに固定長の配列へコピーする箇所 (`rankingClient.cpp:112-135`) や、応答しないサーバーで終了もできなくなる箇所 (`tcp_client.h:155-161`) がある。読み手が動かしたときに落ちると、設計の説明を読んでもらえない。

**読みやすさの問題:** README と設計文書がない。呼び出し元のないコード (`CPlayer::CharactorMovable`、`Coroutine`) や、4回コピーされた読み込み処理 (`mapScriptLoader.cpp`) が残っている。オブジェクトが死ぬときの決まりが書かれておらず、シーンの破棄が「アイテムを食べた」と誤って処理される (`itemGenerator.cpp:195-291`)。

**技術の見せ場の問題:** 既存の State、CRTP、コンセプトなどの工夫に説明がない。作者が C++ の設計力を示せる題材が、文書の形で読み手に届いていない。

リプレイは、この3つをまとめて示す題材になる。入力を差し替える Strategy と記録する Decorator が、実際の機能のために必要になる。再現性の確保には、乱数と更新処理の整理も必要になる。

作者の決定により、ローカルで動作することを完了条件とする。別の PC での再現とサーバーの変更は、この設計の範囲に含めない。

## Goals

作業は次の条件をすべて満たしたときに完了とする。

*   **G1. 欠陥の解消:** 欠陥一覧 D1〜D20 の各項目について、修正コミットと確認の証拠、または直さない理由を記録する。D5 (デバイスロスト) は作者が担当する。
*   **G2. ローカルのリプレイ:** ゲームプレイの入力と乱数の種を記録できる。同じ実行ファイルで2回再生したとき、毎フレームの記録項目が一致する。
*   **G3. リプレイの再生:** ゲーム終了時に記録をローカルのファイルへ保存できる。ランキング画面から直前のプレイを再生できる。再生中は得点を送信せず、`current.bin` も書き込まない。
*   **G4. 再現性の検証:** 同じ実行ファイルと同じ PC で、同じリプレイの確認点と最終得点が一致する。一致しないリプレイは、途中で検出して再生を止め、画面で知らせる。
*   **G5. 通信の失敗への対応:** サーバーあり、サーバーなし、接続後に応答しないサーバーの3通りで、ゲームが固まらない。受信データの長さと内容を検査し、不正なデータで落ちない。
*   **G6. 寿命の決まり:** オブジェクトの「生存、死亡フラグ、削除済み」の3状態をコードと文書で定義する。死亡済みオブジェクトを更新しない。シーンの破棄で得点や効果音が発生しない。
*   **G7. デザインパターン:** ブリーフの候補1〜6と Factory の新規候補 A〜C を導入する。各パターンについて、解決した問題、代償、選ばなかった案を判断の記録に書く。既存のパターンと、入れなかったパターンの理由も文書にする。
*   **G8. 遊びの同等性:** 変更の前後で同じ入力の再生結果を比べる。差が出る変更は、差の内容を事前にコミットの説明へ書く。操作の手触りは、改造前の実行ファイルと遊び比べて確かめる。
*   **G9. 文書:** `README.md`、`docs/ARCHITECTURE.md`、判断の記録、`docs/VERIFICATION.md` を作成し、担当者が主要な設計判断の理由をたどれるようにする。

## Non-Goals

*   見た目の新しい機能の追加。ただし、リプレイの選択に必要な最小の UI は、既存の部品で作る範囲に限って対象とする。
*   手書きの連結リストの入れ替え、ECS への移行。
*   `CPlayer` の状態マシン化やクラス分割。
*   `CManager` シングルトンの廃止。
*   操作の調整値、計算式、更新間隔の変更。欠陥の修正として明記したものを除く。
*   `system.ini`、`current.bin`、ランキング通信の形式の変更。
*   ランキング通信の別スレッド化。通信は時間制限付きで同期的に行う。
*   サーバー経由のリプレイと、別の PC での再現の保証。Future Improvements に記す。
*   不正なスコアやリプレイの検出。
*   通信の暗号化と利用者の認証。
*   欠陥が存在しないことの証明。

## Open Questions

*   **Q1. サーバーの提出:** サーバーは別リポジトリ `cpp_TKG_Server` にある。提出物に含めるか、どう起動してもらうかは未定である。
*   **Q2. 担当者への見せ方:** リプレイを動画で見せるか、記録ファイルを同梱するかは未定である。別の PC で記録を再生する保証はないので、同梱する場合は動作を確かめる。
*   **Q5. 退避用の main:** 間に合わないときに提出する main に、D1、D2、D12 の修正を入れるか、どのコミットを基準にするかは未定である。
*   **Q6. 作業日数:** 計画は10作業日を仮定している。
*   **Q7. 未使用コードの扱い:** `Coroutine` と `Delegate` を消すか、実際に使うかは未定である。
*   **Q8. デバッグ機能:** デバッグキーと BACK の表示「リトライ」の本来の動作は未定である。
*   **Q9. 文字コード:** ソースを UTF-8 へ変換するかは未定である。担当者は読みやすくなるが、編集手順と CI の見直しが必要になる。

## Critical User Journeys (CUJs)

### CUJ 1: プレイの記録

*   **Given**: ゲーム画面にいる。
*   **When**: 制限時間の100秒が終わる。
*   **Then**: 記録が `data/replay/last.bin` へ保存され、得点がサーバーへ送られ、ランキング画面へ移る。通信に失敗しても、ゲームは時間制限の範囲内で先へ進む。

### CUJ 2: 直前のプレイの再生

*   **Given**: ランキング画面にいて、`data/replay/last.bin` が存在する。
*   **When**: プレイヤーが「直前のプレイを再生」を選んで決定する。
*   **Then**: そのプレイが再生される。再生の終了後はランキング画面へ戻る。再生中に得点の送信は発生しない。

### CUJ 3: 再現できない記録

*   **Given**: 再生中の計算結果が、リプレイに記録された途中の得点と一致しなくなる。
*   **When**: 次の確認点に達する。
*   **Then**: 再生を止め、再現できないことを画面に表示して、ランキング画面へ戻る。

### CUJ 4: サーバーなしでの起動

*   **Given**: サーバーが起動していない、または接続後に応答しない。
*   **When**: ゲームを終えるか、ランキング画面を開く。
*   **Then**: 時間制限の範囲内で処理が戻り、通信できなかったことが分かる表示になる。全0の値を実際のランキングとして表示しない。

### CUJ 5: 担当者がコードを読む

*   **Given**: 担当者がリポジトリを初めて開く。
*   **When**: `README.md` から読み始める。
*   **Then**: ビルド手順、読む順番、1フレームの流れ、主要な判断の理由へ、文書のリンクとコードの関数名でたどれる。

--------------------------------------------------------------------------------

## Technical Plan (Architecture)

実装は作者が行う。この節は、作者が判断なしに実装へ進める粒度の構成を定める。作者の決定事項は「作者決定」と記す。

### 全体の構成

```text
 CRanking ── 「直前のプレイを再生」 ──> last.bin を読んで検査
    │
    │  CReplayData を CManager へ預ける (検査済みのものだけ)
    ▼
 CReplay シーン
    │
    └─ BuildGameplayWorld ─┐
 CGame シーン ─────────────┤
    │                      ▼
    │               ステージ・アイテム生成器・得点など (共通)
    │                      │
    │                 CStage ── ILeanInput を読む
    │                      │
    │          ┌───────────┼─────────────────┐
    │          ▼           ▼                 ▼
    │   CLiveLeanInput  CRecordingLeanInput  CPlaybackLeanInput
    │     (実機)        (Decorator: 実機を包む) (CReplayData を読む)
    └─ 終了時: 記録を last.bin へ保存、得点を IRankingService へ送信
                     (CTcpRankingService / CNullRankingService)
```

### 1. オブジェクトの寿命

状態は「生存」「死亡フラグ」「削除済み」の3つとする。手書きのリストは変更しない。

*   `CObject::Release` の2回目は何もしない。死亡の通知は1回だけ出る。
*   `UpdateAll` と `DrawAll` は死亡フラグの立ったオブジェクトを飛ばす。
*   `Find` 系の検索は死亡フラグの立ったオブジェクトを返さない。
*   実際の `delete` は今までどおり `FlagCheckAll` だけが行う。
*   `CManager::Uninit` で `m_pScene->Uninit()` を呼ぶ。

`CObject::UpdateAll` の呼び出しは `CRenderer::Update` から `CManager::Update` へ移す。呼び出しの順番は変えない。

### 2. 型付きの通知 (作者決定)

`std::string_view` と `std::any` の通知をやめ、`enum class EVENT { DEATH, EATEN }` を受け取る形に変える。引数の `any_cast` は現在0件であり、失われる機能はない。実装の変更先は `CObject::NotifyAll`、`Notified` の4実装である。

*   `DEATH` はポインタの無効化だけに使う。
*   `EATEN` は `CItem` が `item.cpp:239` の `Uninit` の直前に出す。シーンの破棄では出ない。これで D9 が閉じる。
*   種別を増やすと `CObject` がゲームの知識を持つことになるため、種別はこの2つに限る。増やす必要が生じたら仕組みを見直す。

`CItemGenerator` は `Notified` の中で処理せず、食べられた種類を `m_vEatenQueue` に積むだけにする (イベントキュー)。自分の `Update` の先頭でまとめて処理する。これで通知の処理中に他のアイテムを破棄する問題が閉じる。

*   キューにはポインタではなく `CItem::TYPE` を積む。
*   全アイテムを破棄するときは `m_vpItem` を別の配列へ移してから `Uninit` を呼ぶ。`DEATH` の処理で元の配列が変わるため。
*   生成器はアイテムより先にリストへ入るので、判定は1フレーム遅れる。基準記録との差としてコミットの説明に書く。

### 3. リプレイの入力 (Strategy、Decorator、Simple Factory)

ゲームの進行に効く入力は `stage.cpp:160-163,174` だけなので、ここを差し替える。

*   `ILeanInput::Read()` は1回の呼び出しで `LeanSample { fVertical, fHorizontal, bVerticalPressed, bHorizontalPressed }` を返す。1フレームに1回だけ呼ぶ。再生の読み取り位置を進める操作なので、値ごとに別の関数にしない。
*   `CLiveLeanInput` は `CInputSystem` のアクションを読む。
*   `CRecordingLeanInput` は `ILeanInput` を包み、値を返しつつ `CReplayData` へ追加する。
*   `CPlaybackLeanInput` は `CReplayData` から順に返す。
*   `CreateLeanInput` は上の3つを1か所で作る。
*   `CStage` は `SetLeanInput` で受け取る。スクリプトの読み込み後に `BuildGameplayWorld` が渡す。

ポーズ中は `UpdateAll` が呼ばれないので `CStage::Update` も止まり、記録には入らない。タイトルとポーズの操作は記録しないので、手動で確認する。

### 4. リプレイのデータ

`CReplayData` は次を持つ。形式はバイナリとする。

| 項目 | 内容 |
| --- | --- |
| ヘッダー | 識別子、形式バージョン、調整値のバージョン、乱数の種、総フレーム数 |
| 入力 | `LeanSample` を「連続フレーム数 + 値」の並びで持つ |
| 確認点 | 60フレームごとの得点。最終得点 |

*   制限時間100秒、最大約62.5フレーム毎秒として最悪で約80KBである。`MAX_REPLAY_BYTES` は 128KB とする。
*   読み込み時は、ヘッダー、バージョン、サイズ、連続フレーム数の合計と総フレーム数の一致を検査する。不正なら再生しない。
*   調整値を変えたら調整値のバージョンを上げる。古い記録は再生しない。

### 5. 再現性

*   ゲーム進行用の乱数と演出用の乱数を分ける。`CRand` は型ごとに関数内 `static` の生成器を持ち、後から種を設定できない (`rand.cpp:29,43,57,71`)。そこで進行用に `CGameplayRandom` を新設し、`CReplayData` の種で初期化する。`itemGenerator.cpp` の配置と体力だけがこれを使う。`CRand` は演出用として残す。
*   `itemGenerator.cpp:112` の `GetMatrix()` をやめ、ローカル座標の `GetPosition()` を使う。これで配置判定は `Draw` に依存しなくなる。距離の基準が変わるので、配置の差はコミットの説明に書く。
*   再生中は確認点の得点を比べる。違ったら止めて CUJ 3 の動作にする。
*   別の PC での再現は保証しない。CPU による浮動小数の差があり得る。

### 6. シーンの構成 (作者決定)

`MODE::REPLAY` と `CReplay` シーンを追加する。

*   ステージの構築は `BuildGameplayWorld(std::unique_ptr<ILeanInput>)` に抽出し、`CGame` と `CReplay` が共通で呼ぶ。`CGame::Start` の中身をここへ移す。
*   `CReplay` は得点の送信と `current.bin` の書き込みをしない。ポーズも受け付けない。
*   再生の終了時と不一致の検出時に `MODE::RANKING` へ遷移する。
*   `CRanking` は「直前のプレイを再生」の選択を受け付け、`last.bin` の読み込みと検査に成功したときだけ `CManager` に記録を預けて遷移する。失敗時はランキング画面に留まり、知らせる。
*   `CReplay` と `CGame` の挙動のずれは、共通化と記録の比較で検出する。

### 7. ランキング通信 (Strategy、Null Object)

通信形式とサーバーは変更しない。`IRankingService` は `SubmitScore(nScore)` と `GetRanking()` を持つ。`CTcpRankingService` と、何もしない `CNullRankingService` を差し替えられるようにする。`CNullRankingService` はサーバー未接続の表示と再生の確認に使う。

*   クライアントは、受信した長さを確保の前に検査する。これで D1 が閉じる。
*   接続、送信、受信に時間制限を設ける。`CTcpClient` に `SendAll` と時間制限付きの `RecvAll` を追加し、`SendWithSize` の戻り値を確かめる。
*   `CTcpClient` はデストラクタでソケットを閉じる (RAII)。
*   サーバーのアドレスは1か所にまとめる。

通信は同期処理のままなので、通信中は画面が止まる。待ち時間の上限は約3秒とし、実測で確かめる。

### 8. その他のパターン

| パターン | 対象 | 内容 |
| --- | --- | --- |
| RAII | 描画状態 | 値を保存して元に戻す `CScopedRenderState` を作る。`timer.cpp`、`shadow.cpp`、`item.cpp` を対象にする。全箇所は変えない |
| 型オブジェクト | アイテム | `ItemTypeDef` の `constexpr` 配列に、モデル、テクスチャ、パーティクルのパスをまとめる。`static_assert` で要素数を確かめる |
| テーブル駆動と Factory | スクリプト読み込み | `ReadBlock` と、ブロック名から生成処理を選ぶ表にする。ファイルの順に生成する動作は変えない。未知のキーはログへ出す |
| Factory | オブジェクト生成 | `template<class T, class... Args> static T *CreateAndInit(Args&&...)` を作る。`Init` が失敗したら `Uninit` を呼び `nullptr` を返す。形が同じ `Create` だけを移す |

### 9. 確認の方法

`CReplay` の仕組みを動作確認にも使う。`_DEBUG` のときだけ、フレームごとのプレイヤー位置、ステージの傾き、得点、アイテム数をファイルへ書く。

*   最初の段階で、`ILeanInput` と記録の導入に加えて、乱数の分離と `itemGenerator.cpp:112` の変更を行う。これらが終わらないと、同じ入力でも再生結果が一致しないためである。この段階の後に基準記録を取る。
*   再現性は、同じ PC の2回の再生で確かめる。

## Alternatives Considered

「Rejected by author」は、作者が選択の場で外した案である。理由は、その選択の場で示した技術的な代償を記す。それ以外は、レビューで提案され、作者が承認したブリーフに記載された見送り案である。

### リプレイの範囲

#### Alternative A: サーバー経由の再生まで完了条件に含める

*   **Pros**: ランキングの記録を再生できる。通信とサーバーの設計も示せる。
*   **Cons**: サーバーの変更、通信形式の拡張、別の PC での再現が必要になり、他の改善の時間を奪う。
*   **Reason for Rejection**: Rejected by author: ローカルで動作すれば十分とし、他の改善を優先する。オンラインへの移行は、ローカルの動作確認の後に余力があれば検討する (Future Improvements)。

#### Alternative B: ローカルを確定し、サーバーは別の PC での判定で条件付きとする

*   **Pros**: 別の PC で再現しない場合の損失を抑えられる。
*   **Cons**: 条件で実装範囲が変わる。判定を通っても時間が足りない可能性がある。
*   **Reason for Rejection**: Rejected by author: 判定を設けず、他の改善を優先する。

### リプレイの再生方式

#### Alternative C: 毎フレームの状態を記録する

*   **Pros**: 別の PC でも再現性の問題が起きない。
*   **Cons**: シミュレーションの代わりになる再生経路が別に要る。アイテムの配置も記録する必要がある。Strategy と Decorator の題材にならない。
*   **Reason for Rejection**: ブリーフで見送り。オンライン化で別の PC の再現が成立しない場合の代替として Future Improvements に残す。

#### Alternative D: 入力機器の全状態を記録する

*   **Pros**: ポーズやタイトルの操作も含められる。
*   **Cons**: データが大きく、サーバーの送受信に向かない。ゲームの進行に効く入力は2軸だけである。
*   **Reason for Rejection**: ブリーフで見送り。

### 再生のシーン構成

#### Alternative E: `CGame` を共用し、終了時の方針を Strategy で差し替える

*   **Pros**: ゲーム進行の経路が1本で、再現性を保ちやすい。
*   **Cons**: 方針のインターフェースが増える。再生固有の表示が増えると方針が肥大化する。
*   **Reason for Rejection**: Rejected by author: 再生固有の処理を1クラスに集め、既存の State 構造に沿わせる。

#### Alternative F: `CGame` の中のフラグで分岐する

*   **Pros**: 変更量が最も少ない。
*   **Cons**: 分岐が散らばる。入れ忘れると再生中に得点を送信する。
*   **Reason for Rejection**: Rejected by author: 設計の説明材料にならず、誤送信の危険がある。

### 通信

#### Alternative H: ランキング通信を別スレッド化する

*   **Pros**: 通信中に画面が止まらない。
*   **Cons**: 完了通知と破棄の順番の設計が増える。
*   **Reason for Rejection**: ブリーフで見送り。時間制限付きの同期処理で、固まる問題は解消する。

### 「食べた」通知

#### Alternative I: 既存の `Delegate` を `CItem` で使う

*   **Pros**: 型があり、破棄時に自動で解除される。
*   **Cons**: 他プロジェクトの痕跡 (`dxs`、コメント) を整理し、内部を説明できる必要がある。
*   **Reason for Rejection**: Rejected by author: 通知の仕組みを `CObject` の1本に保つ。

#### Alternative J: `IItemListener` を追加する

*   **Pros**: 単純で読みやすい。
*   **Cons**: 受け手は1つだけで、破棄時の解除は手で書く。生成器が先に破棄されると古いポインタを呼ぶ。
*   **Reason for Rejection**: Rejected by author: 寿命の扱いを手書きで増やす。

#### Alternative K: `CObject` にマルチキャストの `Delegate` を持たせる、またはイベントバスを導入する

*   **Pros**: 汎用の仕組みになる。
*   **Cons**: オブジェクトごとに確保が増える。この規模では中継する相手が少なく、流れが追いにくくなる。
*   **Reason for Rejection**: ブリーフで見送り。

### オブジェクトの基本構造

#### Alternative L: リストを `vector` と `unique_ptr` やハンドルへ置き換える

*   **Pros**: 標準ライブラリで表現できる。
*   **Cons**: 更新中に追加したオブジェクトが同じフレームに更新される順序を再現する必要がある (`object.cpp:123,308-310`)。欠陥の原因はリストにない。
*   **Reason for Rejection**: ブリーフで見送り。作者もリストの維持を希望した。

#### Alternative M: 修正と削除だけを行い、再設計しない

*   **Pros**: 成功条件に最も近く、リスクが小さい。
*   **Cons**: D9 が順番に頼った修正になる。寿命の決まりを説明できない。作者の目的である C++ の設計力の提示に届かない。
*   **Reason for Rejection**: Rejected by author: デザインパターンの活用とリプレイを目標とした。作業が遅れたときに削る順番の基準としては使う。

### 導入しないパターンと構造変更

#### Alternative N: `CManager` シングルトンの廃止、または依存性の注入

*   **Pros**: 依存が見えるようになる。
*   **Cons**: 36〜37ファイル、76〜77か所に広がる。途中で止まると一貫性を失う。
*   **Reason for Rejection**: ブリーフで見送り。

#### Alternative O: `CPlayer` の状態マシン化やクラス分割

*   **Pros**: パターンの見せ場になる。
*   **Cons**: 問題はクラスの大きさではなく関数の長さである。操作感を壊す危険が大きい。
*   **Reason for Rejection**: ブリーフで見送り。

#### Alternative P: シーンのファクトリ登録、Sound の Null Object、Visitor、Abstract Factory、ECS

*   **Pros**: パターンの数は増える。
*   **Cons**: シーンの `switch` は3件で増える予定がない。Sound は null の確認で足りる。Visitor と Abstract Factory は対応する問題がない。ECS はリストの維持と矛盾する。
*   **Reason for Rejection**: ブリーフで見送り。

#### Alternative Q: Object Pool をすぐ導入する

*   **Pros**: ゲームプログラマーに馴染みのある技術を示せる。
*   **Cons**: 確保の負荷を測っていない。
*   **Reason for Rejection**: ブリーフで見送り。計測のうえで追加作業として判断する。

#### Alternative R: 更新を正確な60Hzに直す

*   **Pros**: `1000 / 60` の切り捨てがなくなる。
*   **Cons**: 操作感と制限時間の実時間が約4%変わる。リプレイのフレーム基準には影響しない。
*   **Reason for Rejection**: ブリーフで見送り。

#### Alternative S: `Debug::Log` を確認の記録に使う

*   **Pros**: 新しいファイル出力が要らない。
*   **Cons**: 件数の上限があり、ファイルへ出せない。時刻が入るので比較に向かない。
*   **Reason for Rejection**: ブリーフで見送り。

--------------------------------------------------------------------------------

## Non-Functional Requirements

*   **セキュリティ:** クライアントは、受信データとリプレイファイルの長さと内容を、確保や解析の前に検査する。不正なスコアの検出は対象外である。
*   **性能:** 通信は同期処理である。1回の操作の待ち時間は、接続1秒、送受信2秒の合計約3秒を上限とする。記録によるフレーム時間の増加は計測して `docs/VERIFICATION.md` に残す。
*   **互換性:** サーバーと通信形式は変更しないので、既存のサーバーとそのまま通信できる。
*   **ロールバック:** 退避用の main を保持する。

--------------------------------------------------------------------------------

### Future Improvements

*   **オンラインのリプレイ:** ローカルの動作確認の後、他の改善を優先したうえで余力があれば検討する。作者が検討の中で決めた方針は次のとおりである。
    *   サーバーは別リポジトリ `GC2\Git\cpp_TKG_Server` であり、作者が変更できる。
    *   得点を登録した後、順位内のときだけ識別番号を付けてリプレイを送る。リプレイの失敗で得点を失わない。
    *   `CRanking` の `T` を「得点と識別番号」のエントリに変える。`ranking.bin` の形式が変わるので変換を用意する。
    *   リプレイ本体は識別番号ごとの別ファイルに保存する。
    *   サーバーの受信上限 (`MAX_RECV_SIZE` 256バイト) とスタック配列の変更が必要である。
    *   別の PC で再現するかを先に確かめる。再現しない場合は、毎フレームの状態を記録する方式を検討する。

--------------------------------------------------------------------------------

## Detailed Implementation Plan

実装は作者が行う。パスは `GC2\Git\BakusyokuTriangle` からの相対である。既存のソースは Shift-JIS と CRLF が多い。編集時に文字コードを保つか、Q9 の決定に従う。新しいファイルの名前は `tools/check_filenames.py` の規則で確かめる。`.h` と `.cpp` の組は1項目にまとめる。

### 実装の順番

各段階の最後に、基準記録との比較を行う。1行を1コミット以上にする。

| 順 | 内容 | 主な項目 |
| --- | --- | --- |
| 1 | `ILeanInput` と記録の導入、`CGameplayRandom`、配置判定の変更、基準記録 | G-1, G-2, G-3, G-19, G-20 |
| 2 | 不要コードの削除 | G-4 |
| 3 | 欠陥の修正、スクリプト読み込み | G-5〜G-9 |
| 4 | 寿命と型付き通知 | G-10〜G-14 |
| 5 | 残りのパターン | G-15〜G-18 |
| 6 | 再生シーンとランキングからの再生 | G-22〜G-26 |
| 7 | 文書とタグ `portfolio-v1` | D-1〜D-4 |
| 8 | 余力があれば、オンライン化の検討 (Future Improvements) | なし |

### Target Files to Create/Modify (ゲーム)

#### G-1. `include/game/replay/leanInput.h`, `src/game/replay/leanInput.cpp` (Create)

*   **Type / Scope**: C++ の入力モジュール
*   **Proposed Changes**: `LeanSample`、`ILeanInput { virtual LeanSample Read(void) = 0; }`、`CLiveLeanInput`、`CRecordingLeanInput(std::unique_ptr<ILeanInput>, CReplayData&)`、`CPlaybackLeanInput(const CReplayData&)`、`CreateLeanInput` を定義する。再生が終端に達したら入力なしの値を返す。

#### G-2. `include/game/replay/replayData.h`, `src/game/replay/replayData.cpp` (Create)

*   **Type / Scope**: C++ のデータモジュール
*   **Proposed Changes**: `CReplayData` を定義する。`AddSample`、`AddCheckpoint`、`Serialize`、`Deserialize` を持つ。`Deserialize` は Technical Plan 4 の検査を行い、失敗なら `false` を返す。ファイルへの保存と読み込みは `CFileStream` を使う。

#### G-3. `include/game/stage/stage.h`, `src/game/stage/stage.cpp` (Modify)

*   **Type / Scope**: ゲームロジック
*   **Proposed Changes**: `SetLeanInput` を追加する。`LeanInput` は `Read()` を1回呼び、その値で傾きを計算する。`CStage::Update` の null 確認を追加する (D4)。`#if 0` のブロックと未使用の入力ポインタを消す。`Notified` の引数を変える。

#### G-4. `src/game/player/player.cpp`, `include/game/player/player.h` (Modify)

*   **Type / Scope**: ゲームロジック
*   **Proposed Changes**: `CharactorMovable` と `#if FALSE` を消す。`StatusChange` と `DIK_R` の処理を `_DEBUG` で囲むか消す (D12、Q8)。負の `m_fValueRot` を0へ減衰させる (D16)。`boader` の綴りを直す。`Update` を文の順番を変えずに名前付きの処理へ分ける。`Notified` の引数を変える。

#### G-5. `include/game/stage/mapScriptLoader.h`, `src/game/stage/mapScriptLoader.cpp` (Modify)

*   **Type / Scope**: データ読み込み
*   **Proposed Changes**: `FieldRule`、`ReadBlock`、`StripCommentAndTabs`、`LoadTimer`、`LoadPlayer`、`LoadField`、`LoadWall` を追加し、ブロック名と生成処理の表にする。終端なしの場合は `E_FAIL` を返す。未知のキーをログへ出す。ファイルの順に生成する。`CPlayerCamera::Create` は `CPlayer` があるときだけ呼ぶ。

#### G-6. `include/network/tcp_client.h`, `src/network/tcp_client.cpp` (Modify)

*   **Type / Scope**: 通信
*   **Proposed Changes**: 時間制限付きの接続、`SendAll`、時間制限付きの `RecvAll` を追加する。デストラクタでソケットを閉じる。`SendWithSize` は各送信の結果を返す。

#### G-7. `src/core/main.cpp`, `src/core/directX.cpp` (Modify)

*   **Type / Scope**: アプリケーションの基盤
*   **Proposed Changes**: メインのループを `GetMessage` にする。DirectX スレッドは次の更新時刻まで待つ。更新間隔の計算は変えない (D18)。

#### G-8. `src/ui/timer.cpp`, `src/object/3d/objectX.cpp`, `src/sound/sound.cpp`, `src/ui/bitePause.cpp` (Modify)

*   **Type / Scope**: 欠陥修正
*   **Proposed Changes**: 最初のフレームの減算をやめる (D17、作者が確認)。`objectX.cpp` は `Init` の失敗を返す (D6)。`sound.cpp` は失敗時に `E_FAIL` を返し、`Play` の先頭で null を確かめ、`dwBytesRead` を更新する (D7)。`bitePause.cpp` の null を確かめる (D8)。

#### G-9. `include/math/constVector.h` (Modify)

*   **Type / Scope**: 数学ライブラリ
*   **Proposed Changes**: 使われていない API を消す。残す `Angle` 系は単位を名前で示す (D20)。

#### G-10. `include/object/base/object.h`, `src/object/base/object.cpp` (Modify)

*   **Type / Scope**: オブジェクトの基盤
*   **Proposed Changes**: `enum class EVENT { DEATH, EATEN }` を追加する。`NotifyAll(EVENT)`、`Notified(CObject*, EVENT)` に変え、`<any>` を外す。`Release` の2回目を無視する。`UpdateAll`、`DrawAll`、`Find` 系で死亡済みを飛ばす。`CreateAndInit` を追加する。状態の定義をコメントに書く。

#### G-11. `include/game/item/item.h`, `src/game/item/item.cpp` (Modify)

*   **Type / Scope**: ゲームロジック
*   **Proposed Changes**: `Uninit` の直前に `EATEN` を通知する。`GetType` 相当の取得関数を追加する。`ItemTypeDef` の表へ移す。描画後の設定を `true` にする (D15)。`Create` を `CreateAndInit` に移す。

#### G-12. `include/game/item/itemGenerator.h`, `src/game/item/itemGenerator.cpp` (Modify)

*   **Type / Scope**: ゲームロジック
*   **Proposed Changes**: `Notified` は `DEATH` で `m_vpItem` から外し、`EATEN` で `m_vEatenQueue` に積む。処理は `Update` の先頭へ移す。全破棄の前に配列を移す。配置の試行に上限を付ける (D11)。生成数の上限を直す (D19、作者が確認)。

#### G-13. `include/effect/modelParticle.h`, `src/effect/modelParticle.cpp` (Modify)

*   **Type / Scope**: 演出
*   **Proposed Changes**: `Notified` の引数を変える。

#### G-14. `include/render/renderer.h`, `src/render/renderer.cpp`, `include/core/manager.h`, `src/core/manager.cpp` (Modify)

*   **Type / Scope**: アプリケーションの基盤
*   **Proposed Changes**: `CRenderer::Update` を消し、`CObject::UpdateAll` を `CManager::Update` の同じ位置で呼ぶ。`Uninit` で `m_pScene->Uninit()` を呼ぶ。`ENABLE_INPUT_SYSTEM` の分岐を消す。BACK の処理を `IsTransition` で守る (Q8)。`SetPendingReplay` と `TakePendingReplay` を追加する。`renderer.cpp` のレンダーターゲット取得の解放漏れは D5 と一緒に作者が直す。

#### G-15. `include/render/scopedRenderState.h` (Create)

*   **Type / Scope**: RAII の補助
*   **Proposed Changes**: コンストラクタで現在値を保存して新しい値を設定し、デストラクタで戻す `CScopedRenderState` を定義する。コピーを禁止する。

#### G-16. `src/ui/timer.cpp`, `src/object/3d/shadow.cpp` (Modify)

*   **Type / Scope**: 描画
*   **Proposed Changes**: 手書きの設定と復元を `CScopedRenderState` に替える。これ以外のファイルは変えない。

#### G-17. `src/game/player/player.cpp`, `src/game/stage/stage.cpp`, `src/ui/timer.cpp` (Modify)

*   **Type / Scope**: 生成処理の共通化
*   **Proposed Changes**: `Create` を `CObject::CreateAndInit` に移す。`Init` の失敗で `nullptr` が返るので、呼び出し側で確かめる。これ以外の `Create` は変えない。

#### G-18. `include/network/rankingService.h`, `src/network/tcpRankingService.cpp` (Create), `include/network/rankingClient.h`, `src/network/rankingClient.cpp` (Delete)

*   **Type / Scope**: 通信
*   **Proposed Changes**: `IRankingService`、`CTcpRankingService`、`CNullRankingService` を定義する。`CTcpRankingService` は `WSAStartup` と `WSACleanup` を RAII で管理する。アドレスとポートはここへまとめる。受信サイズを確保前に検査する (D1)。

#### G-19. `include/math/gameplayRandom.h`, `src/math/gameplayRandom.cpp` (Create)

*   **Type / Scope**: 乱数
*   **Proposed Changes**: 種を設定できる `CGameplayRandom` を定義する。`std::mt19937` を1つ持ち、整数と実数の生成を行う。

#### G-20. `src/game/item/itemGenerator.cpp` (Modify)

*   **Type / Scope**: 再現性
*   **Proposed Changes**: `CGameplayRandom` を使う。プレイヤー位置の取得を `GetPosition()` に替える。

(G-21 は欠番。通信形式を変えないため。)

#### G-22. `include/scene/gameplayWorld.h`, `src/scene/gameplayWorld.cpp` (Create)

*   **Type / Scope**: シーン
*   **Proposed Changes**: `BuildGameplayWorld(std::unique_ptr<ILeanInput>)` を定義する。`CGame::Start` のステージ、生成器、背景、画面効果、得点、次の表示の構築を移す。読み込みの失敗を返す (D4)。

#### G-23. `include/scene/replay.h`, `src/scene/replay.cpp` (Create)

*   **Type / Scope**: シーン
*   **Proposed Changes**: `CReplay` を定義する。`TakePendingReplay` で記録を受け取り、再生する。確認点を比べ、不一致や終了で `MODE::RANKING` へ遷移する。`_DEBUG` のときはフレームの記録を書く。

#### G-24. `include/scene/scene.h`, `src/scene/scene.cpp` (Modify)

*   **Type / Scope**: シーン
*   **Proposed Changes**: `MODE::REPLAY` と生成の分岐を追加する。

#### G-25. `include/scene/game.h`, `src/scene/game.cpp` (Modify)

*   **Type / Scope**: シーン
*   **Proposed Changes**: `BuildGameplayWorld` と記録版の入力を使う。終了時に記録を `data/replay/last.bin` へ保存し、`SubmitScore` を呼ぶ。`CTimer::Find` の null を確かめる。

#### G-26. `include/scene/ranking.h`, `src/scene/ranking.cpp` (Modify)

*   **Type / Scope**: シーン
*   **Proposed Changes**: 「タイトルへ」と「直前のプレイを再生」の選択を追加する。初期の選択は「タイトルへ」とし、決定キーの今の動作を保つ。選択は既存の `CObject2D` のカーソルで示す。再生の決定で `last.bin` を読み、検査に成功したときだけ遷移する。通信に失敗した場合は `CDebugProc::ForcePrint` で知らせ、全0を実データとして表示しない。

#### G-27. `C++31GBM.vcxproj`, `C++31GBM.vcxproj.filters` (Modify)

*   **Type / Scope**: ビルド構成
*   **Proposed Changes**: 新規ファイルの追加、削除ファイルの除外、x64 構成の整理を行う。

### Target Files to Create/Modify (文書)

#### D-1. `README.md` (Create)

*   **Type / Scope**: 文書
*   **Proposed Changes**: ゲームの説明、動画、ビルド手順、DirectX SDK June 2010、操作、サーバーの起動方法、読む順番を1画面で書く。

#### D-2. `docs/ARCHITECTURE.md` (Create)

*   **Type / Scope**: 文書
*   **Proposed Changes**: 層の図、1フレームの流れ、オブジェクトの寿命、既存と新規のパターンを2ページ以内で書く。

#### D-3. `docs/decisions/DR-001〜*.md` (Create)

*   **Type / Scope**: 文書
*   **Proposed Changes**: スレッド分割、連結リスト、傾き操作、入力層、シングルトン、型付き通知、リプレイ方式、通信手順を1件1画面で書く。

#### D-4. `docs/VERIFICATION.md` (Create)

*   **Type / Scope**: 文書
*   **Proposed Changes**: 欠陥一覧、修正コミット、証拠、手動確認の項目を書く。

### Verification Plan

*   **Compilation/Build**: Debug|Win32 と Release|Win32 をビルドする。`C++31GBM.vcxproj` を直接指定する。
*   **Unit Tests**: 自動テストは対象外である。`CReplayData::Deserialize` の異常データは、下の異常テストで確かめる。
*   **Static Analysis & Linting**: `tools/check_filenames.py`、`tools/check_identifiers.py` と CI を通す。`/W4` の警告が増えないことを確かめる。
*   **Test Coverage Requirement**: 対象外である。D1〜D20 の一覧を閉じることで代える。
*   **E2E/Integration**:
    *   同じ実行ファイルでの再生2回で、フレーム記録が一致する。
    *   同じ PC でリプレイを再生し、確認点と最終得点が一致する。
    *   サーバーあり、サーバーなし、応答なしで、CUJ 1、2、4 が通る。
    *   不正な長さの応答、破損したヘッダー、128KB 超の `last.bin` でゲームが落ちない。
    *   設定ファイル、テクスチャ、モデルの欠落でフリーズしない。
    *   シーン遷移の繰り返しでメモリリークの報告が増えない。
    *   D9 の確認として、アイテムが残った状態の時間切れで効果音と得点の変化が起きない。

--------------------------------------------------------------------------------

*Drafted with EGM*
