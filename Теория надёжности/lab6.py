import numpy
import random
from itertools import combinations

print('''Система варианта №4:
  ┌──[  0  ]──┐  ┌──[  2  ]──┐  ┌──[  4  ]──┐  
──┤           ├──┤           ├──┤           ├──                 
  └──[  1  ]──┘  └──[  3  ]──┘  └──[  5  ]──┘  
''')

n = 6   # количество элементов
P = []
Q = []
for i in range(0, int(n/2)):
    tmp = random.random() * 0.1
    P.append(1 - tmp)
    Q.append(tmp)
    P.append(1 - tmp)
    Q.append(tmp)

min_ways = [{0, 2, 4}, {0, 2, 5}, {0, 3, 4}, {0, 3, 5}, {1, 2, 4}, {1, 2, 5}, {1, 3, 4}, {1, 3, 5}]
min_cuts = [{0, 1}, {2, 3}, {4, 5}]

P_tv = (1 - Q[0]**2)*(1 - Q[2]**2)*(1-Q[4]**2)
Q_tv = 1 - P_tv
print("Анализ системы на основе теорем теории вероятности: P = (1-q0q1)(1-q2q3)(1-q4q5) = {}; Q = 1 - P = {}\n".format(P_tv, Q_tv))

print("Анализ системы методом минимальных путей и сечений:")
print("Минимальные пути: {}\nМинимальные сечения: {}\n".format(min_ways, min_cuts))

def getStrExpression(combinations, ch, sign):
    res_str = ""
    for comb in combinations:
        res_str += " " + sign + " "
        for j in range(n): 
            if j in comb:
                res_str += "{}{}".format(ch, j)

    return res_str

def uniCompinations(combinations, k, set_):
    res = []
    for comb in combinations:
        t = set()
        for j in range(len(set_)): 
            if j in comb:
                t = t | set_[j]
        res.append(t)
    return res

def computeCombinations(combinations, p_q):
    res = 0
    for comb in combinations:
        t = 1
        for j in range(n): 
            if j in comb:
                t *= p_q[j]
        res += t
    return res

ways_num = [i for i in range(0, len(min_ways))]
exp_str = ""
exp_res = 0
for k in range(1, len(min_ways) + 1):
    comb = list(combinations(ways_num, k))
    comb = uniCompinations(comb, k, min_ways)
    if (k % 2 != 0):
        exp_res += computeCombinations(comb, P)
    else:
        exp_res -= computeCombinations(comb, P)
    
    str_ = getStrExpression(comb, "p", "+" if (k % 2 != 0) else "-")
    exp_str += str_
exp_str = "P =" + exp_str[2:]
print("Вероятность безотказной работы методом минимальных путей и сечений: {}".format(exp_str)) 
print("P = {}\n".format(exp_res))

cuts_num = [i for i in range(0, len(min_cuts))]
exp_str = ""
exp_res = 0
for k in range(1, len(min_cuts) + 1):
    comb = list(combinations(cuts_num, k))
    comb = uniCompinations(comb, k, min_cuts)
    if (k % 2 != 0):
        exp_res += computeCombinations(comb, Q)
    else:
        exp_res -= computeCombinations(comb, Q)
    
    str_ = getStrExpression(comb, "q", "+" if (k % 2 != 0) else "-")
    exp_str += str_
exp_str = "Q =" + exp_str[2:]
print("Вероятность отказа методом минимальных путей и сечений: {}".format(exp_str)) 
print("Q = {}\n".format(exp_res))

print("Функция работоспособности: y = x1x3x5 ∨ x1x3x6 ∨ x1x4x5 ∨ x1x4x6 ∨ x2x3x5 ∨ x2x3x6 ∨ x2x4x5 ∨ x2x4x6")
print("Функция неработоспособности y = !x1!x2 ∨ !x3!x4 ∨!x5!x6")
print("Разрезание: y = x1x3x5 ∨ x1x3x6 ∨ x1x4x5 ∨ x1x4x6 ∨ x2x3x5 ∨ x2x3x6 ∨ x2x4x5 ∨ x2x4x6 = x1(x3x5 ∨ x3x6 ∨ x4x5 ∨ x2x3x5 ∨ x2x3x6 ∨ x2x4x5 ∨ x2x4x6) ∨ !x1(x2x3x5 ∨ x2x3x6 ∨ x2x4x5 ∨ x2x4x6) = x1(x3(x5 ∨ x6) ∨ x4x5 ∨ x2(x3 ∨ x4)(x5 ∨ x6)) ∨ !x1x2(x3 ∨ x4)(x5 ∨ x6)\n")

#########################################

def schemFunc(X):
    res = X[0] and X[2] and X[4] or X[0] and X[2] and X[5] or X[0] and X[3] and X[4] or X[0] and X[3] and X[5]
    res = res or X[1] and X[2] and X[4] or X[1] and X[2] and X[5] or X[1] and X[3] and X[4] or X[1] and X[3] and X[5]
    return res

def step():
    res = []
    for i in range (n):
        res.append(False if random.random() > P[i] else True)
    return res


for k in range(3):
    if (i != 0):
        P = []
        Q = []
        for i in range(0, int(n/2)):
            tmp = random.random() * 0.1
            P.append(1 - tmp)
            Q.append(tmp)
            P.append(1 - tmp)
            Q.append(tmp)
        P_tv = (1 - Q[0]**2)*(1 - Q[2]**2)*(1-Q[4]**2)
        Q_tv = 1 - P_tv
    print("Вероятности безотказной работы элементов P = {}\nВероятности отказов Q = {}".format(P, Q))
    print("Теоретически: P = {}; Q = {}".format(P_tv, Q_tv))

    N = 1000000
    ok = 0
    for i in range(N):
        X = step()
        if (schemFunc(X)):
            ok += 1
    print("Вероятность безотказной работы в результате эксперимента: ", ok/N)
    print("Вероятность отказа в результате эксперимента: ", (N-ok)/N, "\n")
