#ifndef __AUDIOMON_AUDIOSINK_H__
#define __AUDIOMON_AUDIOSINK_H__

#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

class IAudioSink
{
public:
  virtual void setVolume(float) = 0; 
  virtual void run() = 0; 
  virtual void shutdown() = 0; 
  virtual void addSamples(const std::vector<float> & ) = 0; 

  virtual ~IAudioSink() {
  }
}; 



#endif  // __AUDIOMON_AUDIOSINK_H__
