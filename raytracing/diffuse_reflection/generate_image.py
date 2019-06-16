from PIL import Image
f = open('op', 'r')
lines = f.readlines()

data = []
for line in lines:
    r, g, b = [int(x) for x in line.split(' ')]
    data.append((r, g, b))

params_h = open('Parameters.h', 'r')
lines = params_h.readlines()
for line in lines:
    if line.startswith('#define C_W'):
        C_W = int(line.split(' ')[2])
    elif line.startswith('#define C_H'):
        C_H = int(line.split(' ')[2])

im = Image.new("RGB", (C_W, C_H))
im.putdata(data)
im.save('op.png')
