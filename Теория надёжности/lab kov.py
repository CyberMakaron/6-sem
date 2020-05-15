import random
import math
import numpy
from itertools import combinations
from prettytable import PrettyTable

print('''Система имеет вид: 
  ┌──[  0  ]──[  2  ]──[  4  ]──┐
──┤                             ├──
  └──[  1  ]──[  3  ]──[  5  ]──┘
''')

n = 6          # Количество элементов в системе
probs = []     # P, т. е. вероятности того, что элементы не выйдут из строя
brokenProbs = [] 
cuts = []
roots = []
for i in range(0, n):
     # Качество обезумевшее, вероятность отказа КРАЙНЕ МАЛА
     #probs.append(1 - random.random() * 0.0001)
     t = 1 - random.random() * 0.0001
     #t = 0.5
     probs.append(t)
     brokenProbs.append(1 - t)
print("Сгенерируем вероятности того, что i-й элемент не выйдет из строя. Получились такие вероятности P_i: {}, ладно\n".format(probs))

elements = [0, 1, 2, 3, 4, 5]      # Элементы системы
brokenElements = []                # Множество неисправных элементов системы
# Получаем все подмножества неисправных элементов
tmp = []
for i in range(0, (len(elements) + 1)):
     tmp.append(list(combinations(elements, i)))
for i in range(0, len(tmp)):
     for j in range(0, len(tmp[i])):
          brokenElements.append(tmp[i][j])

# Гипотезы
hypotheses = []
# Проходим по всем возможным неисправностям
for i in range(0, len(brokenElements)):
     hypotheses.append(1)
     for j in range(0, n):
          # И вычисляем P(H)
          if brokenElements[i].__contains__(j):
               hypotheses[i] *= (1 - probs[j])
          else: 
               hypotheses[i] *= probs[j]

def toStr(brokenElement):
     if brokenElement.__contains__(0):
          result = "q{}".format(0)
     else:
          result = "p{}".format(0)
     for i in range (1, n):
          if brokenElement.__contains__(i):
               result += " * q{}".format(i)
          else:
               result += " * p{}".format(i)
     return result

P_h = 0
Q_h = 0
P_str = ""
H_str = ""

for i in range (0, len(brokenElements)):
     firstLineBroken = False
     secondLineBroken = False
     for j in range (0, len(brokenElements[i])):
          if brokenElements[i][j] == 0 or brokenElements[i][j] == 2 or brokenElements[i][j] == 4:
               firstLineBroken = True
          if brokenElements[i][j] == 1 or brokenElements[i][j] == 3 or brokenElements[i][j] == 5:
               secondLineBroken = True
     if firstLineBroken and secondLineBroken:
          Q_h += hypotheses[i]
          cuts.append(brokenElements[i])
     else:
          #print(brokenElements[i])
          P_h += hypotheses[i]
          P_str += toStr(brokenElements[i])
          P_str += " + "
          H_str += "H{} + ".format(i)
          roots.append(brokenElements[i])
P_str = P_str[:-2]
H_str = H_str[:-3]
print("Проведем анализ системы методом перебора гипотез. Всего имеется {} элементов, следовательно, гипотез будет {}".format(n, 2 ** n))
print("Выберем только те гипотезы, при выполнении которых система будет работать исправно. Получим:")
print("P = {} = {}".format(H_str, P_str))
print("Подставим значения и вычислим")
print("Вероятность безотказной работы P = {}, вероятность отказа системы Q = {}\n".format(P_h, Q_h))

print("На основе теорем ТВ:")
Q = (1 - probs[0] * probs[2] * probs[4]) * (1 - probs[1] * probs[3] * probs[5])
P = 1 - Q
print("Вероятность безотказной работы P = {}, вероятность отказа системы Q = {}\n".format(P, Q))

A = [probs[0] * probs[2] * probs[4], probs[1] * probs[3] * probs[5]]
P_cut = 0
for i in range (0, 2):
     P_cut += A[i]
P_cut -= A[0] * A[1]

B = [1 - probs[0] * probs[1], 1 - probs[0] * probs[3], 1 - probs[0] * probs[5],
     1 - probs[1] * probs[2], 1 - probs[2] * probs[3], 1 - probs[2] * probs[5],
     1 - probs[1] * probs[4], 1 - probs[3] * probs[4], 1 - probs[5] * probs[5]]
Q_cut = 0

prevLen = len(cuts[0])
sign = 1
res = 1
print(cuts)
for i in range (0, len(cuts)):
     if (len(cuts[i]) > prevLen):
          sign *= - 1
          prevLen = len(cuts[i])
     for j in range(0, len(cuts[i])):
          res *= brokenProbs[cuts[i][j]]
     Q_cut += sign * res
     res = 1
higher = 1
lower = 1
for i in range(0, len(A)):
     higher *= 1 - A[i]
higher = 1 - higher
for i in range(0, len(B)):
     lower *= 1 - B[i]

print('''Выполним анализ системы методом минимальных путей и минимальных сечений:
Минимальные пути:    (0): 0 - 2 - 4 
                     (1): 1 - 3 - 5''')

print('''Минимальные сечения: (0): 0 - 1,    (1): 0 - 3,    (2): 0 - 5
                     (4): 1 - 2,    (5): 2 - 3,    (6): 2 - 5
                     (7): 1 - 4,    (8): 3 - 4,    (9): 4 - 5''')
print("Вероятность безотказной работы P = {}, вероятность отказа системы Q = {}".format(P_cut, Q_cut))
print("Оценка вероятности безотказной работы: {} ≤ P ≤ {}\n".format(lower, higher))
table = PrettyTable()
table.field_names = ["x0", "x1", "x2", "x3", "x4", "x5"]
P_logical = 0
for i in range(0, len(roots)):
     line = []
     tmp = 1
     for j in range(0, 6):
          if roots[i].__contains__(j):
               line.append(0)
               tmp *= 1 - probs[j]
          else:
               line.append(1)
               tmp *= probs[j]
     P_logical += tmp
     table.add_row(line)

print('''Составим для системы функции алгебры логики.
ДНФ системы:
{}
Вычислим вероятность безотказной работы логико-вероятностым методом: P = {}, Q = {}
Функция работоспособности будет иметь вид:   (x0 x2 x4) v (x1 x3 x5)
Функция неработоспособности будет иметь вид: (x0 x1) v (x0 x3) v (x0 x5) v (x1 x2) v (x2 x3) v (x2 x5) v (x1 x4) v (x3 x4) v (x4 x5) = x0 (x1 v x3 v x5) v x2 (x1 v x3 v x5) v x4 (x1 v x3 v x5)
'''.format(table, P_logical, 1 - P_logical))