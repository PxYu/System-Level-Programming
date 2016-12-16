/****************************************************************************************
 * Copyright (c) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 * File:
 *  ProfilerInfo.h
 *
 * Description:
 *	
 *
 *
 ***************************************************************************************/
#ifndef __PROFILER_INFO_H__
#define __PROFILER_INFO_H__

#define TEST_FILE "result.out"	// used for testing purposes


/***************************************************************************************
 ********************                                               ********************
 ********************              Enum Definitions                 ********************
 ********************                                               ********************
 ***************************************************************************************/
enum CounterAction
{ 
	START, 
	STOP 
};

enum StackAction
{ 
	PUSH, 
	POP 
};

enum JitStatus
{ 
	NONE,
	JITTED, 
	PREJITTED,
};


/***************************************************************************************
 ********************                                               ********************
 ********************             BaseInfo Declaration              ********************
 ********************                                               ********************
 ***************************************************************************************/
class BaseInfo
{
	public:
    
    	BaseInfo( ULONG id );         
        virtual ~BaseInfo();
                
        
	public:
            
    	virtual void Dump();
        
        BOOL Compare( ULONG key );
		Comparison CompareEx( ULONG key );
        
        
 	public:
    
    	ULONG m_id;
        BOOL  m_isValid;
    
}; // BaseInfo


/***************************************************************************************
 ********************                                               ********************
 ********************             StackBaseInfo Declaration         ********************
 ********************                                               ********************
 ***************************************************************************************/
class StackBaseInfo
{
	public:
    
    	StackBaseInfo( ULONG id );         
        virtual ~StackBaseInfo();
                
        
	public:
            
         StackBaseInfo *Clone();
		 void Dump();
        
        
 	public:
    
    	ULONG m_id;

}; // StackBaseInfo


/***************************************************************************************
 ********************                                               ********************
 ********************             StackFunctionInfo Declaration     ********************
 ********************                                               ********************
 ***************************************************************************************/
class StackFunctionInfo
{
	public:
    
    	StackFunctionInfo(); 
        virtual ~StackFunctionInfo();
                
        
	public:
            
	 	void Dump();
        StackFunctionInfo *Clone();
        
        
 	public:
    
        LARGE_INTEGER m_calleeTime;
        LARGE_INTEGER m_profilerTime;
        LARGE_INTEGER m_suspendedTime;
       	LARGE_INTEGER m_inclusiveTime;
		LARGE_INTEGER m_exclusiveTime;
       	LARGE_INTEGER m_currentInclusiveTime;

}; // StackFunctionInfo

 
/***************************************************************************************
 ********************                                               ********************
 ********************      FunctionTimingInfo Declaration           ********************
 ********************                                               ********************
 ***************************************************************************************/
class FunctionTimingInfo :
	public BaseInfo
{
	public:
    
    	FunctionTimingInfo( ULONG id, DWORD win32ThreadID );         
        virtual ~FunctionTimingInfo();
                
        
	public:
            
    	virtual void Dump();
        

 	public:
    
    	DWORD  m_win32ThreadID;
        
		DWORD m_timesCalled;
        LARGE_INTEGER m_calleeTime;
        LARGE_INTEGER m_profilerTime;
    	LARGE_INTEGER m_inclusiveTime;
		LARGE_INTEGER m_exclusiveTime;
        LARGE_INTEGER m_suspendedTime;

		CStack<StackFunctionInfo *> *m_pLatestInvocation;
		
}; // FunctionTimingInfo

 
/***************************************************************************************
 ********************                                               ********************
 ********************            ThreadInfo Declaration             ********************
 ********************                                               ********************
 ***************************************************************************************/
