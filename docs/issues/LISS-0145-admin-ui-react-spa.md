# LISS-0145: 管理者UI（React SPA + SSE）

- Status: proposed
- Phase: phase-0-design-intake
- Priority: medium
- Depends on: LISS-0144

## 目的

`seed_admin`のHTTP API（`curl`でのみ操作可能な現状）に、React SPAの
ブラウザUIを追加する。管理者がログイン・エクスポート・衝突解決キューの
確認とレビュー確定を、ブラウザから行えるようにする。

## 背景・Adjudicator決定事項

- フロントエンドはReactを使用する。
- 管理用APIはSSE（Server-Sent Events）を提供し、SPAはそれを利用する
  （衝突解決キューなどのリアルタイム更新を想定していると解釈しているが、
  具体的にどの情報をSSEで配信するかは未確定 — 下記ambiguity参照）。
- Adjudicator指示: 「ISSUEを作って設計から始める」— 本Issueは設計着手の
  記録であり、Phase 1（Red）以降は別途フェーズ確認のうえ着手する。

## 本Issueで解決しないといけない設計課題（Must not guess）

1. **npm/ビルドツールチェーン選定**: このリポジトリはC++/CMakeのみで、
   Node.js系のビルドツール（Vite、webpack、Create React App等）を
   一度も導入していない。選定と`docs/architecture/dependency-policy.md`の
   adoption checklistが必要。
2. **フロントエンドの配置場所**: `AGENTS.md`の`project-structure.md`は
   `client/`をLISS-0064（ゲームクライアント技術選定）向けに予約している。
   管理者UIをそこに同居させるか、`admin-ui/`のような別ディレクトリに
   分離するかを決める必要がある（ゲームクライアントとは技術的に無関係な
   別Non-Decisionのため、混在させない方が安全と考えられる）。
3. **SSEの配信対象**: 衝突解決キューの新規追加をpushするのか、alias
   エクスポート全体の変更通知か、それとも将来のadmin監査ログ全般か。
   エンドポイント形状（例: `GET /aliases/review-queue/stream`）とペイロード
   契約を決める必要がある。
4. **認証との統合**: 現状の`Authorization: Bearer`ヘッダ方式はSSE
   （`EventSource` API）とは相性が悪い（`EventSource`は任意ヘッダを
   付与できない）。クエリパラメータでトークンを渡す、Cookie方式に
   変更する、`fetch`ベースのstreaming読み取りに置き換える等の
   代替検討が必要。Bearer-only設計（ADR 0017決定3、CSRF回避の根拠）との
   整合性を崩さない方式を選ぶこと。
5. **ビルド成果物の配信**: React SPAの静的ファイルを`seed_admin`
   （cpp-httplib）から配信するのか、別途配信するのかを決める。

## 受入条件（ドラフト、Phase 0時点の仮）

- ログイン画面、エクスポート一覧表示、衝突解決キュー表示、
  Confirm/Rejectボタンを持つ最小SPA。
- SSEまたは同等の仕組みでキューの変化が画面に反映される。
- 既存のBearerトークン認証契約（ADR 0017）と矛盾しない認証統合方式。
- 新規npm依存はdependency-policy.mdのadoption checklistを満たす。

## Remaining decisions

- 上記1〜5はすべて未決定。Adjudicatorとの設計協議、または
  Architecture Path `[THOUGHT]`スキャフォールドでの提示が次のアクション。

## English

Design intake for a React SPA admin UI on top of the LISS-0144 admin HTTP
API, with SSE for live updates. Build tooling, directory placement, SSE
payload contract, and how SSE interacts with the existing Bearer-only auth
model are all open design questions, not yet decided. No implementation has
started.
