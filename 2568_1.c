unsigned int execute_query(char *user_input) {
    char query[256];
    sprintf(query, "SELECT * FROM data WHERE info = '%s'", user_input);
    return run_query(query);
}