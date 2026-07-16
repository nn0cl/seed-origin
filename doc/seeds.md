# Seeds UML model

## 概要

`seeds.asta.bak` に保存されていたUMLモデルの内容を、Markdownで参照できる設計資料として整理したものです。

元ファイルは通常のテキストやソースコードではなく、ZIPコンテナ内にJavaシリアライズ形式の `EntityStore` を格納したJUDE/astah系のモデルバックアップでした。そのため、ここではアーカイブから確認できたモデル情報を文書化しています。

## モデル情報

- モデル名: `Seeds`
- 格納データ: `EntityStore`
- モデル形式: Java serialization version 5
- 確認できたJUDE Community履歴: 6.4.1、6.5
- 図: Class Diagram

## 確認できた主要要素

モデル内には、次のクラス・設計要素が含まれていました。

- `Data`
- `SeedBinary`
- `DiskManager`
- `MemoryManager`
- `IOManager`
- `SocketManager`
- `Manager`
- `DataMap`
- `Data`

`SeedBinary` には、バイナリデータ、`getBinary`、`int64_t` などの属性・操作情報が確認できます。

## 注意事項

このMarkdownは、バイナリUMLモデルから識別できた情報を人間が参照できる形に整理したものです。図の座標、表示設定、内部ID、関連の完全な属性値など、モデルファイル内に存在するすべての情報を完全復元したものではありません。

完全なUML図を再編集する場合は、元のJUDE/astah互換ツールが必要です。
