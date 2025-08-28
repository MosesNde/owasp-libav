static int check_mvset(Mv *mvLXCol, Mv *mvCol,
                       int colPic, int poc,
                       RefPicList *refPicList, int X, int refIdxLx,
                       RefPicList *refPicList_col, int listCol, int refidxCol)
{
    int cur_lt = refPicList[X].isLongTerm[refIdxLx];
    int col_lt = refPicList_col[listCol].isLongTerm[refidxCol];
    int col_poc_diff, cur_poc_diff;
    if (cur_lt != col_lt) {
        mvLXCol->x = 0;
        mvLXCol->y = 0;
        return 0;
    }
    col_poc_diff = colPic - refPicList_col[listCol].list[refidxCol];
    cur_poc_diff = poc    - refPicList[X].list[refIdxLx];
    if (!col_poc_diff)
        col_poc_diff = 1;
    if (cur_lt || col_poc_diff == cur_poc_diff) {
        mvLXCol->x = mvCol->x;
        mvLXCol->y = mvCol->y;
    } else {
        mv_scale(mvLXCol, mvCol, col_poc_diff, cur_poc_diff);
    }
    return 1;
}