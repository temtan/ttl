// tt_track.h

#pragma once

#include "tt_window.h"


// -- TtTrack ------------------------------------------------------------
class TtTrack : public BasedOnTtWindow<TBS_AUTOTICKS, 0, TtWindowClassName::TrackBar> {
public:
  int GetTrackPosition( void );
  void SetTrackPosition( int position );

  int  GetMinimum( void );
  void SetMinimum( int min );
  int  GetMaximum( void );
  void SetMaximum( int max );
  void SetRange( int min, int max );

  unsigned int GetPageSize( void );
  void SetPageSize( unsigned int page_size );

  unsigned int GetLineSize( void );
  void SetLineSize( unsigned int line_size );

  void SetTickFrequency( unsigned int frequency );
  void ClearTick( void );
};

// -- TtTrackVertical ----------------------------------------------------
using TtTrackVertical = TtWindowWithStyle<TtTrack, TBS_VERT>;

// -- TtTrackHorizontal --------------------------------------------------
using TtTrackHorizontal = TtWindowWithStyle<TtTrack, TBS_HORZ>;
