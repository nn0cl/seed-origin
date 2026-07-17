# LISS-0092: カスタム再配布ライセンス

- Status: review
- Phase: phase-0-design
- Type: legal + repository governance
- Priority: high
- Parent: LISS-0041

## Decision

`nn0cl`、`sumstates.net`、`dstechnology.co.jp`が本リポジトリの権利者であることを確認した。標準OSSライセンスは採用せず、ルート`LICENSE`にカスタムライセンスを配置する。

## Permission boundary

- 無改変のソースコードおよびバイナリの再配布は許可する。
- 再配布時はライセンス・著作権表示・原リポジトリ帰属を保持する。
- 改変、翻訳、移植、fork、patch、改変バイナリ、派生物の作成・公開・配布・利用は、権利者の書面許可なしに禁止する。
- 商標、特許、個人データ、ブランド利用の権利は付与しない。

## Review note

この条件はOSI準拠のオープンソースライセンスではない。配布条件の法的有効性と、既存履歴に含まれる第三者コンポーネントの扱いは、公開範囲を拡大する前に専門家確認を行う。

ルート`LICENSE`を追加し、READMEから参照できるようにした。個別の第三者依存物は別ライセンスを優先する。
