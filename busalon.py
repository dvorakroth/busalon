#!/usr/bin/env python3

# Busalon -- a tiny server for showing bus arrivals on a Waveshare 7.5 inch display

# I, Amit Ron, the original author, hereby release this server's code into the
# public domain; (additionally, you may also use it, modify it, distribute it,
# or even claim it as entirely your own, for all personal, or religious, or 
# commercial, or noncommercial uses, as per the the terms of the WTFPLv2.)

# This code WILL fail. It's not a question of if, but a question of when.
# No warranty, no guarantees, everything provided as-is. If anything happens to
# you because of this code, I won't be held responsible, because now you know lol

# (right?)

from PIL import Image
from PIL import ImageFont
from PIL import ImageDraw

from bidi.algorithm import get_display as bidi_algo
from dateutil.parser import parse
import dateutil
from datetime import datetime
import requests
import time
import json
import shapely.geometry
import cherrypy

HEADERS = {"Accept": "application/json"}

SCREEN_W = 480
SCREEN_H = 800

NEIGHBORHOODS_FILENAME = "Neighbourhoods.json"
NEIGHBORHOOD_NAME_FIELD = "shemshchun"

HOME_CITY_HE = "תל אביב יפו"
TIMEZONE = "Asia/Tel_Aviv"

CURLBUS_URL = 'https://curlbus.app/'

lines_font = None
etas_font = None
dest_font = None

neigh_poly_pairs = None

def load_fonts():
    global lines_font, etas_font, dest_font

    lines_font = ImageFont.load('DIALOGH05_4x.pil')
    etas_font = ImageFont.load('DIALOGH05_3x.pil')
    dest_font = ImageFont.load('DIALOGH05_2x.pil')


def load_neighborhood_polygons():
    global neigh_poly_pairs

    neigh_poly_pairs = []

    o = {}

    with open(NEIGHBORHOODS_FILENAME, 'r', encoding='utf8') as f:
        o = json.load(f)

    all_polys = o['features']

    for poly in all_polys:
        neigh_poly_pairs.append((shapely.geometry.shape(poly["geometry"]), poly["properties"]))
    
    return neigh_poly_pairs

def find_neighborhood_for_coord(x, y, poly_pairs):
    point = shapely.geometry.Point(x, y)

    for shape, props in poly_pairs:
        if shape.contains(point):
            return props[NEIGHBORHOOD_NAME_FIELD]
    
    return None

def render_hebrew_for_pillow(logical_text):
    return bidi_algo(logical_text).encode('windows-1255').decode('latin-1')

def format_eta(eta, NOW):
    diff = (eta - NOW).total_seconds()

    if diff < 0:
        diff = 0

    return (str(int(diff / 60)) + "'")

def render_image(processed_data):
    img = Image.new('1', (SCREEN_W, SCREEN_H), 1)
    draw = ImageDraw.Draw(img)

    x = 0
    y = 0

    
    max_line_width = 0
    max_line_height = 0

    NOW = datetime.now(dateutil.tz.gettz(TIMEZONE))

    for i in range(10):
        w, h = draw.textsize(f"{i}{i}{i}", font=lines_font)
        if w > max_line_width:
            max_line_width = w
        
        if h > max_line_height:
            max_line_height = h


    for line_data in processed_data:
        linenum_w, linenum_h = draw.textsize(line_data["line_name"], font=lines_font)
        draw.text((x + max_line_width - linenum_w, y), line_data["line_name"], font=lines_font, fill=0)

        eta_strings = [
            format_eta(eta, NOW)
            for eta in line_data["etas"]
        ]

        etas = ", ".join(eta_strings)

        etas_w, etas_h = draw.textsize(etas, font=etas_font)
        draw.text((img.width - etas_w - 5, y), etas, font=etas_font, fill=0)

        y += int(etas_h * 1)

        
        dest = render_hebrew_for_pillow(line_data["destination"])
        dest_w, dest_h = draw.textsize(dest, font=dest_font)
        draw.text((img.width - dest_w - 5, y), dest, font=dest_font, fill=0)

        y += int(dest_h * 1)
    
    return img

def get_lang_string(s, language_priorities):
    for l in language_priorities:
        if l in s and s[l] is not None and s[l] != "":
            return s[l]
    
    return ""

def process_data(stop_data, language_priorities):
    result = []
    tmpdict = {}

    next(iter(stop_data["visits"].values())).sort(key=lambda x: parse(x["eta"]))

    for visit in next(iter(stop_data["visits"].values())):
        route_id = visit["route_id"]
        eta_abs = parse(visit["eta"])

        if route_id not in tmpdict:
            tmpdict[route_id] = []

            line_name = visit["line_name"]
            destination = visit["static_info"]["route"]["destination"]["address"]["city_multilingual"]

            if destination['HE'] == HOME_CITY_HE:
                # in the home city, find the neighborhood instead
                destination_coords = visit["static_info"]["route"]["destination"]["location"]
                destination = find_neighborhood_for_coord(destination_coords["lon"], destination_coords["lat"], neigh_poly_pairs)
            else:
                destination =  get_lang_string(destination, language_priorities)

            result.append({
                "route_id": route_id,
                "line_name": line_name,
                "destination": destination,
                "etas": tmpdict[route_id]
            })
        
        tmpdict[route_id].append(eta_abs)
    
    return result

neigh_poly_pairs = load_neighborhood_polygons()

def complete_render_image(stopid):
    stop_data = requests.get(CURLBUS_URL + stopid, headers=HEADERS).json()
    
    processed_data = process_data(stop_data, ['HE'])

    return render_image(processed_data)

def image_to_bytes(img):
    result = b''

    i = 0
    b = 0
    for p in img.getdata():
        i += 1
        b = (b << 1) | int(not not p)

        if i == 8:
            result += bytes([b])
            b = 0
            i = 0
    
    return result

class BusalonServer(object):
    @cherrypy.expose
    def stop(self, stopid, rotate=0):
        cherrypy.response.headers['Content-Type'] = 'application/x-octet-stream'
        img = complete_render_image(str(stopid))
        return image_to_bytes(img.rotate(int(rotate), expand=True))



if __name__ == "__main__":
    load_fonts()
    load_neighborhood_polygons()
    cherrypy.quickstart(BusalonServer())
    # complete_render_image("25823").save('test.png')