/****************************************************************************************
 * Copyright (c) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 * File:
 *  ProfilerCallBack.cpp
 *
 * Description:
 *  
 *
 *
 ***************************************************************************************/ 
#include "ProfilerCallback.h"

          
/***************************************************************************************
 ********************                                               ********************
 ********************   Global Functions Used by Function Hooks     ********************
 ********************                                               ********************
 ***************************************************************************************/

//
// The functions EnterStub, LeaveStub and TailcallStub are wrappers. The use of 
// of the extended attribute "__declspec( naked )" does not allow a direct call
// to a profiler callback (e.g., ProfilerCallback::Enter( functionID )).
//
// The enter/leave function hooks must necessarily use the extended attribute
// "__declspec( naked )". Please read the corprof.idl for more details. 
//

/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
void __stdcall EnterStub( FunctionID functionID )
{
    ProfilerCallback::Enter( functionID );
    
} // EnterStub


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
void __stdcall LeaveStub( FunctionID functionID )
{
    ProfilerCallback::Leave( functionID );
    
} // LeaveStub


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
void __stdcall TailcallStub( FunctionID functionID )
{
    ProfilerCallback::Tailcall( functionID );
    
} // TailcallStub


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
void __declspec( naked ) EnterNaked()
{
    __asm
    {
        push eax
        push ecx
        push edx
        push [esp + 16]
        call EnterStub
        pop edx
        pop ecx
        pop eax
        ret 4
    }
    
} // EnterNaked


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
void __declspec( naked ) LeaveNaked()
{
    __asm
    {
        push eax
        push ecx
        push edx
        push [esp + 16]
        call LeaveStub
        pop edx
        pop ecx
        pop eax
        ret 4
    }
    
} // LeaveNaked


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
void __declspec( naked ) TailcallNaked()
{
    __asm
    {
        push eax
        push ecx
        push edx
        push [esp + 16]
        call TailcallStub
        pop edx
        pop ecx
        pop eax
        ret 4
    }
    
} // TailcallNaked


/***************************************************************************************
 ********************                                               ********************
 ********************     ProfilerCallBack Implementation           ********************
 ********************                                               ********************
 ***************************************************************************************/
