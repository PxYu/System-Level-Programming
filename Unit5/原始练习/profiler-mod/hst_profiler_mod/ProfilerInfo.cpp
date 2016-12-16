/****************************************************************************************
 * Copyright (c) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 * File:
 *  ProfilerInfo.cpp
 *
 * Description:
 *	
 *
 *
 ***************************************************************************************/
#include "ProfilerInfo.h"


BaseInfo::BaseInfo( ULONG id ) : m_id( id ), m_isValid( TRUE ) {} // ctor         		
BaseInfo::~BaseInfo(){} // dtor

BOOL BaseInfo::Compare( ULONG key ){
    return (BOOL)(m_id == key);
} // BaseInfo::Compare

Comparison BaseInfo::CompareEx( ULONG key ) {
	Comparison result = LESS_THAN;
	if ( key == m_id )
		result = EQUAL_TO;	
	else if ( key > m_id )
		result =  GREATER_THAN;
	return result;
} // BaseInfo::CompareEx

void BaseInfo::Dump( ) {} // BaseInfo::Dump

StackBaseInfo::StackBaseInfo( ULONG id ) : m_id( id ) {} // ctor         		
StackBaseInfo::~StackBaseInfo() {} // dtor

StackBaseInfo *StackBaseInfo::Clone() {
    return new StackBaseInfo( m_id );
} // StackBaseInfo::Clone

void StackBaseInfo::Dump() {} // StackBaseInfo::Dump

StackFunctionInfo::StackFunctionInfo() {
    m_calleeTime.QuadPart = 0;
    m_profilerTime.QuadPart = 0;
    m_inclusiveTime.QuadPart = 0;
	m_exclusiveTime.QuadPart = 0;
    m_suspendedTime.QuadPart = 0;
	m_currentInclusiveTime.QuadPart = 0;
	
} // ctor         		

StackFunctionInfo::~StackFunctionInfo() {} // dtor

StackFunctionInfo *StackFunctionInfo::Clone() {
    StackFunctionInfo *pFunctionInfo;
        
   	pFunctionInfo = new StackFunctionInfo();
    if ( pFunctionInfo != NULL )
    {
	    pFunctionInfo->m_calleeTime.QuadPart = m_calleeTime.QuadPart;
	    pFunctionInfo->m_profilerTime.QuadPart = m_profilerTime.QuadPart;
	    pFunctionInfo->m_suspendedTime.QuadPart = m_suspendedTime.QuadPart;
	    pFunctionInfo->m_inclusiveTime.QuadPart = m_inclusiveTime.QuadPart;
	    pFunctionInfo->m_exclusiveTime.QuadPart = m_exclusiveTime.QuadPart;
		pFunctionInfo->m_currentInclusiveTime.QuadPart = m_currentInclusiveTime.QuadPart;
  	}
	return pFunctionInfo;
} // StackFunctionInfo::Clone

void StackFunctionInfo::Dump() {} // StackFunctionInfo::Dump

FunctionTimingInfo::FunctionTimingInfo( ULONG id, DWORD win32ThreadID ) : 
	BaseInfo( id ),
	m_timesCalled( 0 ),
    m_pLatestInvocation( NULL ),
    m_win32ThreadID( win32ThreadID ) {

    m_calleeTime.QuadPart = 0;
    m_profilerTime.QuadPart = 0;
    m_suspendedTime.QuadPart = 0;
    m_inclusiveTime.QuadPart = 0;
    m_exclusiveTime.QuadPart = 0;

    // Initialize stack of invocated functions
	m_pLatestInvocation = new CStack<StackFunctionInfo *>();
} // ctor         		

FunctionTimingInfo::~FunctionTimingInfo() {
    if ( m_pLatestInvocation != NULL ) {
    	delete m_pLatestInvocation; 
		m_pLatestInvocation = NULL;
	}
} // dtor

