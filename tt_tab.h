// tt_tab.h

#pragma once

#include <vector>

#include "tt_window.h"
#include "tt_panel.h"


// -- TtTab --------------------------------------------------------------
class TtTab : public BasedOnTtWindow<WS_TABSTOP, WS_EX_COMPOSITED, TtWindowClassName::TabControl> {
public:
  virtual bool CreatedInternal( void ) override;

  int GetCount( void );

  template <class TYPE = TtPanel, class... Args>
  TYPE& Append( const std::string& title, Args... args ) {
    return this->Insert<TYPE, Args...>( this->GetCount(), title, args... );
  }

  template <class TYPE = TtPanel, class... Args>
  TYPE& Insert( int index, const std::string& title, Args... args ) {
    this->InsertNewTab( index, title );
    TYPE* panel = new TYPE( args... );
    this->RegisterPanel( index, panel );
    return *panel;
  }

private:
  void InsertNewTab( int index, const std::string& title );
  void RegisterPanel( int index, TtPanel* panel );

public:
  TtPanel& GetPanelAt( int index );

  void DeleteAt( int index );

  void Clear( void );

  int GetSelectedIndex( void );
  void Select( int index );

  void RegisterTabChangeHandlersTo( TtWindow& parent );

private:
  std::vector<std::unique_ptr<TtPanel>> panels_;
  int                                   last_selected_index_;
};
