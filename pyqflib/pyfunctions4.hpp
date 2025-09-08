/**
@file  pyfunctions4.hpp
@brief Implementation of Python callable functions
*/

#include <pyqflib/pyutils.hpp>

#include <qflib/market/market.hpp>
#include <qflib/products/europeancallput.hpp>
#include <qflib/methods/pde/pde1dsolver.hpp>
#include <qflib/market/volatilitytermstructure.hpp>
#include <qflib/products/digitalcallput.hpp>
#include <qflib/products/americancallput.hpp>


using namespace std;

static
PyObject* pyQfEuroBSPDE(PyObject* pyDummy, PyObject* pyArgs)
{
PY_BEGIN;

  PyObject* pyPayoffType(NULL);
  PyObject* pyStrike(NULL);
  PyObject* pyTimeToExp(NULL);
  PyObject* pySpot(NULL);
  PyObject* pyDiscountCrv(NULL);
  PyObject* pyDivYield(NULL);
  PyObject* pyVolatility(NULL);
  PyObject* pyPdeParams(NULL);
  PyObject* pyAllResults(NULL);

  if (!PyArg_ParseTuple(pyArgs, "OOOOOOOOO",
    &pyPayoffType, &pyStrike, &pyTimeToExp,
    &pySpot, &pyDiscountCrv, &pyDivYield,
    &pyVolatility, &pyPdeParams, &pyAllResults))
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

  std::string name = asString(pyDiscountCrv);
  qf::SPtrYieldCurve spyc = qf::market().yieldCurves().get(name);
  QF_ASSERT(spyc, "error: yield curve " + name + " not found");

  double divYield = asDouble(pyDivYield);
  qf::SPtrVolatilityTermStructure svol;

  if (PyFloat_Check(pyVolatility) || PyLong_Check(pyVolatility)) {
    double vol = asDouble(pyVolatility);
    std::vector<double> times = {1.0};
    std::vector<double> vols = {vol};
    svol = std::make_shared<qf::VolatilityTermStructure>(
      times.begin(), times.end(),
      vols.begin(), vols.end(),
      qf::VolatilityTermStructure::VolType::SPOTVOL);
  } else if (PyUnicode_Check(pyVolatility)) {
    std::string vname = asString(pyVolatility);
    svol = qf::market().volatilities().get(vname);
    QF_ASSERT(svol, "error: volatility term structure " + vname + " not found");
  } else {
    QF_ASSERT(false, "error: unsupported type for volatility input");
  }

  qf::PdeParams pdeparams = asPdeParams(pyPdeParams);
  bool allresults = asBool(pyAllResults);

  qf::SPtrProduct spprod(new qf::EuropeanCallPut(payoffType, strike, timeToExp));
  qf::Pde1DResults results;
  qf::Pde1DSolver solver(spprod, spyc, spot, divYield, svol, results);
  solver.solve(pdeparams);

  PyObject* ret = PyDict_New();
  int ok = PyDict_SetItem(ret, asPyScalar("Price"), asPyScalar(results.prices[0]));

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

static
PyObject* pyQfDigiBSPDE(PyObject* pyDummy, PyObject* pyArgs)
{
PY_BEGIN;

  PyObject* pyPayoffType(NULL);
  PyObject* pyStrike(NULL);
  PyObject* pyTimeToExp(NULL);
  PyObject* pySpot(NULL);
  PyObject* pyDiscountCrv(NULL);
  PyObject* pyDivYield(NULL);
  PyObject* pyVolatility(NULL);
  PyObject* pyPdeParams(NULL);
  PyObject* pyAllResults(NULL);

  if (!PyArg_ParseTuple(pyArgs, "OOOOOOOOO",
    &pyPayoffType, &pyStrike, &pyTimeToExp,
    &pySpot, &pyDiscountCrv, &pyDivYield,
    &pyVolatility, &pyPdeParams, &pyAllResults))
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

  std::string name = asString(pyDiscountCrv);
  qf::SPtrYieldCurve spyc = qf::market().yieldCurves().get(name);
  QF_ASSERT(spyc, "error: yield curve " + name + " not found");

  double divYield = asDouble(pyDivYield);
  qf::SPtrVolatilityTermStructure svol;

  if (PyFloat_Check(pyVolatility)) {
    double vol = asDouble(pyVolatility);
    std::vector<double> times = {1.0};
    std::vector<double> vols = {vol};
    svol = std::make_shared<qf::VolatilityTermStructure>(
      times.begin(), times.end(),
      vols.begin(), vols.end(),
      qf::VolatilityTermStructure::VolType::SPOTVOL);
  } else {
    std::string vname = asString(pyVolatility);
    svol = qf::market().volatilities().get(vname);
    QF_ASSERT(svol, "error: volatility term structure " + vname + " not found");
  }

  qf::PdeParams pdeparams = asPdeParams(pyPdeParams);
  bool allresults = asBool(pyAllResults);

  qf::SPtrProduct spprod(new qf::DigitalCallPut(payoffType, strike, timeToExp));
  qf::Pde1DResults results;
  qf::Pde1DSolver solver(spprod, spyc, spot, divYield, svol, results);
  solver.solve(pdeparams);

  PyObject* ret = PyDict_New();
  int ok = PyDict_SetItem(ret, asPyScalar("Price"), asPyScalar(results.prices[0]));

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

static
PyObject*  pyQfAmerBSPDE(PyObject* pyDummy, PyObject* pyArgs)
{
PY_BEGIN;

  PyObject* pyPayoffType(NULL);
  PyObject* pySpot(NULL);
  PyObject* pyStrike(NULL);
  PyObject* pyTimeToExp(NULL);
  PyObject* pyDiscountCrv(NULL);
  PyObject* pyDivYield(NULL);
  PyObject* pyVolatility(NULL);
  PyObject* pyPdeParams(NULL);
  PyObject* pyAllResults(NULL);

  if (!PyArg_ParseTuple(pyArgs, "OOOOOOOOO", &pyPayoffType, &pyStrike, &pyTimeToExp, 
    &pySpot, &pyDiscountCrv, &pyDivYield, &pyVolatility, &pyPdeParams, &pyAllResults))
    return NULL;

  int payoffType = asInt(pyPayoffType);
  double spot = asDouble(pySpot);
  double strike = asDouble(pyStrike);
  double timeToExp = asDouble(pyTimeToExp);

  std::string name = asString(pyDiscountCrv);
  qf::SPtrYieldCurve spyc = qf::market().yieldCurves().get(name);
  QF_ASSERT(spyc, "error: yield curve " + name + " not found");

  double divYield = asDouble(pyDivYield);
  qf::SPtrVolatilityTermStructure svol;
  if (PyFloat_Check(pyVolatility)) {
      double vol = asDouble(pyVolatility);
      std::vector<double> times = {1.0};
      std::vector<double> vols = {vol};
      svol = std::make_shared<qf::VolatilityTermStructure>(
          times.begin(), times.end(),
          vols.begin(), vols.end(),
          qf::VolatilityTermStructure::VolType::SPOTVOL);
  } else {
      std::string vname = asString(pyVolatility);
      svol = qf::market().volatilities().get(vname);
      QF_ASSERT(svol, "error: volatility term structure " + vname + " not found");
  }

  // read the PDE parameters
  qf::PdeParams pdeparams = asPdeParams(pyPdeParams);
  // read the allresults flag
  bool allresults = asBool(pyAllResults);

  // create the product
  qf::SPtrProduct spprod(new qf::AmericanCallPut(payoffType, strike, timeToExp));
  // create the PDE solver
  qf::Pde1DResults results;
  bool storeAllResults = true;
  qf::Pde1DSolver solver(spprod, spyc, spot, divYield, svol, results, storeAllResults);
  solver.solve(pdeparams);

  // write results
  PyObject* ret = PyDict_New();
  int ok = PyDict_SetItem(ret, asPyScalar("Price"), asPyScalar(results.prices[0]));

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