# Seed 実装概要

## 目的と現在の構成

このリポジトリには、初期のデータ・バイナリ処理実装と、後続のゲーム状態処理実装が統合されています。一般的なC++構成として、実装は `src/`、公開ヘッダーは `include/seed/`、テストは `tests/` に配置し、`seed.xcodeproj` もこの構成を参照します。

主なサブシステムは次の2つです。

1. データ・ファイル処理
2. プレイヤー・状態・フィールド処理

## データ・ファイル処理

### `Data`

`Data` は、`int64_t` のキーから `std::map<int64_t, int64_t>` を引く二重マップを保持します。

- `setMap`: ノードを外側のマップへ登録
- `setParams`: ノードへキーと値を登録
- `getMap`: 外側のマップからノードを検索
- `getParams`: ノードの値を走査して表示

`DataTest::utData` は、1000件の値を作成して登録・取得する簡易テストです。

### `SeedBinary`

`SeedBinary` は、最大1024バイトの `Binary` ブロックを整数インデックスで管理します。

- `setBinary`: ブロックを登録
- `getBinary`: 指定インデックスのブロックを取得
- `getFileIndex`: 登録済みインデックス数を取得

内部では `std::map<int, Binary>` を使い、ファイルを複数ブロックとして扱います。

### `io::FileManager`

`FileManager` はファイルと `SeedBinary` の変換を担当します。

1. `load` で入力ファイルをバイナリ読み込み
2. 1024バイト単位で `SeedBinary` に格納
3. `save` で各ブロックを順に出力

`src/main.cpp` は `INPUT OUTPUT` の2引数を受け取り、ファイルを読み込んで別パスへ書き出します。ユーザー固有の絶対パスは使用しません。

## ゲーム状態処理

### `Status`

`Status` はプレイヤーのHP・MPを保持します。

- `setHp` / `setMp`: 値を設定
- `gainHp` / `gainMp`: 値を加算
- `getHp` / `getMp`: 値を取得

### `Buff`

`Buff` はID、名称、有効期間、開始時刻、効果用の `Status` を保持します。`Player` のバフ一覧に格納され、処理時に効果を参照します。

### `Player`

`Player` は次の状態を所有します。

- プレイヤーID
- 名前バッファ
- `Status`
- `Buff` のリスト
- `Position`

`processBuffs` はバフを走査し、各バフのHP・MP効果をプレイヤーへ加算します。

### `Position`

`Position` はX・Y・Z座標、マップID、プレイヤーIDを保持します。絶対座標設定と相対移動のAPIを持ちます。

### `Action` と `Field`

`Action` は、アクション種別、実行元・実行先プレイヤー、適用する `Status` をまとめます。

`Field` はシングルトンとして動作し、次のキューとプレイヤー一覧を管理します。

- プレイヤー一覧
- 位置更新キュー
- アクションキュー

`processFrame` は位置キューとアクションキューを処理し、フィールド状態を更新する想定です。

### `Connection`

`Connection::open` はTCPポート12345のリスナーを初期化する試作APIです。接続受付とHTTP応答は未実装のため、リスナーを準備して終了します。

## 起動経路

`src/main.cpp` はファイル変換だけを担当します。テスト用コードは `tests/` に分離し、製品の起動経路から参照しません。

## 現状の注意点

今回の整理で修正した主な不具合は次のとおりです。

- `Data::getMap` / `getParams` が取得値を出力引数へ返すよう修正
- `Status` のHP/MP設定と負値処理を修正
- `Position::getPlayerId`、`Field::setPlayer`、アクションキューの停止要因を修正
- `Buff` を値メンバー化し、`Player` のコピー・破棄と `Action` の値保持を安全化
- `Connection::open` の無限ループを除去
- `FileManager.cpp` を標準 `<fstream>` APIへ移行
- `main.cpp` を引数ベースの変換CLIへ整理

なお、`Connection` の接続受付、`Field::gainStatus`、`unsetPlayer` などは仕様未確定のため、今回の範囲では再設計していません。

これらは、機能追加やビルド修正を行う前に、仕様を確認してから個別に扱うべき項目です。
