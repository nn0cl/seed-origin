# LISS-0147: ワールドサーバーのチャレンジ／セッションキー認証への置換

- Status: review
- Phase: phase-2-green-challenge-production
- Related branch: `feature/liss-0147-challenge-production-wiring`
- Priority: high
- Depends on: LISS-0146
- Related ADR: `docs/architecture/adr/0018-registered-player-authentication.md`

## 目的

`seed_server`の`Login` Commandを、自己申告ニックネームの無条件受理から、
`seed_auth`が発行したチャレンジキーのclaimと、正規セッションキーの発行・
検証へ置き換える。

## 受入条件（ドラフト）

- `NetworkCommand::Login`のpayloadは`seed_auth`発行の**チャレンジキー**として
  扱う。
- `seed_server`はPostgresの`player_challenges`でチャレンジキーを原子的にclaim
  し、成功した場合に`player_sessions`へ新しい正規セッションキーを登録する。
- 無効・期限切れ・**既にclaim済み（`claimed_at IS NOT NULL`）**のチャレンジキーは
  ログイン拒否とする。
- 正規セッションキーは30分TTLで発行し、Keep-Aliveによって期限を更新する。
- `seed_server`はパスワードや`users`テーブルには一切触れず、チャレンジキーの
  claim結果から`user_id`を解決する。
- `SessionRegistry::login(claimedId)`の「任意の整形済みニックネームを
  無条件で受理する」現行動作は削除する（死んだコードとして残さない）。
- 複数`seed_server`インスタンスが同じセッショントークンを検証できる
  （in-memoryに依存しない）ことをPostgres共有で確認する。

## 二段階キーのclaimと正規セッション発行（2026-07-22、ADR 0023承認済み）

- ネイティブクライアントは`seed_auth`から2分TTLの単回利用
  `ChallengeKey`を取得し、そのキーを`Login` Commandのpayloadとして送信する。
- `seed_server`はトランザクション内で`player_challenges`の有効な未claim行を
  原子的にclaimする。claimに成功した場合だけ、次のように新しい正規セッション
  キーを`player_sessions`へINSERTする。
  ```sql
  BEGIN;

  -- Lock and validate the challenge row. No row means reject the login.
  SELECT user_id
    FROM player_challenges
   WHERE challenge_key = <presented_challenge_key>
     AND claimed_at IS NULL
     AND expires_at > now()
   FOR UPDATE;

  -- Consume the challenge, then create the independent regular session.
  UPDATE player_challenges
     SET claimed_at = now()
   WHERE challenge_key = <presented_challenge_key>;

  INSERT INTO player_sessions
      (session_token, user_id, created_at, expires_at, revoked_at)
  VALUES
      (<new_player_session_key>, <claimed_user_id>, now(),
       now() + interval '30 minutes', NULL);

  COMMIT;
  ```
- claimが失敗した場合は、無効・期限切れ・既にclaim済みのいずれかとして
  ログイン拒否する。同じチャレンジキーによる後続試行は成功しない。
- 正規セッションキーは以後の通信とKeep-Aliveに使用し、LISS-0122の再接続は
  有効な正規キーを保持した一時切断からの復帰として扱う。チャレンジキーは
  再接続には使用しない。
- LISS-0122の再接続は、ワールドログイン成功時に発行された
  **30分TTLの正規セッションキー**を使って一時切断から復帰する前提とする。
  チャレンジキー（`seed_auth`発行の2分TTL）は一度きりで再接続には使わない。

## Phase 1 設計着手（2026-07-22、ADR 0023承認済み）

`seed_server`は`ChallengeKey`を検証した後、30分TTLの
`PlayerSessionKey`を発行する。以降の通信は正規セッションキーを使い、
クライアント起点のKeep-AliveでTTLを更新・延長する。LISS-0122の再接続は、
有効な正規キーを保持した一時切断からの復帰として扱う。

Phase 1では、チャレンジキーclaim、正規キー発行、期限切れ拒否、
Keep-Aliveによる期限延長、再接続時のSnapshot要求をRedテストとして定義する。

## 設計課題

- `LoginCommandHandler`／`SessionRegistry`をどこまで作り替えるか
  （新規`PostgresPlayerSessionValidator`ポートを追加するか、既存クラスを
  改修するか）。
- チャレンジキーと正規セッションキーの保存・検証ポートの具体的な分割。

## Phase 1 Red artifact（2026-08-20）

- Ports / UseCase contract: `include/seed/ChallengeSessionLogin.h`
- Tests: `tests/ChallengeSessionLoginTest.cpp`（5 scenarios）
- Covered: valid claim → 30 min session; expired / already-claimed reject;
  Keep-Alive extend; reconnect validation of unexpired `PlayerSessionKey`
- Out of this Red: Login Command wire swap, Postgres adapters, anonymous
  `SessionRegistry::login` removal（Phase 2 wiring / later Green slice）
- Expected Red: link failure（`ChallengeSessionLoginService` 未実装）

## Phase 2 Green artifact（2026-08-20）

