from hashlib import sha1
import hmac
from wsgiref.handlers import format_date_time
from datetime import datetime
from time import mktime
import base64
from requests import request
import numpy as np
from PIL import ImageFont, ImageDraw, Image
import cv2
import time

app_id = '6c26c2d4d7bd40c681886a1560120f24'
app_key = 'YUb0RSlnseYtapl10EJPcbujfR4'

class Auth():

    def __init__(self, app_id, app_key):
        self.app_id = app_id
        self.app_key = app_key

    def get_auth_header(self):
        xdate = format_date_time(mktime(datetime.now().timetuple()))
        hashed = hmac.new(self.app_key.encode('utf8'), ('x-date: ' + xdate).encode('utf8'), sha1)
        signature = base64.b64encode(hashed.digest()).decode()

        authorization = 'hmac username="' + self.app_id + '", ' + \
                        'algorithm="hmac-sha1", ' + \
                        'headers="x-date", ' + \
                        'signature="' + signature + '"'
        return {
            'Authorization': authorization,
            'x-date': format_date_time(mktime(datetime.now().timetuple())),
            'Accept - Encoding': 'gzip'
        }

def draw_border(img, pt1, pt2, color, thickness, r, d):
    x1,y1 = pt1
    x2,y2 = pt2
    # Top left
    cv2.line(img, (x1 + r, y1), (x1 + r + d, y1), color, thickness)
    cv2.line(img, (x1, y1 + r), (x1, y1 + r + d), color, thickness)
    cv2.ellipse(img, (x1 + r, y1 + r), (r, r), 180, 0, 90, color, thickness)
    # Top right
    cv2.line(img, (x2 - r, y1), (x2 - r - d, y1), color, thickness)
    cv2.line(img, (x2, y1 + r), (x2, y1 + r + d), color, thickness)
    cv2.ellipse(img, (x2 - r, y1 + r), (r, r), 270, 0, 90, color, thickness)
    # Bottom left
    cv2.line(img, (x1 + r, y2), (x1 + r + d, y2), color, thickness)
    cv2.line(img, (x1, y2 - r), (x1, y2 - r - d), color, thickness)
    cv2.ellipse(img, (x1 + r, y2 - r), (r, r), 90, 0, 90, color, thickness)
    # Bottom right
    cv2.line(img, (x2 - r, y2), (x2 - r - d, y2), color, thickness)
    cv2.line(img, (x2, y2 - r), (x2, y2 - r - d), color, thickness)
    cv2.ellipse(img, (x2 - r, y2 - r), (r, r), 0, 0, 90, color, thickness)

if __name__ == '__main__':

    SCREEN_WEIDTH = 1600
    SCREEN_HEIGHT = 1200
    BLOCK_SIZE_WEIDTH = 370
    BLOCK_SIZE_HEIGHT = 150
    BUS_INFO_LINE_HEIGHT = 420
    BLOCK_AMOUNT = 4
    TOP_PIXEL = 20
    BUS_TITLE_LEFT_MARGIN = BLOCK_SIZE_WEIDTH + 100
    BUS_TIME_LEFT_MARGIN = 30

    a = Auth(app_id, app_key)
    bustime = [None] * BLOCK_AMOUNT
    bustitle = [None] * BLOCK_AMOUNT
    r = request('get', 'https://ptx.transportdata.tw/MOTC/v2/Bus/EstimatedTimeOfArrival/City/Taipei?$filter=StopName%2FZh_tw%20eq%20%27%E8%8F%AF%E5%B1%B1%E6%96%87%E5%89%B5%E5%9C%92%E5%8D%80%27%20and%20EstimateTime%20gt%201&$orderby=EstimateTime%20asc&$top=4&$format=JSON', headers= a.get_auth_header())
    list_of_dicts = r.json()
    print(type(r))
    print(type(list_of_dicts))
    for i in list_of_dicts:
        print(i["RouteName"].get("Zh_tw"),i["EstimateTime"]//60,"分鐘")
    print(type(list_of_dicts[0].get("RouteName").get("Zh_tw")))

    for i in range(BLOCK_AMOUNT):
        if list_of_dicts[i].get("EstimateTime") < 120:
            bustime[i] = "進站中"
        else:
            bustime[i] = str(list_of_dicts[i].get("EstimateTime")//60)+"分"
        bustitle[i] = list_of_dicts[i].get("RouteName").get("Zh_tw")

    

    ## Make canvas and set the color
    img = np.zeros((SCREEN_HEIGHT,SCREEN_WEIDTH,3),np.uint8)
    img[:] = (255, 255, 255)

    for i in range(BLOCK_AMOUNT):
        height = int(((SCREEN_HEIGHT - BUS_INFO_LINE_HEIGHT)/BLOCK_AMOUNT*i)+BUS_INFO_LINE_HEIGHT)+TOP_PIXEL
        print(height)
        if bustime[i] == "進站中":
            cv2.rectangle(img, (BUS_TIME_LEFT_MARGIN,height), (BUS_TIME_LEFT_MARGIN + BLOCK_SIZE_WEIDTH, height + BLOCK_SIZE_HEIGHT), (0, 0, 255), -1)
        else:
            cv2.rectangle(img, (BUS_TIME_LEFT_MARGIN,height), (BUS_TIME_LEFT_MARGIN + BLOCK_SIZE_WEIDTH, height + BLOCK_SIZE_HEIGHT), (0, 0, 0), -1)



    cv2.rectangle(img, (0, 0), (1600, 250), (0, 0, 0), -1)
    cv2.rectangle(img, (1075, 0), (1600, 250), (0, 0, 255), -1)
    cv2.rectangle(img, (0, 250), (1600, 420), (255, 0, 0), -1)
    cv2.line(img, (0, BUS_INFO_LINE_HEIGHT), (SCREEN_WEIDTH, BUS_INFO_LINE_HEIGHT), (0, 0, 0), 5)

    b,g,r,a = 0,0,0,0

    ## Use simsum.ttc to write Chinese.
    fontpath = "./NotoSansCJKtc-Light.otf"     
    font = ImageFont.truetype(fontpath, 200)
    fontbus = ImageFont.truetype(fontpath, 150)
    fonttitle = ImageFont.truetype(fontpath, 150)
    fontgetin = ImageFont.truetype(fontpath, 100)
    img_pil = Image.fromarray(img)
    draw = ImageDraw.Draw(img_pil)
    text = time.strftime("%Y/%m/%d %H:%M", time.localtime())
    
    draw.text((30, -50),  text, font = font, fill = (255, 255, 255, a))
    draw.text((0, 220),  list_of_dicts[0].get("StopName").get("Zh_tw"), font = fonttitle, fill = (b, g, r, a))

    for i in range(BLOCK_AMOUNT):
        height = int(((SCREEN_HEIGHT - BUS_INFO_LINE_HEIGHT)/BLOCK_AMOUNT*i)+BUS_INFO_LINE_HEIGHT)+TOP_PIXEL
        if bustime[i] == "進站中":
            draw.text((75, height),  bustime[i], font = fontgetin, fill = (255, 255, 255, a))
        else:
            draw.text((90, height-40),  bustime[i], font = fontbus, fill = (255, 255, 255, a))
        draw.text((BUS_TITLE_LEFT_MARGIN, height-40),  bustitle[i], font = fontbus, fill = (b, g, r, a))

    img = np.array(img_pil)

    cv2.imwrite("time.bmp", img)

    
