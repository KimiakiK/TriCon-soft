import sys
from pathlib import Path
from PIL import Image
import numpy as np

def checkFolder():
  if len(sys.argv) != 6  or  not Path(sys.argv[1]).is_dir():
    print('フォルダを指定してください')
    exit()
  
  file_list = list( Path(sys.argv[1]).glob('*.png') )
  
  if len(file_list) == 0:
    print('pngファイルがありません')
    exit()
  
  return file_list

def convertArray(im):
  imgArray = np.zeros(im.size, dtype=np.uint16)
  imgMaskArray = np.zeros(im.size, dtype=np.uint8)
  useAlpha = False
  
  for x in range(im.width):
    for y in range(im.height):
      r, g, b, a = im.getpixel((x, y))
      
      # 透過チェック
      if a == 0:
        useAlpha = True
        imgArray[y, x] = 0
        imgMaskArray[y, x] = 1
      else:
        r = int(r / 8) & 0x1F # 赤は5bit
        g = int(g / 4) & 0x3F # 緑は6bit
        b = int(b / 8) & 0x1F # 青は5bit
        imgArray[y, x] = r<<3 | (g&0b111)<<13 | g>>3 | b<<8
        imgMaskArray[y, x] = 0
  
  return imgArray, imgMaskArray, useAlpha

def saveArray(array, file):
    np.save(file, array)

if __name__ == '__main__':
  file_list = checkArgs()
  for file in file_list:
    im = Image.open(file)
    imgArray, imgMaskArray, useAlpha = convertArray(im)
    file = str(file).split('.')[0]
    saveArray(imgArray, file)
    if useAlpha:
      saveArray(imgMaskArray, file + '_mask')
