#!/usr/bin/python

import sys, gobject, pygst
pygst.require('0.10')
import gst
from somasrc import SomaAudioSrc

def main(args):
	
	pipeline = gst.Pipeline('pipeline')
	print "Pipeline Created	"

	somasrc = SomaAudioSrc('SomaSource')
	assert somasrc
	somasrc.set_property('ip', "127.0.0.1")
	somasrc.set_property('cmd', 16)
	somasrc.set_property('src', 0)
	print "SomaSrc Created"
	
	sink = gst.element_factory_make("alsasink", 'sink')
	print "alsasink created"	

	pipeline.add(somasrc, sink)
	print "Somasrc & AlsaSink ADDED to PIPELINE"
	gst.element_link_many(somasrc,sink)
	print "Elements Linked"
	pipeline.set_state(gst.STATE_PLAYING)
	print "Pipeline Started"

	mainloop = gobject.MainLoop()
	
	def bus_event(bus, message):
		t = message.type
		if t == gst.MESSAGE_EOS:
			mainloop.quit()
		elif t == gst.MESSAGE_ERROR:
			err, debug = message.parse_error()
			print "Error: %s" % err, debug
			mainloop.quit()
		return True
	pipeline.get_bus().add_watch(bus_event)

	mainloop.run()

	pipeline.set_state(gst.STATE_NULL)


if __name__ == "__main__":
	sys.exit(main(sys.argv))	

