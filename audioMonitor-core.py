#!/usr/bin/env python

import pygtk, pygst
pygtk.require("2.0")
pygst.require("0.10")

import gtk, gtk.glade, gst, gobject
from somaEventFilter import SomaAudioEventFilter
from somapynet.event import Event
from somapynet.neteventio import NetEventIO
from somapynet import eaddr
from struct import pack, unpack
import dbus, dbus.service, dbus.mainloop.glib

class AudioMonitorCore(dbus.service.Object):
    def __init__(self, *args):

        dbus.service.Object.__init__(self, *args)
        # State Variables
        self.playing = False
        self.audioChan = 0x20;
        self.selectedAudioChan = self.audioChan
        self.subChan = 0;  #most likely will refere to a tetrode sub-channel
        self.selectedSubChan = self.subChan    

        self.eio = NetEventIO("10.0.0.2")
        
        # AudioControl Event Constants setup on 10/13/08
        self.AUDIO_EVENT_CMD = 0x12
        self.ENABLE_AUDIO_EVENTS = 1
        self.DISABLE_AUDIO_EVENTS = 0
        self.AUDIO_REQUEST = 0
        self.AUDIO_UPDATE = 1

        #Create GST elements - pipeline will not be created
        gobject.type_register(SomaAudioEventFilter)
        self.eventSource = gst.element_factory_make('somaeventsource', "SomaEventSource")
        self.audioEventFilter = SomaAudioEventFilter()
        self.AUDIO_SINK = "pulsesink"
        self.audioSink = gst.element_factory_make(self.AUDIO_SINK)
        self.audioConverter = gst.element_factory_make("audioconvert")
        self.audioResampler= gst.element_factory_make("audioresample")
        self.queue = gst.element_factory_make("queue")
        self.pipeline = None
        
    def setChannel(self, src):
        print "setting Channel to:", src
        self.selectedAudioChan = src
    
    def getChannel(self):
        return self.audioChan

    def setSubChannel(self, chan):
        print "stting SubChannel to:", chan
        self.selectedSubChan = chan

    def getSubChannel(self):
        return self.subChan

    def startPlaying(self):
        if self.pipeline == None:
            self.__createNewPipeline()
            print "Create Pipeline" 

        print "Play Pipeline"
        self.pipeline.set_state(gst.STATE_PLAYING)

    def stopPlaying(self):
        print "Destroy Pipeline"
    
        if self.pipeline != None:
            self.pipeline.set_state(gst.STATE_NULL)
    
        self.pipeline = None

# ----- Private Methods, Pipeline control, Event Transmission, and looping 
    def __createNewPipeline(self):

        self.pipeline = gst.Pipeline()
        self.audioEventFilter.set_audio_src(self.audioChan)
        self.pipeline.add(self.eventSource, self.audioEventFilter)
        self.pipeline.add(self.queue, self.audioConverter, self.audioResampler)
        self.pipeline.add(self.audioSink)
        
        gst.element_link_many(self.eventSource, self.audioEventFilter,self.queue, self.audioConverter, self.audioResampler, self.audioSink)
    def __loop(self):

        if self.audioChan != self.selectedAudioChan:
            changeSource()

        if self.subChan != self.selectedSubChan:
            changeSource()

        self.processEvents(self.eio.getEvents(), False)
        
    def changeSource(self):

        self.stopPlaying()
        
        self.disableCurrentSource()
        self.enableDesiredSource()
        
        if self.playing==True:
            self.startPlaying()

    def disableCurrentSource(self):
        e = self.disableEvent()

        self.eio.addRxMask(self.AUDIO_EVENT_CMD, self.audioChan) 

        ea = eaddr.TXDest()
        ea[self.audioChan] = 1
        self.eio.sendEvent(e, ea) # Send the disable Event
        self.processEvents(self.eio.getEvents(), True) # Act when the disable Event is echoed back
        ea[self.audioChan] = 0
        self.playing = False

    def enableDesiredSource(self):
        self.eio.addRxMask(self.AUDIO_EVENT_CMD, self.selectedAudioChan, True) 

        e = self.enableEvent()
        ea[self.selectedAudioChan] = 0        
        self.eio.sendEvent(e, ea) # Send the enable event
        self.processEvents(self.eio.getEvents(), True) # Act when the enable Event is echoed back
    
    def processEvents(self, events, active):
        for e in events:
            src = e.src
            if unpack('>H', e.data[0])[0] == 1: #Reporting a Change
                if unpack('>H', e.data[1])[0] == 0: #Device has been disabled
                    if src==self.audioChan:
                        self.stopPlaying()
                        if active==False: # if the source wasn't actively changed re-enable it
                            self.enableDesiredSource()
                        
                if unpack('>H', e.data[1])[0] == 1: #Device has been enabled
                    if src==self.selectedAudioChan:
                        self.audioChan = self.selectedAudioChan #selected chan is playing make

    def disableEvent(self):
        e = Event()
        e.cmd = self.AUDIO_EVENT_CMD
        e.src = eaddr.NETWORK
        e.data[0] = pack('>H', 0) #Requesting Change
        e.data[1] = pack('>H', 0) #Set Channel to Off
        return e

    def enableEvent(self):
        e = Event()
        e.cmd = self.AUDIO_EVENT_CMD
        e.src = eaddr.NETWORK
        e.data[0] = pack('>H', 0) #Requesting Change
        e.data[1] = pack('>H', 1) #Set Channel to On
        e.data[2] = pack('>H', self.selectedSubChan)
        e.data[3] = pack('>H', 32000)
        return e

               

# ---------- DBus Methods
    @dbus.service.method("soma.audio.AudioMonitor")
    def start_playing(self):
        print "DBus call to start audio monitor"
        self.startPlaying()        
        
    @dbus.service.method("soma.audio.AudioMonitor")
    def stop_playing(self):
        print "DBus call to pause audio monitor"
        self.stopPlaying()        

    @dbus.service.method("soma.audio.AudioMonitor", in_signature='i')
    def set_audio_channel(self, src):
        print src
        self.setChannel(src)

    @dbus.service.method("soma.audio.AudioMonitor", out_signature='i')
    def get_audio_channel(self):
        return self.getChannel()

    @dbus.service.method("soma.audio.AudioMonitor", in_signature='i')
    def set_sub_channel(self, src):
        self.setSubChannel(src)

    @dbus.service.method("soma.audio.AudioMonitor", out_signature='i')
    def get_sub_channel(self):
        return self.getSubChannel()
  
    @dbus.service.method("soma.audio.AudioMonitor", out_signature='b')
    def get_playing_state(self):
        return self.playing
    
if __name__=='__main__':

    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    session_bus = dbus.SessionBus()
    name = dbus.service.BusName("soma.audio.AudioMonitor", session_bus)

    amg = AudioMonitorCore(session_bus, '/SomaAudioMonitor')
    gtk.main()
