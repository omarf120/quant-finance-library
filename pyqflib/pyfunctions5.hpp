/**
@file  pyfunctions5.hpp
@brief Implementation of Python callable functions for quanto options
*/

#include <pyqflib/pyutils.hpp>

#include <qflib/pricers/simplepricers.hpp>
#include <qflib/market/market.hpp>
#include <qflib/products/europeancallput.hpp>
#include <qflib/pricers/bsmcquantopricer.hpp>
#include <qflib/methods/montecarlo/pathgenerator.hpp>
#include <qflib/methods/pde/pde1dsolver.hpp>
#include <qflib/methods/pde/pderesults.hpp>
#include <qflib/math/random/rng.hpp>
#include <qflib/methods/montecarlo/mcparams.hpp>

using namespace std;

static
PyObject* pyQfQuantoEuroBS(PyObject* pyDummy, PyObject* pyArgs)
{
PY_BEGIN;

  PyObject* pyPayoffType;
  PyObject* pySpot;
  PyObject* pyStrike;
  PyObject* pyTimeToExp;
  PyObject* pyDiscRate;
  PyObject* pyGrowthRate;
  PyObject* pyDivYield;
  PyObject* pyAssetVol;
  PyObject* pyFxVol;
  PyObject* pyCorrel;

  if (!PyArg_ParseTuple(pyArgs, "OOOOOOOOOO",
    &pyPayoffType, &pySpot, &pyStrike, &pyTimeToExp,
    &pyDiscRate, &pyGrowthRate, &pyDivYield,
    &pyAssetVol, &pyFxVol, &pyCorrel))
    return NULL;

  int payoffType = asInt(pyPayoffType);
  double spot = asDouble(pySpot);
  double strike = asDouble(pyStrike);
  double timeToExp = asDouble(pyTimeToExp);
  double discRate = asDouble(pyDiscRate);
  double growthRate = asDouble(pyGrowthRate);
  double divYield = asDouble(pyDivYield);
  double assetVol = asDouble(pyAssetVol);
  double fxVol = asDouble(pyFxVol);
  double correl = asDouble(pyCorrel);

  double price = qf::quantoEuropeanOptionBS(
    payoffType, spot, strike, timeToExp,
    discRate, growthRate, divYield,
    assetVol, fxVol, correl
  );
  
  return asPyScalar(price);

PY_END;
}

static
PyObject* pyQfQuantoEuroBSMC(PyObject* pyDummy, PyObject* pyArgs)
{
PY_BEGIN;

  PyObject* pyPayoffType(NULL);
  PyObject* pyStrike(NULL);
  PyObject* pyTimeToExp(NULL);
  PyObject* pySpot(NULL);
  PyObject* pyDiscountCrv(NULL);
  PyObject* pyGrowthCrv(NULL);
  PyObject* pyDivYield(NULL);
  PyObject* pyAssetVol(NULL);
  PyObject* pyFxVol(NULL);
  PyObject* pyCorrel(NULL);
  PyObject* pyMcParams(NULL);
  PyObject* pyNPaths(NULL);

  if (!PyArg_ParseTuple(pyArgs, "OOOOOOOOOOOO",
    &pyPayoffType, &pyStrike, &pyTimeToExp, &pySpot,
    &pyDiscountCrv, &pyGrowthCrv, &pyDivYield,
    &pyAssetVol, &pyFxVol, &pyCorrel, &pyMcParams, &pyNPaths))
    return NULL;

  int payoffType = asInt(pyPayoffType);
  double strike = asDouble(pyStrike);
  double timeToExp = asDouble(pyTimeToExp);
  double spot = asDouble(pySpot);

  std::string discountName = asString(pyDiscountCrv);
  std::string growthName = asString(pyGrowthCrv);
  qf::SPtrYieldCurve discyc = qf::market().yieldCurves().get(discountName);
  qf::SPtrYieldCurve growyc = qf::market().yieldCurves().get(growthName);
  QF_ASSERT(discyc, "error: discount curve " + discountName + " not found");
  QF_ASSERT(growyc, "error: growth curve " + growthName + " not found");

  double divYield = asDouble(pyDivYield);
  double assetVol = asDouble(pyAssetVol);
  double fxVol = asDouble(pyFxVol);
  double correl = asDouble(pyCorrel);

  qf::McParams mcparams = asMcParams(pyMcParams);
  unsigned long npaths = asLong(pyNPaths);

  // Create the product
  qf::SPtrProduct spprod(new qf::EuropeanCallPut(payoffType, strike, timeToExp));

  // Create the pricer
  qf::BsMcQuantoPricer pricer(spprod, discyc, growyc, divYield, assetVol, fxVol, correl, spot, mcparams);

  // Create the statistics calculator
  qf::MeanVarCalculator<double*> stats(pricer.nVariables());

  // Run the simulation
  pricer.simulate(stats, npaths);

  // Collect results
  qf::Matrix const& results = stats.results();
  size_t nsamples = stats.nSamples();
  double mean = results(0, 0);
  double stderror = results(1, 0);
  stderror = std::sqrt(stderror / nsamples);

  // Return result as dictionary
  PyObject* ret = PyDict_New();
  PyDict_SetItem(ret, asPyScalar("Price"), asPyScalar(mean));
  PyDict_SetItem(ret, asPyScalar("StdErr"), asPyScalar(stderror));
  return ret;

PY_END;
}

