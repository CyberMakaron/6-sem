import random
import numpy as np
import matplotlib.pyplot as plt
import math

sz = 1000    #кол-во случайных чисел
ld = 4/5    #лямбда

############## Непрерывная случайная величина ###########################
print("\tНЕПРЕРЫВНАЯ СЛУЧАЙНАЯ ВЕЛИЧИНА")
#sz псевдослучайных чисел равномерного распределения
rand_linear = []
for i in range(sz):
    rand_linear.append(random.random())

fig, (ax1, ax2, ax3) = plt.subplots(1, 3)

ax1.hist(rand_linear, 20)

#sz псевдослучайныхслучайных чисел показательного распределения
rand_exp = np.log(rand_linear) * (-1/ld)
ax2.hist(rand_exp, 20)

#среднее арифметическое и выбороная дисперсия полученной выборки
_x = np.mean(rand_exp)
D = np.mean(np.square(rand_exp)) - np.square(_x)
print("Среднее: ", _x, "\nДисперсия: ", D)

#Разобьём интервал выборки на 4 части и вычислим сколько значений приходится на каждый интервал
x = np.linspace(0, np.max(rand_exp), 5)
n = np.zeros(4)
for X in rand_exp:
    i = 0
    while not (x[i] < X <= x[i+1]):
        i += 1
    n[i] += 1
#Вычислим теоретические частоты
p = np.zeros(4)
for i in range(4):
    p[i] = math.exp(-ld * x[i]) - math.exp(-ld * x[i+1])
n_ = p * sz
#Коэффициент ковариации
K = np.sum([(n[i] - n_[i])**2 / n_[i] for i in range(4)])
K_Pirs = 7.77944
print("Коэффициент ковариации: {}, критерий Пирсона: {}\n{}".format(K, K_Pirs,
"Гипотеза верна!" if K_Pirs >= K else "Гипотезу следует отбросить!"))

############## Дискретная случайная величина ###########################
print("\n\tДИСКРЕТНАЯ СЛУЧАЙНАЯ ВЕЛИЧИНА")
#Вычислим сколько значений приходится на каждый интервал
n = np.zeros(4)
x = [0, 41/176, 71/176, 142/176, 176/176]
for X in rand_linear:
    i = 0
    while not (x[i] < X <= x[i+1]):
        i += 1
    n[i] += 1
M = 0 * 41/176 + 1 * 30/176 + 2 * 71/176 + 3 * 34/176
x_ = (0 * n[0] + 1 * n[1] + 2 * n[2] + 3 * n[3])/sz
D = (0**2 * n[0] + 1**2 * n[1] + 2**2 * n[2] + 3**2 * n[3])/sz - x_**2
print("####### n = ", n)
print("Мат. ожидание: {}\nСреднее: {}\nДисперсия: {}".format(M, x_, D))
#Полученныя частота
#p = n/sz
#print("Полученные частоты: ", p)
#Теоретические частоты
p_teor = [41/176, 30/176, 71/176, 34/176]
n_ = np.multiply(p_teor, sz)
print("Теоретические частоты: ", n_, "\nполученные частоты: ", n)
#Коэффициент ковариации
K = np.sum([(n[i] - n_[i])**2 / n_[i] for i in range(4)])
print("Коэффициент ковариации: {}, критерий Пирсона: {}\n{}".format(K, K_Pirs,
"Гипотеза верна!" if K_Pirs >= K else "Гипотезу следует отбросить!"))
#Гистограмма распределения ДСВ
rand_discr = []
for i in range(4):
    for j in range(int(n[i])):
        rand_discr.append(i)
ax3.hist(rand_discr, 4)
print(30/176+4*71/176+9*34/176 - M**2)
plt.show()
