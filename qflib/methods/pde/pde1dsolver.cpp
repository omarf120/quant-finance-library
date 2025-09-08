/**
@file  pde1dsolver.cpp
@brief Implementation of the 1-dim PDE solver class
*/

#include <qflib/methods/pde/pde1dsolver.hpp>
#include <qflib/math/interpol/interpolation1d.hpp>

BEGIN_NAMESPACE(qf)

/** Solves backwards from one time step to the previous */
void Pde1DSolver::solveFromStepToStep(ptrdiff_t step, double DT)
{
  // initialise operators
  GridAxis& grax = gridAxes_[0];
  deltaOpExplicit_.init(grax.drifts, DT, grax.DX, 1.0 - theta_);
  deltaOpImplicit_.init(grax.drifts, DT, grax.DX, theta_);

  gammaOpExplicit_.init(grax.variances, DT, grax.DX, 1.0 - theta_);
  gammaOpImplicit_.init(grax.variances, DT, grax.DX, theta_);

  // build the explicit and implicit operators
  opExplicit_.init(grax.NX, 0.0, 1.0, 0.0);
  opExplicit_ += deltaOpExplicit_;
  opExplicit_ += gammaOpExplicit_;
  opImplicit_.init(grax.NX, 0.0, 1.0, 0.0);
  opImplicit_ -= deltaOpImplicit_;
  opImplicit_ -= gammaOpImplicit_;

  // adjust for boundary conditions
  adjustOpsForBoundaryConditions(opExplicit_, opImplicit_, grax.DX);

  // main PDE step loop over variables
  for (size_t j = 0; j < nLayers_; ++j) {
    auto v1 = prevValues->col(j);
    auto v2 = currValues->col(j);
    opExplicit_.apply(v1, v2);
    opImplicit_.applyInverse(v2, v1);
  }

  applyBoundaryConditions(*prevValues);
}

void Pde1DSolver::initValLayers()
{
  QF_ASSERT(nFactors() == 1, "1D PDE is handles 1 asset only!");
  values1.resize(gridAxes_[0].NX + 2, nLayers_);
  values2.resize(gridAxes_[0].NX + 2, nLayers_);

  for (size_t i = 0; i < gridAxes_[0].NX + 2; ++i)
    for (size_t j = 0; j < nLayers_; ++j)
      values1(i, j) = values2(i, j) = 0.0;

  prevValues = &values1;
  currValues = &values2;

  results_.times.resize(nSteps_);
  results_.values.resize(nSteps_);
  if (storeAllResults_)
    results_.values.resize(nSteps_);
}

/** Evaluates the product at the passed-in time step index */
void Pde1DSolver::evalProduct(size_t stepIdx)
{
  ptrdiff_t eventIdx = stepindex_[stepIdx];
  if (eventIdx >= 0) {
    Vector const & payTms = spprod_->payTimes();
    double fixtime = spprod_->fixTimes()[eventIdx];
    Vector spots(1);
    for (size_t node = 0; node <= gridAxes_[0].NX + 1; ++node) {
      spots[0] = gridAxes_[0].Slevels[node];
      spprod_->eval(eventIdx, spots, (*prevValues)(node, 0));
      Vector const & payAms = spprod_->payAmounts();
      size_t i = 0;
      while (fixtime > payTms[i]) ++i;
      (*prevValues)(node, 0) = payAms[eventIdx];
    }
  }
  results_.times[stepIdx] = timesteps_[stepIdx];
  if (storeAllResults_)
    results_.values[stepIdx] = *prevValues;
}

void Pde1DSolver::storeResults()
{
  results_.gridAxes = gridAxes_;
  results_.prices.resize(nLayers_);
  for (size_t j = 0; j < nLayers_; ++j) {
    double X0 = gridAxes_[0].coordinateChange->fromRealToDiffused(spots_[0]);
    Vector temp(prevValues->col(j));
    LinearInterpolation1D<Vector> interp(gridAxes_[0].Xlevels, temp);
    results_.prices[j] = interp.getValue(X0);
  }
}

void Pde1DSolver::discountFromStepToStep(double df)
{
  *prevValues *= df;
}

Pde1DSolver::Pde1DSolver(
  SPtrProduct product,
  SPtrYieldCurve discountYieldCurve,
  SPtrYieldCurve growthYieldCurve,
  double spot,
  double divyield,
  double assetVol,
  double fxVol,
  double correl,
  Pde1DResults& results,
  bool storeAllResults)
: PdeBase(product), results_(results), storeAllResults_(storeAllResults),
  assetVol_(assetVol), fxVol_(fxVol), correl_(correl), isQuanto_(true)
{
nAssets_ = product->nAssets();
nLayers_ = 1;
spdiscyc_ = discountYieldCurve;
spots_.push_back(spot);
spaccrycs_.push_back(discountYieldCurve);  
divyields_.push_back(divyield);

std::vector<double> times = {1.0};
std::vector<double> vols = {assetVol};
SPtrVolatilityTermStructure svol = std::make_shared<VolatilityTermStructure>(
  times.begin(), times.end(),
  vols.begin(), vols.end(),
  VolatilityTermStructure::VolType::SPOTVOL
);
vols_.push_back(svol);

double rd = discountYieldCurve->spotRate(product->fixTimes().back());
double rf = growthYieldCurve->spotRate(product->fixTimes().back());

double quantoDivYield = rd - rf + divyield - correl * assetVol * fxVol;
divyields_[0] = quantoDivYield;  
}


END_NAMESPACE(qf)
