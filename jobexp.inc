// =============================================================================
//  jobexp.inc - Job EXP & Level system for SA-MP / open.mp roleplay servers
//  by ptraaa (AMERTA Roleplay) - MIT License
//
//  Per-(player, job) experience with levels 1-10 derived from EXP, an hourly
//  anti-farm cap, and a pay scale so higher job levels earn more. Storage is
//  deliberately NOT built in: the include keeps EXP in memory and exposes
//  JobExp_GetEXP / JobExp_SetEXP so your gamemode can persist it with whatever
//  it already uses (MySQL, SQLite, files). A full MySQL example ships in
//  examples/jobexp_mysql.pwn.
//
//  Quick start:
//      #include <jobexp>
//
//      // when a player finishes one unit of job 3 work:
//      JobExp_Give(playerid, 3, 10);
//
//      // when paying them, scale the wage by their level in that job:
//      new pay = floatround(1000 * JobExp_PayScale(playerid, 3));
//
//  Optional tuning (define BEFORE the include):
//      #define JOBEXP_MAX_JOBS   32    // how many job ids you use
//      #define JOBEXP_HOURLY_CAP 300   // max EXP per job per rolling hour
// =============================================================================

#if defined _jobexp_included
    #endinput
#endif
#define _jobexp_included

#if !defined JOBEXP_MAX_JOBS
    #define JOBEXP_MAX_JOBS 32
#endif

#if !defined JOBEXP_HOURLY_CAP
    #define JOBEXP_HOURLY_CAP 300
#endif

#define JOBEXP_MAX_LEVEL 10

// Cumulative EXP needed to BE each level (index 0 = level 1).
static const JobExp_Thresholds[JOBEXP_MAX_LEVEL] =
    {0, 100, 250, 450, 700, 1000, 1400, 1900, 2500, 3200};

static JobExp_EXP[MAX_PLAYERS][JOBEXP_MAX_JOBS];
static JobExp_WindowStart[MAX_PLAYERS][JOBEXP_MAX_JOBS]; // unix time the current cap window began
static JobExp_WindowEarned[MAX_PLAYERS][JOBEXP_MAX_JOBS];

// Called when a player gains a job level. Implement it in your gamemode
// (message, bonus, sound, whatever) - it is optional.
forward OnPlayerJobLevelUp(playerid, jobid, newlevel);

// -----------------------------------------------------------------------------
// Level is ALWAYS derived from EXP, never stored, so the two can never drift.
// -----------------------------------------------------------------------------
stock JobExp_LevelFromEXP(exp)
{
    new level = 1;
    for (new i = 1; i < JOBEXP_MAX_LEVEL; i++)
    {
        if (exp >= JobExp_Thresholds[i]) level = i + 1;
        else break;
    }
    return level;
}

stock JobExp_GetEXP(playerid, jobid)
{
    if (jobid < 0 || jobid >= JOBEXP_MAX_JOBS) return 0;
    return JobExp_EXP[playerid][jobid];
}

stock JobExp_GetLevel(playerid, jobid)
{
    return JobExp_LevelFromEXP(JobExp_GetEXP(playerid, jobid));
}

// EXP still needed to reach the next level, or 0 when already level 10.
stock JobExp_EXPToNext(playerid, jobid)
{
    new exp = JobExp_GetEXP(playerid, jobid);
    new level = JobExp_LevelFromEXP(exp);
    if (level >= JOBEXP_MAX_LEVEL) return 0;
    return JobExp_Thresholds[level] - exp;
}

// Wage multiplier for a player's level in a job: level 1 pays base (1.00),
// every level above adds +2%, capped at +18% on level 10.
stock Float:JobExp_PayScale(playerid, jobid)
{
    new level = JobExp_GetLevel(playerid, jobid);
    new Float:scale = 1.0 + (0.02 * float(level - 1));
    if (scale > 1.18) scale = 1.18;
    return scale;
}

// Set EXP directly (use this when loading a player from your database).
stock JobExp_SetEXP(playerid, jobid, exp)
{
    if (jobid < 0 || jobid >= JOBEXP_MAX_JOBS) return 0;
    if (exp < 0) exp = 0;
    JobExp_EXP[playerid][jobid] = exp;
    return 1;
}

// Reset one job, or every job when jobid is -1 (use on player disconnect if
// your load flow does not overwrite all slots on login).
stock JobExp_Reset(playerid, jobid = -1)
{
    if (jobid == -1)
    {
        for (new j = 0; j < JOBEXP_MAX_JOBS; j++)
        {
            JobExp_EXP[playerid][j] = 0;
            JobExp_WindowStart[playerid][j] = 0;
            JobExp_WindowEarned[playerid][j] = 0;
        }
        return 1;
    }
    if (jobid < 0 || jobid >= JOBEXP_MAX_JOBS) return 0;
    JobExp_EXP[playerid][jobid] = 0;
    JobExp_WindowStart[playerid][jobid] = 0;
    JobExp_WindowEarned[playerid][jobid] = 0;
    return 1;
}

// -----------------------------------------------------------------------------
// Give EXP for completed work. Returns the amount ACTUALLY granted, which is
// lower than requested when the rolling hourly cap bites. Call this once per
// completed work unit (one delivered package, one harvested field, ...), never
// per second or per checkpoint, or the cap loses its meaning.
// -----------------------------------------------------------------------------
stock JobExp_Give(playerid, jobid, amount)
{
    if (jobid < 0 || jobid >= JOBEXP_MAX_JOBS || amount <= 0) return 0;

    new now = gettime();
    if (now - JobExp_WindowStart[playerid][jobid] >= 3600)
    {
        JobExp_WindowStart[playerid][jobid] = now;
        JobExp_WindowEarned[playerid][jobid] = 0;
    }

    new room = JOBEXP_HOURLY_CAP - JobExp_WindowEarned[playerid][jobid];
    if (room <= 0) return 0;
    if (amount > room) amount = room;

    new oldlevel = JobExp_GetLevel(playerid, jobid);
    JobExp_EXP[playerid][jobid] += amount;
    JobExp_WindowEarned[playerid][jobid] += amount;

    new newlevel = JobExp_GetLevel(playerid, jobid);
    if (newlevel > oldlevel)
    {
        CallLocalFunction("OnPlayerJobLevelUp", "iii", playerid, jobid, newlevel);
    }
    return amount;
}

// The player's highest level across all jobs, handy for stats pages.
stock JobExp_BestLevel(playerid, &bestjob = 0)
{
    new best = 1;
    bestjob = 0;
    for (new j = 0; j < JOBEXP_MAX_JOBS; j++)
    {
        new lvl = JobExp_GetLevel(playerid, j);
        if (lvl > best)
        {
            best = lvl;
            bestjob = j;
        }
    }
    return best;
}
