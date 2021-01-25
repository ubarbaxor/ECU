#ifndef __CAS_STATES__
# define __CAS_STATES__

# ifndef RISING
#  define RISING 1
# endif

# ifndef FALLING
#  define FALLING -1
# endif

# ifndef STABLE
#  define STABLE 0
# endif

typedef struct s_cas_state {
  int shape = STABLE;
  int sample = 0;
  int tick[3] = {0,0,0};
} cas_state;

cas_state cas_tick(cas_state last, int cas_sample);
int       cas_calibration(int pin, bool log_cas);
void      print_cas_state(cas_state to_print);

#endif