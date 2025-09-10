# qflib – Quant Finance Library

A fast C++ quantitative finance library with Python bindings for option pricing, yield curves, and risk modeling.

qflib is a quantitative finance library providing mathematical utilities, market object creation, and option pricing models (closed-form, Monte Carlo, and PDE).  
This README serves as a complete reference to all relevant directories and functions exposed via `qflib.pyqflib` and a usage example.

---

## qflib and pyqflib

The repository is organized into two main components:

### `/qflib` - C++ Core
The qflib library contains all of the C++ source code for the project.  
It implements the core quantitative finance functionality, including:
- Mathematical utilities (outer products, polynomial operations, error functions, matrix decompositions, eigenvalues, etc.)
- Yield curve and volatility term structure objects
- Forward pricing and compounding conversions
- Black–Scholes option pricing (European, digital, knockout)
- Monte Carlo simulation engines
- PDE solvers for European, digital, American, and quanto options
- Quanto-specific extensions for cross-currency option pricing

This C++ layer provides the computational efficiency and numerical stability for the library.

---

### `/pyqflib` - Python Wrapper Layer
The pyqflib module exposes the C++ functionality to Python via wrapper functions.  
It allows users to call the full power of qflib from Python scripts and notebooks, making it accessible for analysis and experimentation.  
This layer includes:
- Pythonic wrapper functions around the C++ implementations
- High-level APIs for pricing options, building market objects, and running simulations
- Example scripts and homework assignments showing usage in Python

---

## Homework and Project

### `/homework` – Quantitative Finance Assignments
This folder contains 11 homework assignments that showcase different applications of the `qflib` library.  
Each homework file:  
- Demonstrates one or more wrapper functions in Python  
- Combines theoretical concepts with implementation examples  
- Covers a variety of topics implied by the function set, including:  
  - Yield curve and volatility term structure creation  
  - Discounting, forward rates, and compounding conversions  
  - Cholesky decompositions, eigenvalue methods, and spectral truncation  
  - Monte Carlo simulation for European and Asian basket options  
  - PDE methods for European, digital, and American options  
  - Forward and knockout forward pricing

Together, these assignments serve as a walkthrough of the library’s core functionality while reinforcing quantitative finance methods.
 
### `/project` – Quanto Option Pricing Analysis
This folder contains a dedicated project focused on the analysis and implementation of quanto option pricing models.  
It demonstrates how to use the `qflib` wrapper functions in Python to:  
- Price quanto European options under the Black–Scholes framework  
- Compare results across closed-form, Monte Carlo, and PDE approaches  
- Analyze the effect of FX volatility and correlations on pricing  

This project serves as a practical application of the library to a complex derivatives pricing problem.

--- 

## Function Index

### Function Group 0 – Core Math / Utilities
- `version()` → qflib version string  
- `sayHello(name)` → Returns `"Hello <name>"`  
- `outerProd(v1, v2)` → Outer product of two vectors  
- `polyProd(v1, v2)` → Multiply two polynomials  
- `erf(x)` → Error function  
- `invErf(x)` → Inverse error function  
- `normalCdf(x)` → Standard normal CDF  
- `normalInvCdf(x)` → Inverse standard normal CDF  
- `echoMatrix(mat)` → Echo back input matrix  
- `cholDcmp(mat)` → Cholesky decomposition  
- `eigenSym(mat)` → Eigenvalues & eigenvectors of symmetric matrix  
- `specTrunc(mat)` → Nearest positive-definite matrix (spectral truncation)  
- `ppolyEval(...)` → Evaluate piecewise polynomial at points  
- `ppolyIntegral(...)` → Integrate piecewise polynomial  
- `ppolySum(...)` → Sum of two piecewise polynomials  
- `polyBracket(...)` → Bracket polynomial roots  
- `polySecant(...)` → Root search via secant method  
- `toContCmpd(rate, annfreq)` → Convert periodic → continuous compounding  
- `fromContCmpd(rate, annfreq)` → Convert continuous → periodic compounding  

---

### Function Group 1 – Forward & Option Pricing
- `fwdPrice(...)` → Forward price of an asset  
- `qFwdPrice(...)` → Quanto forward price  
- `digiBS(...)` → Digital option (Black–Scholes), price & Greeks  
- `euroBS(...)` → European option (Black–Scholes), price & Greeks  
- `koFwd(...)` → Knockout forward price  

---

### Function Group 2 – Market Objects
- `mktList()` → List all market objects (yield curves, vols)  
- `mktClear()` → Clear all market objects  
- `ycCreate(...)` → Create yield curve  
- `discount(ycname, tmat)` → Discount factor from curve  
- `fwdDiscount(ycname, t1, t2)` → Forward discount factor  
- `spotRate(ycname, tmat)` → Spot interest rate  
- `fwdRate(ycname, t1, t2)` → Forward interest rate  
- `volCreate(...)` → Create volatility term structure  
- `spotVol(volname, tmat)` → Spot volatility  
- `fwdVol(volname, t1, t2)` → Forward volatility  

---

### Function Group 3 – Monte Carlo Pricing
- `euroBSMC(...)` → Monte Carlo price of European option (Black–Scholes)  
- `asianBasketBSMC(...)` → Monte Carlo price of Asian basket option (Black–Scholes)  

---

### Function Group 4 – PDE Pricing
- `euroBSPDE(...)` → PDE price of European option  
- `digiBSPDE(...)` → PDE price of digital option  
- `amerBSPDE(...)` → PDE price of American option  

---

### Function Group 5 – Quanto Options
- `qEuroBS(...)` → Quanto European option (closed-form)  
- `qEuroBSMC(...)` → Quanto European option (Monte Carlo)  
- `qEuroBSPDE(...)` → Quanto European option (PDE)

---

## Usage Example

Here’s a quick example showing how to use the Python wrapper:

```python
import pyqflib as qf

# Check version
print(qf.version())

# Price a European call option using Black–Scholes
price, delta, gamma, vega, theta, rho = qf.euroBS(
    spot=100.0,
    strike=100.0,
    rate=0.05,
    vol=0.2,
    tmat=1.0,
    optionType="call"
)

print(f"European Call Price: {price:.4f}, Delta: {delta:.4f}")
```

Authors/Developers

Michael Sotiropoulos, PhD  
Managing Director, Quantitative Execution at Imperative Execution Inc.  

Omar Faruque  
Master’s in Quantitative Finance – Fordham University
