#include "polynomial.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>


/********************************************************************************
 ****************************************************************************//**
 *** Scalar Function class, primarily for root-finding.
 ********************************************************************************
 ********************************************************************************/

class ScalarFunction {
public:
  /// Constructor
  ScalarFunction() {}

  /// Destructor (don't really need this to be virtual)
  virtual ~ScalarFunction() {}

  /// Evaluate the scalar function.
  /// @param[in]  x the location at which the scalar function is to be evaluated.
  /// @return     the value of the scalar function evaluated at x.
  virtual double evaluate(double x) const = 0;

  /// Evaluate the derivative of the scalar function (not required).
  /// @param[in]  x the location at which the derivative of the scalar function is to be evaluated.
  /// @return     the derivative of the scalar function evaluated at x.
  virtual double derivative(double x) const { return NAN; }

  /// Solve the continuous scalar function y = f(x) for x, given y.
  /// The function evaluated at the bounds should surround the desired value y, i.e.
  ///     (f(left) - y) * (f(right) - y) <= 0,
  /// otherwise NAN is returned. This is a sufficient condition to guarantee convergence, but not necessary,
  /// so it is desirable to bracket the interval as tightly as possible to contain only one root.
  /// @param[in]  y     the value of the scalar function for which to solve.
  /// @param[in]  left  the left  end of the interval in which to search for the solution.
  /// @param[in]  right the right end of the interval in which to search for the solution.
  /// @return     the parameter x for which y = f(x), or
  ///             NAN, if no root was found in the specified interval.
  double solve(double y, double left, double right) const;
};


/********************************************************************************
 * FindRootOfScalarFunction
 ********************************************************************************/

double ScalarFunction::solve(double y, double x0, double x1) const
{
  unsigned      k   = 60;                             /* Max iterations, though 8 is typical */
  const double  eps = 2.2204460492503130808e-16;      /* 1 ULP */
  double  x, f0, f1, f, d, dx;

  /* Assure that x0 and x1 are distinct, and evaluate at those points */
  if ((f0 = evaluate(x0) - y) == 0)
    return x0;                                        /* Solution at left end of interval */
  if ((f1 = evaluate(x1) - y) == 0)
    return x1;                                        /* Solution at right end of interval */
  if (f0 * f1 > 0)
    return NAN;                                       /* No solution in interval */

  if (x0 > x1)
  {
    f = f0; f0 = f1; f1 = f;                          /* Assure x0 < x1 */
    x = x0; x0 = x1; x1 = x;
  }
  
  if (fabs(f0) < fabs(f1))
  {                                                   /* f(x0) is closer to 0 */
    x = x0;
    f = f0;
  }
  else
  {                                                   /* f(x1) is closer to 0 */
    x = x1;
    f = f1;
  }

  if ((d = derivative(0.)) != d || (d = derivative(1.)) != d)
  { /* No derivative supplied: use secant or regula falsi */
    return NAN;
  }
  else
  { /* Use Newton or bisection */
    do {  // TODO: Brent's method to avoid worst case Newton?
      if ((d = derivative(x)) == 0 || !(x0 < (x -= f / d) && (x < x1)))  /* If Newton fails, ... */
        x = (x0 + x1) * .5;                                              /* ... bisect */
      if ((f = evaluate(x) - y) == 0)
        return x;
      if (f * f0 > 0)
      {                                                                 /* Squeeze left end of interval */
        f0 = f;
        dx = x - x0;
        x0 = x;
      }
      else
      {                                                                 /* Squeeze right end of interval */
        f1 = f;
        dx = x1 - x;
        x1 = x;
      }
    } while (dx > eps * fabs(x) && --k);
    return x;
  }
}



/********************************************************************************
 ********************************************************************************
 *** Polynomial
 ********************************************************************************
 ********************************************************************************/


#define MAXORDER 6

/********************************************************************************
 * EvaluatePolynomial
 ********************************************************************************/

double EvaluatePolynomial(unsigned n, const double *cf, double x)
{
  double y;
  for (cf += n, y = 0; n--;)
  { /* Horner's method */
    y = y * x + *--cf;
  }
  return y;
}


/********************************************************************************
 * EvaluatePolynomialDerivative
 ********************************************************************************/

double EvaluatePolynomialDerivative(unsigned n, const double *cf, double x)
{
  double y;
  for (cf += n, y = 0; --n;)
  { /* Horner's method */
    y = y * x + *--cf * n;
  }
  return y;
}


/********************************************************************************
 * Minimize Polynomial Order.
 * This removes high-order zero coefficients.
 * @param[in,out] n    the order of the polynomial, adjusted downward on output to remove high-order zero coefficients.
 * @param[in]     cf  the coefficients of the polynomial.
 ********************************************************************************/

