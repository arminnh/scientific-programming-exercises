% calculate the amount of points that are necessary in the composite trapezoid rule to achieve the desired precision
% calculate the amount by using the error bound for the composite trapezoid rule
function [ y ] = getCompositeTrapeziumN(fun, a, b, precision)
    syms x N
    secondDerivative(x)  = diff(fun(x), 2);
    
    maxXSecondDerivative = fminbnd(matlabFunction(-1 * secondDerivative), -1, 1);
    maxSecondDerivative  = max([secondDerivative(maxXSecondDerivative) secondDerivative(-1) secondDerivative(1)]);
    
    error = ((5 * (b - a)^3) / (12 * N^2)) * maxSecondDerivative;
    
    y = max(floor(solve(error < precision, N, 'Real', true)));
end