ProfilerCallback *g_pCallbackObject;  // global pointer to profiler callback interface


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
ProfilerCallback::ProfilerCallback() :
    PrfInfo(),
    m_refCount( 0 ),
	m_dwShutdown( 0 )
{
    //
	// Global pointer to the profiler callback interface. This will be 
    // helpful for releasing the callback interface in the event that 
    // an abnormal shutdown occurs of the profilee.
	//
    g_pCallbackObject = this;
    
    
	// We are monitoring events that are interesting for determining
    // the hot spots of a managed CLR program (profilee). This includes
    // thread related events, function enter/leave events, exception 
    // related events, and unmanaged/managed transition events. Note 
    // that we disable inlining. Although this does indeed affect the 
    // execution time, it provides better accuracy for determining
    // hot spots.
	//
	
	//
	// If the system does not support high precision counters, then
    // do not profile anything. This is determined in the ctor for
    // PrfInfo.
  	//
	if ( m_bHighPrecisionSupported == TRUE )
	{
		m_dwEventMask = (DWORD)(COR_PRF_MONITOR_THREADS			|
                                COR_PRF_DISABLE_INLINING		|
                                COR_PRF_MONITOR_SUSPENDS    	|                             
                                COR_PRF_MONITOR_ENTERLEAVE 		|
                                COR_PRF_MONITOR_EXCEPTIONS  	|  
			                    COR_PRF_MONITOR_CACHE_SEARCHES	|
							    COR_PRF_MONITOR_JIT_COMPILATION | 
                                COR_PRF_MONITOR_CODE_TRANSITIONS);
	}
	else
		m_dwEventMask = (DWORD)COR_PRF_MONITOR_NONE;

} // ctor


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
ProfilerCallback::~ProfilerCallback()
{
   	g_pCallbackObject = NULL;
        
} // dtor

        
/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
ULONG ProfilerCallback::AddRef() 
{

    return InterlockedIncrement( &m_refCount );

} // ProfilerCallback::AddRef


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
ULONG ProfilerCallback::Release() 
{
    long refCount;


    refCount = InterlockedDecrement( &m_refCount );
    if ( refCount == 0 )
        delete this;
     

    return refCount;

} // ProfilerCallback::Release


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::QueryInterface( REFIID riid, void **ppInterface )
{
    if ( riid == IID_IUnknown )
        *ppInterface = static_cast<IUnknown *>( this ); 

    else if ( riid == IID_ICorProfilerCallback2 )
        *ppInterface = static_cast<ICorProfilerCallback2 *>( this );

    else
    {
        *ppInterface = NULL;


        return E_NOINTERFACE;
    }
    
    reinterpret_cast<IUnknown *>( *ppInterface )->AddRef();

    return S_OK;

} // ProfilerCallback::QueryInterface 


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public static */
HRESULT ProfilerCallback::CreateObject( REFIID riid, void **ppInterface )
{
    HRESULT hr = E_NOINTERFACE;
    
     
   	*ppInterface = NULL;
    if ( (riid == IID_IUnknown) || (riid == IID_ICorProfilerCallback2) )
    {           
        ProfilerCallback *pProfilerCallback;
        
                
        pProfilerCallback = new ProfilerCallback();
        if ( pProfilerCallback != NULL )
        {
        	hr = S_OK;
            
            pProfilerCallback->AddRef();
            *ppInterface = static_cast<ICorProfilerCallback2 *>( pProfilerCallback );
        }
        else
            hr = E_OUTOFMEMORY;
    }    
    

    return hr;

} // ProfilerCallback::CreateObject


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::Initialize( IUnknown *pICorProfilerInfoUnk )
{     
    HRESULT hr;

    
    TEXT_OUTLN( ".NET Profiler Hot Spots Tracker Sample" )
    
    
    hr = pICorProfilerInfoUnk->QueryInterface( IID_ICorProfilerInfo, (void **)&m_pProfilerInfo );   
    if ( SUCCEEDED( hr ) )
    { 
        hr = m_pProfilerInfo->SetEventMask( m_dwEventMask );
        if ( SUCCEEDED( hr ) )
        {
	        //
			// Set the function hooks for enter, leave and tail calls
			//
	        hr = m_pProfilerInfo->SetEnterLeaveFunctionHooks( (FunctionEnter *)&EnterNaked,
	                                                          (FunctionLeave *)&LeaveNaked,
	                                                          (FunctionTailcall *)&TailcallNaked );        
	        if ( FAILED( hr ) )
	            Failure( "ICorProfilerInfo::SetEnterLeaveFunctionHooks() FAILED" );
		}
		else
			Failure( "ICorProfilerInfo::SetEventMask() FAILED" );
    }       
    else
        Failure( "Allocation for Profiler Test FAILED" );           
              
              
    return S_OK;

} // ProfilerCallback::Initialize


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::Shutdown()
{
	m_dwShutdown++;
	DumpTables();


    return S_OK;          

} // ProfilerCallback::Shutdown


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::DllDetachShutdown()
{
    //
    // If no shutdown occurs during DLL_DETACH, release the callback
    // interface pointer. This scenario will more than likely occur
    // with any interop related program (e.g., a program that is 
    // comprised of both managed and unmanaged components).
    //
	m_dwShutdown++;
    if ( (m_dwShutdown == 1) && (g_pCallbackObject != NULL) )
	{
		DumpTables();
		g_pCallbackObject->Release();	
		g_pCallbackObject = NULL;
	}

    
    return S_OK;          

} // ProfilerCallback::DllDetachShutdown


/***************************************************************************************
 *  Method:
 *
 *
 *  Purpose:
 *
 *
 *  Parameters: 
 *
 *
 *  Return value:
 *
 *
 *  Notes:
 *
 ***************************************************************************************/
