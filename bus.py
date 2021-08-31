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


if __name__ == '__main__':
    a = Auth(app_id, app_key)
    r = request('get', 'https://ptx.transportdata.tw/MOTC/v2/Bus/EstimatedTimeOfArrival/City/Taipei?$filter=StopName%2FZh_tw%20eq%20%27%E8%8F%AF%E5%B1%B1%E6%96%87%E5%89%B5%E5%9C%92%E5%8D%80%27%20and%20EstimateTime%20gt%201&$orderby=EstimateTime%20asc&$top=3&$format=JSON', headers= a.get_auth_header())
    list_of_dicts = r.json()
    print(type(r))
    print(type(list_of_dicts))
    for i in list_of_dicts:
        print(i["RouteName"].get("Zh_tw"),i["EstimateTime"]//60,"分鐘")
    print(type(list_of_dicts[0].get("RouteName").get("Zh_tw")))
    bus1 = list_of_dicts[0].get("RouteName").get("Zh_tw")+"\t\t\t\t\t\t"+str(list_of_dicts[0].get("EstimateTime")//60)+"分"
    bus2 = list_of_dicts[1].get("RouteName").get("Zh_tw")+"\t\t\t\t\t\t"+str(list_of_dicts[1].get("EstimateTime")//60)+"分"
    bus3 = list_of_dicts[2].get("RouteName").get("Zh_tw")+"\t\t\t\t\t\t"+str(list_of_dicts[2].get("EstimateTime")//60)+"分"
    ## Make canvas and set the color
    img = np.zeros((1200,1600,3),np.uint8)
    b,g,r,a = 255,255,255,0

    ## Use simsum.ttc to write Chinese.
    fontpath = "./NotoSansCJKtc-Light.otf"     
    font = ImageFont.truetype(fontpath, 200)
    fontbus = ImageFont.truetype(fontpath, 250)
    img_pil = Image.fromarray(img)
    draw = ImageDraw.Draw(img_pil)
    text = time.strftime("%Y/%m/%d %H:%M", time.localtime())
     
    draw.text((30, 0),  text, font = font, fill = (b, g, r, a))
    draw.text((30, 200),  list_of_dicts[0].get("StopName").get("Zh_tw"), font = font, fill = (b, g, r, a))
    draw.text((30, 400),  bus1, font = fontbus, fill = (b, g, r, a))
    draw.text((30, 650),  bus2, font = fontbus, fill = (b, g, r, a))
    draw.text((30, 900),  bus3, font = fontbus, fill = (b, g, r, a))
    img = np.array(img_pil)

    cv2.imwrite("time.bmp", img)

    
