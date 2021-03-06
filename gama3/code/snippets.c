/* pomp C snippet file: snippets */
/* Time: 2020-05-23 19:04:00.351 -0400 */
/* Salt: 5BF4ACDC2464C7810197D323 */

#include <pomp.h>
#include <R_ext/Rdynload.h>


double calc_beta(double td, double a00, double a01, double a10, double a11) {
  static int *indices = NULL;
  static double *contacts = NULL;
  static int max_t = 0;
  static int num_v = 0;

  if (indices == NULL) {
    FILE *file;

    file = fopen("gama5/indices", "r");

    int idx;
    while (fscanf(file, "%d", &idx) > 0) max_t++;
    rewind(file);

    indices = (int *)malloc(sizeof(int)*max_t);
    int i = 0;
    while (fscanf(file, "%d", &idx) > 0) {
      indices[i] = idx;
      i++;
    }
    fclose(file);

    file = fopen("gama5/contacts", "r");
    float val;
    while (fscanf(file, "%f", &val) > 0) num_v++;
    rewind(file);

    contacts = (double *)malloc(sizeof(double)*num_v);
    i = 0;
    while (fscanf(file, "%f", &val) > 0) {
      contacts[i] = val;
      i++;
    }
    fclose(file);

    //Rprintf("%d %d\n", max_t, num_v);
  }

  double beta = 0;

  int t = (int) td;
  if (max_t <= t) t = max_t - 1;
  int idx = indices[t];
  int ninf = 0;
  while (-1 < contacts[idx]) {
    int ncont = (int) contacts[idx++];
    double y = contacts[idx++];
    for (int i = 0; i < ncont; i++) {
      double x = contacts[idx++];
      double p = a00 + a01 * y + a10 *x + a11 * x * y;
      beta += 1 - exp(-p);
    }
    ninf++;
  }

  if (0 < ninf) {
    beta /= ninf;
  }

  //Rprintf("%lg = %lg\n", td, beta);

  return beta;
}
 


/* C snippet: 'rinit' */
#define a00		(__p[__parindex[0]])
#define a01		(__p[__parindex[1]])
#define a10		(__p[__parindex[2]])
#define a11		(__p[__parindex[3]])
#define pop		(__p[__parindex[4]])
#define S_0		(__p[__parindex[5]])
#define E_0		(__p[__parindex[6]])
#define I_0		(__p[__parindex[7]])
#define R_0		(__p[__parindex[8]])
#define rho		(__p[__parindex[9]])
#define sigma		(__p[__parindex[10]])
#define gamma		(__p[__parindex[11]])
#define S		(__x[__stateindex[0]])
#define E		(__x[__stateindex[1]])
#define I		(__x[__stateindex[2]])
#define R		(__x[__stateindex[3]])
#define C		(__x[__stateindex[4]])

void __pomp_rinit (double *__x, const double *__p, double t, const int *__stateindex, const int *__parindex, const int *__covindex, const double *__covars)
{
 
  double m = pop/(S_0 + E_0 + I_0 + R_0);

  S = nearbyint(m*S_0);
  E = nearbyint(m*E_0);
  I = nearbyint(m*I_0);
  R = nearbyint(m*R_0);

  C = 0;
 
}

#undef a00
#undef a01
#undef a10
#undef a11
#undef pop
#undef S_0
#undef E_0
#undef I_0
#undef R_0
#undef rho
#undef sigma
#undef gamma
#undef S
#undef E
#undef I
#undef R
#undef C

/* C snippet: 'step.fn' */
#define a00		(__p[__parindex[0]])
#define a01		(__p[__parindex[1]])
#define a10		(__p[__parindex[2]])
#define a11		(__p[__parindex[3]])
#define pop		(__p[__parindex[4]])
#define S_0		(__p[__parindex[5]])
#define E_0		(__p[__parindex[6]])
#define I_0		(__p[__parindex[7]])
#define R_0		(__p[__parindex[8]])
#define rho		(__p[__parindex[9]])
#define sigma		(__p[__parindex[10]])
#define gamma		(__p[__parindex[11]])
#define S		(__x[__stateindex[0]])
#define E		(__x[__stateindex[1]])
#define I		(__x[__stateindex[2]])
#define R		(__x[__stateindex[3]])
#define C		(__x[__stateindex[4]])