/* public */
void ProfilerCallback::Enter( FunctionID functionID )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( g_pCallbackObject ) );
	/////////////////////////////////////////////////////////////////////////// 
    
     
    try
    {
    	g_pCallbackObject->UpdateCallStack( functionID, PUSH );
   	}
    catch ( BaseException *exception )
    {    	
    	exception->ReportFailure();
        delete exception;
        
        g_pCallbackObject->Failure();       	    
    }

} // ProfilerCallback::Enter


/***************************************************************************************
 *  Method:
 *
 *
 *  Purpose:
 *
 *
 *  Parameters: 
 *
 *
 *  Return value:
 *
 *
 *  Notes:
 *
 ***************************************************************************************/
/* public */
void ProfilerCallback::Leave( FunctionID functionID )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( g_pCallbackObject ) );
	/////////////////////////////////////////////////////////////////////////// 
    
     
    try
    {
    	g_pCallbackObject->UpdateCallStack( functionID, POP );
   	}
    catch ( BaseException *exception )
    {    	
    	exception->ReportFailure();
        delete exception;
        
        g_pCallbackObject->Failure();       	    
    }

} // ProfilerCallback::Leave


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
void ProfilerCallback::Tailcall( FunctionID functionID )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( g_pCallbackObject ) );
	///////////////////////////////////////////////////////////////////////////
    
      
    try
    {
    	g_pCallbackObject->UpdateCallStack( functionID, POP );
   	}
    catch ( BaseException *exception )
    {    	
    	exception->ReportFailure();
        delete exception;
        
        g_pCallbackObject->Failure();       	    
    }

} // ProfilerCallback::Tailcall


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RuntimeThreadSuspended( ThreadID threadID )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( this ), threadID );
	///////////////////////////////////////////////////////////////////////////
    
      
    try
    {
    	UpdateSuspendedTimer( threadID, START ); 
   	}
    catch ( BaseException *exception )
    {    	
    	exception->ReportFailure();
        delete exception;
        
        Failure();       	    
    }
    
    
    return S_OK;
    
} // ProfilerCallback::RuntimeThreadSuspended


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RuntimeThreadResumed( ThreadID threadID )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( this ), threadID );
	/////////////////////////////////////////////////////////////////////////// 
    
     
    try
    {
    	UpdateSuspendedTimer( threadID, STOP ); 
   	}
    catch ( BaseException *exception )
    {    	
    	exception->ReportFailure();
        delete exception;
        
        Failure();       	    
    }


    return S_OK;
    
} // ProfilerCallback::RuntimeThreadResumed


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::UnmanagedToManagedTransition( FunctionID functionID,
                                                        COR_PRF_TRANSITION_REASON reason )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( this ) );
	///////////////////////////////////////////////////////////////////////////  
	HRESULT hr = E_FAIL;


	switch ( reason )
	{
		case COR_PRF_TRANSITION_RETURN:
		{
		    try
			{
		    	// pop the native frame
		    	UpdateCallStack( functionID, POP );
		   	}
		    catch ( BaseException *exception )
		    {    	
		    	exception->ReportFailure();
		        delete exception;
		        
		        Failure();       	    
		    }

		}
		break;


		//
		// For any other case, there is no need to do anything 
		// since the profiler will receive a function enter 
		// callback for the managed function. All the updating 
		// will take place at that point.
		//
		case COR_PRF_TRANSITION_CALL:
		default:
			;
	
	} // switch


    return S_OK;

} // ProfilerCallback::UnmanagedToManagedTransition


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ManagedToUnmanagedTransition( FunctionID functionID,
                                                        COR_PRF_TRANSITION_REASON reason )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo*>(this) );
	///////////////////////////////////////////////////////////////////////////  
	HRESULT hr = E_FAIL;


	switch ( reason )
	{
		case COR_PRF_TRANSITION_CALL:
		{
		    try
		    {
		    	UpdateCallStack( NULL, PUSH );
		   	}
		    catch ( BaseException *exception )
		    {    	
		    	exception->ReportFailure();
		        delete exception;
		        
		        Failure();       	    
		    }
		}
		break;


		//
		// For any other case, there is no need to do anything 
		// since the profiler will receive a function leave 
		// callback for the managed function. All the updating 
		// will take place at that point.
		//
		case COR_PRF_TRANSITION_RETURN:
		default:
			;
            
	} // switch


    return S_OK;

} // ProfilerCallback::ManagedToUnmanagedTransition


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */ 
HRESULT ProfilerCallback::ThreadCreated( ThreadID threadID )
{
	try
    {
    	//
		// Mark the thread as valid.
		//
    	AddThread( threadID ); 
   	}
    catch ( BaseException *exception )
    {    	
    	exception->ReportFailure();
        delete exception;
        
        Failure();       	    
    }


    return S_OK; 
    
} // ProfilerCallback::ThreadCreated


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ThreadDestroyed( ThreadID threadID )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( this ), threadID );
	///////////////////////////////////////////////////////////////////////////  

	
    try
    {
	    //
		// Mark the thread as invalid. Note, the profiling end time 
        // will be annotated inside of RemoveThread()
		//    	
        RemoveThread( threadID );
   	}
    catch ( BaseException *exception )
    {    	
    	exception->ReportFailure();
        delete exception;
        
        Failure();       	    
    }


    return S_OK;
    
} // ProfilerCallback::ThreadDestroyed


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ThreadAssignedToOSThread( ThreadID managedThreadID,
                                                    DWORD osThreadID ) 
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( this ), managedThreadID );
	///////////////////////////////////////////////////////////////////////////  

   
   	try
	{      	
    	//
		// A Runtime thread has modified the Win32 thread that 
        // it is assigned to. Update the thread ID accordingly.
		//	
 		UpdateOSThreadID( managedThreadID, osThreadID );       
	}
	catch ( BaseException *exception )
	{
		exception->ReportFailure();
	    delete exception;
	   
	  	Failure();    
	}


    return S_OK;
    
} // ProfilerCallback::ThreadAssignedToOSThread


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionUnwindFunctionEnter( FunctionID functionID )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( this ) );
	///////////////////////////////////////////////////////////////////////////  

	
    try
    {
    	//
		// We have to push the function ID to the stack related to the thread 
        // that invoked the ExceptionUnwindFunctionEnter() callback. We need 
        // to cache the function ID to handle the nested callback case. Otherwise,
        // we will not know which function's counters (times) to update.  
		//
    	UpdateUnwindStack( &functionID, PUSH ); 
   	}
    catch ( BaseException *exception )
    {    	
    	exception->ReportFailure();
        delete exception;
        
        Failure();       	    
    }


    return S_OK;

} // ProfilerCallback::ExceptionUnwindFunctionEnter


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionUnwindFunctionLeave( )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( this ) );
	///////////////////////////////////////////////////////////////////////////  

	 
    try
    {
    	FunctionID functionID;
        
        
    	//
		// We have to pop the function ID from the stack related to the thread that
		// invoked the ExceptionUnwindFunctionLeave() callback. We will use this 
        // function ID to update its counters (timers) after we do a search in 
        // the function's list. 
		//
    	UpdateUnwindStack( &functionID, POP );
		if ( functionID != NULL )
			UpdateCallStack( functionID, POP );
		
		else
			Failure( "Unwind stack for the current thread was empty" );
   	}
    catch ( BaseException *exception )
    {    	
    	exception->ReportFailure();
        delete exception;
        
        Failure();       	    
    }


    return S_OK;

} // ProfilerCallback::ExceptionUnwindFunctionLeave


