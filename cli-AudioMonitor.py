import pygst
pygst.require('0.10')
import gst, sys
import gobject
import errno
from struct import pack, unpack
from somaEventFilter import * # SomaAudioEventFilter

gobject.threads_init ()


def main(args):

    dev = int(sys.argv[1])
    gobject.type_register(SomaAudioEventFilter)

    audioSrc = gst.element_factory_make('somaeventsource', 'SomaEventSource')
    #audioSrc = gst.element_factory_make('audiotestsrc', "test src")

    gst.info('About to create SomaAudioFilter')
    #somaAudioEventFilter = PyIdentity()
    somaAudioEventFilter = SomaAudioEventFilter()
    print "Created somaAudioEventFilter"
    somaAudioEventFilter.set_audio_src(dev)
    
    pipeline = gst.Pipeline()
    pipeline.add(audioSrc)
    pipeline.add(somaAudioEventFilter)

    finalSink = gst.element_factory_make("pulsesink")
    
    #finalSink = gst.element_factory_make("filesink")
    #finalSink.set_property("location", '/tmp/somaeventsourcetest.dat')

    pipeline.add(finalSink)
    ac = gst.element_factory_make("audioconvert")
    ars = gst.element_factory_make("audioresample")
    queue = gst.element_factory_make("queue")
    queue.set_property("max-size-buffers", 10000)
    pipeline.add(queue)
    pipeline.add(ac)
    pipeline.add(ars)

##     caps = gst.Caps("audio/x-raw-int,rate=44100")
##     filter = gst.element_factory_make("capsfilter", "filter")
##     filter.set_property("caps", caps)
##     pipeline.add(filter)
    
    gst.element_link_many(audioSrc, somaAudioEventFilter, queue,
                          ac, ars, finalSink)
    #gst.element_link_many(audioSrc,somaAudioEventFilter, ars, filter, finalSink)
    #gst.element_link_many(audioSrc,somaAudioEventFilter, queue, finalSink)
    #gst.element_link_many(audioSrc, finalSink)

    #gst.element_link_many(audioSrc,somaAudioEventFilter, finalSink)
    print "somaAudioFilter linked to finalSink"

    print "Pipeline setup, setting STATE to PLAYING"
    pipeline.set_state(gst.STATE_PLAYING)


    gobject.MainLoop().run()

if __name__=="__main__":
    
    main(sys.argv)


