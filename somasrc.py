import sys, gobject, pygst
pygst.require('0.10')
import gst
from somapynet.event import Event
from somapynet.neteventio import NetEventIO
from somapynet import eaddr
import struct
from time import time

class SomaAudioSrc(gst.BaseSrc):
	__gsttemplates__ = (
		gst.PadTemplate("src", 
						gst.PAD_SRC, 
						gst.PAD_ALWAYS, 
						gst.caps_new_any())
		)



	def __init__(self,name):
#		print "SomaAudioSrc::__init__"
		self.__gobject_init__()
		self.set_name(name)
		self.caps = gst.caps_from_string("audio/x-raw-int,rate=8000,channels=1,endianness=1234, width=16, depth=16, signed=true")
		self.i = 1

	datasrc = None

	def set_property(self, name, value):
#		print "SomaAudioSrc::set_property"
		if name=="ip":
			self.ip = value
		if name=="cmd":
			self.cmd = value
		if name=="src":
			self.src = value

	def initDataSource(self):
#		print "SomaAudioSrc::initDataSource"
		self.datasrc = AudioEventRx(self.ip, self.cmd, self.src)
		

	def do_create(self, offset, size):
#		self.i += 1
#		print "SomaAudioSrc::do_create", offset
		if not self.datasrc:
			self.initDataSource()
		data = self.datasrc.getAudioBuffer(size)
#		print "do_create: ", offset
		
		return gst.FLOW_OK, self.makeBuffer(data)


	def makeBuffer(self,data):

		buffer = gst.Buffer(data)
		buffer.set_caps(self.caps)
		return buffer

gobject.type_register(SomaAudioSrc)


class AudioEventRx(object):
	def __init__(self, ip, cmd,src):
#		print "AudioEventRx::__init__"
		self.ip = ip
		self.eio = NetEventIO(ip)
		self.eio.addRXMask(cmd,src)
		self.eio.start()
		self.buffer = ""
		self.count = 0
	
	def getAudioBuffer(self, size):

		if len(self.buffer)<size:		
			self.getData()

		data = self.buffer[:size]
		self.buffer = self.buffer[size:]

		return data

	def getData(self):
		self.count +=1
		erx = self.eio.getEvents()
		if self.count%10==0:	print len(erx)
		if len(erx)>640:
			erx = erx[640:]
		
		for event in erx:
			self.buffer += struct.pack('<h', event.data[1])
			self.buffer += struct.pack('<h', event.data[2])
			self.buffer += struct.pack('<h', event.data[3])
			self.buffer += struct.pack('<h', event.data[4])
	