void FunctionTimingInfo::Dump() {    	 
	HRESULT	hr;
	FunctionInfo *pFunctionInfo = NULL;
	
	pFunctionInfo = (g_pPrfInfo->m_pFunctionTable)->Lookup( m_id );
	if ( pFunctionInfo != NULL )
	{
	    // 
	    // The intention is to dump the data into a semi-colon delimited list. The
	    // data can then be imported into a spreadsheet and then analysed. The format
	    // is essentially comprised of 
	    //
	    //		thread ID
	    //      function name and parameters
	    //      times called
	    //      exclusive time
		//		callee exclusive time
	    //      inclusive time
	    //      callee time
	    // 		suspended time
	    //      profiler time
	    //
    	
    	//
		// Dump thread ID, return type, function name, and function 
        // parameters; note, function parameters are separated by a 
        // '+' sign.
		//
		LOG_TO_FILE( ("0x%08x;", m_win32ThreadID) )        
		if ( pFunctionInfo->m_bIsStatic == TRUE )
			LOG_TO_FILE( ("static ") )


		if ( pFunctionInfo->m_returnTypeStr[0] != NULL )
        	LOG_TO_FILE( ("%S ", pFunctionInfo->m_returnTypeStr) )		        		
            
            
       if ( pFunctionInfo->m_functionName[0] != NULL )
        	LOG_TO_FILE( ("%S(", pFunctionInfo->m_functionName) )
            
            
		if ( pFunctionInfo->m_argCount > 0 ) {
			WCHAR *parameter;
            WCHAR *separator = L"+";

			// Parse and dump parameters
            parameter = wcstok( pFunctionInfo->m_functionParameters, separator );
		   	while ( parameter != NULL )	{
		     	LOG_TO_FILE( (" %S", parameter) ) 		      
		      	parameter = wcstok( NULL, separator );
                if ( parameter != NULL )
                	LOG_TO_FILE( (",") )
   		  	} // while				               
		}
        LOG_TO_FILE( (" );") )
        
        // Dump statistics
        double exclusiveTime;
        
        // to compute exclusive time for the function we subtract the callee time, 
        // suspended time, and profiler time from the inclusive time
        exclusiveTime = ( ((double)m_inclusiveTime.QuadPart / (double)g_frequency.QuadPart) -                
	                    	( ((double)m_calleeTime.QuadPart / (double)g_frequency.QuadPart)	+
	                    	  ((double)m_suspendedTime.QuadPart / (double)g_frequency.QuadPart) +
	                    	  ((double)m_profilerTime.QuadPart / (double)g_frequency.QuadPart) ) );
                        
        LOG_TO_FILE( ("%d;%f;%f;%f;%f;%f;%f\n", 
	                   m_timesCalled,
                       exclusiveTime,
					   ((double)m_exclusiveTime.QuadPart / (double)g_frequency.QuadPart),
	                   ((double)m_inclusiveTime.QuadPart / (double)g_frequency.QuadPart),
	                   ((double)m_calleeTime.QuadPart / (double)g_frequency.QuadPart),
	                   ((double)m_suspendedTime.QuadPart / (double)g_frequency.QuadPart),
	                   ((double)m_profilerTime.QuadPart / (double)g_frequency.QuadPart) ));
	}

} // FunctionTimingInfo::Dump

ThreadInfo::ThreadInfo( ThreadID threadID ) : 
	BaseInfo( threadID ),		
	m_hThread( NULL ), 
	m_win32ThreadID( 0 ),
    m_pCallStack( NULL ),
    m_pFunctionTimes( NULL ),    
	m_originalTID( threadID ),
	m_lastPoppedFID( 0xFFFFFFFF ),
    m_pLatestUnwoundFunction( NULL ) {

	m_runningTime.QuadPart = 0;
  	m_profilerTime.QuadPart = 0;
	m_suspendedTime.QuadPart = 0;
	m_currentProfilerTime.QuadPart = 0;
	m_currentSuspensionTime.QuadPart = 0; 

	// Initialize the stacks and tables
	m_pCallStack = new CStack<StackBaseInfo *>();
   	m_pLatestUnwoundFunction = new CStack<StackBaseInfo *>();
    m_pFunctionTimes = new Table<FunctionTimingInfo *, FunctionID>();
} // ctor         		

ThreadInfo::~ThreadInfo() {
    if ( m_pCallStack != NULL ) {
    	delete m_pCallStack; 
		m_pCallStack = NULL;
	}

    if ( m_pLatestUnwoundFunction != NULL )	{
    	delete m_pLatestUnwoundFunction; 
		m_pLatestUnwoundFunction = NULL;
	}

    if ( m_pFunctionTimes != NULL ) {
    	delete m_pFunctionTimes; 
		m_pFunctionTimes = NULL;
	}
    
} // dtor
        