static void MinimizePolynomialOrder(unsigned *pn, const double *cf)
{
  unsigned n = *pn;
  for (cf += n - 1; 0 == *cf && n; --cf)
    --n;
  *pn = n;
}


/********************************************************************************
 * Bound Polynomial Roots, algorithm due to Cauchy.
 * @param[in]   n       the order of the polynomial (number of coefficients).
 * @param[in]   cf      the coefficients of the polynomial.
 * @param[out]  bounds  the resultant bounds.
 * @param[in]   y       find bounds for roots of y == poly(x), rather than 0 == poly(x).
 ********************************************************************************/

void BoundPolynomialRoots(unsigned n, const double *cf, double bounds[2], double y)
{
  double max, m0, m1;
  int i;

  if (n < 2)
  {
    bounds[0] = bounds[1] = NAN;
    return;
  }

  for (i = n - 2, max = 0; i > 0; --i)
  { /* Find max of the middle coefficients */
    if (max < (m1 = fabs(cf[i])))
      max = m1;
  }
  m0 = fabs(cf[0] - y);
  m1 = fabs(cf[n-1]);
  bounds[0] = m0 / (m0 + ((max >= m1) ? max : m1));
  bounds[1] = 1. + ((max >= m0) ? max : m0) / m1;
}


class PolynomialFunction : public ScalarFunction {
public:
  PolynomialFunction(unsigned numCoeff, const double *coeff) { m_numCoeff = numCoeff; MinimizePolynomialOrder(&m_numCoeff, m_coeff = coeff); };
  double   evaluate(double x) const override { return EvaluatePolynomial(m_numCoeff, m_coeff, x); }
  double derivative(double x) const override { return EvaluatePolynomialDerivative(m_numCoeff, m_coeff, x); }
private:
  unsigned      m_numCoeff;
  const double  *m_coeff;
};


double FindRootOfPolynomial(unsigned n, const double *cf, double x0, double x1, double y)
{
  PolynomialFunction poly(n, cf);
  return poly.solve(y, x0, x1);
}



/********************************************************************************
 ********************************************************************************
 *** Distortion Polynomial
 ********************************************************************************
 ********************************************************************************/


/********************************************************************************
 * EvaluateDistortionPolynomial
 ********************************************************************************/

double EvaluateDistortionPolynomial(unsigned n, const double *cf, double x)
{
  double f, x2;

  for (f = 0, cf += n, x2 = x * x; n--;)
  { /* Horner's method */
    f += *--cf;
    f *= x2;
  }
  f += 1;
  f *= x;
  return f;
}


/********************************************************************************
 * EvaluateDistortionPolynomialDerivative
 ********************************************************************************/

double EvaluateDistortionPolynomialDerivative(unsigned n, const double *cf, double x)
{
  double d;
  int j;

  for (d = 0, cf += n, j = 2 * n + 1, x *= x; j > 1; j -= 2)
  { /* Horner's method */
    d += *--cf * j;
    d *= x;
  }
  d += 1;
  return d;
}


/********************************************************************************
 * DistortionPolynomialFunction
 ********************************************************************************/

class DistortionPolynomialFunction : public ScalarFunction {
public:
  DistortionPolynomialFunction(unsigned numCoeff, const double *coeff) { m_numCoeff = numCoeff; m_coeff = coeff; };
  double evaluate(double x) const override { return EvaluateDistortionPolynomial(m_numCoeff, m_coeff, x); }
  double derivative(double x) const override { return EvaluateDistortionPolynomialDerivative(m_numCoeff, m_coeff, x); }
private:
  unsigned      m_numCoeff;
  const double  *m_coeff;
};


/****************************************************************************//**
 * Bracket the First Zero Crossing of the polynomial representing the derivative of the distortion polynomial.
 * Such a polynomial is even with the constant coefficient equal to 1.
 * we know that the first negative coefficient will probably be responsible for the zero crossing, so we 
 ********************************************************************************/

static bool BracketFirstDistortionDerivativeZeroCrossing(unsigned n, const double *cf, double bounds[2])
{
  unsigned i;

  for (i = 2; i <= n; ++i)
  {
    if (cf[i-1] < 0)                                                                        /* Find the first negative coefficients */
    {
      BoundPolynomialRoots(i, cf, bounds);                                                  /* Determine bounds for a lower-order polynomial */
      if (EvaluatePolynomial(n, cf, bounds[0]) * EvaluatePolynomial(n, cf, bounds[1]) <= 0) /* Check that the bounds bracket the roots */
        return true;
    }
  }
  return false;
}


/********************************************************************************
 * FindFlatOfDistortionPolynomial
 ********************************************************************************/