- Implementation: `src/ChallengeSessionLogin.cpp`
- Behavior: claim via port → issue session key → create 30 min session;
  reject with `invalid_challenge`; keep-alive extends by 30 min from now;
  `validateSession` delegates to `PlayerSessionStorePort::isActive`
- Still out of scope: Login Command wire swap, Postgres adapters, anonymous
  login removal

## Phase 3 Refactor（2026-08-20）

- Ports を `ChallengeSessionPorts.h` へ分離（サービス契約とポート境界の分離）
- 結果構築と TTL 計算を private ヘルパーへ抽出（挙動不変）
- Assertions / public API は変更なし

## UseCase slice completion（2026-08-20）

- PR #4 merged to main (`4436d66`). UseCase claim/keep-alive/validate is on main.
- Remaining LISS-0147 work is the Login Command wire + gameplay session binder
  + eventual anonymous login removal / Postgres adapters.

## Phase 1 Red — Login wire（2026-08-20）

- Contract: `include/seed/ChallengeLoginCommandHandler.h`
- Tests: `tests/ChallengeLoginCommandHandlerTest.cpp`（3 scenarios）
- Covered: Login payload = ChallengeKey; invalid challenge reject; client
  internalId rejected before claim
- Expected Red: link failure（handler 未実装）
- Out of this Red: ServerCommandDispatcher swap, Postgres adapters, deleting
  anonymous `LoginCommandHandler` / `SessionRegistry::login(claimedId)`

## Phase 2 Green — Login wire（2026-08-20）

- Implementation: `src/ChallengeLoginCommandHandler.cpp`
- Behavior: validate Login Command → `loginWithChallenge(payload)` →
  `GameplaySessionPort::openAuthenticated(userId)` → return player session key
- Still out of scope: dispatcher swap, Postgres adapters, anonymous login removal

## Phase 3 Refactor — Login wire（2026-08-20）

- `GameplaySessionPort` を `GameplaySessionPort.h` へ分離
- `rejected` / `accepted` 結果構築をメンバヘルパーへ整理（挙動不変）

## Wire slice Adjudicator approval（2026-08-20）

- Phase 1–3 Login wire (`ChallengeLoginCommandHandler`) approved.
- Remaining for full LISS-0147 closure: ServerCommandDispatcher swap,
  Postgres adapters, anonymous `LoginCommandHandler` /
  `SessionRegistry::login(claimedId)` removal.

## Phase 1 Red — Dispatcher challenge login（2026-08-20）

- Tests: `tests/ServerCommandDispatcherChallengeLoginTest.cpp`
- Covered: dispatch Login with valid ChallengeKey; reject nickname when
  challenge auth is bound
- Expected Red: link failure（challenge dispatcher constructor 未実装）
- Anonymous `ServerCommandDispatcher(registry)` path remains for existing tests
  until Green migrates runtime wiring

## Phase 2 Green — Dispatcher challenge login（2026-08-20）

- New constructor binds `ChallengeSessionLoginService` + `GameplaySessionPort`
- Login dispatch uses `ChallengeLoginCommandHandler` when bound; otherwise the
  existing anonymous `LoginCommandHandler`
- `CommandDispatchResult.playerSessionKey` populated on challenge login

## Phase 3 Refactor — Dispatcher challenge login（2026-08-20）

- `dispatchLogin` / `usesChallengeLogin` を抽出（挙動不変）
- 匿名 constructor 経路は既存テスト互換のため残置

## Dispatcher slice Adjudicator approval（2026-08-20）

- Phase 1–3 dispatcher challenge Login wiring approved.
- Remaining for full LISS-0147: ServerRuntime bind, Postgres adapters,
  anonymous `LoginCommandHandler` / `SessionRegistry::login(claimedId)` removal.

## Phase 1 Red — Runtime LoginResponse session key（2026-08-20）

- Tests: accepted LoginResponse may carry PlayerSessionKey payload;
  `processFrame` writes that payload after challenge Login
- Expected Red: codec validation rejects non-empty Accepted payload; runtime
  still sends empty payload on accept
- Out of this Red: Postgres adapters, anonymous login deletion, ServerMain
  production constructor (follows codec/runtime Green)

## Phase 2 Green — Runtime LoginResponse session key（2026-08-20）

- Accepted LoginResponse may include a PlayerSessionKey payload (empty still valid)
- `processClientFrames` copies `CommandDispatchResult.playerSessionKey` into
  the accepted LoginResponse payload

## Phase 3 Refactor — Runtime LoginResponse session key（2026-08-20）

- `loginResponseFromDispatch` を抽出（挙動不変）

## Runtime slice Adjudicator approval（2026-08-20）

- Phase 1–3 Runtime LoginResponse session key approved.
- Remaining for full LISS-0147: ServerMain production bind, Postgres adapters,
  anonymous `LoginCommandHandler` / `SessionRegistry::login(claimedId)` removal.

## Phase 1 Red — ServerMain bootstrap bind（2026-08-20）

