# samp-jobexp

Job EXP & Level system for SA-MP / open.mp roleplay servers. Per-(player, job) experience, levels 1-10 derived from EXP, an anti-farm hourly cap, and a wage pay scale that rewards players who stick with a job.

Built and battle-tested as part of [AMERTA Roleplay](https://amerta.pages.dev), a live Indonesian roleplay server, then rewritten clean and standalone so any server can drop it in.

## Features

- EXP tracked separately for every job, for every player
- Levels 1-10 are always derived from EXP (thresholds 0 / 100 / 250 / 450 / 700 / 1000 / 1400 / 1900 / 2500 / 3200), so level and EXP can never disagree
- Rolling hourly cap (default 300 EXP per job) so players cannot bot one job to max level in an afternoon
- Pay scale: level 1 pays base wage, +2% per level, capped at +18% on level 10
- `OnPlayerJobLevelUp` callback for messages, bonuses, or celebrations
- No storage lock-in: EXP lives in memory, you persist it with your own stack. A ready MySQL (R41+) example is included

## Install

1. Copy `jobexp.inc` into your `pawno/include` folder (or your project's include path).
2. In your gamemode: `#include <jobexp>`
3. Wire persistence: load with `JobExp_SetEXP` after login, save `JobExp_GetEXP` whenever you save the player. See `examples/jobexp_mysql.pwn` for a complete MySQL implementation.
4. Try `examples/jobexp_demo.pwn` as a filterscript to see it working in two minutes (`/work <jobid>`, `/joblevel`).

## Usage

```pawn
// One unit of work finished (a delivered package, a swept route, ...)
JobExp_Give(playerid, JOB_TRUCKER, 10);

// Pay wages scaled by the player's level in that job
new pay = floatround(BASE_WAGE * JobExp_PayScale(playerid, JOB_TRUCKER));
GivePlayerMoney(playerid, pay);
```

Tuning, before the include:

```pawn
#define JOBEXP_MAX_JOBS   32   // default 32
#define JOBEXP_HOURLY_CAP 300  // default 300
#include <jobexp>
```

## Design notes

- Call `JobExp_Give` once per completed work unit, never per second or per checkpoint, or the hourly cap stops meaning anything.
- The cap window is per player and per job, so switching jobs is a legitimate way to keep progressing, which is the behaviour you want on a roleplay server.
- Level is never stored anywhere. Anywhere you need it, derive it from EXP with `JobExp_GetLevel`. One source of truth, zero drift bugs.

## Indonesian summary

Sistem EXP dan level per pekerjaan untuk server roleplay SA-MP / open.mp. Setiap pekerjaan punya EXP sendiri, level 1-10 dihitung selalu dari EXP, ada batas EXP per jam supaya tidak bisa di-farm, dan gaji naik sampai +18% di level 10. Penyimpanan bebas mengikuti gamemode Anda, contoh MySQL lengkap tersedia di folder examples.

## License

MIT. Use it on your server, free or commercial. A credit line is appreciated but not required.

Created by ptraaa.

## Support

If this project helped you, you can support its development on Trakteer: https://trakteer.id/adel_putra2

Kalau proyek ini membantu Anda, Anda bisa mendukung pengembangannya lewat Trakteer: https://trakteer.id/adel_putra2