void ThreadInfo::Dump() {	
	//
	// The EE is silently turning off the profiling events after the 
    // shutdown callback. This behavior causes the profiler not to 
    // receive any ThreadDestroyed or RuntimeThreadResumed events for 
    // certain threads. To ensure that we get the best possible timings, 
    // we need to update the counters (timers) assuming that the Shutdown 
    // callback is made at the same time that both the RuntimethreadResumed 
    // and ThreadDestroyed callbacks are made.
	//
	// Notice that we do not attemp to update the counters of the functions 
    // in the current thread's callstack. The reason for this is to ensure 
    // that that there are no frames in the callstack when the thread goes 
    // away.
	//
	if ( m_isValid == TRUE )
		UpdateTimer( NULL, &m_runningTime, STOP );
	
    
	if ( m_currentSuspensionTime.QuadPart != 0 )
		UpdateTimer( &m_suspendedTime, &m_currentSuspensionTime, STOP );

	// Print out information
	m_pFunctionTimes->Dump();
	LOG_TO_FILE( ("\n\n") )	

} // ThreadInfo::Dump

void ThreadInfo::UpdateTimer( LARGE_INTEGER *globalTimer, 
							  LARGE_INTEGER *currentTimer, 
                              CounterAction action ) {	
	// This function is multi-purpose:
    //
	//		if action == START, capture and return the current value of the timer. 
	//		if action == STOP,  capture the current value of the timer, subtract 
    //						    it from the value of currentTimer and return the 
    //						    difference; it also accumulates the difference in 
    //							globalTimer.
	//
	switch ( action )
	{
		case START:
			QueryPerformanceCounter( currentTimer );
			break;
		
		case STOP:
			{
				LARGE_INTEGER endTime;
				

				endTime.QuadPart = 0;
				QueryPerformanceCounter( &endTime );                
				(*currentTimer).QuadPart = (endTime.QuadPart - (*currentTimer).QuadPart);
                
				if ( globalTimer != NULL )
					(*globalTimer).QuadPart += (*currentTimer).QuadPart;
			}
			break;
	} // switch
} // ThreadInfo::UpdateTimer

void ThreadInfo::GetStackFunctionInfo( StackFunctionInfo **pFunctionInfo ) {	
	StackBaseInfo *pStackTop;

	// For every managed thread that gets created during the profiling process, 
	// the profiler creates an entry to the thread table. Every entry in the 
    // thread table contains a list of functions that the managed thread has 
    // invoked during its life. Every entry of that list contains some counters 
    // for that function and a stack to track the counters from the last invocation 
    // of that specific function. This is done to handle recursive or multiple 
    // calls of the function. The maximum depth of that stack is equal to the 
    // maximum recursive depth that was encountered for the specific function, 
    // on the specific thread.  
	//	
    *pFunctionInfo = NULL;
	pStackTop = m_pCallStack->Top();
	if ( pStackTop != NULL ) {
		FunctionTimingInfo *pCurrentFunction;
                
        pCurrentFunction = m_pFunctionTimes->Lookup( pStackTop->m_id );	
		if ( pCurrentFunction != NULL )
			*pFunctionInfo = (pCurrentFunction->m_pLatestInvocation)->Top();
	}
} // ThreadInfo::GetStackFunctionInfo

FunctionInfo::FunctionInfo( FunctionID functionID ) : 
    BaseInfo( functionID ),    	
	m_argCount( 0 ),
	m_bIsStatic( FALSE ) {
   	wcscpy( m_functionName, L"unknown" );
   	wcscpy( m_returnTypeStr, L"void" );
	wcscpy( m_functionParameters, L"void" );
} // ctor         		

FunctionInfo::~FunctionInfo() {} // dtor
        
void FunctionInfo::Dump() {} // FunctionInfo::Dump

PrfInfo::PrfInfo() :         
	m_dwEventMask( 0 ),
    m_pThreadTable( NULL ),
    m_pProfilerInfo( NULL ),
    m_bHighPrecisionSupported( FALSE )	 {

	// Initialize critical section and thread table
    m_pThreadTable = new SList<ThreadInfo *, ThreadID>();
	m_pFunctionTable = new Table<FunctionInfo *, FunctionID>();	
	
	// Determine if high precision counters are supported
	g_frequency.QuadPart = 0;
	m_bHighPrecisionSupported = QueryPerformanceFrequency( &g_frequency );	

    // Initialize global PrfInfo pointer
	g_pPrfInfo = this;
} // ctor


PrfInfo::~PrfInfo() {
    if ( m_pProfilerInfo != NULL ) {
    	m_pProfilerInfo->Release();
		m_pProfilerInfo = NULL;
	}

    if ( m_pThreadTable != NULL ) {    
    	delete m_pThreadTable;
		m_pThreadTable = NULL;
	}
    
    if ( m_pFunctionTable != NULL ) {    
    	delete m_pFunctionTable;
		m_pFunctionTable = NULL;
	}

	g_pPrfInfo = NULL;
} // dtor 				 

