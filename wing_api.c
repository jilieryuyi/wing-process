
zval *wing_zend_read_property(zend_class_entry *scope, zval *object, const char *name)
{
	TSRMLS_FETCH();
#if PHP_MAJOR_VERSION >= 7
	return zend_read_property(scope, object, name, strlen(name), 0, 0 TSRMLS_CC);
#else
	return zend_read_property(scope, object, name, strlen(name), 0 TSRMLS_CC);
#endif
}


/**
* ?��?????php??????????????php???????? <?php ???
*
* @param char* file
* @return BOOL
*/
int wing_file_is_php(const char *file)
{

    char find_str[]     = " ";
    char path[MAX_PATH] = {0};

    char *find          = strstr(file, find_str);
    if (find != NULL)
    strncpy((char*)path, file, (size_t)(find-file));
    else
    strcpy((char*)path, file);

    FILE *handle = fopen(path, "r");
    if (!handle) {
        return 0;
    }
    //char *find = NULL;
    char line[8] = { 0 };
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



//int wing_write_cmdline(unsigned long process_id, char *cmdline)
//{
//    char buffer[MAX_PATH];
//    sprintf(buffer, "/proc/%lu/cmdline", process_id);
//    if (access(buffer, F_OK) == 0) {
//        //linux处理
//        return 1;
//    }
    /*char tmp[MAX_PATH] = {0};
    wing_get_tmp_dir((char**)&tmp);
    char path[MAX_PATH] = {0};
    strcpy(path, tmp);
    strcpy((char*)(path+strlen(tmp)), "/");
    char _process_id[32] = {0};
    sprintf(_process_id, "%lu", process_id);
    strcpy((char*)(path+strlen(tmp)+1), _process_id);

    if (access(path, F_OK) != 0) {
        if (0 != mkdir(path, 0777)) {
            return 0;
        }
    }

    strcpy((char*)(path+strlen(tmp)+1+strlen(_process_id)), "/cmdline");

    FILE *handle = fopen((const char*)path, "w");
    if (handle) {
        if (1 != fwrite(cmdline, strlen(cmdline), 1, handle)) {
            fclose(handle);
            //写入失败
            return 0;
        }
        fclose(handle);
        return 1;
    }
    return 0;*/
//}
#ifdef __APPLE__

void wing_get_cmdline(int pid, char **buffer) {
    int    mib[3], argmax, nargs, c = 0;
    size_t    size;
    char    *procargs, *sp, *np, *cp;
    int show_args = 1;

    //fprintf(stderr, "Getting argv of PID %d\n", pid);

    mib[0] = CTL_KERN;
    mib[1] = KERN_ARGMAX;

    size = sizeof(argmax);
    if (sysctl(mib, 2, &argmax, &size, NULL, 0) == -1) {
        goto ERROR_A;
    }

    /* Allocate space for the arguments. */
    procargs = (char *)malloc(argmax);
    if (procargs == NULL) {
        goto ERROR_A;
    }


    /*
     * Make a sysctl() call to get the raw argument space of the process.
     * The layout is documented in start.s, which is part of the Csu
     * project.  In summary, it looks like:
     *
     * /---------------\ 0x00000000
     * :               :
     * :               :
     * |---------------|
     * | argc          |
     * |---------------|
     * | arg[0]        |
     * |---------------|
     * :               :
     * :               :
     * |---------------|
     * | arg[argc - 1] |
     * |---------------|
     * | 0             |
     * |---------------|
     * | env[0]        |
     * |---------------|
     * :               :
     * :               :
     * |---------------|
     * | env[n]        |
     * |---------------|
     * | 0             |
     * |---------------| <-- Beginning of data returned by sysctl() is here.
     * | argc          |
     * |---------------|
     * | exec_path     |
     * |:::::::::::::::|
     * |               |
     * | String area.  |
     * |               |
     * |---------------| <-- Top of stack.
     * :               :
     * :               :
     * \---------------/ 0xffffffff
     */
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROCARGS2;
    mib[2] = pid;


    size = (size_t)argmax;
    if (sysctl(mib, 3, procargs, &size, NULL, 0) == -1) {
        goto ERROR_B;
    }

    memcpy(&nargs, procargs, sizeof(nargs));
    cp = procargs + sizeof(nargs);

    /* Skip the saved exec_path. */
    for (; cp < &procargs[size]; cp++) {
        if (*cp == '\0') {
            /* End of exec_path reached. */
            break;
        }
    }
    if (cp == &procargs[size]) {
        goto ERROR_B;
    }

    /* Skip trailing '\0' characters. */
    for (; cp < &procargs[size]; cp++) {
        if (*cp != '\0') {
            /* Beginning of first argument reached. */
            break;
        }
    }
    if (cp == &procargs[size]) {
        goto ERROR_B;
    }
    /* Save where the argv[0] string starts. */
    sp = cp;

    /*
     * Iterate through the '\0'-terminated strings and convert '\0' to ' '
     * until a string is found that has a '=' character in it (or there are
     * no more strings in procargs).  There is no way to deterministically
     * know where the command arguments end and the environment strings
     * start, which is why the '=' character is searched for as a heuristic.
     */
    for (np = NULL; c < nargs && cp < &procargs[size]; cp++) {
        if (*cp == '\0') {
            c++;
            if (np != NULL) {
                /* Convert previous '\0'. */
                *np = ' ';
            } else {
                /* *argv0len = cp - sp; */
            }
            /* Note location of current '\0'. */
            np = cp;

            if (!show_args) {
                /*
                 * Don't convert '\0' characters to ' '.
                 * However, we needed to know that the
                 * command name was terminated, which we
                 * now know.
                 */
                break;
            }
        }
    }

    /*
     * sp points to the beginning of the arguments/environment string, and
     * np should point to the '\0' terminator for the string.
     */
    if (np == NULL || np == sp) {
        /* Empty or unterminated string. */
        goto ERROR_B;
    }

    *buffer = procargs;
    size = (size_t)argmax;
    memset(*buffer, 0, size);
    /* Make a copy of the string. */
    strcpy(*buffer, sp);//, MAX_PATH-1);

    /* Clean up. */
   // free(procargs);
    return;

ERROR_B:
    free(procargs);
    *buffer = NULL;
    procargs = NULL;
ERROR_A:
    *buffer = NULL;
    //fprintf(stderr, "Sorry, failed\n");
    //exit(2);
}


#else
void wing_get_cmdline(int process_id, char **buffer)
{
    *buffer = NULL;
    char file[MAX_PATH] = {0};
    sprintf(file, "/proc/%d/cmdline", process_id);
    if (access(file, R_OK) != 0) {
        return;
    }


    FILE *handle = fopen((const char*)file, "r");
    if (!handle) {
        *buffer = NULL;
        return;
    }

   // fseek(handle, 0L, SEEK_END);
    int filesize = 0;//ftell(handle);
    while(!feof(handle)){
    getc(handle);filesize++;
    }
    if (filesize <= 0) {
        fclose(handle);
        return;
    }
    rewind(handle);

    *buffer = (char*)malloc(filesize+1);
    if (*buffer == NULL) {
        fclose(handle);
        return;
    }
    memset(*buffer,0,filesize+1);

    int c;
    char *cs;
    cs = *buffer;

    while(!feof(handle)) {
        c = getc(handle);
        if (!c || c == NULL || c < 32) {
            c = ' ';
        }
        *cs++ = c;
    }
    *cs='\0';
    fclose(handle);

}
#endif
