// tt_menu.h

#pragma once

#include "tt_bmp_image.h"
#include "tt_window.h"
#include "tt_utility.h"


class TtMenu;
class TtSubMenu;

// -- TtMenuItem ---------------------------------------------------------
class TtMenuItem {
  friend TtMenu;
public:
  explicit TtMenuItem( HMENU parent, int index );

  HMENU GetParentHandle( void );
  int GetIndex( void );

  bool GetEnabled( void );
  void SetEnabled( bool flag );

  bool GetCheck( void );
  void SetCheck( bool flag );

  bool GetHilight( void );
  void SetHilight( bool flag );

  void SetText( const std::string& text );
  std::string GetText( void );

  void SetCommandID( int id );
  int GetCommandID( void );

  void SetBmpImage( TtBmpImage& bmp );

  // メニュー全項目をグループにするので注意
  void CheckRadio( void );

  template <class TYPE>
  TYPE GetParameterAs( void ) {
    MENUITEMINFO info = {sizeof( MENUITEMINFO )};
    info.fMask  = MIIM_DATA;
    this->GetInfo( info );
    return reinterpret_cast<TYPE>( info.dwItemData );
  }

  template <class TYPE>
  void SetParameterAs( TYPE data ) {
    MENUITEMINFO info = {sizeof( MENUITEMINFO )};
    info.fMask = MIIM_DATA;
    info.dwItemData = reinterpret_cast<ULONG_PTR>( data );
    this->SetInfo( info );
  }

private:
  void SetInfo( MENUITEMINFO& info );
  void GetInfo( MENUITEMINFO& info );

private:
  HMENU   parent_;
  int     index_;
};

// -- TtMenuItemWith -----------------------------------------------------
template <class TYPE>
class TtMenuItemWith : public TtMenuItem {
public:
  using TtMenuItem::TtMenuItem;
  TtMenuItemWith( TtMenuItem& item ) : TtMenuItem( item ) {}

  TYPE GetParameter( void ) {
    return this->GetParameterAs<TYPE>();
  }
  void SetParameter( TYPE data ) {
    this->SetParameterAs<TYPE>( data );
  }
};


// -- TtMenu -------------------------------------------------------------
class TtMenu : public TtUtility::WindowsHandleHolder<HMENU> {
public:
  explicit TtMenu( HMENU handle, bool auto_destroy );

  int GetItemCount( void );

  TtMenuItem AppendNewItem( int command_id = 0, const std::string& text = ""  );
  TtMenuItem InsertNewItem( int index, int command_id = 0, const std::string& text = "" );

  void AppendMenu( TtSubMenu& sub_menu, const std::string& text = "" );
  void InsertMenu( int index, TtSubMenu& sub_menu, const std::string& text = "" );

  TtMenuItem GetItemAt( int index );
  TtMenuItem GetLastItem( void );
  TtSubMenu  GetSubMenuAt( int index );

  void CheckRadioItem( int index, int group_start = 0, int group_end = -1 );

  void DeleteAt( int index );
  void Clear( void );

  void SetInfoOfMenuCommand( bool flag );
  void SetInfoOfCheckOrBMP( bool flag );
  void SetInfoOfNoCheck( bool flag );

protected:
  void InsertInfo( int index, MENUITEMINFO& info );

  void SetInfo( MENUINFO& info );
  void GetInfo( MENUINFO& info );

  void ChangeInfoOfStyle( DWORD style, bool flag );

};

// -- TtSubMenu ----------------------------------------------------------
class TtSubMenu : public TtMenu {
public:
  static TtSubMenu Create( bool auto_destroy = true );

  explicit TtSubMenu( HMENU handle, bool auto_destroy );

  void AppendSeparator( void );
  void InsertSeparator( int index );

  void PopupAbsolute( TtWindow& parent, int x, int y );
  void PopupRelative( TtWindow& parent, int x, int y );
};

// -- TtSubMenuCommand ---------------------------------------------------
class TtSubMenuCommand : public TtSubMenu {
public:
  static TtSubMenuCommand Create( bool auto_destroy = true );

  explicit TtSubMenuCommand( HMENU handle, bool auto_destroy );
};

// -- TtMenuBar ----------------------------------------------------------
class TtMenuBar : public TtMenu {
public:
  explicit TtMenuBar( bool auto_destroy = true );
  explicit TtMenuBar( HMENU handle, bool auto_destroy );
};


// -- TtSubMenuCommandMaker ----------------------------------------------
class TtSubMenuCommandMaker {
public:
  // -- element classes ----------
  struct Element {
  public:
    virtual ~Element() {};
    template <class TYPE>
    bool TypeIs( void ) const {
      return dynamic_cast<TYPE*>( this ) != nullptr;
    }
    template <class TYPE>
    bool TypeIsNot( void ) const {
      return NOT( this->TypeIs<TYPE>() );
    }
    template <class TYPE>
    TYPE* CastTo( void ) {
      return dynamic_cast<TYPE*>( this );
    }

    bool IsSubMenu( void ) const   { return this->TypeIs<const SubMenu>(); }
    bool IsSeparator( void ) const { return this->TypeIs<const Separator>(); }
    bool IsItem( void ) const      { return this->TypeIs<const Item>(); }
  };

  using ElementVector = std::vector<std::shared_ptr<Element>>;

  struct SubMenu : public Element {
    std::string   name_;
    ElementVector elements_;
  };

  struct Separator : public Element {
  };

  struct Item : public Element {
  public:
    virtual std::string GetName( void ) const = 0;
    virtual void AfterMake( TtMenuItem& item ) = 0;
  };

  explicit TtSubMenuCommandMaker( void );

  ElementVector& GetRoot( void );
  void SetAtMakeMenu( std::function<void ( TtSubMenuCommand& )> func );

  TtSubMenuCommand MakeMenu( void );

private:
  ElementVector                             root_;
  std::function<void ( TtSubMenuCommand& )> at_make_menu_;
};