/***************************************************************************************
 *  Method:
 *
 *
 *  Purpose:
 *
 *
 *  Parameters: 
 *
 *
 *  Return value:
 *
 *
 *  Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::JITCompilationFinished( FunctionID functionID,
                                                  HRESULT hrStatus,
                                                  BOOL fIsSafeToBlock )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( g_pCallbackObject ) );
	/////////////////////////////////////////////////////////////////////////// 


   	try
    {      		
		AddFunction( functionID );
   	}
    catch ( BaseException *exception )
    {
    	exception->ReportFailure();
        delete exception;
       
      	Failure();    
    }

    return S_OK;
    
} // ProfilerCallback::JITCompilationFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::JITCachedFunctionSearchStarted( FunctionID functionID,
														  BOOL *pbUseCachedFunction )
{
	*pbUseCachedFunction = TRUE;
    
    return S_OK;
       
} // ProfilerCallback::JITCachedFunctionSearchStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::JITCachedFunctionSearchFinished( FunctionID functionID,
														   COR_PRF_JIT_CACHE result )
{
	///////////////////////////////////////////////////////////////////////////
	TimeTracker timer( static_cast<PrfInfo *>( g_pCallbackObject ) );
	/////////////////////////////////////////////////////////////////////////// 

	if ( result == COR_PRF_CACHED_FUNCTION_FOUND )
	{
	    try
	    {      		
			AddFunction( functionID );
	   	}
	    catch ( BaseException *exception )
	    {
	    	exception->ReportFailure();
	        delete exception;
	       
	      	Failure();    
	    }
	}

    return S_OK;
      
} // ProfilerCallback::JITCachedFunctionSearchFinished


/***************************************************************************************
 ********************                                               ********************
 ********************     Callbacks With Default Implementation     ********************
 ********************                                               ********************
 ***************************************************************************************/

