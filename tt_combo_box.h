// tt_combo_box.h

#pragma once

#include <unordered_map>

#include "tt_window.h"


// -- TtComboBox ---------------------------------------------------------
class TtComboBox : public BasedOnTtWindow<WS_TABSTOP, WS_EX_CLIENTEDGE, TtWindowClassName::ComboBox> {
public:
  int GetCount( void );
  unsigned int GetTextLengthAt( unsigned int index );
  std::string GetTextAt( unsigned int index );
  int Find( unsigned int start, const std::string& str );

  int Push( const std::string& str );
  int InsertAt( unsigned int index, const std::string& str );
  int DeleteAt( unsigned int index );
  void Clear( void );

  int GetSelectedIndex( void );
  std::string GetSelectedString( void );
  int SetSelect( unsigned int index );
  int SetSelectByString( const std::string& str );
};

// -- TtComboBoxDropDownWithEdit -----------------------------------------
using TtComboBoxDropDownWithEdit = TtWindowWithStyle<TtComboBox, CBS_DROPDOWN>;

// -- TtComboBoxDropDownNotEdit ------------------------------------------
using TtComboBoxDropDownNotEdit = TtWindowWithStyle<TtComboBox, CBS_DROPDOWNLIST>;

// -- TtComboBoxTable ----------------------------------------------------
template <class ComboBox, class Value>
class TtComboBoxTable : public ComboBox {
public:
  Value GetItemData( unsigned int index ) {
    return (Value)( this->SendMessage( CB_GETITEMDATA, index ) );
  }

  Value GetSelectedItemData( void ) {
    return this->GetItemData( this->GetSelectedIndex() );
  };

  void SetItemData( unsigned int index, Value data ) {
    this->SendMessage( CB_SETITEMDATA, index, ((LPARAM)( data )) );
    table_[data] = this->GetTextAt( index );
    this->SetAlias( data, data );
  }

  void PushWithData( const std::string& str, Value data ) {
    this->Push( str );
    this->SetItemData( this->GetCount() - 1, data );
    table_[data] = str;
  }

  void SetSelectByItemData( Value data ) {
    this->SetSelectByString( table_[alias_table_[data]] );
  }

  void SetAlias( Value other, Value real ) {
    alias_table_[other] = real;
  }

private:
  std::unordered_map<Value, std::string> table_;
  std::unordered_map<Value, Value>       alias_table_;
};

// -- TtComboBoxTableDropDownWithEdit ------------------------------------
template <class V>
using TtComboBoxTableDropDownWithEdit = TtComboBoxTable<TtComboBoxDropDownWithEdit, V>;

// -- TtComboBoxTableDropDownNotEdit -------------------------------------
template <class V>
using TtComboBoxTableDropDownNotEdit = TtComboBoxTable<TtComboBoxDropDownNotEdit, V>;
