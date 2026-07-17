# LISS-0106: World frameの決定論的適用

- Status: review
- Phase: phase-1-red
- Type: feature + safety
- Priority: critical
- Parent: LISS-0023
- Depends on: LISS-0105, LISS-0020, LISS-0022

## 目的

検証済みのFrameActionsを既存FieldのAction処理境界へ渡し、WorldUpdate EventとWorld状態
変更の順序を固定する。Event生成を先に完了させ、生成失敗時にはFieldを変更しない。

## 受入条件

- FrameActionsからWorldUpdate Eventを先に生成・検証する。
- 不正ActionをFieldへ投入しない。
- 有効Actionを到着順にFieldのAction queueへ投入する。
- 1フレームの投入後にFieldのprocessFrameを1回だけ呼ぶ。
- 適用結果のWorldUpdateと入力World tickが一致する。
- Field／Playerの詳細状態をEvent payloadへ勝手に追加しない。
- エーテル減衰、属性伝導率、攻撃・魔法の意味論は後続Issueで扱う。

## 実装資料

`WorldFrameApplier`を追加し、WorldUpdate生成成功後にFieldへActionを投入して1回処理する
境界を実装した。不正Actionの事前拒否と受入テスト資料を追加した。テスト・ビルドは
実行していない。

## 次のIssue

Action種別ごとの意味論（移動、攻撃、魔法）をWorld更新器へ段階的に追加する。