static
PyObject* pyQfQuantoEuroBSPDE(PyObject* pyDummy, PyObject* pyArgs)
{
PY_BEGIN;

  PyObject* pyPayoffType(NULL);
  PyObject* pyStrike(NULL);
  PyObject* pyTimeToExp(NULL);
  PyObject* pySpot(NULL);
  PyObject* pyDiscountCrv(NULL);
  PyObject* pyGrowthCrv(NULL);
  PyObject* pyDivYield(NULL);
  PyObject* pyAssetVol(NULL);
  PyObject* pyFxVol(NULL);
  PyObject* pyCorrel(NULL);
  PyObject* pyPdeParams(NULL);
  PyObject* pyAllResults(NULL);

  if (!PyArg_ParseTuple(pyArgs, "OOOOOOOOOOOO",
    &pyPayoffType, &pyStrike, &pyTimeToExp, &pySpot,
    &pyDiscountCrv, &pyGrowthCrv, &pyDivYield,
    &pyAssetVol, &pyFxVol, &pyCorrel,
    &pyPdeParams, &pyAllResults))
    return NULL;

  int payoffType;
  if (PyLong_Check(pyPayoffType)) {
    payoffType = asInt(pyPayoffType);
  } else if (PyFloat_Check(pyPayoffType)) {
    payoffType = static_cast<int>(asDouble(pyPayoffType));
  } else {
    QF_ASSERT(false, "error: invalid payoff type");
  }

  double strike = asDouble(pyStrike);
  double timeToExp = asDouble(pyTimeToExp);
  double spot = asDouble(pySpot);

  std::string discName = asString(pyDiscountCrv);
  qf::SPtrYieldCurve discyc = qf::market().yieldCurves().get(discName);
  QF_ASSERT(discyc, "error: discount curve " + discName + " not found");

  std::string growName = asString(pyGrowthCrv);
  qf::SPtrYieldCurve growyc = qf::market().yieldCurves().get(growName);
  QF_ASSERT(growyc, "error: growth curve " + growName + " not found");

  double divYield = asDouble(pyDivYield);
  double assetVol = asDouble(pyAssetVol);
  double fxVol = asDouble(pyFxVol);
  double correl = asDouble(pyCorrel);

  qf::PdeParams pdeparams = asPdeParams(pyPdeParams);
  bool allresults = asBool(pyAllResults);

  qf::SPtrProduct spprod(new qf::EuropeanCallPut(payoffType, strike, timeToExp));
  qf::Pde1DResults results;
  qf::Pde1DSolver solver(spprod, discyc, growyc, spot, divYield, assetVol, fxVol, correl, results);
  solver.solve(pdeparams);

  PyObject* ret = PyDict_New();
  PyDict_SetItem(ret, asPyScalar("Price"), asPyScalar(results.prices[0]));

  if (allresults) {
    qf::Vector spots;
    results.getSpotAxis(0, spots);
    qf::Matrix values(results.times.size(), results.values.front().size());
    for (size_t i = 0; i < results.times.size(); ++i)
      for (size_t j = 0; j < results.values.front().size(); ++j)
        values(i, j) = results.values[i](j, 0);

    PyDict_SetItem(ret, asPyScalar("Times"), asNumpy(results.times));
    PyDict_SetItem(ret, asPyScalar("Spots"), asNumpy(spots));
    PyDict_SetItem(ret, asPyScalar("Values"), asNumpy(values));
  }

  return ret;

PY_END;
}