void __pomp_stepfn (double *__x, const double *__p, const int *__stateindex, const int *__parindex, const int *__covindex, const double *__covars, double t, double dt)
{
 
  double beta;
  double foi;
  double rate[3], trans[3];

  beta = calc_beta(t, a00, a01, a10, a11);

  // expected force of infection
  foi = beta * I/pop;

  rate[0] = foi;      // stochastic force of infection
  rate[1] = sigma;    // rate of ending of latent stage
  rate[2] = gamma;    // recovery

  // transitions between classes
  reulermultinom(1, S, &rate[0], dt, &trans[0]);
  reulermultinom(1, E, &rate[1], dt, &trans[1]);
  reulermultinom(1, I, &rate[2], dt, &trans[2]);

  S += -trans[0];
  E += trans[0] - trans[1];
  I += trans[1] - trans[2];
  R = pop - S - E - I;

  // Assigning the right number to the accumulation variable that's used
  // in the observation model is absolutely critical!!!!
  C += trans[2];
 
}

#undef a00
#undef a01
#undef a10
#undef a11
#undef pop
#undef S_0
#undef E_0
#undef I_0
#undef R_0
#undef rho
#undef sigma
#undef gamma
#undef S
#undef E
#undef I
#undef R
#undef C

/* C snippet: 'rmeasure' */
#define a00		(__p[__parindex[0]])
#define a01		(__p[__parindex[1]])
#define a10		(__p[__parindex[2]])
#define a11		(__p[__parindex[3]])
#define pop		(__p[__parindex[4]])
#define S_0		(__p[__parindex[5]])
#define E_0		(__p[__parindex[6]])
#define I_0		(__p[__parindex[7]])
#define R_0		(__p[__parindex[8]])
#define rho		(__p[__parindex[9]])
#define sigma		(__p[__parindex[10]])
#define gamma		(__p[__parindex[11]])
#define S		(__x[__stateindex[0]])
#define E		(__x[__stateindex[1]])
#define I		(__x[__stateindex[2]])
#define R		(__x[__stateindex[3]])
#define C		(__x[__stateindex[4]])
#define cases		(__y[__obsindex[0]])

void __pomp_rmeasure (double *__y, const double *__x, const double *__p, const int *__obsindex, const int *__stateindex, const int *__parindex, const int *__covindex, const double *__covars, double t)
{
 
  cases = rbinom(I, rho);
 
}

#undef a00
#undef a01
#undef a10
#undef a11
#undef pop
#undef S_0
#undef E_0
#undef I_0
#undef R_0
#undef rho
#undef sigma
#undef gamma
#undef S
#undef E
#undef I
#undef R
#undef C
#undef cases

/* C snippet: 'dmeasure' */
#define a00		(__p[__parindex[0]])
#define a01		(__p[__parindex[1]])
#define a10		(__p[__parindex[2]])
#define a11		(__p[__parindex[3]])
#define pop		(__p[__parindex[4]])
#define S_0		(__p[__parindex[5]])
#define E_0		(__p[__parindex[6]])
#define I_0		(__p[__parindex[7]])
#define R_0		(__p[__parindex[8]])
#define rho		(__p[__parindex[9]])
#define sigma		(__p[__parindex[10]])
#define gamma		(__p[__parindex[11]])
#define S		(__x[__stateindex[0]])
#define E		(__x[__stateindex[1]])
#define I		(__x[__stateindex[2]])
#define R		(__x[__stateindex[3]])
#define C		(__x[__stateindex[4]])
#define cases		(__y[__obsindex[0]])
#define lik		(__lik[0])

void __pomp_dmeasure (double *__lik, const double *__y, const double *__x, const double *__p, int give_log, const int *__obsindex, const int *__stateindex, const int *__parindex, const int *__covindex, const double *__covars, double t)
{
 
  lik = dbinom(cases, I, rho, give_log);
 
}

#undef a00
#undef a01
#undef a10
#undef a11
#undef pop
#undef S_0
#undef E_0
#undef I_0
#undef R_0
#undef rho
#undef sigma
#undef gamma
#undef S
#undef E
#undef I
#undef R
#undef C
#undef cases
#undef lik

