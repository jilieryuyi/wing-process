#include "wing_api.h"

/**
 * 判断是否为php文件，此方法依据php文件开头的 <?php 识别
 *
 * @param char* file
 * @return BOOL
 */
int file_is_php(const char *file)
{
    FILE *handle = fopen(file, "r");
    if (!handle) {
        return 0;
    }
    char *find = NULL;
    char line[8] = {0};
    memset(line, 0, 8);
    fgets(line, 7, handle);

    find = strstr(line, "<?php");
    if (find == line) {
        fclose(handle);
        return 1;
    }

    memset(line, 0, 8);
    fgets(line, 7, handle);
    find = strstr(line, "<?php");
    if (find == line) {
        fclose(handle);
        return 1;
    }
    fclose(handle);

    return 0;
}
