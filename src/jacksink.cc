#include <math.h>
#include "jacksink.h"

const size_t sample_size = sizeof(jack_default_audio_sample_t);

#define DEFAULT_RB_SIZE 2000		/* ringbuffer size in frames */


int jack_process (jack_nframes_t nframes, void *arg)
{
  JackSink * js = (JackSink*)arg; 
  return js->process(nframes); 

}

int jack_srate (jack_nframes_t nframes, void *arg){
  JackSink * js = (JackSink*)arg; 
  return js->srate(nframes); 

}

void jack_error (const char *desc){
  std::cerr << desc << std::endl; 
}

void jack_shutdown (void *arg){
  // pass
}

JackSink::JackSink(std::string clientname): 
  overruns_(0),
  underruns_(0)
{

  /* tell the JACK server to call error() whenever it
     experiences an error.  Notice that this callback is
     global to this process, not specific to each client.
     
     This is set here so that it can catch errors in the
     connection process
  */
  jack_set_error_function (jack_error);
  
  /* try to become a client of the JACK server */
  
  if ((jack_client_ = jack_client_new (clientname.c_str())) == 0) {
    std::cerr << "jack server not running?" << std::endl; 
    throw std::runtime_error("Jack Server not running"); 
  }
  
  /* tell the JACK server to call `process()' whenever
     there is work to be done.
  */
  
  jack_set_process_callback (jack_client_, jack_process, this);
  
  /* tell the JACK server to call `srate()' whenever
     the sample rate of the system changes.
  */
  
  jack_set_sample_rate_callback (jack_client_, jack_srate, this);
  
  /* tell the JACK server to call `jack_shutdown()' if
     it ever shuts down, either entirely, or if it
     just decides to stop calling us.
  */
  
  jack_on_shutdown (jack_client_, jack_shutdown, 0);
  
  /* display the current sample rate. once the client is activated 
     (see below), you should rely on your own sample rate
     callback (see above) for this value.
  */
  
  output_port_ = jack_port_register (jack_client_, 
				     "output", JACK_DEFAULT_AUDIO_TYPE, 
				     JackPortIsOutput, 0);
  
  if (jack_activate (jack_client_)) {
    std::cerr << "cannot activate client" << std::endl; 
    throw std::runtime_error("Cannot activate client"); 
  }
  

  ringbuffer_ = jack_ringbuffer_create(DEFAULT_RB_SIZE); 
  
}

void JackSink::run()
{
  const char **ports;

  /* connect the ports*/
  if ((ports = jack_get_ports (jack_client_, NULL, NULL, 
			       JackPortIsPhysical|JackPortIsInput)) 
      == NULL) {
    std::cerr << "Cannot find any physical playback ports" << std::endl; 
    exit(1);
  }
  
  int i=0;
  while(ports[i]!=NULL){
    if (jack_connect (jack_client_, jack_port_name (output_port_),
		      ports[i])) {
      std::cerr << "cannot connect output ports" << std::endl; 
    }
    i++;
  }
  
}

void JackSink::shutdown() 
{
  jack_client_close (jack_client_);

}

JackSink::~JackSink()
{
  jack_ringbuffer_free(ringbuffer_); 


}

void JackSink::setVolume(float x)
{


}

size_t min(size_t x, size_t y) 
{
  if (x < y ) 
    return x;
  return y; 

}

void JackSink::addSamples(const std::vector<float> & buffer)
{


  for (int i = 0; i < buffer.size(); i++) {
    while(jack_ringbuffer_write_space (ringbuffer_) < sample_size) 
      {
	// spinning is fun
      }
      jack_ringbuffer_write(ringbuffer_, (char *) &buffer[i],
			    sample_size); 
  }

}


int JackSink::process(jack_nframes_t nframes)
{
  sample_t *out = (sample_t *) jack_port_get_buffer (output_port_, nframes);
  
  size_t frame = 0; 
  sample_t lastval = 0.0; 
  
  while (frame < nframes) { 
    if (jack_ringbuffer_read_space(ringbuffer_) >= sample_size) {
      
      jack_ringbuffer_read(ringbuffer_, (char*)&lastval, sample_size); 
      out[frame] = lastval; 
      frame++; 
      
    } else {
      out[frame] = lastval; 
      //frame++;
      std::cout << "underruns = " << underruns_ << std::endl; 
      underruns_++; 
    }
  }
  return 0; 
}


int JackSink::srate(jack_nframes_t nframes) 
{
  std::cout << "JackSink::srate " << nframes << std::endl;
  
}

void JackSink::error(const char * desc) 
{
  
  
}

