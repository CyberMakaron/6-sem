import numpy as np
import matplotlib.pyplot as plt
import random

N = 11
phi_num = 6
sigma_const = 0.5

def Fx(x):
    return x * np.sqrt(x)
def phi_i(i, x):
    return np.cos(i*x) + np.sin(i*x)

"""def Fx(x):
    return np.log(x**2)
def phi_i(i, x):
    return np.exp((i-1)*x)"""


""" def Fx(x):
  return np.sin(np.exp(0.2*x)) 
def phi_i(i, x):
    return np.sin(i**2*x) """

y_t = []
x = []
y = []
for i in range(1, N + 1):
    y_t.append(Fx(i))
    x.append(i)

sigma = []
for i in range (1, N + 1):
    sigma.append(sigma_const * i * 0.01)
    a = random.random()
    y.append(y_t[i-1] + (sigma[i-1] if int(a*10) % 2 == 0 else -sigma[i-1]))
    #y.append(y_t[i-1] + sigma[i-1])

K = np.zeros((N, N))
for i in range(0, N):
    K[i][i] = sigma[i]**(-2)

F = np.zeros((N, phi_num))
for j in range(0, N):
    for i in range(1, phi_num + 1):
        F[j][i-1] = phi_i(i, x[j])

y_vec = np.zeros((N, 1))
for i in range (0, N):
    y_vec[i][0] = y[i]

#(F^tr * K * F)^-1 * F_tr * K * y
a = np.linalg.inv(F.transpose().dot(K).dot(F)).dot(F.transpose()).dot(K).dot(y_vec)

def Zx(x):
    res = 0
    for i in range(1, phi_num + 1):
        res += a[i-1][0] * phi_i(i, x)
    return res

Z = [Zx(i) for i in x]

print("F(4.5) = ", Fx(4.5))
print("Z(4.5) = ", Zx(4.5))
print("F(9) = ", Fx(9))
print("Z(9) = ", Zx(9))

plt.scatter(x, y, s = 15, label = "y_сигма")
plt.scatter(x, y_t, marker = "s", s = 15, label = "y_точное")
plt.plot(x, Z, label = "Z")

x = np.arange(1, 11, 0.1)
test = []
for i in x:
    test.append(Zx(i))
plt.plot(x, test, label = "Z_точная")

plt.legend()
plt.show()
