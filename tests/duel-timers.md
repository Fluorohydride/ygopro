# Host timer policy

The host process reads these settings from `system.conf` (or `load-once.conf`)
and snapshots them when each duel starts. Joining clients cannot select them.
The wire protocol and its confirmation-before-response requirement are unchanged.

```ini
duel_timer_lenient = 0
duel_timer_grace = 10
```

Strict mode is the default. Each side has a separate presentation allowance equal
to the room's initial time limit, for the entire duel. Tag partners share their
side's allowance. It does not refill on a new turn or retry prompt. Up to
`duel_timer_grace` seconds (clamped to 0–60) of the remaining allowance can be
spent before confirmation on one prompt. Every such second consumes the
allowance; after it runs out, the server charges the decision bank even if the
client withholds `CTOS_TIME_CONFIRM`. Confirmation ends grace immediately and
does not reset time already charged. Set grace to zero to charge immediately.

Thus a prompt's latest deadline is the remaining decision bank plus the smaller
of its grace cap and remaining presentation allowance, measured from the server
request. A confirmation can shorten that deadline but cannot extend it. This
still grants a finite amount of time a modified client could use for thinking;
the server cannot prove that a client is displaying an animation.

With `duel_timer_lenient = 1`, each prompt gets up to 60 seconds of grace,
with no cumulative limit. The server starts charging the decision bank when
the client confirms or when those 60 seconds have elapsed, whichever comes
first. For example, a player with three seconds left who never confirms gets
60 free ticks and loses on tick 63. A new prompt has its own grace period.

Timing retains the existing one-second server ticks. These allowances therefore
use the same tick granularity as the decision bank. Untimed duels are unchanged.
Both modes reject duplicate/wrong-player confirmations, and an exhausted bank
expires on confirmation before a queued response can be processed.

Run `python tests/check_duel_timers.py --output-dir <temporary-directory>` with
a C++17 compiler (`CXX` is supported), or pass `--msbuild <MSBuild.exe>` on Windows.
The regression compiles the actual initialization, waiting, confirmation,
response-accounting and timeout methods against controlled hosts. It covers
strict deadlines without confirmation, early/late/duplicate confirmation,
cumulative exhaustion across prompts, zero grace, zero/maximum banks, team
sharing, policy snapshots, lenient grace across prompts and inactive/untimed sessions.
It is not a complete GUI build or socket integration test.
