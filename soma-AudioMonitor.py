#!/usr/bin/env python

import pygtk, pygst
pygtk.require("2.0")
pygst.require("0.10")

import gtk, gtk.glade, gst, gobject
from somaEventFilter import SomaAudioEventFilter

import dbus, dbus.service, dbus.mainloop.glib

class AudioMonitorGui(dbus.service.Object):
    def myInit(self): #used so I don't have to overload the dbus.service.object contsructor

        gladefile = "audioMonitor.glade"
        windowname = "main_window"
        abt_dlg_name = "about_dialog"
        self.wTree = gtk.glade.XML(gladefile,windowname)
        self.aTree = gtk.glade.XML(gladefile,abt_dlg_name)
        self.window = self.wTree.get_widget(windowname)
        self.about_dlg = self.aTree.get_widget(abt_dlg_name)
#        self.wTree.get_widget()
        self.window.show()
        self.playing = False
        self.audioSrcChan = 0x20;
        self.tetrodeChan = 0;        
        self.changes_made_while_paused = False

        combo = self.wTree.get_widget("source_combo")
        combo.set_active(0)        

        dic = { "on_play_pause_button_clicked"  : self.play_pause_clicked,
                "on_source_combo_changed"       : self.source_combo_action,
                "on_ch0_radio_pressed"          : self.ch0_radio_pressed,
                "on_ch1_radio_pressed"          : self.ch1_radio_pressed,
                "on_ch2_radio_pressed"          : self.ch2_radio_pressed,
                "on_ch3_radio_pressed"          : self.ch3_radio_pressed,
                "on_main_window_destroy"        : self.quit, 
                "on_quit_menu_activate"         : self.quit,
                "on_about_menu_activate"        : self.display_about,
                "on_ch0_radio_group_changed"    : self.radio_group_change
               }
    
        self.wTree.signal_autoconnect(dic)
 #       self.initialize_pipeline()                      

    def initialize_pipeline(self):
        gobject.type_register(SomaAudioEventFilter)

        self.eventSource = gst.element_factory_make('somaeventsource', "SomaEventSource")
        self.somaAudioEventFilter = SomaAudioEventFilter()
        self.update_source()
        self.alsaSink = gst.element_factory_make("osssink")

        self.pipeline = gst.Pipeline()
        self.pipeline.add(self.eventSource, self.somaAudioEventFilter)
        self.ac = gst.element_factory_make("audioconvert")
        self.ars = gst.element_factory_make("audioresample")
        self.queue = gst.element_factory_make("queue")
        self.pipeline.add(self.queue, self.ac, self.ars)
        self.pipeline.add(self.alsaSink)
        
        gst.element_link_many(self.eventSource, self.somaAudioEventFilter,self.queue, self.ac, self.ars, self.alsaSink)

    def restart_pipeline(self):
        print "Changing pipeline"
        self.pipeline_pause()
        print "pushing new src to SomaAudioEventFilter"
        self.update_source()
        self.pipeline_start()

    def set_source(self, src):
        if (self.audioSrcChan == src):
            print "set_source called but same source given"
            return

        self.audioSrcChan = src
        self.wTree.get_widget('source_combo').set_active(self.audioSrcChan)
        print "New source selected:", self.audioSrcChan
        if self.playing==True:
            self.restart_pipeline()        
        
        else:
            self.changes_made_while_paused = True

    def update_source(self):
        print "Changing caps, setting event source to:", self.audioSrcChan
        self.audioSrcChan = 0x20
        self.somaAudioEventFilter.set_audio_src(self.audioSrcChan)       

    def pipeline_start(self):
        print "Starting pipeline, setting state to gst.STATE_PLAYING"
        self.pipeline.set_state(gst.STATE_PLAYING)

    def pipeline_pause(self):
        print "Pausing Pipeline, setting state to gst.STATE_NULL"
        self.pipeline.set_state(gst.STATE_NULL)

    def check_for_changes(self):
        if self.changes_made_while_paused == True:
            print "Changes made while paused, updating src"
            self.update_source()
    
    def set_play(self):
        self.wTree.get_widget('play_pause_button').set_label("gtk-media-pause")
        self.check_for_changes()
        self.pipeline_start()
        self.playing = True

    def set_pause(self):
        self.wTree.get_widget('play_pause_button').set_label("gtk-media-play")
        self.pipeline_pause()
        self.playing = False

# ---------- DBus Methods
    @dbus.service.method("soma.audio.AudioMonitor")
    def start_audio_monitor(self):
        print "DBus call to start audio monitor"
        self.set_play()        
        
    @dbus.service.method("soma.audio.AudioMonitor")
    def pause_audio_monitor(self):
        print "DBus call to pause audio monitor"
        self.set_pause()        

    @dbus.service.method("soma.audio.AudioMonitor")
    def set_audio_src(self, src):
        self.set_source(src)

#    @dbus.service.method("com.Soma.AudioMonitor")
#    def set_tetrode_channel(self):
#        print "Not yet implemented"

# ---------- GTK+  Callbacks below this point

    def source_combo_action(self, widget):
        newSrc = widget.get_active()
        self.set_source(newSrc)

    def quit_menu_selected(self, widget):
        print "Quit application selected"
    def about_menu_selected(self, widget):
        print "About selected"

    def radio_group_change(self, widget):
        print "CHANGE"

    def ch0_radio_pressed(self, widget):
        print "Ch0 radio button pressed"
        self.tetrodeChan=0

    def ch1_radio_pressed(self, widget):
        print "Ch1 radio button pressed"
        self.tetrodeChan=1

    def ch2_radio_pressed(self, widget):
        print "Ch2 radio button pressed"
        self.tetrodeChan=2

    def ch3_radio_pressed(self, widget):
        print "Ch3 radio button pressed"
        self.tetrodeChan=3

    def play_pause_clicked(self, widget):
        #if current state is paused, then button is clicked to play, once clicked 
        #the playbin starts and the button becomes a  pause buttom
        print "Play pause button pressed, setting button icon:", 

        if self.playing==False: #Not currently playing, set to play, then change widget
            print "pause"
            self.set_play()

        elif self.playing==True: #Currently playing set to pause, then change widget
            print "play"
            self.set_pause()

    def display_about(self, widget):
        resp = self.about_dlg.run()
        if  resp == gtk.RESPONSE_CANCEL:
            self.about_dlg.hide()
        print resp

    def quit(self, widget):
        print "Main window destroy called by:", widget.get_name()
        gtk.main_quit()

if __name__=='__main__':

    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    session_bus = dbus.SessionBus()
    name = dbus.service.BusName("soma.audio.AudioMonitor", session_bus)

    amg = AudioMonitorGui(session_bus, '/SomaAudioMonitor')
    amg.myInit()
    gtk.main()
    



