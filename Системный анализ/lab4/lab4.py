import numpy as np
import matplotlib.pyplot as plt
import math
import scipy 
import scipy.optimize as opt
import scipy.integrate as integrate

# вектор измерений
R=np.array([
            [8.0793965679E+01],
            [9.0335359796E+01],
            [1.0101542671E+02],
            [1.1286657686E+02],
            [1.2603977797E+02],
            [1.4071606722E+02],
            [1.5698966423E+02],
            [1.8147505917E+02],
            [2.4167563012E+02],
            [3.2073393977E+02],])
# диагональные элементы корреляционной матрицы
k11 =   2.6124774305E-04
k22 =   3.2666584497E-04
k33 =   4.0818434440E-04
k44 =   5.0961197115E-04
k55 =   6.3562047735E-04
k66 =   7.9194416813E-04
k77 =   9.8562495945E-04
k88 =   1.3170222149E-03
k99 =   2.3362624717E-03
k1010 = 4.1080950883E-03
# корреляционная матрица
Kv = np.zeros((10, 10))
di = np.diag_indices_from(Kv)
Kv[di] = [k11, k22, k33, k44, k55, k66, k77, k88, k99, k1010]
# дельта
delta = 0.1
# оцениваемые параметры
x1 = 6.15
x2 = 6.15

x1add = x1+delta
x2add = x2+delta

x1sub=x1-delta
x2sub=x2-delta

## система ДУ
def dx1_dt(t,x1,x2):
  return np.sqrt(2*x1*x1+3)

def dx2_dt(t,x1,x2):
  return np.cos(x2)+x1+t
###

##измеряемая ф
def s(x1,x2):
  return x1+x2/3

##шаг интегрирования
step=1.0E-3
###количество шагов измерений
arr_N=[10,25,40,55,70,85,100,120,160,200]

def RK45(f1, f2, x10, x20, step, N):#N должно быть на 1 больше чем max элемент  в массиве arr_N
  h=step 
  H=h/2
  X1=[]
  X2=[]
  x1 = [x10] 
  x2 = [x20] 
  for i in range(N): 
    k11 = f1((i+1)*h, x1[-1], x2[-1]) 
    k12 = f2((i+1)*h, x1[-1], x2[-1]) 
    k21 = f1((i+1)*h + H, x1[-1] + H*k11, x2[-1] + H*k12) 
    k22 = f2((i+1)*h + H, x1[-1] + H*k11, x2[-1] + H*k12) 
    k31 = f1((i+1)*h + H, x1[-1] + H*k21, x2[-1] + H*k22) 
    k32 = f2((i+1)*h + H, x1[-1] + H*k21, x2[-1] + H*k22) 
    k41 = f1((i+1)*h + h, x1[-1] + h*k31, x2[-1] + h*k32) 
    k42 = f2((i+1)*h + h, x1[-1] + h*k31, x2[-1] + h*k32) 
    x1.append(x1[-1] + (h/6)*(k11 + 2*k21 + 2*k31 + k41)) 
    x2.append(x2[-1] + (h/6)*(k12 + 2*k22 + 2*k32 + k42)) 
    for j in range(len(arr_N)):
      if( arr_N[j]==(i+1) ):
        X1.append(x1[-1] + (h/6)*(k11 + 2*k21 + 2*k31 + k41)) 
        X2.append(x2[-1] + (h/6)*(k12 + 2*k22 + 2*k32 + k42)) 
  return X1, X2 


def Get_vectors2():
  NN=max(arr_N)+1
  s1_add= []
  X11,X22=RK45(dx1_dt, dx2_dt, x1add,x2, step, NN) 
  for i in range(len(X11)):
    s1_add.append(s(X11[i],X22[i]) )

  s1_sub= []
  X11,X22=RK45(dx1_dt, dx2_dt, x1sub,x2, step, NN) 
  for i in range(len(X11)):
    s1_sub.append(s(X11[i],X22[i]) )

  s2_add= []
  X11,X22=RK45(dx1_dt, dx2_dt, x1,x2add, step, NN) 
  for i in range(len(X11)):
    s2_add.append(s(X11[i],X22[i]) )

  s2_sub= []
  X11,X22=RK45(dx1_dt, dx2_dt, x1,x2sub, step, NN) 
  for i in range(len(X11)):
    s2_sub.append(s(X11[i],X22[i]) )

  ss= []
  X11,X22=RK45(dx1_dt, dx2_dt, x1,x2, step, NN) 
  for i in range(len(X11)):
    ss.append(s(X11[i],X22[i]) )
  return s1_add,s1_sub,s2_add,s2_sub,ss


def Get_L(s1_add,s1_sub,s2_add,s2_sub):
  L=[]  
  ddq=1/(2*delta)
  L=np.zeros((2, len(s1_add) ))
  for i in range( len(s1_add) ):
    tx1=s1_add[i]-s1_sub[i]
    tx2=s2_add[i]-s2_sub[i]
    tx1=tx1*ddq
    tx2=tx2*ddq
    L[0][i]= tx1
    L[1][i]= tx2
    #print( tx1,tx2)
  return L

def Get_a(Kv,L,dR):
  a1=np.dot(L,Kv)
  #print(np.linalg.det(Kv))
  a2=np.dot(a1,L.transpose())
  a3=np.linalg.inv(a2)#K0 -кор матр
  #a3=np.linalg.pinv(a2)#K0 -кор матр
  a4=np.dot(a3,L)
  a5=np.dot(a4,Kv)
  dq=a5.dot(dR)
  #dq=((np.linalg.inv((L.dot(Kv)).dot(L.transpose())).dot(L)).dot(Kv)).dot(dR) 
  return  dq
















###
### Вперед решать 
###


Kv=np.linalg.inv(Kv) # Kv в -1

k=0
coun=50
while(k<coun):
  k=k+1
  s1_add,s1_sub,s2_add,s2_sub,ss=Get_vectors2()
  L=Get_L(s1_add,s1_sub,s2_add,s2_sub)#L матрица производных
  #print(L)
  
  dR=np.zeros((10,1))
  for i in (range(len(ss))):
    dR[i][0]=R[i][0]-ss[i]
####   Находим вектор коэфициентов - a 
  a=Get_a(Kv,L,dR)
  #print('a=',a)
  #print(a[0][0],a[1][0])
###   a найден
  #print('---------------------------------------------------------------------')
#  print('x1=',x1,'x2=',x2)
#  print( s1_add,s1_sub,s2_add,s2_sub,ss)
#  print(L)
#  print(dR)
#  print(a)
#  print('dx1=',a[0][0],'dx2=',a[1][0])
  #print('---------------------------------------------------------------------')


  md=np.sqrt(a[0][0]*a[0][0]+a[1][0]*a[1][0]) 
  print('md===',md)
  print('x1=',x1,'x2=',x2)
  x1=x1+a[0][0]#x1+delta a
  x2=x2+a[1][0]#x2+delta a
  x1add=x1+delta
  x2add=x2+delta
  x1sub=x1-delta
  x2sub=x2-delta
  if(md<10E-6):#если модуль вектора меньше погрешности
    break

#print(Kv)
#print(R)
print("all itterate = {0}    real itterate={1}".format(coun,k))
print('x1=',x1,'x2=',x2)
print('md===',md)