- Contract: `include/seed/ServerBootstrap.h`
- Tests: `tests/ServerBootstrapTest.cpp`（3 scenarios）
- Covered: `createCommandDispatcher` selects challenge path when
  `ChallengeAuthBundle` is provided; anonymous path when nullptr;
  `SEED_CHALLENGE_AUTH` env gate
- Expected Red: link failure（`ServerBootstrap` 未実装）
- Out of this Red: Postgres adapters, `ServerMain.cpp` wiring,
  anonymous login removal

## Phase 2 Green — ServerMain bootstrap bind（2026-08-20）

- Implementation: `src/ServerBootstrap.cpp`
- `ServerMain.cpp` uses `ServerBootstrap::createCommandDispatcher` (anonymous
  path when `ChallengeAuthBundle` is nullptr)
- `SEED_CHALLENGE_AUTH=1` without Postgres challenge adapters fails fast at
  startup with a clear error
- Still out of scope: Postgres adapters, anonymous login removal, full challenge
  production wiring when env is set

## Phase 3 Refactor — ServerMain bootstrap bind（2026-08-20）

- `createProductionCommandDispatcher` を抽出（env gate + anonymous dispatcher）
- `ServerMain.cpp` は production bootstrap 呼び出しのみ（挙動不変）

## ServerMain slice Adjudicator approval（2026-08-20）

- Phase 1–3 ServerMain bootstrap bind approved.
- Remaining for full LISS-0147: Postgres adapters, challenge production wiring
  when `SEED_CHALLENGE_AUTH` is set, anonymous login removal.

## Phase 1 Red — Postgres challenge/session adapter（2026-08-20）

- Contract: `include/seed/PostgresPlayerSessionStore.h`
- DDL: `db/migrations/0003_player_challenges_sessions.sql`
- Tests: `tests/PostgresPlayerSessionStoreTest.cpp`（6 scenarios, `seed_postgres_tests`）
- Covered: atomic claim; expired / already-claimed reject; UseCase creates
  `player_sessions` row; extend; expired `isActive` reject
- Expected Red: link failure（`PostgresPlayerSessionStore` 未実装）
- Out of this Red: `ServerBootstrap` challenge production wiring, anonymous
  login removal

## Phase 2 Green — Postgres challenge/session adapter（2026-08-20）

- Implementation: `src/PostgresPlayerSessionStore.cpp`
- `claim` uses `SELECT ... FOR UPDATE` + `claimed_at` update in one transaction
- `create` / `extend` / `isActive` use parameterized SQL on `player_sessions`
- Still out of scope: `ServerBootstrap` challenge production wiring when env is
  set, anonymous login removal

## Phase 3 Refactor — Postgres challenge/session adapter（2026-08-20）

- active session predicate を private helper constant へ抽出（挙動不変）
- Postgres integration tests を `tests/PostgresTestMain.cpp` に集約

## Postgres adapter slice Adjudicator approval（2026-08-20）

- Phase 1–3 Postgres challenge/session adapter approved.
- Remaining for full LISS-0147: challenge production wiring when
  `SEED_CHALLENGE_AUTH` is set, anonymous login removal.

## Phase 1 Red — Challenge production wiring（2026-08-20）

- Contracts: `RegistryGameplaySessionPort`, `SystemWallClock`,
  `RandomSessionKeyIssuer`; `SessionRegistry::openAuthenticatedSession`;
  `ChallengeProductionTestHook` on `ServerBootstrap`
- Tests: `production_dispatcher_uses_challenge_login_when_env_and_test_hook_provided`
- Covered: `SEED_CHALLENGE_AUTH=1` + test hook selects challenge Login path;
  fail-fast without Postgres when hook is absent
- Expected Red: assertion failure（test hook ignored; production bootstrap still
  fail-fast）
- Out of this Red: production port implementations, anonymous login removal

## Phase 2 Green — Challenge production wiring（2026-08-20）

- Implementation: `RegistryGameplaySessionPort`, `SystemWallClock`,
  `RandomSessionKeyIssuer`, `SessionRegistry::openAuthenticatedSession`
- `ServerBootstrap::createProductionCommandDispatcher` selects challenge Login
  when `SEED_CHALLENGE_AUTH=1` and a complete test hook is provided
- Without a hook/Postgres wiring, production bootstrap still fails fast
- Still out of scope: live Postgres-backed production bootstrap, anonymous
  login removal

## Phase 3 Refactor — Challenge production wiring（2026-08-20）

- test hook validation を helper へ抽出（挙動不変）
- hook 用 `ChallengeSessionLoginService` の保持を static helper に明示化

## Challenge production wiring Adjudicator approval（2026-08-20）

- Phase 1–3 challenge production wiring approved.
- Remaining for full LISS-0147: live Postgres-backed production bootstrap,
  anonymous login removal.

## Remaining decisions

- TTL、Keep-Alive、再接続の基本方針は確定済み。Phase 1 Redのテストレビュー後に
  Phase 2 Greenへ進む。

## English

Phase 1 Red design for replacing seed_server's anonymous claimed-ID login with
Postgres-backed challenge claim and session-key validation, resolving a user_id
without the world server ever seeing credentials.
