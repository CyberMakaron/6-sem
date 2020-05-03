import numpy as np
import math 

table_str = "db "
for y in np.sin(np.linspace(-math.pi, math.pi,32*2)):   #8х32 (256)
    hex_num = "0"+hex(int(round(y*127))+127)[2:]+"h, "
    table_str = table_str+hex_num
print("Синусоида: ")
print(table_str+table_str[3:]*3)

table_str = "db "
for y in np.sin(np.linspace(-math.pi, math.pi,32*2)):   #8х32 (256)
    hex_num = "0"+hex(int(round(y*64))+127)[2:]+"h, "
    table_str = table_str+hex_num
print("\nУзкая синусоида: ")
print(table_str+table_str[3:]*3)

table_str = "db "
for x in range(32):
    y = x/(32)*200
    hex_num = "0"+hex(int(y))[2:]+"h, "
    table_str = table_str+hex_num
print("\nПила: ")
print(table_str+table_str[3:]*7)