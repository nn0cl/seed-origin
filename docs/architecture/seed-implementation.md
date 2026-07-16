# Seed 実装概要

## 目的と現在の構成

このリポジトリには、初期のデータ・バイナリ処理実装と、後続のゲーム状態処理実装が統合されています。現在のソースはリポジトリ直下に配置され、`seed.xcodeproj` から参照されます。

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

現在の `main.cpp` は固定されたローカルパスを使い、ファイルを読み込んで別パスへ書き出します。

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

`Connection::open` はTCPポート12345で待ち受けるHTTP応答処理の試作です。ただし、接続受付部分はコメントアウトされており、現在は待機ループから抜けません。

## 起動経路

`main.cpp` は `FileManager`、`SeedBinaryTest`、`DataTest` を参照します。現状ではファイル変換処理の後に `return` するため、後半のテスト処理は実行されません。

## 現状の注意点

以下はソース上確認できる未完成箇所または修正候補です。本文書では挙動を変更していません。

- `Data::getMap` は取得したノードを引数へコピーしていない
- `Data::getParams` の引数 `value` に取得値を設定していない
- `Status::setHp` と `setMp` は計算後に入力値で上書きしている
- `Status::setMp` はHPメンバーへ代入している
- `Position::getPlayerId` は自身を再帰呼び出ししている
- `Field::setPlayer` は自身を再帰呼び出ししている
- `Field::processFrame` の一部キューが消費されず、該当プレイヤー未存在時の検査もない
- `Buff`、`Player`、`Action` の所有権管理が生ポインタ中心で、コピー時の二重解放リスクがある
- `Connection::open` は無限ループになっている
- `FileManager.cpp` は古い `<fstream.h>` に依存している
- `main.cpp` の入出力パスがユーザー固有の絶対パスになっている

これらは、機能追加やビルド修正を行う前に、仕様を確認してから個別に扱うべき項目です。
