# LISS-0064: クライアント技術選定・アプリシェル

- Status: proposed
- Phase: phase-0-design
- Priority: medium
- Parent: LISS-0041
- Depends on: LISS-0055
- Related ADR: `docs/architecture/adr/0020-game-client-mvp-godot.md`
  （MVP/デモ範囲のみGodot 4.7 + GDScript、Godot MVPとは別系統）、
  `docs/architecture/adr/0021-production-game-client-unreal-engine.md`
  （本番クライアントはUnreal Engineに決定、2026-07-18時点で未承認）

対象OS、C++/Swift等の技術、3Dレンダラー、通信層、入力層、アセット配置、ログ出力、ビルド配布方式を決定し、接続可能な空アプリを作る。

## 決定事項（2026-07-18）

本番クライアントのエンジンは**Unreal Engine**に決定した（ADR 0021）。
Godot MVP（ADR 0020）は検証用の使い捨てであり、本番クライアントへ
進化させるものではない。Unreal Engineバージョン、スクリプティング方針
（C++/Blueprint）、リポジトリ内の配置場所（`client/`直下か別ディレクトリか）
はADR 0021のOpen Questionsとして未決定。