double FindFlatOfDistortionPolynomial(unsigned n, const double *cf, double flatValue)
{
  double    x;
  unsigned  i;
  double    der[MAXORDER+1], bounds[2];

  MinimizePolynomialOrder(&n, cf);

  /* Compute derivative polynomial: polynomial of order n+1 in x^2 */
  if (n >= sizeof(der)/sizeof(der[0]))
    return NAN;     /* If we get here, we need to enlarge the der array above */
  for (i = n; i; i--)
  {
    der[i] = cf[i-1] * (i * 2 + 1);
  }
  der[0] = 1. - flatValue;
  ++n;

  if (BracketFirstDistortionDerivativeZeroCrossing(n, der, bounds))
  {
    x = FindRootOfPolynomial(n, der, bounds[0], bounds[1]);   /* This could return a NaN */
    x = sqrt(x);
  }
  else
  {
    x = INFINITY; // HUGE_VAL might be more portable
  }
  return x;
}


/********************************************************************************
 * BoundDistortionPolynomialRoots, algorithm due to Cauchy.
 * @param[in]   n       the number of coefficients in the distortion polynomial.
 * @param[in]   cf      the coefficients of the distortion polynomial.
 * @param[out]  bounds  the resultant bounds.
 * @param[in]   y       find bounds for roots of y == poly(x), rather than 0 == poly(x).
 ********************************************************************************/

static void BoundDistortionPolynomialRoots(unsigned n, const double *cf, double bounds[2], double y = 0)
{
  double fake[MAXORDER+2];

  if (n == 0)
  {
    bounds[0] = bounds[1] = y;
    return;
  }

  for (unsigned i = n; n--;)                    /* Make fake poly = { 0, 1, cf[0], cf[1], ... } */
  {
    fake[i + 2] = cf[i];
  }
  fake[1] = 1;
  fake[0] = 0;
  BoundPolynomialRoots(n + 2, fake, bounds, y);
  bounds[0] = sqrt(bounds[0]);
  bounds[1] = sqrt(bounds[1]);
}


/********************************************************************************
 * FindRootOfDistortionPolynomial
 ********************************************************************************/

double FindRootOfDistortionPolynomial(unsigned n, const double *cf, double maxX, double y)
{
  double f0, f1, x0, x1;
  if (y < 0)                                                  /* The argument should be positive, ... */
  {
    return -FindRootOfDistortionPolynomial(n, cf, maxX, -y);  /* ... but we can get a valid solution since the polynomial is odd */
  }                                                           /* Hereafter, y >= 0 */

  /* Since undistorted is the identity and distortion should be slight, the root should be close to y */
  f1 = EvaluateDistortionPolynomial(n, cf, x1 = y) - y;       /* Let us find the value at y */
  if (f1 >= 0)                                                /* We have a bracket in [0, y] */
  {
    f0 = -y;                                                  /* We know the [negative] value ... */
    x0 = 0;                                                   /* ... at 0 */
  }
  else
  {
    x0 = x1;
    f0 = f1;
    f1 = EvaluateDistortionPolynomial(n, cf, x1 = maxX) - y;  /* Let us find the value at maxX, to bracket between [y, maxX] */
    if (!(f1 >= 0))                                           /* No bracket in the interval */
      return NAN;
  }
  DistortionPolynomialFunction poly(n, cf);
  return poly.solve(y, x0, x1);
}


/****************************************************************************//**
 * Integrate an even polynomial of the form:
 * c0 + c1 x^2 + c2 x^4 + ... + c[nCoeffs-1] x^(2 * nCoeffs - 2)
 * @param[in] numCoeffs the number of coefficients.
 * @param[in] coeff     the coefficients of the polynomial.
 * @param[in] x0        the left  end of integration.
 * @param[in] x1        the right end of integration.
 ********************************************************************************/

static double IntegrateEvenPolynomial(unsigned numCoeffs, const double *coeff, double x0, double x1)
{
  const double  x02 = x0 * x0,
                x12 = x1 * x1;
  unsigned      i;
  double        d, e, a0, a1;

  for (i = numCoeffs, coeff += numCoeffs - 1, d = 2 * numCoeffs - 1, a0 = a1 = 0; i--; --coeff, d -= 2)
  { /* Horners method */
    e = *coeff / d;
    a0 = a0 * x02 + e;
    a1 = a1 * x12 + e;
  }
  a0 *= x0;
  a1 *= x1;
  return a1 - a0;
}


/********************************************************************************
 * MultiplyPolynomials
 ********************************************************************************/

void MultiplyPolynomials(unsigned np, const double *p, unsigned nq, const double *q, double *r)
{
  memset(r, 0, (np + nq - 1) * sizeof(*r));
  for (unsigned ip = 0; ip < np; ++ip)
  {
    for (unsigned iq = 0; iq < nq; ++iq)
    {
      r[ip + iq] += p[ip] * q[iq];
    }
  }
}


/********************************************************************************
 * SubtractVector
 ********************************************************************************/

