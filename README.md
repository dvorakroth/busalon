# Busalon

busalon is a combination of a small server, and a tiny embedded chip, used to show bus schedules on an 7.5" e-ink display in my living room.

you can set up your own too, but it might be a bit difficult and/or a fool's errand

![my living room wall. an e-paper display, framed in a white plastic and stuck to the wall, shows some bus departures in hebrew. it's connected by a ribbon cable to a bare circuitboard, also stuck to the wall. this setup is hung next to an analog clock and a shelf](example-wallshot.JPEG)

## the data i used

i used [curlbus](https://curlbus.app/)'s api; thanks, elad!

## the hardware i used

* https://www.waveshare.com/product/7.5inch-e-paper-hat.htm
* https://www.waveshare.com/product/iot-communication/short-range-wireless/wifi/e-paper-esp8266-driver-board.htm
* https://www.waveshare.com/catalog/product/view/id/3672/s/7.5inch-e-paper-case/category/37/

(hopefully you're reading this in the not too distant future, and all these links still work!)

waveshare themselves do claim that the plastic frame isn't "compatible" with this chip but they don't mean that this doesn't WORK. they mean that this LOOKS bad (see picture above). which i didn't mind

## waveshare's code

i can't quite find the original source code online anymore, but the C++ code in the esp8266 directory was mostly written by an employee at waveshare; so thanks to them for their work!

i just tweaked the main function, and added the option to stream data directly from the server to the e-ink display

## the fonts i used

i really like the default dialog font from the hebrew-enabled and hebrew-localized versions of windows 98 second edition; so i naturally whipped out a perfectly legally owned copy for personal use only, and extracted the DIALOGH.FON file from C:\Windows\Fonts

the problem is that [Pillow](https://python-pillow.org/), unsurprisingly enough, doesn't actually support loading microsoft's proprietary bitmap font file format from literally last century! in fact, the only bitmap font format that Pillow **does** support is some weird combo of an image and some random ass binary format that they made up and **didn't actually document**.

the only tools Pillow offers to convert fonts into that format support two ancient X Window System font formats, of course(??), so i had no choice but to trudge through their old C code from the 90s to make my own [mkpilfont](https://github.com/dvorakroth/mkpilfont) utility, to be used in conjuctions with [mkwinfont](https://github.com/juanitogan/mkwinfont), for converting old windows 98 bitmap fonts into an even more obscure and even less documented bitmap font format.

so uh,,,,,,,

don't say i didn't warn you that getting your own busalon up and running might be a fool's errand

## setting up and testing the server

ostensibly, you should be able to just run `busalon.py`, and you're set

to verify that the server is indeed giving you the results you want, you can download its response to a file, and then feed that into `tools/imgtest.html`

good luck lol

## legal stuff

I, Amit Ron, the original author, hereby release everything in this repository **EXCEPT FOR THE FILES UNDER THE DIRECTORY "esp8266"** into the public domain; (additionally, everything here except the contents of the "esp8266" directory, may be used, modified, distributed, or even claimed as entirely your own, for all personal, or religious, or commercial, or noncommercial uses, as per the the terms of the WTFPLv2)

This code, eventually, WILL fail. It's not a question of if, but a question of when. No warranty, no guarantees, everything provided as-is. If anything bad happens to you because of this code, I won't be held responsible, because now you know lol

(right?)