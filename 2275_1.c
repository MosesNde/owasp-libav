#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sqlite3.h>

int vulnerable_query(sqlite3 *db, const char *user_input) {
    char sql[1024];
    snprintf(sql, sizeof(sql), "SELECT * FROM users WHERE username = '%s'", user_input);
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    return 0;
}