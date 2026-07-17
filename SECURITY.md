# Security Policy

## 対象

本ポリシーは、mainブランチの最新状態を対象とします。公開済みリリースや古いコミットについては、修正対象となるか個別に判断します。

## 報告方法

脆弱性は公開Issue、Pull Request、チャット、コミットメッセージへ投稿しないでください。

非公開で、以下のメールアドレスへ報告してください。

`nn0cl@dstechnology.co.jp`

GitHubの非公開Security Advisoryが利用できる場合は、そちらも使用できます。

## 報告に含める情報

- 影響を受けるコミット、ファイル、バージョン
- 再現手順または最小再現例
- 想定される影響
- 既知の緩和策
- 報告者が希望する連絡先と公開時のクレジット

秘密情報、実ユーザー情報、不要な個人情報は送らないでください。

## 優先して報告する問題

- メモリ破壊、use-after-free、double free、境界外アクセス
- socket、FrameAccumulator、バイナリ入出力の境界不備
- 未検証CommandによるWorld状態の破壊
- 認証・セッションID・ユーザーID名寄せの漏洩やなりすまし
- サーバー権威値のクライアント上書き
- 秘密情報や個人情報の意図しないログ出力

## 対応方針

受領後、再現性と影響範囲を確認し、必要に応じて最優先Issueを作成します。修正、資料、テスト資料、CodeQL結果を確認した後、公開範囲と公開時期を決定します。

報告者の協力が必要な場合は、非公開の連絡を継続します。修正前に脆弱性の詳細を公開しません。

---

# English version

The Japanese version is authoritative. This section is a translation; if the
two versions conflict, the Japanese version prevails.

## Scope

This policy covers the latest state of the `main` branch. Older releases and
commits are evaluated individually for support and remediation.

## Reporting

Do not disclose vulnerabilities in public Issues, Pull Requests, chats, or
commit messages.

Report vulnerabilities privately to:

`nn0cl@dstechnology.co.jp`

Use a private GitHub Security Advisory when available.

## Include in a report

- Affected commit, file, or version
- Reproduction steps or a minimal reproduction
- Expected impact
- Known mitigations
- A preferred contact and credit name, if desired

Do not send secrets, real user data, or unnecessary personal information.

## Priority examples

- Memory corruption, use-after-free, double free, or out-of-bounds access
- Socket, FrameAccumulator, or binary I/O boundary failures
- Unvalidated Commands that can corrupt World state
- Session ID, identity matching, impersonation, or disclosure issues
- Client overwriting server-authoritative state
- Accidental logging of secrets or personal data

## Response

After receipt, the owner will assess reproducibility and impact and create a
highest-priority Issue when appropriate. The fix, documentation, test artifacts,
and CodeQL result are reviewed before deciding the disclosure scope and timing.
Vulnerability details are not made public before remediation.