/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::AppDomainCreationStarted( AppDomainID appDomainID )
{
    
    return S_OK;

} // ProfilerCallback::AppDomainCreationStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::AppDomainCreationFinished( AppDomainID appDomainID,
													 HRESULT hrStatus )
{

    return S_OK;

} // ProfilerCallback::AppDomainCreationFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::AppDomainShutdownStarted( AppDomainID appDomainID )
{

    return S_OK;

} // ProfilerCallback::AppDomainShutdownStarted

	  

/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::AppDomainShutdownFinished( AppDomainID appDomainID,
													 HRESULT hrStatus )
{

    return S_OK;

} // ProfilerCallback::AppDomainShutdownFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::AssemblyLoadStarted( AssemblyID assemblyID )
{

    return S_OK;

} // ProfilerCallback::AssemblyLoadStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::AssemblyLoadFinished( AssemblyID assemblyID,
												HRESULT hrStatus )
{

    return S_OK;

} // ProfilerCallback::AssemblyLoadFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::AssemblyUnloadStarted( AssemblyID assemblyID )
{

    return S_OK;

} // ProfilerCallback::AssemblyUnLoadStarted

	  
/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::AssemblyUnloadFinished( AssemblyID assemblyID,
												  HRESULT hrStatus )
{

    return S_OK;

} // ProfilerCallback::AssemblyUnLoadFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ModuleLoadStarted( ModuleID moduleID )
{

    return S_OK;

} // ProfilerCallback::ModuleLoadStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ModuleLoadFinished( ModuleID moduleID,
											  HRESULT hrStatus )
{

    return S_OK;

} // ProfilerCallback::ModuleLoadFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ModuleUnloadStarted( ModuleID moduleID )
{

    return S_OK;

} // ProfilerCallback::ModuleUnloadStarted
	  

