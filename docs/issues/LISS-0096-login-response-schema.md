# LISS-0096: LoginResponse schemaとframe codec

- Status: review
- Phase: phase-1-red
- Type: architecture + security
- Priority: critical
- Parent: LISS-0095
- Depends on: LISS-0055, LISS-0074, LISS-0094

## 目的

Login成功・拒否を固定長ヘッダー付きのversioned frameとして表現し、応答の検証と
エンコード／デコードを通信境界へ追加する。

## 受入条件

- Acceptedは正の接続セッションIDを持ち、エラーpayloadを持たない。
- RejectedはセッションIDを持たず、空でない拒否理由を持つ。
- 未知のstatus、異なるprotocol version、サイズ超過、identity不整合を拒否する。
- 部分フレームをcodecが受け入れず、既存のFrameAccumulatorへ渡せる形式である。
- codecはWorld状態、SessionRegistry、ソケットを直接変更しない。

## 識別子に関する暫定方針

現行Protocol v1では、Login後のCommandに必要な接続セッションIDを応答へ含める。
これは認証済みの恒久IDではなく、仮セッションの接続用IDである。外部公開IDと内部IDを
分離する変更はLISS-0095の契約確定後に行う。

## 実装資料

`LoginResponse`、応答frame codec、成功・拒否・不正identityの受入テスト資料と
CMake登録を追加した。テスト・ビルドは実行していない。
