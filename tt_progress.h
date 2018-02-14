// tt_progress.h

#pragma once

#include "tt_window.h"


// -- TtProgress ---------------------------------------------------------
class TtProgress : public BasedOnTtWindow<0, 0, TtWindowClassName::Progress> {
public:
  int GetMaximum( void );
  int SetMaximum( unsigned short max );

  int GetAmount( void );
  int SetAmount( unsigned short position );

  int SetStep( int step );
  int StepUp( int amount );
  int StepIt( void );
};

// -- TtProgressSmooth ---------------------------------------------------
using TtProgressSmooth = TtWindowWithStyle<TtProgress, PBS_SMOOTH>;
