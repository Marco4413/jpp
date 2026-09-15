#ifndef _ARGS_CPP
#define _ARGS_CPP

static const char *shift_args(int &argc, const char **&argv, const char *or_else)
{
    return argc > 0 ? (--argc, *(argv++)) : or_else;
}

#endif // _ARGS_CPP
