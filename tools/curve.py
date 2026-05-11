import matplotlib.pyplot as plt
import random
import math

FIXED_POINT_BITS = 6
SPEED_FACTOR = 1
MAX_VX = 3
MAX_AX = 1

MAX_X=128
MAX_Y=159
INITIAL_X=62
INITIAL_Y=25
TARGET_X=55+(13>>1)
TARGET_Y=(159+146)>>1

speed = 5<<FIXED_POINT_BITS

dist_y = TARGET_Y - INITIAL_Y
t = dist_y // ((speed>>FIXED_POINT_BITS) * SPEED_FACTOR)

vy = (speed) * SPEED_FACTOR
vx = (MAX_VX << FIXED_POINT_BITS) * SPEED_FACTOR

targetFx = TARGET_X << FIXED_POINT_BITS
deltaX = targetFx - (INITIAL_X << FIXED_POINT_BITS)
numerator = 2 * (deltaX - (vx*t))
ax = numerator // (t*t)
print(f"t={t}")
print(f"target: ({targetFx}, {TARGET_Y})")
print(f"vx={vx}, ax={ax}")

x = INITIAL_X << FIXED_POINT_BITS # Fixed point arithmetic for x
y = INITIAL_Y << FIXED_POINT_BITS
x_pts = [INITIAL_X]
y_pts = [INITIAL_Y]
while y < (MAX_Y << FIXED_POINT_BITS):
    x = x + vx
    y = y + vy
    vx = vx + ax
    x_pts.append(x >> FIXED_POINT_BITS)
    y_pts.append(y >> FIXED_POINT_BITS)

for x,y in zip(x_pts, y_pts):
    print(f"({x}, {y})")

plt.xlim(0, MAX_X)
plt.ylim(0, MAX_Y)
plt.plot(x_pts, y_pts)
plt.gca().invert_yaxis()
plt.show()