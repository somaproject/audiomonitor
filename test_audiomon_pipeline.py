
import sys
from somapynet.event import Event
from somapynet import eaddr
from somapynet.neteventio import NetEventIO

import struct
import time

somaIP = "127.0.0.1"

eio = NetEventIO(somaIP)
dev = 40

eio.addRXMask(xrange(256), dev)

eio.start()
        
e = Event()
e.src = eaddr.NETWORK
e.cmd =  0x30
e.data[0] = 1
e.data[1] = 1
e.data[2] = 1
e.data[3] = 0

ea = eaddr.TXDest()
ea[dev] = 1
eio.sendEvent(ea, e)

for i in range(100):
    erx = eio.getEvents(blocking=True)
    for e in erx:
        print e

eio.stop()
        


import pygtk, pygst
pygtk.require("2.0")
pygst.require("0.10")

import gtk, gtk.glade, gst, gobject
from somaEventFilter import SomaAudioEventFilter

eventSource = gst.element_factory_make('somaeventsource', "SomaEventSource")
somaAudioEventFilter = SomaAudioEventFilter()

audioSrcChan = 0x20
somaAudioEventFilter.set_audio_src(audioSrcChan)

#self.update_source()
alsaSink = gst.element_factory_make("osssink")

pipeline = gst.Pipeline()
pipeline.add(eventSource, somaAudioEventFilter)
ac = gst.element_factory_make("audioconvert")
ars = gst.element_factory_make("audioresample")
queue = gst.element_factory_make("queue")
pipeline.add(queue, ac, ars)
pipeline.add(alsaSink)
        
gst.element_link_many(eventSource, somaAudioEventFilter,
                      queue, ac, ars, alsaSink)

pipeline.set_state(gst.STATE_PLAYING)

gobject.MainLoop().run()
