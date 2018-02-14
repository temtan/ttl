// tt_tab.cpp

#include "ttl_define.h"
#include "tt_exception.h"

#include "tt_tab.h"


// -- TtTab --------------------------------------------------------------
bool
TtTab::CreatedInternal( void )
{
  this->OverrideWindowProcedureByTTL();

  this->RegisterSingleHandler( WM_SIZE, [this] ( WPARAM, LPARAM l_param ) -> WMResult {
    RECT rect = {0, 0, LOWORD( l_param ), HIWORD( l_param )};
    this->SendMessage( TCM_ADJUSTRECT, 0, reinterpret_cast<LPARAM>( &rect ) );
    for ( auto& panel : panels_ ) {
      panel->SetRectangle( rect );
    }
    return {WMResult::Incomplete};
  }, false );
  return true;
}


int
TtTab::GetCount( void )
{
  return this->SendMessage( TCM_GETITEMCOUNT );
}


void
TtTab::InsertNewTab( int index, const std::string& title )
{
  TCITEM item;
  item.mask = TCIF_TEXT;
  item.pszText = const_cast<char*>( title.c_str() );
  if ( TabCtrl_InsertItem( handle_, index, &item ) == -1 ) {
    throw TT_WIN_SYSTEM_CALL_EXCEPTION( REAL_FUNC_NAME_OF( TabCtrl_InsertItem ) );
  }
}

void
TtTab::RegisterPanel( int index, TtPanel* panel )
{
  panel->Create( TtWindow::CreateParameter( this ) );
  RECT rect = {0, 0, this->GetWidth(), this->GetHeight()};
  this->SendMessage( TCM_ADJUSTRECT, 0, reinterpret_cast<LPARAM>( &rect ) );
  panel->SetRectangle( rect );
  panels_.insert( panels_.begin() + index, std::unique_ptr<TtPanel>( panel ) );
  if ( this->GetSelectedIndex() == index ) {
    panels_[index]->Show();
  }
}

TtPanel&
TtTab::GetPanelAt( int index )
{
  return *(panels_[index].get());
}


void
TtTab::DeleteAt( int index )
{
  this->SendMessage( TCM_DELETEITEM, index );
  panels_.erase( panels_.begin() + index );
}


void
TtTab::Clear( void )
{
  this->SendMessage( TCM_DELETEALLITEMS );
  panels_.clear();
}


int
TtTab::GetSelectedIndex( void )
{
  return TabCtrl_GetCurSel( handle_ );
}

void
TtTab::Select( int index )
{
  if ( static_cast<unsigned int>( index ) < panels_.size() ) {
    int tmp = this->SendMessage( TCM_SETCURSEL, index );
    panels_[tmp]->Hide();
    panels_[index]->Show();
  }
}


void
TtTab::RegisterTabChangeHandlersTo( TtWindow& parent )
{
  parent.RegisterSingleHandler( WM_NOTIFY, [this, self_id = this->GetWindowLong( GWL_ID )]( WPARAM w_param, LPARAM l_param ) -> WMResult {
    int id = w_param;
    if ( id == self_id ) {
      switch ( reinterpret_cast<NMHDR*>( l_param )->code ) {
      case TCN_SELCHANGING:
        last_selected_index_ = this->GetSelectedIndex();
        return {WMResult::Done};

      case TCN_SELCHANGE:
        this->StopRedraw( [this] ( void ) {
          panels_[this->GetSelectedIndex()]->Show();
          panels_[last_selected_index_]->Hide();
        } );
        return {WMResult::Done};
      }
    }
    return {WMResult::Incomplete};
  }, true );
}
