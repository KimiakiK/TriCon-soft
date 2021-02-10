import sys
from pathlib import Path
from PIL import Image

def checkArgs():
  if len(sys.argv) != 7  or  not Path(sys.argv[1]).is_file():
    print('usage: png2array.py "*.png" "name" "position x" "position y" "size w" "size h"')
    exit()
  
  png = sys.argv[1]
  name = sys.argv[2]
  x = int(sys.argv[3])
  y = int(sys.argv[4])
  w = int(sys.argv[5])
  h = int(sys.argv[6])
  
  return (png, name, x, y, w, h)


if __name__ == '__main__':
  png, name, x, y, w, h = checkArgs()
  im = Image.open(png)
  
  output = ""
  
  output += "static const uint8_t img_" + name + " [] = {\n"
  for row in range(y, y+h):
    for col in range(x, x+w):
      color = im.getpixel((col, row))
      if color == 0:
        output += " 0,"
      else:
        output += " 1,"
    output += "\n"
  
  output += "};\n"
  
  output += "static const sprite_t sprite_" + name + " = {img_" + name + ", " + str(x) +", " + str(y) +", " + str(w) +", " + str(h) +"};\n"
  
  print(output)