class ThreadInfo :
	public BaseInfo
{
	public:

		ThreadInfo( ThreadID threadID );         
		virtual ~ThreadInfo();
        

	public:
    
    	virtual void Dump();
		void GetStackFunctionInfo( StackFunctionInfo **pFunctionInfo );
       	void UpdateTimer( LARGE_INTEGER *globalTimer, 
        				  LARGE_INTEGER *currentTimer, 
                          CounterAction action );
        
        
	public:	
  	  		
		// 
		// Once a thread dies, it is possible for its ID to be re-used.
		// We remember the original ID in order to print the information
		// for each thread at the end of the run.
		//
		DWORD m_originalTID;
        
        
        // win32 information
        HANDLE m_hThread;
        DWORD  m_win32ThreadID;
		ULONG  m_lastPoppedFID;

		// timers for the thread
    	LARGE_INTEGER m_runningTime;
        LARGE_INTEGER m_profilerTime;
        LARGE_INTEGER m_suspendedTime;
        LARGE_INTEGER m_currentProfilerTime;
   		LARGE_INTEGER m_currentSuspensionTime;

		// stacks and tables
		CStack<StackBaseInfo *> *m_pCallStack;
		CStack<StackBaseInfo *> *m_pLatestUnwoundFunction;
		Table<FunctionTimingInfo *, FunctionID> *m_pFunctionTimes;
		        
}; // ThreadInfo


/***************************************************************************************
 ********************                                               ********************
 ********************          FunctionInfo Declaration             ********************
 ********************                                               ********************
 ***************************************************************************************/
class FunctionInfo :
	public BaseInfo
{
	public:

		FunctionInfo( FunctionID functionID );
   		virtual ~FunctionInfo();
      

	public:
    
    	virtual void Dump();     
        
        
	public:	
  	  	                        	
		ULONG m_argCount;	
		BOOL m_bIsStatic;
		WCHAR m_functionName[MAX_LENGTH];
		WCHAR m_returnTypeStr[MAX_LENGTH];
		WCHAR m_functionParameters[10 * MAX_LENGTH];
        
}; // FunctionInfo


/***************************************************************************************
 ********************                                               ********************
 ********************              PrfInfo Declaration              ********************
 ********************                                               ********************
 ***************************************************************************************/
class PrfInfo
{	         
	friend class TimeTracker;
	friend class FunctionTimingInfo;


    public:
    
        PrfInfo();                     
        virtual ~PrfInfo();                      
                      
                      	
   	public:
                    
		void DumpTables();
		void Failure( char *message = NULL );

    	void AddThread( ThreadID threadID );                        
    	void RemoveThread( ThreadID threadID );

    	void AddFunction( FunctionID functionID );                        
    	void RemoveFunction( FunctionID functionID );
        
   		void UpdateCallStack( FunctionID functionID, StackAction action );
   		void UpdateOSThreadID( ThreadID managedThreadID, DWORD osThreadID );
		void UpdateSuspendedTimer( ThreadID threadID, CounterAction action );
		void UpdateUnwindStack( FunctionID *functionID, StackAction action );


  	private:
    
        void _GetThreadInfoHelper( ThreadInfo **ppThreadInfo );
        void _GetFunctionInfoHelper( FunctionInfo **ppFunctionInfo );


    protected:    

        DWORD m_dwEventMask;
        BOOL m_bHighPrecisionSupported;
        ICorProfilerInfo2 *m_pProfilerInfo;
	 
        SList<ThreadInfo *, ThreadID> *m_pThreadTable;        
        Table<FunctionInfo *, FunctionID> *m_pFunctionTable;        

}; // PrfInfo


/***************************************************************************************
 ********************                                               ********************
 ********************          TimeTracker Declaration              ********************
 ********************                                               ********************
 ***************************************************************************************/
class TimeTracker
{
	public:

		TimeTracker( PrfInfo *pPrfInfo, ThreadID threadID = NULL );
		~TimeTracker();
    

	private:	
  	  	                        	
		ThreadInfo *m_pThreadInfo;
        
}; // TimeTracker


static PrfInfo *g_pPrfInfo;			// global pointer to PrfInfo object
static LARGE_INTEGER g_frequency;	// global high precision counter

#endif // __PROFILER_INFO_H___

// End of File