/* C snippet: 'toEst' */
#define a00		(__p[__parindex[0]])
#define a01		(__p[__parindex[1]])
#define a10		(__p[__parindex[2]])
#define a11		(__p[__parindex[3]])
#define pop		(__p[__parindex[4]])
#define S_0		(__p[__parindex[5]])
#define E_0		(__p[__parindex[6]])
#define I_0		(__p[__parindex[7]])
#define R_0		(__p[__parindex[8]])
#define rho		(__p[__parindex[9]])
#define sigma		(__p[__parindex[10]])
#define gamma		(__p[__parindex[11]])
#define T_a00		(__pt[__parindex[0]])
#define T_a01		(__pt[__parindex[1]])
#define T_a10		(__pt[__parindex[2]])
#define T_a11		(__pt[__parindex[3]])
#define T_pop		(__pt[__parindex[4]])
#define T_S_0		(__pt[__parindex[5]])
#define T_E_0		(__pt[__parindex[6]])
#define T_I_0		(__pt[__parindex[7]])
#define T_R_0		(__pt[__parindex[8]])
#define T_rho		(__pt[__parindex[9]])
#define T_sigma		(__pt[__parindex[10]])
#define T_gamma		(__pt[__parindex[11]])

void __pomp_to_trans (double *__pt, const double *__p, const int *__parindex)
{
 	T_a00 = log(a00);
	T_a01 = log(a01);
	T_a10 = log(a10);
	T_a11 = log(a11); 
}

#undef a00
#undef a01
#undef a10
#undef a11
#undef pop
#undef S_0
#undef E_0
#undef I_0
#undef R_0
#undef rho
#undef sigma
#undef gamma
#undef T_a00
#undef T_a01
#undef T_a10
#undef T_a11
#undef T_pop
#undef T_S_0
#undef T_E_0
#undef T_I_0
#undef T_R_0
#undef T_rho
#undef T_sigma
#undef T_gamma

/* C snippet: 'fromEst' */
#define a00		(__p[__parindex[0]])
#define a01		(__p[__parindex[1]])
#define a10		(__p[__parindex[2]])
#define a11		(__p[__parindex[3]])
#define pop		(__p[__parindex[4]])
#define S_0		(__p[__parindex[5]])
#define E_0		(__p[__parindex[6]])
#define I_0		(__p[__parindex[7]])
#define R_0		(__p[__parindex[8]])
#define rho		(__p[__parindex[9]])
#define sigma		(__p[__parindex[10]])
#define gamma		(__p[__parindex[11]])
#define T_a00		(__pt[__parindex[0]])
#define T_a01		(__pt[__parindex[1]])
#define T_a10		(__pt[__parindex[2]])
#define T_a11		(__pt[__parindex[3]])
#define T_pop		(__pt[__parindex[4]])
#define T_S_0		(__pt[__parindex[5]])
#define T_E_0		(__pt[__parindex[6]])
#define T_I_0		(__pt[__parindex[7]])
#define T_R_0		(__pt[__parindex[8]])
#define T_rho		(__pt[__parindex[9]])
#define T_sigma		(__pt[__parindex[10]])
#define T_gamma		(__pt[__parindex[11]])

void __pomp_from_trans (double *__p, const double *__pt, const int *__parindex)
{
 	a00 = exp(T_a00);
	a01 = exp(T_a01);
	a10 = exp(T_a10);
	a11 = exp(T_a11); 
}

#undef a00
#undef a01
#undef a10
#undef a11
#undef pop
#undef S_0
#undef E_0
#undef I_0
#undef R_0
#undef rho
#undef sigma
#undef gamma
#undef T_a00
#undef T_a01
#undef T_a10
#undef T_a11
#undef T_pop
#undef T_S_0
#undef T_E_0
#undef T_I_0
#undef T_R_0
#undef T_rho
#undef T_sigma
#undef T_gamma

static int __pomp_load_stack = 0;

void __pomp_load_stack_incr (void) {++__pomp_load_stack;}

void __pomp_load_stack_decr (int *val) {*val = --__pomp_load_stack;}

void R_init_snippets (DllInfo *info)
{
R_RegisterCCallable("snippets", "__pomp_load_stack_incr", (DL_FUNC) __pomp_load_stack_incr);
R_RegisterCCallable("snippets", "__pomp_load_stack_decr", (DL_FUNC) __pomp_load_stack_decr);
R_RegisterCCallable("snippets", "__pomp_rinit", (DL_FUNC) __pomp_rinit);
R_RegisterCCallable("snippets", "__pomp_stepfn", (DL_FUNC) __pomp_stepfn);
R_RegisterCCallable("snippets", "__pomp_rmeasure", (DL_FUNC) __pomp_rmeasure);
R_RegisterCCallable("snippets", "__pomp_dmeasure", (DL_FUNC) __pomp_dmeasure);
R_RegisterCCallable("snippets", "__pomp_to_trans", (DL_FUNC) __pomp_to_trans);
R_RegisterCCallable("snippets", "__pomp_from_trans", (DL_FUNC) __pomp_from_trans);
}
