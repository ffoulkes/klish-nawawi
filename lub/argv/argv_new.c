/*
 * argv_new.c
 */
#include "private.h"
#include "lub/string.h"

#include <stdlib.h>
#include <string.h>

/*--------------------------------------------------------- */
static char * unescape_special_chars(const char *str)
{
    char *res = NULL;
    char *s = NULL;

    for (s = strchr(str, '\\'); s; s = strchr(str, '\\')) {
        lub_string_catn(&res, str, s - str);
        str = s + 1;
    }
    lub_string_cat(&res, str);

    return res;
}

/*--------------------------------------------------------- */
static void
lub_argv_init(lub_argv_t *this,
              const char *line,
              size_t      offset)
{
    size_t      len;
    const char *word;
    lub_arg_t  *arg;
    bool_t      quoted;

    /* first of all count the words in the line */
    this->argc = lub_argv_wordcount(line);

    /* allocate space to hold the vector */
    arg = this->argv = malloc(sizeof(lub_arg_t) * this->argc);

    if(arg)
    {
        /* then fill out the array with the words */
        for(word = lub_argv_nextword(line,&len,&offset,&quoted);
            *word;
            word = lub_argv_nextword(word+len,&len,&offset,&quoted))
        {
            char * tmp = lub_string_dupn(word,len);
            (*arg).arg = unescape_special_chars(tmp);
            lub_string_free(tmp);
            (*arg).offset = offset;
            (*arg).quoted = quoted;

            offset += len;

            if(BOOL_TRUE == quoted)
            {
                len    += 1; /* account for terminating quotation mark */
                offset += 2; /* account for quotation marks */
            }
            arg++;
        }
    }
    else
    {
        /* failed to get memory so don't pretend otherwise */
        this->argc = 0;
    }
}
/*--------------------------------------------------------- */
lub_argv_t *
lub_argv_new(const char *line,
             size_t      offset)
{
    lub_argv_t *this;
    
    this = malloc(sizeof(lub_argv_t));
    if(NULL != this)
    {
        lub_argv_init(this,line,offset);
    }
    return this;
}
/*--------------------------------------------------------- */
