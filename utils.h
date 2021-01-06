#ifndef __STRING_H__
# define __STRING_H__

# define WHITESPACE_CHARSET " \t\v\r\n\f"

# define streq(s1, s2) strcmp(s1, s2) == 0

bool  is_whitespace(char c);

char  *strtrim(char *str);

#endif