/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ModuleUnloadFinished( ModuleID moduleID,
												HRESULT hrStatus )
{

    return S_OK;

} // ProfilerCallback::ModuleUnloadFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ModuleAttachedToAssembly( ModuleID moduleID,
													AssemblyID assemblyID )
{

    return S_OK;

} // ProfilerCallback::ModuleAttachedToAssembly


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ClassLoadStarted( ClassID classID )
{

    return S_OK;

} // ProfilerCallback::ClassLoadStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ClassLoadFinished( ClassID classID, 
											 HRESULT hrStatus )
{

    return S_OK;

} // ProfilerCallback::ClassLoadFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ClassUnloadStarted( ClassID classID )
{

    return S_OK;

} // ProfilerCallback::ClassUnloadStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ClassUnloadFinished( ClassID classID, 
											   HRESULT hrStatus )
{

    return S_OK;

} // ProfilerCallback::ClassUnloadFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::FunctionUnloadStarted( FunctionID functionID )
{

    return S_OK;

} // ProfilerCallback::FunctionUnloadStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::JITCompilationStarted( FunctionID functionID,
                                                 BOOL fIsSafeToBlock )
{

    return S_OK;
    
} // ProfilerCallback::JITCompilationStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::JITFunctionPitched( FunctionID functionID )
{
    
    return S_OK;
    
} // ProfilerCallback::JITFunctionPitched


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::JITInlining( FunctionID callerID,
                                       FunctionID calleeID,
                                       BOOL *pfShouldInline )
{

    return S_OK;

} // ProfilerCallback::JITInlining


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RemotingClientInvocationStarted()
{

    return S_OK;
    
} // ProfilerCallback::RemotingClientInvocationStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RemotingClientSendingMessage( GUID *pCookie,
    													BOOL fIsAsync )
{

    return S_OK;
    
} // ProfilerCallback::RemotingClientSendingMessage


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RemotingClientReceivingReply(	GUID *pCookie,
	    												BOOL fIsAsync )
{

    return S_OK;
    
} // ProfilerCallback::RemotingClientReceivingReply


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RemotingClientInvocationFinished()
{

   return S_OK;
    
} // ProfilerCallback::RemotingClientInvocationFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RemotingServerReceivingMessage( GUID *pCookie,
    													  BOOL fIsAsync )
{

    return S_OK;
    
} // ProfilerCallback::RemotingServerReceivingMessage


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RemotingServerInvocationStarted()
{

    return S_OK;
    
} // ProfilerCallback::RemotingServerInvocationStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RemotingServerInvocationReturned()
{

    return S_OK;
    
} // ProfilerCallback::RemotingServerInvocationReturned


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RemotingServerSendingReply( GUID *pCookie,
    												  BOOL fIsAsync )
{

    return S_OK;

} // ProfilerCallback::RemotingServerSendingReply


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RuntimeSuspendStarted( COR_PRF_SUSPEND_REASON suspendReason )
{

    return S_OK;
    
} // ProfilerCallback::RuntimeSuspendStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RuntimeSuspendFinished()
{

    return S_OK;
    
} // ProfilerCallback::RuntimeSuspendFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RuntimeSuspendAborted()
{

    return S_OK;
    
} // ProfilerCallback::RuntimeSuspendAborted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RuntimeResumeStarted()
{

    return S_OK;
    
} // ProfilerCallback::RuntimeResumeStarted


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RuntimeResumeFinished()
{

    return S_OK;
    
} // ProfilerCallback::RuntimeResumeFinished


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::MovedReferences( ULONG cmovedObjectIDRanges,
                                           ObjectID oldObjectIDRangeStart[],
                                           ObjectID newObjectIDRangeStart[],
                                           ULONG cObjectIDRangeLength[] )
{

    return S_OK;

} // ProfilerCallback::MovedReferences


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ObjectAllocated( ObjectID objectID,
                                           ClassID classID )
{

    return S_OK;

} // ProfilerCallback::ObjectAllocated


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ObjectsAllocatedByClass( ULONG classCount,
                                                   ClassID classIDs[],
                                                   ULONG objects[] )
{

    return S_OK;

} // ProfilerCallback::ObjectsAllocatedByClass


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ObjectReferences( ObjectID objectID,
                                            ClassID classID,
                                            ULONG objectRefs,
                                            ObjectID objectRefIDs[] )
{

    return S_OK;

} // ProfilerCallback::ObjectReferences


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::RootReferences( ULONG rootRefs,
                                          ObjectID rootRefIDs[] )
{

    return S_OK;

} // ProfilerCallback::RootReferences


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionThrown( ObjectID thrownObjectID )
{

    return S_OK;

} // ProfilerCallback::ExceptionThrown 


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionSearchFunctionEnter( FunctionID functionID )
{

    return S_OK;

} // ProfilerCallback::ExceptionSearchFunctionEnter


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionSearchFunctionLeave()
{

    return S_OK;

} // ProfilerCallback::ExceptionSearchFunctionLeave


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionSearchFilterEnter( FunctionID functionID )
{

    return S_OK;

} // ProfilerCallback::ExceptionSearchFilterEnter


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionSearchFilterLeave()
{

    return S_OK;

} // ProfilerCallback::ExceptionSearchFilterLeave 


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionSearchCatcherFound( FunctionID functionID )
{

    return S_OK;

} // ProfilerCallback::ExceptionSearchCatcherFound


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionCLRCatcherFound()
{
    return S_OK;
}

