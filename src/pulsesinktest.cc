#include <iostream>
#include "pulsesink.h"
#include <unistd.h>
#include <sys/time.h>

float get_time_delta(const timeval  & t1, const  timeval & t2)
{
  float secs = t2.tv_sec - t1.tv_sec; 
  
  float usecs = t2.tv_usec - t1.tv_usec; 
  
  return secs + usecs/1e6; 
}


void input_thread(PulseSink * ps) 
{

  int pos = 0; 
  double fs = 44120; 
  double rate = 550.0; 

  timeval lasttime, now; 
  gettimeofday(&lasttime, NULL); 
  double waitingtime = 0.0; 
  double totaltime = 0.0; 
  
  while(totaltime < 20.0) { 
    gettimeofday(&now, NULL); 
    float delta = get_time_delta(lasttime, now); 
    waitingtime += delta; 
    totaltime += delta; 
    std::vector<float> buffer; 
    
    while(waitingtime > 1./fs) {
      
      double t = pos * 1./fs; 
      float val = sin(t * rate * 3.14 * 2); 
      
      pos++; 

      waitingtime -= 1./fs; 
      buffer.push_back(val); 
    }

    ps->addSamples(buffer); 

    lasttime.tv_sec = now.tv_sec; 
    lasttime.tv_usec = now.tv_usec;
  }

  ps->shutdown(); 
  
}

int main()
{
  PulseSink * ps = PulseSink::createWithMainloop(); 
  
  
  boost::thread thread(input_thread, ps); 
  ps->run(); 

  thread.join(); 
  
  delete ps; 

}
