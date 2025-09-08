/**
@file  simplepricers.hpp
@brief Declaration of simple pricing functions
*/

#ifndef QF_SIMPLEPRICERS_HPP
#define QF_SIMPLEPRICERS_HPP

#include <qflib/defines.hpp>
#include <qflib/exception.hpp>
#include <qflib/math/matrix.hpp>
#include <qflib/market/market.hpp>

BEGIN_NAMESPACE(qf)

/** The forward price of an asset */
double fwdPrice(double spot, double timeToExp, double intRate, double divYield);

/** The quanto forward price of an asset */
double quantoFwdPrice(double spot, double timeToExp, double intRate, double divYield,
                      double assetVol, double fxVol, double correl);

/** Price of a European digital option in the Black-Scholes model*/
Vector digitalOptionBS(int payoffType, double spot, double strike, double timeToExp,
                       double intRate, double divYield, double volatility);

/** Price and Greeks of a European option in the Black-Scholes model*/
Vector europeanOptionBS(int payoffType, double spot, double strike, double timeToExp, 
                        double intRate, double divYield, double volatility);

/** Price of a single point knock-out forward contract*/
double knockoutFwd(double spot, double strike, double kolevel, double timeToExp,
                   double timeToKO, double intRate, double divYield, double volatility);

/** Price of a European caplet/floorlet in the Black-Scholes model*/
double capFloorletBS(int payoffType, SPtrYieldCurve spyc, double strikeRate, 
                     double timeToReset, double tenor, double fwdRateVol);

/** Present value of a credit default swap */
Vector cdsPV(SPtrYieldCurve sprfyc, double credSprd, double cdsRate,
                 double recov, double timeToMat, size_t payFreq);

/** Price of a European quanto option in the Black-Scholes model */
double quantoEuropeanOptionBS(int payoffType, double spot, double strike, double timeToExp,
    double discRate, double growthRate, double divYield,
    double assetVol, double fxVol, double correl);

END_NAMESPACE(qf)

#endif // QF_SIMPLEPRICERS_HPP
