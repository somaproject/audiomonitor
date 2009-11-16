#ifndef __AUDIOMON_JACKSINK_H__
#define __AUDIOMON_JACKSINK_H__


#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <audiosink.h> 
#include <jack/jack.h>
#include <jack/ringbuffer.h>

class JackSink : public IAudioSink
{
  /*
    JackSink is a multithreaded interface to jack, where we 
    (out-of-thread) add in samples to a queue, and we play
    those samples ASAP. 
    

  */ 
public:
  JackSink(std::string name); // default connection to start with
  ~JackSink(); 

  void setVolume(float); 
  
  void run(); 
  void shutdown(); 
  
  void addSamples(const std::vector<float> & ); 


  int process(jack_nframes_t nframes); 
  int srate(jack_nframes_t nframes); 
  void error(const char * desc); 

  inline int samplesInBuffer() {
    throw std::runtime_error("Not implemented"); 
  }

private:
  jack_client_t * jack_client_;
  jack_port_t *output_port_;
  

//   boost::mutex samples_mutex_; 

//   std::list<float> samples_; 
  
  typedef float sample_t; 

  jack_ringbuffer_t * ringbuffer_; 
  size_t overruns_; 
  size_t underruns_; 

}; 





#endif 
