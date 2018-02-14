// tt_window_utility.h

#pragma once

#include <unordered_map>

#include "tt_image_list.h"
#include "tt_window.h"
#include "tt_window_controls.h"


// -- TtValueToRadioCheck ------------------------------------------------
template <class TYPE>
class TtValueToRadioCheck {
public:
  explicit TtValueToRadioCheck( TtRadioButton& default_radio ) :
  table_(), default_radio_( default_radio ) {}

  void Register( TYPE value, TtRadioButton& radio ) {
    table_[value] = &radio;
  }

  void SetCheck( TYPE value ) {
    if ( table_.find( value ) != table_.end() ) {
      table_[value]->SetCheck( true );
    }
    else {
      default_radio_.SetCheck( true );
    }
  }

private:
  std::unordered_map<TYPE, TtRadioButton*> table_;
  TtRadioButton&                           default_radio_;
};


// -- TtRadioCheckToValue ------------------------------------------------
template <class TYPE>
class TtRadioCheckToValue {
public:
  explicit TtRadioCheckToValue( TYPE default_value ) :
  table_(), default_value_( default_value ) {}

  void Register( TtRadioButton& radio, TYPE value ) {
    table_[&radio] = value;
  }

  TYPE GetValue( void ) const {
    for ( auto& it : table_ ) {
      if ( it.first->GetCheck() ) {
        return it.second;
      }
    }
    return default_value_;
  }

private:
  std::unordered_map<TtRadioButton*, TYPE> table_;
  TYPE                                     default_value_;
};


// -- TtDragHandler ------------------------------------------------------
class TtDragHandler {
private:
  static void DestroyImageList( HIMAGELIST handle );

public:
  explicit TtDragHandler( void );
  explicit TtDragHandler( HIMAGELIST handle );

  bool IsBegun( void );
  bool IsEntered( void );

  void Begin( int index, int x, int y );
  void Enter( int x, int y );
  void Enter( TtWindow& window, int x, int y );
  void Move( int x, int y );
  void ReEnter( int x, int y, std::function<void ( void )> function );
  void Leave( void );
  void End( void );

private:
  void InternalEnter( HWND handle, int x, int y );

  using ImageListHandler = std::shared_ptr<std::remove_pointer<HIMAGELIST>::type>;

  ImageListHandler image_list_handler_;
  HWND             last_entered_;
  bool             is_begun_;
  bool             is_entered_;
};