static void SubtractVector(unsigned n, const double *l, const double *r, double *d)
{
  while (n--)
    *d++ = *l++ - *r++;
}


/********************************************************************************
 * RISDiffDistortionPolynomial
 ********************************************************************************/

double RISDiffDistortionPolynomial(unsigned n, const double *a, const double *b, double max)
{
  const unsigned maxCoeffs = 4;
  double d[maxCoeffs], q[maxCoeffs * 2 + 2];
  if (!(n <= maxCoeffs))
    return NAN;
  SubtractVector(n, a, b, d);
  memset(q, 0, sizeof(q));
  MultiplyPolynomials(n, d, n, d, q + 3);
  return sqrt(IntegrateEvenPolynomial(n * 2 + 2, q, 0, max));
}


/********************************************************************************
 * RMSDiffDistortionPolynomial
 ********************************************************************************/

double RMSDiffDistortionPolynomial(unsigned n, const double *a, const double *b, double max)
{
  double x = RISDiffDistortionPolynomial(n, a, b, max);
  return sqrt(x * x / max);
}


/********************************************************************************
 * RootsOfQuadraticPolynomial
 ********************************************************************************/

unsigned RootsOfQuadraticPolynomial(const double *c, double *re, double *im)
{
  double d  = c[1] * c[1] - 4. * c[2] * c[0];

  if (im)
  {
    im[0] = im[1] = 0.;
  }
  if (d > 0)
  {
    d = sqrt(d);
    double q = (-c[1] + ((c[1] < 0) ? -d : d)) * .5;
    re[0] = q / c[2];
    re[1] = c[0] / q;
    return 2;
  }
  else
  {
    re[0] = re[1] = -c[1] / (2. * c[2]);
    if (d == 0)
    {
      return 1;
    }
    else 
    {
      if (im)
      {
        im[0] = sqrt(-d) / (2. * c[2]);
        im[1] = -im[0];
      }
      return 0;
    }
  }
}



/********************************************************************************
 * RootsOfCubicPolynomial
 ********************************************************************************/

unsigned RootsOfCubicPolynomial(const double *c, double *re, double *im)
{
  double  a1 = c[2] / c[3],
          a2 = c[1] / c[3],
          a3 = c[0] / c[3],
          q  = (a1 * a1 - 3. * a2) / 9.,
          r  = (2. * a1 * a1 * a1 - 9. * a1 * a2 + 27. * a3) / 54.,
          q3 = q * q * q,
          d  = q3 - r * r;

  if (im)
  {
    im[0] = im[1] = im[2] = NAN;  /* TODO: implement complex roots */
  }
  if (d >= 0)
  {
    if (q3)
    {
      double t  = acos(r / sqrt(q3));
      double qq = sqrt(q);
      if (t)
      {
        re[0] = -2. * qq * cos( t              / 3.) - a1 / 3.;
        re[1] = -2. * qq * cos((t + 2. * M_PI) / 3.) - a1 / 3.;
        re[2] = -2. * qq * cos((t + 4. * M_PI) / 3.) - a1 / 3.;
        return 3;
      }
      else
      {
        re[0] =   -2. * qq - a1 / 3.;
        re[1] = re[2] = qq - a1 / 3.;
        return 2;
      }
    }
    else
    {
      re[0] = re[1] = re[2] = -a1 / 3.;
      return 1;
    }
  }
  else
  {
    double e = cbrt(sqrt(-d) + fabs(r));
    if (r > 0)
      e = -e;
    re[0] = (e + q / e) - a1 / 3.;
    return 1;
  }
}


double MaxDiffDistortionPolynomial(unsigned n, const double *a, const double *b, double max)
{
  double d[4], r[3];
  unsigned m;
  
  switch (n)
  { /* Compute derivative of difference */
    case 4: d[3] = 9. * (a[3] - b[3]);
    case 3: d[2] = 7. * (a[2] - b[2]);
    case 2: d[1] = 5. * (a[1] - b[1]);
            d[0] = 3. * (a[0] - b[0]);
            break;
    default:
      return NAN; /* Unimplemented */
  }

  switch (n)
  {
    case 4: m = RootsOfCubicPolynomial    (d, r, nullptr); break;
    case 3: m = RootsOfQuadraticPolynomial(d, r, nullptr); break;
    case 2: m = 1; r[0] = -d[0] / d[1];                 break;
  }
  d[0] = fabs(EvaluateDistortionPolynomial(n, a, max) - EvaluateDistortionPolynomial(n, b, max));
  for (unsigned i = 0; i < m; ++i)
  {
    if (0 < r[i] && (d[2] = sqrt(r[i])) < max &&
      d[0] < (d[1] = fabs(EvaluateDistortionPolynomial(n, a, d[2]) - EvaluateDistortionPolynomial(n, b, d[2])))
    )
    {
      d[0] = d[1];
    }
  }
  return d[0];
}

