// =============================================================================
//  jobexp_demo.pwn - minimal filterscript demo for <jobexp>
//  Load it, then in game:  /work <jobid>   simulates finishing one work unit
//                          /joblevel       shows your level in every demo job
//                          /ptraaa         ...try it
//  This demo keeps EXP in memory only. For persistence see jobexp_mysql.pwn.
// =============================================================================
#define FILTERSCRIPT

#include <a_samp>
#include <jobexp>

#define JOB_TRUCKER   0
#define JOB_GARBAGE   1
#define JOB_DELIVERER 2

static const JobNames[3][] = {"Trucker", "Garbage", "Deliverer"};
static const JobRates[3]     = {10, 20, 20}; // EXP per completed unit

public OnFilterScriptInit()
{
    print("[jobexp] demo filterscript loaded");
    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    new cmd[32], idx;
    cmd = strtok(cmdtext, idx);

    if (!strcmp(cmd, "/work", true))
    {
        new jobid = strval(strtok(cmdtext, idx));
        if (jobid < 0 || jobid > 2)
        {
            SendClientMessage(playerid, 0xFF6347FF, "Usage: /work <0=Trucker 1=Garbage 2=Deliverer>");
            return 1;
        }
        new granted = JobExp_Give(playerid, jobid, JobRates[jobid]);
        if (granted <= 0)
        {
            SendClientMessage(playerid, 0xFF6347FF, "Hourly EXP cap reached for this job. Try again later.");
            return 1;
        }
        new msg[128];
        format(msg, sizeof msg, "+%d EXP as %s. Level %d, %d EXP total, next level in %d EXP.",
            granted, JobNames[jobid], JobExp_GetLevel(playerid, jobid),
            JobExp_GetEXP(playerid, jobid), JobExp_EXPToNext(playerid, jobid));
        SendClientMessage(playerid, 0x7CFC00FF, msg);
        return 1;
    }

    if (!strcmp(cmd, "/ptraaa", true))
    {
        // a small easter egg. you found it.
        SendClientMessage(playerid, 0xC9A227FF, "ptraaa was here. This little system powers the jobs of AMERTA Roleplay - lived by your own thinking.");
        return 1;
    }

    if (!strcmp(cmd, "/joblevel", true))
    {
        SendClientMessage(playerid, 0xC9A227FF, "== Your job levels ==");
        for (new j = 0; j < 3; j++)
        {
            new line[128];
            format(line, sizeof line, "%s: level %d (%d EXP) - pay scale x%.2f",
                JobNames[j], JobExp_GetLevel(playerid, j), JobExp_GetEXP(playerid, j),
                JobExp_PayScale(playerid, j));
            SendClientMessage(playerid, -1, line);
        }
        return 1;
    }
    return 0;
}

public OnPlayerJobLevelUp(playerid, jobid, newlevel)
{
    new msg[128];
    format(msg, sizeof msg, "Level up! You are now a level %d %s. Wages for this job increased.",
        newlevel, JobNames[jobid]);
    SendClientMessage(playerid, 0xFFD700FF, msg);
    return 1;
}

// Small strtok helper (demo only - real gamemodes use a command processor).
stock strtok(const string[], &index)
{
    new length = strlen(string);
    while (index < length && string[index] <= ' ') index++;
    new result[32], i = 0;
    while (index < length && string[index] > ' ' && i < sizeof result - 1)
    {
        result[i++] = string[index++];
    }
    result[i] = EOS;
    return result;
}