/* throws BaseException */
void PrfInfo::AddThread( ThreadID threadID )
{    
	ThreadInfo *pThreadInfo;
	LARGE_INTEGER localTimer;
	  
	// For accuracy reasons, this function starts the profiler 
	// timer as well for the ThreadCreated callback
    localTimer.QuadPart = 0;
	QueryPerformanceCounter( &localTimer );
    pThreadInfo = new ThreadInfo( threadID );
	if ( pThreadInfo != NULL ) {
    	try {
    		_GetThreadInfoHelper( &pThreadInfo );
        	m_pThreadTable->AddEntry( pThreadInfo, threadID );
			(pThreadInfo->m_runningTime).QuadPart = localTimer.QuadPart;
			(pThreadInfo->m_currentProfilerTime).QuadPart = localTimer.QuadPart;
			
			QueryPerformanceCounter( &localTimer );
			(pThreadInfo->m_profilerTime).QuadPart += localTimer.QuadPart - (pThreadInfo->m_currentProfilerTime).QuadPart;
			(pThreadInfo->m_currentProfilerTime).QuadPart = 0;
       	}
        catch ( BaseException *exception ) {
        	delete pThreadInfo;
            throw;    		     
     	}                    
    }
    else
    	_THROW_EXCEPTION( "Allocation for ThreadInfo Object FAILED" )
		        
} // PrfInfo::AddThread

/* throws BaseException */
void PrfInfo::RemoveThread( ThreadID threadID ) {    
	if ( threadID != NULL ) {
    	ThreadInfo *pThreadInfo;
    	
		pThreadInfo = m_pThreadTable->Lookup( threadID );
		if ( pThreadInfo != NULL ) {
	    	_ASSERT_( pThreadInfo->m_isValid == TRUE );
	    	pThreadInfo->m_id = 0xbadf00d;
	    	pThreadInfo->m_isValid = FALSE;
			pThreadInfo->UpdateTimer( NULL, &(pThreadInfo->m_runningTime), STOP );
			
			//
			// Update the profiler timer for the thread that dies
			// and reset the m_currentProfilerTime
			//
			pThreadInfo->UpdateTimer( &(pThreadInfo->m_profilerTime), &(pThreadInfo->m_currentProfilerTime), STOP );
			(pThreadInfo->m_currentProfilerTime).QuadPart = 0;
		}
		else
	    	_THROW_EXCEPTION( "Thread was NOT Found in the Thread Table" )
	}
	else
    	_THROW_EXCEPTION( "ThreadID is NULL" )
		                
} // PrfInfo::RemoveThread

/* throws BaseException */
void PrfInfo::AddFunction( FunctionID functionID ) {    
	FunctionInfo *pFunctionInfo;
	
    pFunctionInfo = new FunctionInfo( functionID );
	if ( pFunctionInfo != NULL ) {
    	try {
    		_GetFunctionInfoHelper( &pFunctionInfo );
        	m_pFunctionTable->AddEntry( pFunctionInfo, functionID );
       	}
        catch ( BaseException *exception ) {
        	delete pFunctionInfo;
            throw;    		     
     	}                    
    }
    else
    	_THROW_EXCEPTION( "Allocation for FunctionInfo Object FAILED" )

} // PrfInfo::AddFunction

/* throws BaseException */
void PrfInfo::RemoveFunction( FunctionID functionID ) {    
	if ( functionID != NULL ) {
    	FunctionInfo *pFunctionInfo;
    	
		pFunctionInfo = m_pFunctionTable->Lookup( functionID );
		if ( pFunctionInfo != NULL ) {
	    	_ASSERT_( pFunctionInfo->m_isValid == TRUE );
	    	pFunctionInfo->m_id = 0xbadf00d;
	    	pFunctionInfo->m_isValid = FALSE;
		} else
	    	_THROW_EXCEPTION( "Function was NOT Found in the Function Table" )
	} else
    	_THROW_EXCEPTION( "FunctionID is NULL" )
		                
} // PrfInfo::RemoveFunction

