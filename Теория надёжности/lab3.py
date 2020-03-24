import random
import math
import numpy
import matplotlib.pyplot as pyplot

i_ = 41
j_ = 30
k_ = 17
sz = 10000              #количество испытаний
base_state = 0
state = base_state      # текущее состояние: 0 - A, 1 - B, 2 - C
count = 0               # количество перелётов

ab = j_  / (j_ + k_)
ac = k_ / (j_ + k_) 
ba = i_ / (i_ + j_)
bc = j_ / (i_ + j_)
ca = i_ / (i_ + j_ + k_)
cb = j_ / (i_ + j_ + k_)
cc = k_ / (i_ + j_ + k_)


# Матрица переходных вероятностей
#     A    B    C
P = [[0,  ab,  ac], # A
     [ba,  0,  bc], # B
     [ca, cb,  cc]] # C
print(P)

for i in range(0, sz):
    r = random.random()
    j = 0
    while r > P[state][j]:
        r -= P[state][j]
        j += 1
    state = j
    count += 1
    while state != 0:
        r = random.random()
        j = 0
        while r > P[state][j]:
            r -= P[state][j]
            j += 1
        state = j
        count += 1
count /= sz
print("В ходе эксперемента муха возвращалась в точку А в среднем за {} перелётов".format(count))

'''Q = [[0,  lo, 0 ],
     [ol, oo, op], 
     [0,  po, 0 ]]

R = [[lr, 0], 
     [0, 0], 
     [0, pk]]

K_t = 1.64     # 6 степеней свободы
N = numpy.linalg.inv(numpy.eye(3) - Q)
print("Теория:")
print("N:\n", N)
B = N.dot(R)
print("B:\n", B)
print("\tРека: ", int(B[base_state-2][0]*sz))
print("\tКопья: ", int(B[base_state-2][1]*sz))
print("\tСреднее время жизни: ", numpy.sum([N[base_state-2][i] for i in range(0, 3)]))
print("Проверка гипотезы о том, что экспериментальная вероятность поглотиться в S_0 (река), начиная с S_3 (обе ноги) соотвествует теоретическому значению:")
p0 = math.sqrt(B[base_state-2][0])
K_v = (river_count/sz - p0)/(math.sqrt(p0*(1-p0)/6))
print("К_в = {}, К_т: {}\n{}".format(K_v, K_t,
"Гипотеза верна!" if abs(K_v) < K_t else "Гипотезу следует отбросить!"))'''
