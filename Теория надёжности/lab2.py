import random
import math
import numpy
import matplotlib.pyplot as pyplot

i_ = 41
j_ = 30
k_ = 17
sz = 10000 #количество испытаний
base_state = 3
state = base_state   # текущее состояние: 0 - Р, 1 - К, 2 - Л, 3 - О, 4 - П
spears_count = 0    # сколько раз на копья
river_count = 0     # сколько раз в реку
average_lifetime = 0    # средне время жизни

lr = i_ / (i_ + j_)
lo = j_ / (i_ + j_)
ol = i_ / (i_ + j_ + k_)
oo = j_ / (i_ + j_ + k_)
op = k_ / (i_ + j_ + k_)
po = j_  / (j_ + k_)
pk = k_ / (j_ + k_) 

# Матрица переходных вероятностей
#     Р   К   Л   О   П
P = [[1,  0,  0,  0,  0 ], # Р
     [0,  1,  0,  0,  0 ], # К
     [lr, 0,  0,  lo, 0 ], # Л
     [0,  0,  ol, oo, op], # О
     [0,  pk, 0,  po, 0 ]] # П
print(P)

for i in range(0, sz):
     state = base_state
     k = 0          # количество шагов
     while state >= 2:
          r = random.random()
          j = 0
          while r > P[state][j]:
               r -= P[state][j]
               j += 1
          state = j
          k += 1
     if state == 0:
          river_count += 1
     elif state == 1:
          spears_count += 1
     average_lifetime += k
average_lifetime /= sz

print("Эксперемент:")
print("\tРека: ", river_count)
print("\tКопья: ", spears_count)
print("\tСреднее время жизни: ", average_lifetime)

Q = [[0,  lo, 0 ],
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
"Гипотеза верна!" if abs(K_v) < K_t else "Гипотезу следует отбросить!"))