/* throws BaseException */
void PrfInfo::UpdateSuspendedTimer( ThreadID threadID, CounterAction action ) {    
	if ( threadID != NULL ) {
    	ThreadInfo *pThreadInfo;
    	
		pThreadInfo = m_pThreadTable->Lookup( threadID );
		if ( pThreadInfo != NULL ) 		{
			_ASSERT_( pThreadInfo->m_isValid == TRUE );
			switch ( action )
			{
				case START:
					pThreadInfo->UpdateTimer( NULL, &(pThreadInfo->m_currentSuspensionTime), START );
					break;


				case STOP:
					{
						//
						// Lookup and update the suspend time for the function in the top of the stack
						//
						StackFunctionInfo *pFunctionInfo = NULL;


						pThreadInfo->UpdateTimer( &(pThreadInfo->m_suspendedTime),
									  			  &(pThreadInfo->m_currentSuspensionTime),
									  			  STOP );
						
						pThreadInfo->GetStackFunctionInfo( &pFunctionInfo );
						if ( pFunctionInfo != NULL )
							(pFunctionInfo->m_suspendedTime).QuadPart += (pThreadInfo->m_currentSuspensionTime).QuadPart;


						(pThreadInfo->m_currentSuspensionTime).QuadPart = 0; 
					}
					break;
		
			} // switch
		} else
	    	_THROW_EXCEPTION( "Thread was NOT Found in the Thread Table" )
	} else
    	_THROW_EXCEPTION( "ThreadID is NULL" )
        
} // PrfInfo::UpdateSuspendedTimer

/* throws BaseException */
void PrfInfo::UpdateOSThreadID( ThreadID managedThreadID, DWORD win32ThreadID ) {
	if ( managedThreadID != NULL )	{
	  	ThreadInfo *pThreadInfo;
		
		pThreadInfo = m_pThreadTable->Lookup( managedThreadID );
		if ( pThreadInfo != NULL ) {
	    	_ASSERT_( pThreadInfo->m_isValid == TRUE );
			pThreadInfo->m_win32ThreadID = win32ThreadID;
		} else
	    	_THROW_EXCEPTION( "Thread was NOT Found in the Thread Table" )
	} else
    	_THROW_EXCEPTION( "ThreadID is NULL" )          
          
} // PrfInfo::UpdateOSThreadID

/* throws BaseException */
void PrfInfo::UpdateUnwindStack( FunctionID *functionID, StackAction action ) {
	HRESULT hr;
	ThreadID threadID;

	hr = m_pProfilerInfo->GetCurrentThreadID( &threadID );
	if ( SUCCEEDED( hr ) ) 	{
		ThreadInfo *pThreadInfo;
        
		pThreadInfo =  m_pThreadTable->Lookup( threadID );
		if ( pThreadInfo != NULL ) {
			StackBaseInfo *stackElement = NULL;
			
	    	_ASSERT_( pThreadInfo->m_isValid == TRUE );
			switch ( action )
			{
				case PUSH:
					{
						stackElement = new StackBaseInfo( *functionID );
                        if ( stackElement != NULL )
							(pThreadInfo->m_pLatestUnwoundFunction)->Push( stackElement );
                            
                      	else
                        	_THROW_EXCEPTION( "Allocation for Stack Element FAILED" )
					}
					break;


				case POP:
					{
						stackElement = (pThreadInfo->m_pLatestUnwoundFunction)->Pop();
						if ( stackElement != NULL )
						{
							*functionID = stackElement->m_id;
							delete stackElement;
						}
						else
							_THROW_EXCEPTION( "Stack Contains Bad Data" )
					}
					break;

			} // switch
		} else 				
			_THROW_EXCEPTION( "Thread Structure was not found in the thread list" )
	} else
		_THROW_EXCEPTION( "ICorProfilerInfo::GetCurrentThreadID() FAILED" )
    
} // PrfInfo::UpdateUnwindStack

