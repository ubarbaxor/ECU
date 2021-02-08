#ifndef __CAS_STATES__
# define __CAS_STATES__

# ifndef CHANGE
#  define CHANGE 1
# endif

# ifndef FALLING
#  define FALLING 2
# endif

# ifndef RISING
#  define RISING 3
# endif

# ifndef CAS_MARGIN
#  define CAS_MARGIN 1
# endif

// Pardon, c'est sale.
# ifndef SHAPE_NAME
#  define SHAPE_NAME(shp) \
  shp == RISING ? "\"RISING\"" \
  : shp == FALLING ? "\"FALLING\"" \
  : shp == CHANGE ? "\"CHANGE\"" \
  : "\"UNKNOWN\""
# endif

typedef struct s_cas_state {
  int             shape = CHANGE;
  int             sample = 0;
  unsigned long   tick[3] = {0,0,0};
} cas_state;

cas_state cas_tick(cas_state last, int cas_sample);
int       cas_calibration(int pin, bool log_cas);
void      print_cas_state(cas_state to_print);

#endif