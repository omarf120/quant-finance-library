/**
@file  digitalcallput.hpp
@brief The payoff of a European Digital Call/Put option
*/

#ifndef QF_DIGITALCALLPUT_HPP
#define QF_DIGITALCALLPUT_HPP

#include <qflib/products/product.hpp>

BEGIN_NAMESPACE(qf)

/** The European digital call/put class */
class DigitalCallPut : public Product
{
public:
  /** Initializing ctor */
  DigitalCallPut(int payoffType, double strike, double timeToExp);

  /** The number of assets this product depends on */
  virtual size_t nAssets() const override { return 1; }

  /** Evaluates the product given the passed-in path */
  virtual void eval(Matrix const& pricePath) override;

  /** Evaluates the product at fixing time index idx */
  virtual void eval(size_t idx, Vector const& spots, double contValue) override;

private:
  int payoffType_;     // 1: call; -1: put
  double strike_;
  double timeToExp_;
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
DigitalCallPut::DigitalCallPut(int payoffType, double strike, double timeToExp)
  : payoffType_(payoffType), strike_(strike), timeToExp_(timeToExp)
{
  QF_ASSERT(payoffType == 1 || payoffType == -1, "DigitalCallPut: the payoff type must be 1 (call) or -1 (put)!");
  QF_ASSERT(strike > 0.0, "DigitalCallPut: the strike must be positive!");
  QF_ASSERT(timeToExp > 0.0, "DigitalCallPut: the time to expiration must be positive!");

  fixTimes_.resize(1);
  fixTimes_[0] = timeToExp_;

  payTimes_.resize(1);
  payTimes_[0] = timeToExp_;

  payAmounts_.resize(1);
}

inline void DigitalCallPut::eval(Matrix const& pricePath)
{
  double S_T = pricePath(0, 0);
  if (payoffType_ == 1)
    payAmounts_[0] = S_T >= strike_ ? 1.0 : 0.0;
  else
    payAmounts_[0] = S_T <= strike_ ? 1.0 : 0.0;
}

inline void DigitalCallPut::eval(size_t idx, Vector const& spots, double contValue)
{
  QF_ASSERT(idx == 0, "DigitalCallPut: wrong fixing time index!");
  double S_T = spots[idx];
  if (payoffType_ == 1)
    payAmounts_[idx] = S_T >= strike_ ? 1.0 : 0.0;
  else
    payAmounts_[idx] = S_T <= strike_ ? 1.0 : 0.0;
}

END_NAMESPACE(qf)

#endif // QF_DIGITALCALLPUT_HPP