/* throws BaseException */
void PrfInfo::UpdateCallStack( FunctionID functionID, StackAction action ) {
	HRESULT hr;
	ThreadID threadID;

	hr = m_pProfilerInfo->GetCurrentThreadID( &threadID );
	if ( SUCCEEDED( hr ) )	{
		ThreadInfo *pThreadInfo;
        
        pThreadInfo = m_pThreadTable->Lookup( threadID );
		if ( pThreadInfo != NULL ) {
			StackBaseInfo *stackElement = NULL;
			
			switch ( action )
			{
				case PUSH:
					{
						//
						// Push the element in the callstack of the specific thread
						//
						stackElement = new StackBaseInfo( functionID );
						if ( stackElement != NULL )
							(pThreadInfo->m_pCallStack)->Push( stackElement );

						else
							_THROW_EXCEPTION( "Allocation for StackBaseInfo FAILED" )

						// Add the functionID to the table of function that this thread invoked
						// if it does not already exist there
						FunctionTimingInfo *pFunction;
                        
                        pFunction = (pThreadInfo->m_pFunctionTimes)->Lookup( functionID );
						if ( pFunction == NULL ) {
							// The thread executes this method for the fisrt time
							pFunction = new FunctionTimingInfo( functionID, pThreadInfo->m_win32ThreadID );
							if ( pFunction == NULL )
								_THROW_EXCEPTION( "Allocation for FunctionTimingInfo FAILED" )

							(pThreadInfo->m_pFunctionTimes)->AddEntry( pFunction, functionID );
						}
						pFunction->m_timesCalled++;

						// Create a new stack entry and push it on the stack for the specific function
						StackFunctionInfo *pFunctionInfo;
                        
                        pFunctionInfo = new StackFunctionInfo();
						if ( pFunctionInfo != NULL ) {

							// Push it on the stack and start the inclusive time counter
							(pFunction->m_pLatestInvocation)->Push( pFunctionInfo );
							(pFunctionInfo->m_currentInclusiveTime).QuadPart = (pThreadInfo->m_currentProfilerTime).QuadPart;
						} else
							_THROW_EXCEPTION( "Allocation for StackFunctionInfo FAILED" )
					}
					break;
				
                
				case POP:
					{
						//
						// Pop the current element from the callstack
						//
						stackElement = (pThreadInfo->m_pCallStack)->Pop();
						if ( stackElement != NULL )
						{
							//
							// Store away the popped functionID, you will need to update
							// the timers for that ID in the EndProfilingTime
							//
							pThreadInfo->m_lastPoppedFID = stackElement->m_id; 
							delete stackElement;
						}
						else
							_THROW_EXCEPTION( "Stack Contains Bad Data" )
					}
					break;
				
			} // switch
		}
		else 				
			_THROW_EXCEPTION( "Thread Structure was NOT Found in the Thread List" )
	}
	else
		_THROW_EXCEPTION( "ICorProfilerInfo::GetCurrentThreadID() FAILED" )
    
} // PrfInfo::UpdateCallStack

void PrfInfo::DumpTables() {
    // Dump the thread table if you actully performed profiling
	if ( BASEHELPER::FetchEnvironment( LOG_ENVIRONMENT ) != 0xFF /* don't log anything */ ) 	{	   	
	   	if ( (m_pThreadTable != NULL) && (m_dwEventMask != (DWORD)COR_PRF_MONITOR_NONE) ) {
			LOG_TO_FILE( ("Thread ID;Function;Times Called;Exclusive Time;Callee Exclusive Time;Inclusive Time;Callee Time;Suspended Time;Profiler Time\n\n") )
    		m_pThreadTable->Dump();
    	}
   	}
    
#ifdef _TESTING_            
	// For testing purposes we create a result file. The file indicates
    // whether the profiler ran without problem for a given input.
	//
	FILE *stream;
   	char tempBuffer[MAX_PATH];
   	char resultFile[MAX_LENGTH];
	
	GetWindowsDirectoryA( tempBuffer, MAX_PATH );
	
	// set sentinel values
	tempBuffer[3] = NULL;
	resultFile[0] = NULL;
	sprintf( resultFile, "%s%s", tempBuffer, TEST_FILE );

	//
	// create a result file and write success for testing purposes
	//
	stream = fopen( resultFile, "w" );
    if ( stream != NULL )
    {
	    fprintf( stream, "0" );
	    fflush( stream );
	    fclose( stream );
	}
	else
	   TEXT_OUTLN( "Unable to Create Result file" )
#endif // _TESTING_	

} // PrfInfo::DumpTables


void PrfInfo::Failure( char *message )
{
	if ( message == NULL )     	
	 	message = "**** SEVERE FAILURE: TURNING OFF APPLICABLE PROFILING EVENTS ****";  
	
	//
	// Display the error message and discontinue monitoring CLR events, except the 
	// IMMUTABLE ones. Turning off the IMMUTABLE events can cause crashes. The only
	// place that we can safely enable or disable immutable events is the Initialize
	// callback.
	TEXT_OUTLN( message )
	m_pProfilerInfo->SetEventMask( (m_dwEventMask & (DWORD)COR_PRF_MONITOR_IMMUTABLE) );    
               	         
} // PrfInfo::Failure

/* throws BaseException */
void PrfInfo::_GetThreadInfoHelper( ThreadInfo **ppThreadInfo )
{
   	if ( m_pProfilerInfo != NULL )
    {
    	HRESULT hr;
        
        
	    hr = m_pProfilerInfo->GetThreadInfo( (*ppThreadInfo)->m_id, 
	        							     &((*ppThreadInfo)->m_win32ThreadID) );
	  	if ( SUCCEEDED( hr ) )
	   	{
	  		hr = m_pProfilerInfo->GetHandleFromThread( (*ppThreadInfo)->m_id, 
	    										       &((*ppThreadInfo)->m_hThread) );  	
			if ( FAILED( hr ) )
	      		_THROW_EXCEPTION( "ICorProfilerInfo::GetHandleFromThread() FAILED" )	
	  	}
	    else	
	    	_THROW_EXCEPTION( "ICorProfilerInfo::GetThreadInfo() FAILED" )
 	}
    else
    	_THROW_EXCEPTION( "ICorProfilerInfo Interface has NOT been Initialized" )		
        
} // PrfInfo::_GetThreadInfoHelper

