# include "de.h"

double evaluate(int D, double tmp[], long *nfeval)
{
/* polynomial fitting problem */
  double s;
  int i;
  s = 0;
  for (i = 0; i < D; i++)
    s += tmp[i] * tmp[i];
  return s;
}
