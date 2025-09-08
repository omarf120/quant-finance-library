#ifndef QF_PDE1DSOLVER_HPP
#define QF_PDE1DSOLVER_HPP

#include <qflib/methods/pde/pdebase.hpp>
#include <qflib/methods/pde/tridiagonalops1d.hpp>
#include <qflib/methods/pde/pderesults.hpp>

BEGIN_NAMESPACE(qf)

class Pde1DSolver : public PdeBase
{
public:
  // 6-argument constructor (backward-compatible)
  Pde1DSolver(SPtrProduct product,
              SPtrYieldCurve discountYieldCurve,
              double spot,
              double divyield,
              SPtrVolatilityTermStructure svol,
              Pde1DResults& results)
  : Pde1DSolver(product, discountYieldCurve, spot, divyield, svol, results, false)
  {}

  // 7-argument standard PDE constructor
  Pde1DSolver(SPtrProduct product,
              SPtrYieldCurve discountYieldCurve,
              double spot,
              double divyield,
              SPtrVolatilityTermStructure svol,
              Pde1DResults& results,
              bool storeAllResults)
  : PdeBase(product), results_(results), storeAllResults_(storeAllResults), isQuanto_(false)
  {
    nAssets_ = product->nAssets();
    nLayers_ = 1;
    spdiscyc_ = discountYieldCurve;
    spots_.push_back(spot);
    spaccrycs_.push_back(discountYieldCurve);
    divyields_.push_back(divyield);
    vols_.push_back(svol);
  }

  // 10-argument quanto constructor
  Pde1DSolver(SPtrProduct product,
              SPtrYieldCurve discountYieldCurve,
              SPtrYieldCurve growthYieldCurve,
              double spot,
              double divyield,
              double assetVol,
              double fxVol,
              double correl,
              Pde1DResults& results,
              bool storeAllResults = false);
  
  virtual ~Pde1DSolver() override {}

  virtual void solveFromStepToStep(ptrdiff_t step, double DT) override;
  virtual void initValLayers() override;
  virtual void evalProduct(size_t stepIdx);
  virtual void storeResults();
  virtual void discountFromStepToStep(double df);

protected:
  bool isQuanto_;
  double assetVol_, fxVol_, correl_;

  DeltaOp1D<Vector> deltaOpExplicit_, deltaOpImplicit_;
  GammaOp1D<Vector> gammaOpExplicit_, gammaOpImplicit_;
  TridiagonalOp1D<Vector> opExplicit_, opImplicit_;

  bool storeAllResults_;
  Pde1DResults& results_;

  Matrix values1, values2;
  Matrix* prevValues, * currValues;
};

END_NAMESPACE(qf)

#endif
