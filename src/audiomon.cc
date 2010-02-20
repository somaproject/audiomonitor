#include <boost/foreach.hpp>

#include "logging.h"
using namespace boost::logging; 

#include "audiomon.h"



AudioMonitor::AudioMonitor(somanetwork::pNetworkInterface_t ni, IAudioSink * sink) :
  pNetwork_(ni), 
  pSink_(sink), 
  paused_(false),
  src_(0),
  isEventLogging_(false)
{
  pNetwork_->enableEventRX(); 
  int read_fd = pNetwork_->getEventFifoPipe(); 
  
  Glib::signal_io().connect(sigc::mem_fun(*this, &AudioMonitor::event_rx_callback),
			    read_fd, Glib::IO_IN);

  pNetwork_->run(); 
  
  
}

AudioMonitor::~AudioMonitor() {

  
  pNetwork_->shutdown(); 
  disableEventLogging(); 

}
  
void AudioMonitor::setSource(sn::eventsource_t src) {
  /*
    Disable the current audio source we are connected to 
    Enable the new source . 

    FIXME: multiple audio monitors thus may fight to enable/disable
    other channels. We should fix this. 
    
  */ 
  AML_(info) << "AudioMonitor::setSource " << (unsigned int)src << std::endl; 
  disableSource(src_); 
  enableSource(src); 
  src_ = src; 
  
}


void AudioMonitor::pause() 
{
  paused_ = true;  

}

void AudioMonitor::play()
{
  paused_ = false; 
  
}
  
void  AudioMonitor::enableSource(sn::eventsource_t src)
{
  // FIXME log
  AML_(debug) << "AudioMonitor::enableSource " << (unsigned int)src << std::endl; 
  
  sn::EventTXList_t etxl; 

  sn::EventTX_t etx; 
  etx.destaddr[src] = true; 
  etx.event.cmd = CMD_AUDIOCOMMAND_TX; 
  etx.event.data[0] = 1; 
  etx.event.data[1] = 1;
  etx.event.data[2] = 0; 

  etxl.push_back(etx); 

  pNetwork_->sendEvents(etxl); 
  

}

void  AudioMonitor::disableSource(sn::eventsource_t src)
{
  AML_(debug) << "AudioMonitor::disableSource " << (unsigned int)src << std::endl; 
  // fixme log
  sn::EventTXList_t etxl; 

  sn::EventTX_t etx; 
  etx.destaddr[src_] = true; 
  etx.event.cmd = CMD_AUDIOCOMMAND_TX; 
  etx.event.data[0] = 1; 
  etx.event.data[1] = 0; // always on

  etxl.push_back(etx); 

  pNetwork_->sendEvents(etxl); 

}
  

bool AudioMonitor::event_rx_callback(Glib::IOCondition io_condition)
{

  if ((io_condition & Glib::IO_IN) == 0) {
    // FIXME log
    std::cerr << "Invalid fifo response" << std::endl;
  }
  else 
    {
      char x; 
      read(pNetwork_->getEventFifoPipe(), &x, 1); 
      
      sn::pEventPacket_t rdp = pNetwork_->getNewEvents(); 

      // LOGGING TO DISK  // this should be less thant 1.2 MB/sec
      if(isEventLogging_) {
	BOOST_FOREACH(sn::Event_t & e, *(rdp->events)) { 
	  logfstream_.write((char*)&e.cmd, 1); 
	  logfstream_.write((char*)&e.src, 1); 
	  logfstream_.write((char*)&e.data[0], 10); 
	}
      }
      

      processInboundEvents(rdp->events); 

    }
  
  return true; 
  
}

void AudioMonitor::processInboundEvents(const sn::pEventList_t & events)
{
  recentpeak_ = -1; // total hack! 

  bool received_audio_sample_ = false; 
  BOOST_FOREACH(sn::Event_t & e, *events) {
    if(e.cmd != 0x10) {
    }
    // log all event status messages from all sources
    if (e.cmd == CMD_AUDIOBCAST) { 
      if (e.src == src_) { 
	if (e.data[0] == 0)  {
	  newAudioStateEvent(e); 
	} else { 
	  newAudioSamplesEvent(e);
	  received_audio_sample_ = true; 
	} 
	// FIXME : we should log the others anyway

      } else {
	// from someone else, just log 
      }
    } 
  }
  if (received_audio_sample_) {
    peaksignal_.emit(float(recentpeak_) / (1 << 15)); 
  }

}

void AudioMonitor::newAudioSamplesEvent(const sn::Event_t evt)
{
  /* 
     New samples have arrived, put them in sink
     
   */ 
  if (paused_) {
    return; 
  }
  std::vector<float> samples(4) ; 
  for(int i = 0; i < 4; i++) {
    int16_t val = (int16_t)evt.data[i+1]; 
    int16_t abs_val = abs(val); 
    if (abs_val > recentpeak_) {
      recentpeak_ = abs_val; 
    }
    samples[i] = float(val)/ (1<<15); 
  }
  
  pSink_->addSamples(samples); 
  AML_(debug) << "AudioMonitor::newAudioSamplesEvent, event= " << evt << std::endl; 

}

void AudioMonitor::newAudioStateEvent(const sn::Event_t evt) 
{
  /*
    we assume it is for us at this point
    
  */ 
  bool is_on; 

  if(evt.data[1] == 0) { 
    is_on = false; 
  } else {
    is_on = true; 
  }

  channel_ = evt.data[2]; 
  samplingrate_ = evt.data[3]; 
  volscale_ = evt.data[4]; 

  statussignal_.emit(is_on, channel_, samplingrate_, volscale_); 
  
  AML_(debug) << "AudioMonitor::newAudioStateEvent, event= " << evt << std::endl; 
  AML_(info) << "AudioMonitor::newAudioStateEvent, channel ="
	     << channel_ << " enabled=" << is_on 
	     << " sampling rate = " << samplingrate_
	     << " volume scale =" << volscale_; 
  
}
  
sigc::signal<void, bool, int, int, int> & AudioMonitor::statusSignal()
{
  return statussignal_;   

}

void AudioMonitor::setChannel(int channum)
{

  setRemoteState(true, channum, volscale_); 
  
}

void AudioMonitor::setRemoteState(bool on, int channum, int volscale)
{
  sn::EventTXList_t etxl; 

  sn::EventTX_t etx; 
  etx.destaddr[src_] = true; 
  etx.event.cmd = CMD_AUDIOCOMMAND_TX; 
  etx.event.data[0] = 1; 
  etx.event.data[1] = on; 
  etx.event.data[2] = channum; 
  etx.event.data[3] = volscale; 

  etxl.push_back(etx); 

  pNetwork_->sendEvents(etxl); 
  
}


void AudioMonitor::setVolume(double x) 
{
  AML_(info) << "AudioMonitor::setVolume, value= " << x << std::endl; 
  
  pSink_->setVolume(x); 

}

void AudioMonitor::enableEventLogging(boost::filesystem::path file)
{
  isEventLogging_ = true; 
  logfstream_.open(file.string().c_str(),
		   std::fstream::out | std::fstream::binary); 

}

void AudioMonitor::disableEventLogging()
{
  if(isEventLogging_) { 
    logfstream_.close(); 
  }
  
  isEventLogging_ = false; 

}

void AudioMonitor::setVolumeScaling(int scale)
{
  
  setRemoteState(true, channel_, scale); 

}


sigc::signal<void, float> & AudioMonitor::peakSignal()
{
  return peaksignal_; 

}
