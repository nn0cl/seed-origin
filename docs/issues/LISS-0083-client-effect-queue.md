# LISS-0083: クライアントEffectQueue

- Status: proposed
- Phase: phase-2-integration
- Type: feature + UX
- Priority: high
- Parent: LISS-0065
- Related: LISS-0080, LISS-0082, LISS-0061

## Scope

Eventを表示用Effectへ変換し、クライアント描画フレームで再生する。魔法・攻撃・命中・エーテル変化などを独立した演出として処理する。

## Acceptance criteria

- EffectQueueはSnapshot状態更新と独立している。
- Effectごとに再生時間、優先度、対象、重複排除IDを持つ。
- 演出完了後にEffectを削除する。
- キュー詰まり時に上限、圧縮、短縮、省略方針を適用する。
- Effect再生の遅延がHP・位置・エーテルの表示更新を停止させない。
- Eventの順序はserver tick/sequenceに基づいて扱う。
