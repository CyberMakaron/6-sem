import random
import math
import numpy
import matplotlib.pyplot as pyplot
from numpy import linalg as la

i_ = 41
j_ = 30
k_ = 17
sz = 10000              #количество испытаний             

ab = j_  / (j_ + k_)
ac = k_ / (j_ + k_) 
ba = i_ / (i_ + j_)
bc = j_ / (i_ + j_)
ca = i_ / (i_ + j_ + k_)
cb = j_ / (i_ + j_ + k_)
cc = k_ / (i_ + j_ + k_)


# Матрица переходных вероятностей
P = numpy.array([
    [0,  ab,  ac], # A
    [ba,  0,  bc], # B
    [ca, cb,  cc]]) # C
#     A    B    C
print("Матрица переходных вероятностей P:\n", P)

for k in range (0, 3):
    state = base_state = k  #состояние: 0 - A, 1 - B, 2 - C
    count = 0               # количество перелётов
    for i in range(0, sz):
        r = random.random()
        j = 0
        while r > P[state][j]:
            r -= P[state][j]
            j += 1
        state = j
        count += 1
        while state != base_state:
            r = random.random()
            j = 0
            while r > P[state][j]:
                r -= P[state][j]
                j += 1
            state = j
            count += 1
    count /= sz
    print("В ходе эксперемента муха возвращалась в точку {} в среднем за {} перелётов".format(chr(65+k), count))

print("\tТеперь посчитаем теоретически среднее количество перелётов")
#t - непдвижная точка, P1 - матрица коэффициетов СЛАУ
P1 = [[-1,  ab,  ac],
     [ca, cb, cc-1],
     [1, 1,  1]]
b = [0, 0, 1]   #столбец свободных членов
t = la.solve(P1, b)
print("Неподвижная точка t:")
print(t)
print("Проверим, умножив на матрицу переходных вероятностей (t*P):")
print(numpy.dot(P, t))
M = 1/t
print("Среднее количество перелётов для возвращения в каждую точку M:")
print(M)