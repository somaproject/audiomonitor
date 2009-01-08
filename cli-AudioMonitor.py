import pygst
pygst.require('0.10')
import gst, sys
import gobject
import errno
from struct import pack, unpack
from somaEventFilter import SomaAudioEventFilter

gobject.threads_init ()


def main(args):

    gobject.type_register(SomaAudioEventFilter)
    print "SomaAudioEventFilter registered with GObject"

    audioSrc = gst.element_factory_make('somaeventsource', 'SomaEventSource')

    print "Created SomaEventSource"

    gst.info('About to create SomaAudioFilter')
    somaAudioEventFilter = SomaAudioEventFilter()
    print "Created somaAudioEventFilter"
    somaAudioEventFilter.set_audio_src(0x20)
    
    pipeline = gst.Pipeline()
    pipeline.add(audioSrc, somaAudioEventFilter)
    print "somaEventSource and somaAudioEventFilter added to pipeline"

    finalSink = gst.element_factory_make("alsasink")
    
    #finalSink = gst.element_factory_make("filesink")
    #finalSink.set_property("location", '/tmp/somaeventsourcetest.dat')

    ac = gst.element_factory_make("audioconvert")
    ars = gst.element_factory_make("audioresample")
    queue = gst.element_factory_make("queue")
    pipeline.add(queue)
    pipeline.add(ac)
    pipeline.add(ars)
    pipeline.add(finalSink)
    print "Alsasink created and added to pipeline"

    gst.element_link_many(audioSrc,somaAudioEventFilter,queue, ac, ars, finalSink)
    #gst.element_link_many(audioSrc,somaAudioEventFilter, finalSink)
    print "somaAudioFilter linked to finalSink"

    print "Pipeline setup, setting STATE to PLAYING"
    pipeline.set_state(gst.STATE_PLAYING)


    gobject.MainLoop().run()

if __name__=="__main__":
    
    main(sys.argv)


