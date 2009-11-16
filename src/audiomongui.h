#ifndef __AUDIOMONITOR_GUI_H__
#define __AUDIOMONITOR_GUI_H__

#include <gtkmm.h>
#include <gtkmm/comboboxtext.h>

#include "audiomon.h"

class AudioMonGui : public Gtk::Window
{
public:
  explicit AudioMonGui(AudioMonitor & am); 
  virtual ~AudioMonGui(); 

private:
  AudioMonitor & audiomonitor_; 
  
  Gtk::ComboBoxText sourceCombo_;
  
  Gtk::VBox vbox_; 
  Gtk::HBox hbox_; 
  
  
  Gtk::Frame sourceframe_; 
  
  Gtk::Statusbar statusbar_; 

  Gtk::RadioButton::Group channelgroup_; 
  Gtk::RadioButton channel1_; 
  Gtk::RadioButton channel2_; 
  Gtk::RadioButton channel3_; 
  Gtk::RadioButton channel4_; 
  Gtk::RadioButton channelC_; 

  Gtk::VBox channelvbox_; 

  Gtk::VBox lefthbox_; 

  Gtk::HBox sourceHBox_; 
  
  Gtk::ToggleButton playbutton_; 

  Gtk::HScale volume_; 
  
  Gtk::Label sourcelabel_; 

  Gtk::HBox volhbox_; 

  class SourceModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    SourceModelColumns()
    { add(m_col_id), 
	add(m_col_name); }
    
    Gtk::TreeModelColumn<int> m_col_id;
    Gtk::TreeModelColumn<Glib::ustring> m_col_name;
  };

  SourceModelColumns source_cols_;

  void on_source_changed(); 
  
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;

  void on_channel_button_clicked();
  void on_play_button_clicked(); 

  void on_volume_value_changed(); 
  
};






#endif 