/* private */
/* throws BaseException */
void PrfInfo::_GetFunctionInfoHelper( FunctionInfo **ppFunctionInfo )
{
   	if ( m_pProfilerInfo != NULL )
    {
	 	HRESULT hr;


	 	hr = BASEHELPER::GetFunctionProperties( m_pProfilerInfo,
												(*ppFunctionInfo)->m_id,
								   				&((*ppFunctionInfo)->m_bIsStatic),
								   				&((*ppFunctionInfo)->m_argCount),
								   				(*ppFunctionInfo)->m_returnTypeStr, 
								   				(*ppFunctionInfo)->m_functionParameters,
								   				(*ppFunctionInfo)->m_functionName );
		if ( FAILED( hr ) )
			LOG_TO_FILE( ("Unable to Retreive Information about the Function Name, Parameters and Return Type\n") )
 	}
    else
    	_THROW_EXCEPTION( "ICorProfilerInfo Interface has NOT been Initialized" )		
        

} // PrfInfo::_GetFunctionInfoHelper

TimeTracker::TimeTracker( PrfInfo *pPrfInfo, ThreadID threadID ) : 
	m_pThreadInfo( NULL )
{
	HRESULT hr = S_OK;
	LARGE_INTEGER localTimer;

	// This is a special purpose class used to capture the current value of a
    // high precision timer and store it in a data member (m_currentProfilerTime)
    // of a ThreadInfo object. A ThreadInfo object corresponds to a managed thread
    // that the CLR reported during the ThreadCreated callback. The data member can
    // be used as a basis for any timer related calculation because it holds the 
    // starting point for the current time counting operation.
	//
	// This class is typically used as a local for the whole scope of many of the 
    // callbacks. 

	// Get the time as soon as you start
	localTimer.QuadPart = 0;
	QueryPerformanceCounter( &localTimer );
	if ( threadID == NULL )
    	hr = (pPrfInfo->m_pProfilerInfo)->GetCurrentThreadID( &threadID );

    if ( SUCCEEDED( hr ) )   {	
		//
		// Find the thread object based on the threadID and store 
		// the time that tracks how much time is spent in the current 
        // callback.
		//
		m_pThreadInfo = (pPrfInfo->m_pThreadTable)->Lookup( threadID );
    	
    	//
		// Make sure that all other timing calculations for that thread 
		// have completed at this point
		//
    	_ASSERT_( (m_pThreadInfo->m_currentProfilerTime).QuadPart == 0 );
		(m_pThreadInfo->m_currentProfilerTime).QuadPart = localTimer.QuadPart;
	}
    
} // ctor         		

