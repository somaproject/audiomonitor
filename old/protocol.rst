Audio monitor protocol
======================


The audio monitor interface allows a device to transmit a series
of events for "out-of-band" audio monitoring. 

There are two classes of events that are used -- a broadcast
event (sent to everyone on the event bus) that indicates
changes in status and provides the data payload, and a command
event that is sent to the device to change settings. 

settings: 

We can turn audio event transmission on and off. We can select
which of the five possible "subchannels" 
of audio data to monitor, A.1 - A.C. 

Command formats
-------------------
The broadcast status/data command: 

CMD = 0x18
if data[0] == 0: status update command
   data[1] ==> current status (1 == on, 0 == off)
   data[2] ==> subchannel selected
   data[3] ==> unsigned int of sampling rate, rounded to int
   data[4] ==> volume

if data[0] == 1: data command
   data[1] ==> sample 0...
   data[4] ==> sample 3


To change a state, send command 0x30 to the device:

data[0] = 1 to request a change (should always be 1), 0 is just query

data[1] = set audio transmission on (=1) or off (= 0) 
data[2] = the channel that is selected / to be selected
data[3] = volume

