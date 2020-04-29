import random
import math
import numpy
from scipy.special import factorial as fact

i = 4
j = 1

n = 3 + ((i + j) // 8)          # Количество каналов передачи сообщений
m = n + 1                       # Размер очереди
lambd = 1 + i / 4               # Интенсивность потока заявок
tau = 5 / (5 + j)               # Среднее время обработки сообщений

print("Количество каналов передачи n = {}\nРазмер очереди m = {}\nСообщений в минуту λ = {}\nСреднее время обработки сообщений τ = {}".format(n, m, lambd, tau))

mu = 1 / tau                    # Интенсивность потока обслуживания
ro = lambd / mu                 # Приведенная интенсивность потока заявок (интенсивность нагрузки)
print("Интенсивность потока обслуживания μ = {}\nПриведенная интенсивность потока ρ = {}\n".format(mu, ro))
P = [0]                         # Предельные вероятности (среднее относительное время, которое канал занят (p0 - все свободны))
for i in range (0, n + 1):
     P[0] += (ro ** i) / fact(i)
P[0] += (ro ** (n + 1)) * (1 - (ro / n) ** m) / (n * fact(n) * (1 - ro / n))
P[0] = P[0] ** -1
for i in range (1, n + 1):
     P.append((ro ** i) / fact(i) * P[0])
for r in range(1, m + 1):
     p.append((ro ** (n + r)) / (n ** r * fact(n)) * P[0])
print("Предельные вероятности P = {}\n".format(P))

P_o = (ro ** n) / fact(n) * P[0]                # Вероятность отказа (все каналы заняты)
Q = 1 - P_o                                     # Относительная пропускная способность
A = lambd * Q                                   # Абсолютная пропускная способность  
k = 0                                           # Среднее число занятых каналов 
for i in range(0, n + 1):
     k += i * P[i]

print("Теоретические значения:")
print("Относительная пропускная способность Q = ", Q)
print("Абсолютная пропускная способность A = ", A)
print("Вероятность отказа в обработке P_отк = ", P_o)
print("Среднее число занятых каналов ~k = {}\n".format(k))

# Возвращает номер свободного канала, иначе - -1
def freeChannel(channels):
     for i in range(0, n):
          if channels[i] == 0:
               return i
     return -1
# Обработка уже имеющихся сообщений
def messagesProcessing(channels, dt):
     for i in range (0, len(channels)):
          if (channels[i] > dt):
               channels[i] -= dt
          else:
               channels[i] = 0
# Получение нового сообщения 
def newMessage(message, channels):
     pos = freeChannel(channels)
     if (pos != -1):
          channels[pos] = message
          return True
     return False 

channels = [0 for i in range (0, n)] # Каналы связи (0, если не обрабатывается, иначе - оставшееся время)
maxTime = 100000         # Время работы
busyChannels = 0         # Занятые каналы
unProcessedMessages = 0  # Необработанные сообщения
totalMessages = 0        # Всего сообщений
dt = 0.01                # Δt    
averageTime = 0          # Среднее время обработки сообщения
for currentTime in range (0, int(maxTime / dt)):
     if (random.random() < 1 - math.exp(-1 * lambd * dt)):       # Если сообщение пришло
          message = tau - 0.05 + random.random() / 10            # Назначаем ему время обработки
          averageTime += message
          totalMessages += 1                         
          if(newMessage(message, channels) != True):             # Отправляем сообщение в свободный канал
               unProcessedMessages += 1                          # Если все каналы заняты, сообщение не обработано           
     busyChannels += n - channels.count(0) 
     messagesProcessing(channels, dt)                            # Обрабатываем сообщение
busyChannels = busyChannels / (maxTime / dt)
averageTime = averageTime / totalMessages

print("Результаты эксперимента ({} минут, Δt = {}):".format(maxTime, dt))
print("Всего сообщений: {}, отказов: {}".format(totalMessages, unProcessedMessages))
print("Среднее время обработки сообщения: ", averageTime)
print("\nОтносительная пропускная способность Q = ", (totalMessages - unProcessedMessages) / totalMessages)
print("Абсолютная пропускная способность А = ", (totalMessages - unProcessedMessages) / maxTime)
print("Вероятность отказа при обработке P_отк = ", unProcessedMessages / totalMessages)
print("Среднее число занятых каналов при этом составило ~k = ", busyChannels)