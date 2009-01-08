import pygst
pygst.require('0.10')
import gst, sys
import gobject
import errno
from struct import pack, unpack
gobject.threads_init ()

class SomaAudioEventFilter(gst.Element):

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
        self.srccaps =  gst.caps_from_string('audio/x-raw-int,rate=8000,channels=1,endianness=1234,width=16,depth=16,signed=true')
        self.audioCMD = 0x30;
        self.lastvalue = 0
        
    def set_audio_src(self, newSrc):
        self.audioSRC = newSrc

    def src_getcaps_function(self, *foo):
        print "src getcaps"
        gst.info('src getcaps')
        return self.srccaps

    def src_setcaps_function(self, pad, caps):
        print "src setcaps"
        gst.info("attempting to src  setcaps:%s" % caps.to_string())
        self.srccaps = caps
        return True
    
    def sink_getcaps_function(self, *foo):

        gst.info('sink getcaps')
        capsString = "soma/event,src=%s"  % (self.audioSRC)

        caps = gst.caps_from_string(capsString)
        return caps
    
    def sink_setcaps_function(self, *foo):
        print "SomaAudioFilter setcaps"
        gst.info('sink setcaps')
        return True
        
    def chainfunc(self, pad, buffer):
        #self.info("%s timestamp(buffer):%d" % (pad, buffer.timestamp))
        buffer_out = ""	
        nEvents = len(buffer)/12;  
        #gst.info("somaEventFilter.chainfunc length of Buffer IN:" +  str(len(buffer)) + ' with nEvents:' + str(nEvents))
        
##         if len(buffer) % 12 != 0:
##             gst.warn( "Error: nEvents%12 !=0")
##             return ""

        for i in range(nEvents):
            buffer_out = buffer_out + (self.parseEvent( buffer[ (i*12) : (i*12)+12 ] ))
       
        buf = gst.Buffer(buffer_out)
        caps = self.srccaps
        buf.set_caps(caps)
        #buf.stamp(buffer)
        #gst.info("somaEventFilter.chainfunc() returning buffer of len: " + str(len(buffer_out)))

        return  self.srcpad.push(buf)

    def eventfunc(self, pad, event):
        print "FILTER ELEMENT GOT EVENT", event.type
        self.info("%s event:%r" % (pad, event.type))
    
        return self.srcpad.push_event(event)
    
    def parseEvent(self, buffer):
##         if len(buffer)<12:
##             gst.info("ParsingEvents, buffer len <12")
##             return ''

        CMD = 'B'
        SRC = 'B'
        DATA_IN = '>h'
        DATA_OUT = '<h'

        data = ''

        cmd = unpack(CMD, buffer[0:1])[0]
##         if cmd != self.audioCMD:
##             gst.info("Parsing Events, CMD IS NOT :" +  str(self.audioCMD) + '  CMD IS:' +  str(cmd)) 
##             return ""


        buffer_out = buffer[4:]
        #print unpack(">hhhh", buffer[4:])
        
##      buffer_out = ""        
##         for i in range(4):
##             buffer_out += pack(">h",  self.lastvalue)

##             if self.lastvalue == 2**15 -1:
##                 self.lastvalue = -2**15
##             else:
##                  self.lastvalue += 1
                 
        return buffer_out
		
		
