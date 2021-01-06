#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

char  *strtrim(char *str) {
  if (!str) { return str; }
  // We assume the string has no chars as first.
  bool    found_char = false;
  size_t  first_char = 0;
  size_t  last_char = 0;
  size_t  char_len = 0;

  size_t  index = 0;
  // Run as long as index contains something
  while (str[index]) {
    // Detect if we're on a char
    if (!isspace(str[index])) {
      // If we hadn't found one already
      if (!found_char) {
        // Now we have
        found_char = true;
        // It's the first one. Memorize its position.
        first_char = index;
      }
      // Anything that's a char is the last char so far.
      last_char = index;
    }
    // Go to the next index.
    index++;
  }
  // Now we've run through the whole string.
  // align non-space stuff to beginning. Zero-fill behind.
  if (found_char)
    char_len = last_char - first_char + 1;
  memmove(str, str + first_char, char_len);
  memset(str + char_len, '\0', index - char_len);
  return str;
}