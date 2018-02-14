// tt_dialog.h

#pragma once

#include "tt_form.h"


// -- TtDialog -----------------------------------------------------------
class TtDialog : public TtForm {
protected:
  static BOOL CALLBACK WindowProcedureForTTLDialog( HWND handle, UINT msg, WPARAM w_param, LPARAM l_param );

protected:
  virtual DWORD  GetStyle( void ) override;
  virtual DWORD  GetExtendedStyle( void ) override;
  virtual bool CreatedInternal( void ) override;

public:
  explicit TtDialog( void );
  virtual ~TtDialog( void );

  virtual void CreatePrimitive( CreateParameter& parameter ) override;

  virtual int ShowDialog( TtForm& parent );
  virtual void EndDialog( int result );

  int GetResult( void );

protected:
  void*   template_object_;
  int     dialog_result_;
};


// -- TtDialogModeless ---------------------------------------------------
class TtDialogModeless : public TtDialog {
public:
  explicit TtDialogModeless( void );

  virtual ~TtDialogModeless();

  virtual int ShowDialog( TtForm& parent ) override;
  int ShowDialog( void );
  int ShowDialogPrimitive( HINSTANCE instance_handle, HWND parent );

  virtual void EndDialog( int result ) override;
};