/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionCLRCatcherExecute()
{
    return S_OK;
}


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionOSHandlerEnter( FunctionID functionID )
{

    return S_OK;

} // ProfilerCallback::ExceptionOSHandlerEnter

    
/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionOSHandlerLeave( FunctionID functionID )
{

    return S_OK;

} // ProfilerCallback::ExceptionOSHandlerLeave


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionUnwindFinallyEnter( FunctionID functionID )
{

    return S_OK;

} // ProfilerCallback::ExceptionUnwindFinallyEnter


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionUnwindFinallyLeave()
{

    return S_OK;

} // ProfilerCallback::ExceptionUnwindFinallyLeave


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionCatcherEnter( FunctionID functionID,
    											 ObjectID objectID )
{

    return S_OK;

} // ProfilerCallback::ExceptionCatcherEnter


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::ExceptionCatcherLeave()
{

    return S_OK;

} // ProfilerCallback::ExceptionCatcherLeave


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::COMClassicVTableCreated( ClassID wrappedClassID,
                                                   REFGUID implementedIID,
                                                   void *pVTable,
                                                   ULONG cSlots )
{

    return S_OK;

} // ProfilerCallback::COMClassicWrapperCreated


/***************************************************************************************
 *	Method:
 *
 *
 *	Purpose:
 *
 *
 *	Parameters: 
 *
 *
 *	Return value:
 *
 *
 *	Notes:
 *
 ***************************************************************************************/
/* public */
HRESULT ProfilerCallback::COMClassicVTableDestroyed( ClassID wrappedClassID,
                                                     REFGUID implementedIID,
                                                     void *pVTable )
{

    return S_OK;

} // ProfilerCallback::COMClassicWrapperDestroyed


HRESULT ProfilerCallback::SurvivingReferences( ULONG cmovedObjectIDRanges,
                                           ObjectID objectIDRangeStart[],
                                           ULONG cObjectIDRangeLength[] )
{
	return S_OK;
}

	HRESULT ProfilerCallback::ThreadNameChanged( 
            /* [in] */ ThreadID threadId,
            /* [in] */ ULONG cchName,
            /* [in] */ WCHAR name[  ])
{
    return S_OK;
} 

		HRESULT ProfilerCallback::FinalizeableObjectQueued( 
            /* [in] */ DWORD finalizerFlags,
            /* [in] */ ObjectID objectID)
{    
    return S_OK;
}

		HRESULT   ProfilerCallback::RootReferences2( 
            /* [in] */ ULONG cRootRefs,
            /* [size_is][in] */ ObjectID rootRefIds[  ],
            /* [size_is][in] */ COR_PRF_GC_ROOT_KIND rootKinds[  ],
            /* [size_is][in] */ COR_PRF_GC_ROOT_FLAGS rootFlags[  ],
            /* [size_is][in] */ UINT_PTR rootIds[  ])
{   
    return S_OK;
}

		HRESULT  ProfilerCallback::HandleCreated(
            UINT_PTR handleId,
            ObjectID initialObjectId)
{    
    return S_OK;
}

		HRESULT   ProfilerCallback::HandleDestroyed(
            UINT_PTR handleId)
{   
    return S_OK;
}

		HRESULT   ProfilerCallback::GarbageCollectionStarted(
    int cGenerations,
    BOOL generationCollected[],
    COR_PRF_GC_REASON reason)
{    
    return S_OK;
}

		HRESULT  ProfilerCallback::GarbageCollectionFinished()
{   
    return S_OK;
}

/***************************************************************************************
 ********************                                               ********************
 ********************              DllMain/ClassFactory             ********************
 ********************                                               ********************
 ***************************************************************************************/ 
#include "dllmain.hpp"

// End of File