TimeTracker::~TimeTracker()
{
	LARGE_INTEGER storedCounterValue;

	// The destructor is used to calculate the accumulative times for the thread 
    // and the function that is on top of the callstack for the current thread. 
	storedCounterValue.QuadPart = (m_pThreadInfo->m_currentProfilerTime).QuadPart;
	(m_pThreadInfo->m_currentProfilerTime).QuadPart = 0;

	if ( (m_pThreadInfo != NULL) && (m_pThreadInfo->m_isValid == TRUE) ) {		
		LARGE_INTEGER localTimer;
   		BOOL bInRecursiveCall = FALSE;
   		StackBaseInfo *pStackTop = NULL;
		StackFunctionInfo *pFunctionInfo = NULL;
		StackFunctionInfo *pFunctionInfo2 = NULL;
		FunctionTimingInfo *pPoppedFunction = NULL;
		FunctionTimingInfo *pCurrentFunction = NULL;

		// pPoppedFunction points to the entry of the function that we have just popped
		// and pFunctionInfo points to the most recent entry of its timers in the function's
		// timer stack. This pointer will be NULL if we are in case 1 or 2

		// pCurrentFunction points to the entry of the function that is on the top of the callstack
		// and pFunctionInfo points to the most recent entry of its timers in the function's
		// timer stack. This pointer will be NULL is we are in case 1 and the callstack is empty 
		_ASSERT_( storedCounterValue.QuadPart != 0 );
		if ( m_pThreadInfo->m_lastPoppedFID != 0xFFFFFFFF ) {
			pPoppedFunction = (m_pThreadInfo->m_pFunctionTimes)->Lookup( m_pThreadInfo->m_lastPoppedFID );	
			if ( pPoppedFunction != NULL )
			   pFunctionInfo = (pPoppedFunction->m_pLatestInvocation)->Pop();
		} 
		
        
        pStackTop = (m_pThreadInfo->m_pCallStack)->Top();
		if ( pStackTop != NULL ) {
			pCurrentFunction = (m_pThreadInfo->m_pFunctionTimes)->Lookup( pStackTop->m_id );	
			if ( pCurrentFunction != NULL )
			   pFunctionInfo2 = (pCurrentFunction->m_pLatestInvocation)->Top();
		} 

		// Get the time just before you finish
		localTimer.QuadPart = 0;
		QueryPerformanceCounter( &localTimer );
		
		// Update the timer of the specific thread for the current
		// profiling event
		(m_pThreadInfo->m_profilerTime).QuadPart += (localTimer.QuadPart - storedCounterValue.QuadPart);

		// Up to this point we have accessed the appropriate thread and function data
        // structures and we have made the initial computation to determine the profiler
        // time for the thread. 
        //  
		// We have the following 2 cases to consider:
	    //
		//		1. A callback that is related to a leave event 
        //		2. A callback that is either not related to any function enter or leave event
        //         or strictly a function enter event

		// Case 1: Leave Event
        //
		if ( pPoppedFunction != NULL )	{

			// check if the call stack is returning from a recursive call
			if ( pCurrentFunction != NULL )
				bInRecursiveCall = ( (pCurrentFunction->m_id) == (pPoppedFunction->m_id) ) ? TRUE : FALSE;

			_ASSERT_((pFunctionInfo->m_currentInclusiveTime).QuadPart != 0);
			(pFunctionInfo->m_profilerTime).QuadPart += (localTimer.QuadPart - storedCounterValue.QuadPart);
			(pFunctionInfo->m_inclusiveTime).QuadPart += (localTimer.QuadPart - (pFunctionInfo->m_currentInclusiveTime).QuadPart);
			(pFunctionInfo->m_currentInclusiveTime).QuadPart = 0;
			(pFunctionInfo->m_exclusiveTime).QuadPart += (pFunctionInfo->m_inclusiveTime).QuadPart
				- (pFunctionInfo->m_profilerTime).QuadPart 
				- (pFunctionInfo->m_suspendedTime).QuadPart
				- (pFunctionInfo->m_calleeTime).QuadPart; //av

			// Update the global counters for that function
			(pPoppedFunction->m_calleeTime).QuadPart += (pFunctionInfo->m_calleeTime).QuadPart;
			(pPoppedFunction->m_profilerTime).QuadPart += (pFunctionInfo->m_profilerTime).QuadPart;
			(pPoppedFunction->m_suspendedTime).QuadPart += (pFunctionInfo->m_suspendedTime).QuadPart;

			
			// do not update the inclusive time if we have a return from a recursive call
			if ( bInRecursiveCall == FALSE ) {
				(pPoppedFunction->m_inclusiveTime).QuadPart += (pFunctionInfo->m_inclusiveTime).QuadPart;
			} 
			(pPoppedFunction->m_exclusiveTime).QuadPart += (pFunctionInfo->m_exclusiveTime).QuadPart;
			

			// Update the timers for the top function in the call stack	
			// and we do not have a recursive call
			if ( (pCurrentFunction != NULL) && (pFunctionInfo2 != NULL) && ( bInRecursiveCall == FALSE ) ) {
				(pFunctionInfo2->m_calleeTime).QuadPart += (pFunctionInfo->m_inclusiveTime).QuadPart;
			}
			if ( (pCurrentFunction != NULL) && (pFunctionInfo2 != NULL) ) {
				(pFunctionInfo2->m_exclusiveTime).QuadPart += (pFunctionInfo->m_exclusiveTime).QuadPart;
			}

			m_pThreadInfo->m_lastPoppedFID = 0xFFFFFFFF;
		}

		// Case 2: No Enter or Leave Event, Or strictly an Enter Event
		else {		   
			if ( (pCurrentFunction != NULL) && (pFunctionInfo2 != NULL) )
				(pFunctionInfo2->m_profilerTime).QuadPart += (localTimer.QuadPart - storedCounterValue.QuadPart);
		}
        
       	// ensure that has been reset and clean up the Popped (copied) stack element
	    _ASSERT_( m_pThreadInfo->m_lastPoppedFID == 0xFFFFFFFF );
        if ( pFunctionInfo != NULL )
	        delete pFunctionInfo;
	}
} // dtor
