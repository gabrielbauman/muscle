/* This file is Copyright 2000-2013 Meyer Sound Laboratories Inc.  See the included LICENSE.txt file for details. */  

#ifndef DebugTimer_h
#define DebugTimer_h

#include "util/Hashtable.h"
#include "util/String.h"

#ifndef MUSCLE_DEBUG_TIMER_CLOCK
# if defined(__BEOS__) || defined(__HAIKU__) || defined(__ATHEOS__) || defined(WIN32) || (defined(MUSCLE_USE_LIBRT) && defined(_POSIX_MONOTONIC_CLOCK)) || (defined(TARGET_PLATFORM_XENOMAI) && !defined(MUSCLE_AVOID_XENOMAI))
#  define MUSCLE_DEBUG_TIMER_CLOCK GetRunTime64()       /**< Function call that DebugTimer should use to get the current time.  Defaults to GetRunTime64() except under OS's where GetRunTime64() has large granularity; for these platforms GetCurrentTime64() is used instead.  May be overridden at compile-time via -DMUSCLE_DEBUG_TIMER_CLOCK=SomeOtherFunc() */
# else
#  define MUSCLE_DEBUG_TIMER_CLOCK GetCurrentTime64()   /**< Function call that DebugTimer should use to get the current time.  Defaults to GetRunTime64() except under OS's where GetRunTime64() has large granularity; for these platforms GetCurrentTime64() is used instead.  May be overridden at compile-time via -DMUSCLE_DEBUG_TIMER_CLOCK=SomeOtherFunc() */
# endif
#endif

namespace muscle {

/** This is a little class that can be helpful for debugging.  It will record the amount of time
 *  spent in various modes, and then when the DebugTimer object goes away, it will Log a message
 *  describing how much time was spent in each mode.
 */
class DebugTimer MUSCLE_FINAL_CLASS
{
public:
   /** Constructor
    *  @param title Title to display in the debug report generated by our constructor.  Defaults to "timer".
    *  @param minLogTime Logging of any timer values less than this value (in microseconds) will be suppressed (so as not to distract you with trivia).  Defaults to 1000 (a.k.a. 1 millisecond)
    *  @param startMode What mode the timer should begin in.  Each mode has its elapsed time recorded separately.  Default is mode zero.
    *  @param debugLevel log level to log at.  Defaults to MUSCLE_LOG_INFO.  If set to a negative number, we'll call printf() instead of LogTime().
    */
   DebugTimer(const String & title = "timer", uint64 minLogTime = 1000, uint32 startMode = 0, int debugLevel = MUSCLE_LOG_INFO);

   /** Destructor.  Prints out a log message with the elapsed time, in milliseconds, spent in each mode. */
   ~DebugTimer();

   /** Set the timer to record elapsed time to a different mode.
    *  @param newMode the mode we switching our timing mechanism to record elapsed time into.  (mode-numbering is arbitrary and up to the caller)
     */
   void SetMode(uint32 newMode);

   /** Returns the currently active mode number */
   uint32 GetMode() const {return _currentMode;}

   /** Convenience method:  Equivalent to GetElapsedTime(GetMode()) */
   uint64 GetElapsedTime() const {return GetElapsedTime(GetMode());}

   /** Returns the amount of elapsed time, in microseconds, that has been spent in the given mode.
    *  Note that if (whichMode) is the currently active mode, the returned value will be growing from moment to moment. 
    *  @param whichMode the mode we are querying the elapsed-time for (mode-numbering is arbitrary and up to the caller)
    */
   uint64 GetElapsedTime(uint32 whichMode) const 
   {
      const uint64 * et = _modeToElapsedTime.Get(whichMode);
      return (et ? *et : 0) + ((whichMode == _currentMode) ? (MUSCLE_DEBUG_TIMER_CLOCK-_startTime) : 0);
   }

   /** Set whether or not the destructor should print results to the system log.  Default is true. 
     * @param e true to enable logging, false to disable it 
     */
   void SetLogEnabled(bool e) {_enableLog = e;}

   /** Returns the state of the print-to-log-enabled, as set by SetLogEnabled() */
   bool IsLogEnabled() const {return _enableLog;}

   /** Set the minimum-log-time value, in microseconds.  Time intervals shorter than this will not be logged.  Defaults to zero.
     * @param lt time threshold, in microseconds
     */
   void SetMinLogTime(uint64 lt) {_minLogTime = lt;}

   /** Returns the current minimum-log-time value, in microseconds. */
   uint64 GetMinLogTime() const {return _minLogTime;}

private:
   uint32 _currentMode;
   uint64 _startTime;    // time at which we entered the current mode
   Hashtable<uint32, uint64> _modeToElapsedTime;

   String _title;
   uint64 _minLogTime;
   int _debugLevel;
   bool _enableLog;
};

/** A macro for quickly declaring a DebugTimer object on the stack.  Usage example:  DECLARE_DEBUGTIMER("hi") */
#ifdef _MSC_VER
# define DECLARE_DEBUGTIMER(...) DECLARE_ANONYMOUS_STACK_OBJECT(DebugTimer, __VA_ARGS__)
#else
# define DECLARE_DEBUGTIMER(args...) DECLARE_ANONYMOUS_STACK_OBJECT(DebugTimer, args)
#endif

} // end namespace muscle

#endif
