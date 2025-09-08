# qflib library examples

#%%
import qflib as qf
import numpy as np
import os

ver = qf.version()
print(f'qflib version: {ver}')
pid = os.getpid()
print(f'pid: {pid}')

# Greet
print('----------------')
name = "World"
print(qf.sayHello(name))

# Outer product
print('----------------')
x = [1, 2, 3]
y = [4, 5]
op = qf.outerProd(x, y)
print(f'x: {x}\ny: {y}')
print(f'outerProd:\n{op}')

# Matrix
print('----------------')
m = np.array([[1, 2, 3], [4, 5, 6]])
em = qf.echoMatrix(m)
print(f'orig matrix:\n{m}')
print(f'echo matrix:\n{em}')

#PPolyEval
print('----------------')
xbpt = np.arange(1, 6)
yval =  np.arange(-10, 15, 5)
pord = 1
xval = np.arange(0.5, 6.0, 0.5)
pval = qf.ppolyEval(xbpt, yval, pord, xval, 0)
pder = qf.ppolyEval(xbpt, yval, pord, xval, 1)
pint = qf.ppolyIntegral(xbpt, yval, pord, xval[0], xval)
print('Piecewise polynomial')
print(f'bkpts={xbpt}')
print(f'yvals={yval}')
print(f'pord={pord}')
print(f'xval={xval}')
print(f'pval={pval}')
print(f'pder={pder}')
print(f'pint={pint}')

#Root bracketing and secant method search
print('----------------')
pcoeffs = [0, 2, 1]
lolim = -4
uplim = 3
nsubs = 10

brkts = qf.polyBracket(pcoeffs, lolim, uplim, nsubs)
print(f"Roots of polynomial: {pcoeffs}")
print(f"Root brackets:\n {brkts}")

root = qf.polySecant(pcoeffs, brkts[0, 0], brkts[0, 1], 1e-12)
print(f"root: {root:.4f}")

#ToContCmpd, FromContComp
print('----------------')
anfreq = 1
inrate = 0.10
ccrate = qf.toContCmpd(inrate, anfreq)
outrate = qf.fromContCmpd(ccrate, anfreq)
print('To and from continuous compounding')
print(f'InRate={inrate:.4f}, CCRate={ccrate:.4f}, PerRate={outrate:.4f}')

#%%
# function group 1
print('================')
print('Analytic prices')

#fwdprice
fwdpx = qf.fwdPrice(spot = 100, timetoexp = 1.0, intrate = 0.02, divyield = 0.04)
print('Forward price analytic solution')
print(f'Price={fwdpx:.4f}')

digi = qf.digiBS(payofftype = 1, spot = 100, timetoexp = 1.0, strike = 100,
                 intrate = 0.02, divyield = 0.04, volatility = 0.2)
print('Digital option using Black-Scholes analytic solution')
print(f'Price={digi:.4f}')

euro = qf.euroBS(payofftype = 1, spot = 100, timetoexp = 1.0, strike = 100,
                intrate = 0.02, divyield = 0.04, volatility = 0.4)
print('European option using Black-Scholes analytic solution')
print(f'Price={euro:.4f}')

#%%
# function group 2
print('================')
print('Market objects')

#yccreate
yc = qf.ycCreate(ycname = 'USD', 
                 tmats =  [1/12,  1/4,  1/2,   3/4,    1,     2,    3,     4,    5,      10],
                 vals = [0.01,   0.02, 0.03, 0.035, 0.04, 0.045, 0.05, 0.055, 0.0575, 0.065],
                 valtype = 0)
print(f'Created yield curve: {yc}')

#discount, spotrate, fwdrate
df = qf.discount(ycname = yc, tmat = 2)
spotrate = qf.spotRate(ycname = yc, tmat = 2)
fwdrate = qf.fwdRate(ycname = yc, tmat1 = 1, tmat2 = 2)
print(f'DF={df:.4f}, SpotRate={spotrate:.4f} FwdRate={fwdrate:.4f}')

print('Market list')
print(qf.mktList())