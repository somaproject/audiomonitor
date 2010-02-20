#include <boost/format.hpp>
#include "logging.h"

#include "audiomongui.h"

using namespace boost::logging; 

AudioMonGui::AudioMonGui(AudioMonitor & am) :
  audiomonitor_(am),
  sourceframe_("Channel"), 
  channel1_("Channel 1"),
  channel2_("Channel 2"),
  channel3_("Channel 3"),
  channel4_("Channel 4"),
  channelC_("Channel C"), 
  playbutton_("Play"), 
  local_volume_label_("Local Volume:"), 
  local_volume_(0, 150, 5),
  remote_volume_label_("Full scale:"), 
  remote_volume_(0, 20, 1), 
  sourcelabel_("source:")
{

  set_title("Soma Audio Monitor");
  add(vbox_); 
  
  vbox_.pack_start(hbox_); 
  //vbox_.pack_start(statusbar_); 
  hbox_.pack_start(lefthbox_); 
  
  hbox_.pack_end(sourceframe_); 
  
  channelgroup_ = channel1_.get_group(); 
  channel2_.set_group(channelgroup_); 
  channel3_.set_group(channelgroup_); 
  channel4_.set_group(channelgroup_); 
  channelC_.set_group(channelgroup_); 
  
  sourceframe_.add(channelvbox_); 
  channelvbox_.pack_start(channel1_); 
  channelvbox_.pack_start(channel2_); 
  channelvbox_.pack_start(channel3_); 
  channelvbox_.pack_start(channel4_); 
  channelvbox_.pack_start(channelC_); 

  levelbar_.set_orientation(Gtk::PROGRESS_LEFT_TO_RIGHT); 

 
  lefthbox_.pack_start(sourceHBox_); 
  sourceHBox_.pack_start(sourcelabel_);

  sourceHBox_.pack_start(sourceCombo_); 
  sourceHBox_.pack_start(playbutton_);  

  m_refTreeModel = Gtk::ListStore::create(source_cols_);
  sourceCombo_.set_model(m_refTreeModel);

  Gtk::TreeModel::Row row = *(m_refTreeModel->append());
  
  
  for(int i = 0; i < 64; i++) { 
    row[source_cols_.m_col_id] = i + 8;
    row[source_cols_.m_col_name] = boost::str(boost::format("datasrc %d") % i); 
    row = *(m_refTreeModel->append());

  }

  sourceCombo_.pack_start(source_cols_.m_col_name); 
  
  local_volume_hbox_.pack_start(local_volume_label_); 
  local_volume_hbox_.pack_start(local_volume_); 
  local_volume_.set_value_pos(Gtk::POS_RIGHT); 
  local_volume_.set_size_request(150); 

  remote_volume_hbox_.pack_start(remote_volume_label_); 
  remote_volume_hbox_.pack_start(remote_volume_); 
  remote_volume_.set_value_pos(Gtk::POS_RIGHT); 
  remote_volume_.set_size_request(150); 
  
  local_volume_hbox_.pack_start(playbutton_); 
  
  lefthbox_.pack_start(local_volume_hbox_); 
  lefthbox_.pack_start(remote_volume_hbox_); 

  lefthbox_.pack_end(levelbar_); 

  //left
  
  show_all(); 

  sourceCombo_.signal_changed().connect(sigc::mem_fun(*this, 
						      &AudioMonGui::on_source_changed)); 
  channel1_.signal_clicked().connect(sigc::mem_fun(*this,
						   &AudioMonGui::on_channel_button_clicked) );
  channel2_.signal_clicked().connect(sigc::mem_fun(*this,
						   &AudioMonGui::on_channel_button_clicked) );
  channel3_.signal_clicked().connect(sigc::mem_fun(*this,
						   &AudioMonGui::on_channel_button_clicked) );
  channel4_.signal_clicked().connect(sigc::mem_fun(*this,
						   &AudioMonGui::on_channel_button_clicked) );
  channelC_.signal_clicked().connect(sigc::mem_fun(*this,
						   &AudioMonGui::on_channel_button_clicked) );

  playbutton_.signal_clicked().connect(sigc::mem_fun(*this, 
						     &AudioMonGui::on_play_button_clicked)); 

  local_volume_.signal_value_changed().connect(sigc::mem_fun(*this,
    &AudioMonGui::on_local_volume_value_changed));

  remote_volume_.signal_value_changed().connect(sigc::mem_fun(*this,
    &AudioMonGui::on_remote_volume_value_changed));

  audiomonitor_.peakSignal().connect(sigc::mem_fun(*this, 
 						   &AudioMonGui::on_peak_signal)); 

}

AudioMonGui::~AudioMonGui() 
{


}


void AudioMonGui::on_source_changed()
{
  Gtk::TreeModel::iterator iter = sourceCombo_.get_active(); 
  if(iter)
    {
      Gtk::TreeModel::Row row = *iter;
      if(row)
	{
	  //Get the data for the selected row, using our knowledge of the tree
	  //model:
	  int id = row[source_cols_.m_col_id];
	  // set it 
	  std::cout << "setting source to " << id << std::endl; 
	  audiomonitor_.setSource(id); 
	  
	}
    }
  else
    std::cout << "invalid iter" << std::endl;
  
}

void AudioMonGui::on_channel_button_clicked()
{
    
  int i = 0; 
  if(channel1_.get_active()) {
    i = 0; 
  } else if (channel2_.get_active()) {
    i = 1; 
  } else if (channel3_.get_active()) {
    i = 2; 
  } else if (channel4_.get_active()) {
    i = 3; 
  } else if (channelC_.get_active()) {
    i = 4; 
  }
  
  audiomonitor_.setChannel(i);
 
  AML_(info) << "AudioMonGui: Requesting setting of channel to " 
	     << i << std::endl; 
  
}

void AudioMonGui::on_play_button_clicked()
{
  if(playbutton_.get_active()) {
    audiomonitor_.play(); 
  } else {
    audiomonitor_.pause();
  } 

}

void AudioMonGui::on_local_volume_value_changed()
{
  double val = local_volume_.get_value() / 100.0;
  audiomonitor_.setVolume(val); 
  
}

void AudioMonGui::on_remote_volume_value_changed()
{
  int val = remote_volume_.get_value(); 
  AML_(info) << "AudioMonGui:: setting remote volume to " 
	     << val << std::endl; 

  audiomonitor_.setVolumeScaling(val); 
  
}

void AudioMonGui::on_peak_signal(float x)
{
  levelbar_.set_fraction(x); 
  
}
