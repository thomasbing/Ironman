#ifndef _POLYNOMIAL_H
#define _POLYNOMIAL_H


/** Evaluate a polynomial.
 * @param[in] n   the order of the polynomial (i.e. the number of coefficients).
 * @param[in] cf  the list of coefficients, starting with the constant term.
 * @param[in] x   the location at which the polynomial is to be evaluated.
 * @return    the value of the given polynomial at the specified point.
 */
double EvaluatePolynomial(unsigned n, const double *cf, double x);


/** Evaluate the derivative of a polynomial.
 * @param[in] n   the order of the polynomial (i.e. the number of coefficients).
 * @param[in] cf  the list of coefficients, starting with the constant term.
 * @param[in] x   the location at which the polynomial derivative is to be evaluated.
 * @return    the derivative of the given polynomial at the specified point.
 */
double EvaluatePolynomialDerivative(unsigned n, const double *cf, double x);


/** Find a root of a polynomial in an interval.
 * The polynomial should have opposite signs at each end of the interval,
 * so it is guaranteed that there is a root.
 * It is best if the interval contains only one root, because it is not guaranteed which root will be found.
 * Multiple roots can slow down convergence, and the precision may be lower; there will necessarily be an odd number with out opposite sign requirement.
 * @param[in] n   the order of the polynomial (i.e. the number of coefficients).
 * @param[in] cf  the list of coefficients in the polynomial.
 * @param[in] x0  the left  of the interval in which to look for the root.
 * @param[in] x1  the right of the interval in which to look for the root.
 * @param[in] y   the value to solve for: p(x) = y. (default 0)
 * @return    the root found in the interval.
 */
double FindRootOfPolynomial(unsigned n, const double *cf, double x0, double x1, double y = 0.);


/** Bound the magnitude of the real roots of a polynomial poly(x) = y.
 * @param[in]   n       the order of the polynomial (number of coefficients).
 * @param[in]   cf      the coefficients of the polynomial.
 * @param[out]  bounds  the resultant bounds on the magnitude of the roots.
 * @param[in]   y       the value of the polynomial to be bounded.
 */
void BoundPolynomialRoots(unsigned n, const double *cf, double bounds[2], double y = 0);


/** Evaluate the distortion polynomial
 *     x * (1 + k[0] * x^2 + ... + k[n-1] * x^(2n)
 * at the point x.
 * @param[in]  n  the number of coefficients cf[].
 * @param[in]  cf the array of coefficients.
 * @param[in]  x  the place to evaluate the polynomial.
 * @return     the distortion polynomial evaluated at the selected point.
 */
double EvaluateDistortionPolynomial(unsigned n, const double *cf, double x);


/** Evaluate the derivative of the distortion polynomial
 *     x * (1 + k[0] * x^2 + ... + k[n-1] * x^(2n)
 * at the point x.
 * @param[in]  n  the number of coefficients cf[].
 * @param[in]  cf the array of coefficients.
 * @param[in]  x  the place to evaluate the polynomial.
 * @return     the distortion polynomial evaluated at the selected point.
 */
double EvaluateDistortionPolynomialDerivative(unsigned n, const double *cf, double x);


/** Find the place in the distortion polynomial where it goes flat, to restrict the domain to where it is monotonic.
 * @param[in] n     the number of coefficients in the distortion polynomial.
 * @param[in] cf    the list of coefficients, starting with the cubic (the linear coefficient is assumed to be 1).
 * @param[in] slope the value of slope that is considered to be zero (default is zero).
 * @return    the location at which the distortion polynomial flattens out (zero slope), or
 *            INFINITY if it doesn't seem to flatten out, or
 *            NAN if there are computational problems, such as too many coefficients were supplied.
 */
double FindFlatOfDistortionPolynomial(unsigned n, const double *cf, double slope = 0.);


/** Find a root of an odd distortion polynomial closest to 0.
 * @param[in] n     the number of coefficients, starting with cubic (the linear term has a coefficient of 1).
 * @param[in] cf    the list of coefficients in the polynomial.
 * @param[in] maxX  look for the value in [0, maxX].
 * @param[in] y     the value to solve for: p(x) = y. (default 0)
 * @return    the root found in the interval, or NaN if none was found.
 */
double FindRootOfDistortionPolynomial(unsigned n, const double *cf, double maxX, double y = 0.);


/** Multiply two polynomials.
 * @param[in]   np  the number of coefficients in polynomial p.
 * @param[in]   p   the coefficients of polynomial p.
 * @param[in]   nq  the number of coefficients in polynomial q.
 * @param[in]   q   the coefficients of polynomial q.
 * @param[out]  r   the coefficients of the resultant polynomial, with (np + nq - 1) coefficients.
 */
void MultiplyPolynomials(unsigned np, const double *p, unsigned nq, const double *q, double *r);


/** Compute the error between two distortion polynomials as
 *  the square root of the integral of the squared (RIS) distance between the two curves.
 *  The distortion polynoimials take the form
 *  r(1 + c0 * r^2 + c1 * r^4 + ...).
 * @param[in] n the number of coefficients in the distortion polynomial.
 * @param[in] a one distortion polynomial.
 * @param[in] n the other distortion polynomial.
 * @param[in] max the maximum value of integration (the minimum is 0).
 * @return    the RIS error.
 * @todo  Would the integral of the square error be better?
 */
double RISDiffDistortionPolynomial(unsigned n, const double *a, const double *b, double max);


/** Compute the RMS error between two distortion polynomials, i.e.
 *  the square root of the mean of the squared distance between the two curves.
 *  The distortion polynoimials take the form
 *  r(1 + c0 * r^2 + c1 * r^4 + ...).
 * @param[in] n the number of coefficients in the distortion polynomial.
 * @param[in] a one distortion polynomial.
 * @param[in] n the other distortion polynomial.
 * @param[in] max the maximum value of integration (the minimum is 0).
 * @return    the RMS error.
 */
double RMSDiffDistortionPolynomial(unsigned n, const double *a, const double *b, double max);


/** Compute the roots of the given quadratic polynomial, in the form
 *  c[0] + c[1] * x + c[2] * x^2 .
 * @param[in]   c   the coefficients.
 * @param[out]  re  the real part of the roots.
 * @param[out]  im  the imaginary part of the roots. Can be nullptr if not needed.
 * @return      the number of real roots.
 */
unsigned RootsOfQuadraticPolynomial(const double *c, double *re, double *im);


/** Compute the roots of the given cubic polynomial, in the form
 *  c[0] + c[1] * x + c[2] * x^2 + c[3] * x^3 .
 * @param[in]   c   the coefficients.
 * @param[out]  re  the real part of the roots.
 * @param[out]  im  unused in this implementation.
 * @return      the number of real roots.
 * @bug         complex roots are not returned.
 */
unsigned RootsOfCubicPolynomial(const double *c, double *re, double *im);


/** Compute the maximum distance between two polynomials over a given range.
 * @param[in] n the number of coefficients in the distortion polynomial.
 * @param[in] a one distortion polynomial.
 * @param[in] n the other distortion polynomial.
 * @param[in] max the maximum value of integration (the minimum is 0).
 * @return    the maximum magnitude of difference over the interval [0, max].
 */
double MaxDiffDistortionPolynomial(unsigned n, const double *a, const double *b, double max);


#endif // _POLYNOMIAL_H

