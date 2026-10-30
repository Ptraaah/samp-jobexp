// =============================================================================
//  jobexp_mysql.pwn - persistence example for <jobexp> (MySQL R41+ plugin)
//  Not a standalone script: copy these functions into your gamemode and call
//  JobExpDB_Load right after your player logs in, and JobExpDB_SaveJob
//  whenever EXP changes (or save all jobs on disconnect with the loop in
//  JobExpDB_SaveAll).
//
//  Schema (run once in your database):
//
//      CREATE TABLE IF NOT EXISTS jobexp (
//          account_id INT NOT NULL,
//          job_id     INT NOT NULL,
//          exp        INT NOT NULL DEFAULT 0,
//          PRIMARY KEY (account_id, job_id)
//      );
//
//  Replace g_AccountID[playerid] with wherever your gamemode keeps the
//  logged-in account id, and MySQL_Handle with your connection handle.
// =============================================================================

// #include <a_mysql>
// #include <jobexp>

stock JobExpDB_Load(playerid)
{
    new query[128];
    mysql_format(MySQL_Handle, query, sizeof query,
        "SELECT job_id, exp FROM jobexp WHERE account_id = %d", g_AccountID[playerid]);
    mysql_tquery(MySQL_Handle, query, "JobExpDB_OnLoad", "i", playerid);
    return 1;
}

forward JobExpDB_OnLoad(playerid);
public JobExpDB_OnLoad(playerid)
{
    new rows = cache_num_rows();
    for (new i = 0; i < rows; i++)
    {
        new jobid, exp;
        cache_get_value_name_int(i, "job_id", jobid);
        cache_get_value_name_int(i, "exp", exp);
        JobExp_SetEXP(playerid, jobid, exp);
    }
    return 1;
}

stock JobExpDB_SaveJob(playerid, jobid)
{
    new query[192];
    mysql_format(MySQL_Handle, query, sizeof query,
        "INSERT INTO jobexp (account_id, job_id, exp) VALUES (%d, %d, %d) \
         ON DUPLICATE KEY UPDATE exp = VALUES(exp)",
        g_AccountID[playerid], jobid, JobExp_GetEXP(playerid, jobid));
    mysql_tquery(MySQL_Handle, query);
    return 1;
}

stock JobExpDB_SaveAll(playerid)
{
    for (new j = 0; j < JOBEXP_MAX_JOBS; j++) JobExpDB_SaveJob(playerid, j);
    return 1;
}
