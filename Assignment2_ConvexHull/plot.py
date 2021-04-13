from sys import argv
import re
import matplotlib.pyplot as plt
import matplotlib.patches as ptc
import math
from tqdm import tqdm

lines = open("input.txt").readlines()

n = int(lines[0].strip())
r = float(lines[1].strip())

points = []
for a in tqdm([l.split(" ") for l in lines[2:]]) :
    points.append([float(a[0]), float(a[1])])
p_x, p_y = zip(*points)

print(n, r)

fig, ax = plt.subplots()
ax.set_xlim(0,2)
ax.set_ylim(0,2)
ax.scatter(p_x, p_y)

for p in tqdm(points):
    ax.add_artist(plt.Circle(p, r, color='r', fill=False))

uh = []
lh = []
with open("output.txt" ) as f:
    state = 0
    for l in tqdm(f.readlines()):
        if l.strip() == "" :
            continue
        if "Upper hull" in l:
            state = 1
        elif "Lower hull" in l:
            state = 2
        elif "The containment zone" in l:
            state = 3
        elif "Upper section" in l:
            state = 4
        elif "Lower section" in l:
            state = 5
        else:
            if state == 1:
                a = l.strip().split(" ")
                # print(a)
                uh.append([float(a[0]), float(a[1])])
            elif state == 2:
                a = l.strip().split(" ")
                lh.append([float(a[0]), float(a[1])])
            elif state == 4 or state == 5:
                if "Tangent" in l:
                    _, points_str = l.strip().split(":")
                    points = map(eval, re.findall("From (.*) to (.*)", points_str.strip())[0])
                    p_x, p_y = zip(*points)
                    print(p_x, p_y)
                    ax.plot(p_x, p_y, color='y')
                elif "Arc" in l:
                    _, arc_str = l.strip().split(":")
                    # print(arc_str)
                    arc = list(map(eval, re.findall("(.*) From (.*) to (.*)", arc_str.strip())[0]))
                    ax.add_artist(ptc.Arc(arc[0], 2 * r, 2 * r, theta2=arc[1] * 180 / math.pi, theta1=arc[2] * 180 / math.pi, color='y'))

p_x, p_y = zip(*uh)
ax.plot(p_x, p_y, color='g')

p_x, p_y = zip(*lh)
ax.plot(p_x, p_y, color='g')

plt.savefig("img.png")
