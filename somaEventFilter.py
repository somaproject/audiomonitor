import pygtk
pygtk.require("2.0")
import gobject
gobject.threads_init ()

import pygst
pygst.require('0.10')
import gst, sys
import gobject
import errno
from struct import pack, unpack
import numpy as np


class SomaAudioEventFilter(gst.Element):
    """
    The Audio Event Filter interfaces with the Audio Monitor
    protocol.
    
    """

    
    _sinkpadtemplate = gst.PadTemplate ("sinkpadtemplate",
                                        gst.PAD_SINK,
                                        gst.PAD_ALWAYS,
                                        gst.caps_new_any())

    _srcpadtemplate = gst.PadTemplate ("srcpadtemplate",
                                        gst.PAD_SRC,
                                        gst.PAD_ALWAYS,
                                        gst.caps_new_any())

    def __init__(self):
        gst.Element.__init__(self)

        gst.info('creating srcpadpad')
        self.srcpad = gst.Pad(self._srcpadtemplate, "src")
        gst.info('adding srcpad to self')
        self.add_pad(self.srcpad)
        self.srcpad.set_getcaps_function(self.src_getcaps_function)
        self.srcpad.set_setcaps_function(self.src_setcaps_function)

        gst.info('creating sinkpad')
        self.sinkpad = gst.Pad(self._sinkpadtemplate, "sink")
        gst.info('adding sinkpad to self')
        self.add_pad(self.sinkpad)
        self.sinkpad.set_getcaps_function(self.sink_getcaps_function)
        self.sinkpad.set_setcaps_function(self.sink_setcaps_function)

        gst.info('setting chain/event functions')
        self.sinkpad.set_chain_function(self.chainfunc)
        self.sinkpad.set_event_function(self.eventfunc)
        self.srccaps =  gst.caps_from_string('audio/x-raw-int,rate=32000,channels=1,endianness=1234,width=16,depth=16,signed=true')
        self.AUDIO_BCAST_CMD = 0x18;
        self.lastvalue = 0
        
    def set_audio_src(self, newSrc):
        self.audioSRC = newSrc

    def src_getcaps_function(self, *foo):
        print "src_getcaps_function"
        gst.info('src getcaps')
        return self.srccaps

    def src_setcaps_function(self, pad, caps):
        print "src_setcaps_function"
        gst.info("attempting to src  setcaps:%s" % caps.to_string())
        self.srccaps = caps
        return True
    
    def sink_getcaps_function(self, *foo):
        gst.info('sink getcaps')
        capsString = "soma/event,src=%d"  % (self.audioSRC)
        caps = gst.caps_from_string(capsString)
        return caps
    
    def sink_setcaps_function(self, *foo):
        gst.info('sink setcaps')
        return True
        
    def chainfunc(self, pad, buffer):
        """
        This is the primary function that takes in event buffers and emits
        audio buffers.
        
        """
        buffer_out = ""	
        nEvents = len(buffer)/12;  
        gst.info("somaEventFilter.chainfunc length of Buffer IN:" +  str(len(buffer)) + ' with nEvents:' + str(nEvents))

        if len(buffer) % 12 != 0:
            gst.warn( "Error: nEvents % 12 !=0")
            return ""

        for i in range(nEvents):
            buffer_out = buffer_out + (self.parseEvent( buffer[ (i*12) : (i*12)+12 ] ))

        buf = gst.Buffer(buffer_out)
        caps = self.srccaps 

        buf.set_caps(caps)
        gst.info("returning %d samples" % len(buffer))
        return  self.srcpad.push(buf)
        

    def eventfunc(self, pad, event):

        self.info("%s event:%r" % (pad, event.type))
    
        return self.srcpad.push_event(event)
    
    def parseEvent(self, buffer):
        """
        Take in a 12-byte string and extract out the command
        to see if it's an audio command. 

        """
        
        CMD = 'B'
        SRC = 'B'

        cmd = unpack(CMD, buffer[0:1])[0]
        
        if cmd != self.AUDIO_BCAST_CMD:
            gst.info("Parsing Events, CMD IS NOT :" +  str(self.AUDIO_BCAST_CMD) + '  CMD IS:' +  str(cmd)) 
            return ""
        
        #data = np.fromstring(buffer[4:], dtype=np.int16)
        #print data
        #datas = data.astype(np.float)
        #scaled = (data * 1) # .astype(np.int16)
        #scaled = scaled.astype(">h")
        #scaled = np.array([0, 2**15-1, 0, 2**15-1], dtype=np.int16)
        #buffer_out = scaled.tostring()
        #gst.error("HELLO") # %s" % data)
        buffer_out = buffer[4:]
        return buffer_out
		
		
gobject.type_register(SomaAudioEventFilter)
