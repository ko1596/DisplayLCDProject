import numpy as np
from PIL import ImageFont, ImageDraw, Image
import cv2
import time


## Make canvas and set the color
img = np.zeros((1200,1600,3),np.uint8)
b,g,r,a = 255,255,255,0

## Use cv2.FONT_HERSHEY_XXX to write English.
text = time.strftime("%Y/%m/%d %H:%M:%S %Z", time.localtime()) 
cv2.putText(img,  text, (100,600), cv2.FONT_HERSHEY_SIMPLEX, 3, (b,g,r), 2, cv2.LINE_AA)

## Use simsum.ttc to write Chinese.
fontpath = "./NotoSansCJKtc-Light.otf"     
font = ImageFont.truetype(fontpath, 160)
img_pil = Image.fromarray(img)
draw = ImageDraw.Draw(img_pil)
draw.text((0, 0),  "一二三四五六七八九十", font = font, fill = (b, g, r, a))
img = np.array(img_pil)

cv2.imwrite("time.bmp", img)
