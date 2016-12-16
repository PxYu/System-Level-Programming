/****************************************************************************************
 * Copyright (c) 1999-2001 Microsoft Corporation.  All Rights Reserved.
 *
 * File:
 *  ProfilerCallback.h
 *
 * Description:
 *  
 *
 *
 ***************************************************************************************/
#ifndef __PROFILER_CALLBACK_H__
#define __PROFILER_CALLBACK_H__

#include "ProfilerInfo.h"


/////////////////////////////////////////////////////////////////////////////////////////
// Each sample should provide the following definitions (with a new GUID)
//
	// {FA0C6E77-AFF4-4c74-95F9-6216AFDE992C}
	extern const GUID __declspec( selectany ) CLSID_PROFILER = 
		{ 0xfa0c6e77, 0xaff4, 0x4c74, { 0x95, 0xf9, 0x62, 0x16, 0xaf, 0xde, 0x99, 0x2c } };

	#define THREADING_MODEL     "Both"
	#define PROGID_PREFIX       "ProfilerHST"
  	#define COCLASS_DESCRIPTION "Microsoft CLR Hot Spots Tracker"
   	#define PROFILER_GUID       "{FA0C6E77-AFF4-4c74-95F9-6216AFDE992C}"
//
/////////////////////////////////////////////////////////////////////////////////////////


/***************************************************************************************
 ********************                                               ********************
 ********************       ProfilerCallback Declaration            ********************
 ********************                                               ********************
 ***************************************************************************************/

class ProfilerCallback : 
	public PrfInfo,
	public ICorProfilerCallback2 
{
    public:
    
        ProfilerCallback();
        ~ProfilerCallback();


    public:

        //
        // IUnknown 
        //
        COM_METHOD( ULONG ) AddRef(); 
        COM_METHOD( ULONG ) Release();
        COM_METHOD( HRESULT ) QueryInterface( REFIID riid, void **ppInterface );


        //
        // STARTUP/SHUTDOWN EVENTS
        //
        virtual COM_METHOD( HRESULT ) Initialize( IUnknown *pICorProfilerInfoUnk );
               
		HRESULT DllDetachShutdown();                           
        COM_METHOD( HRESULT ) Shutdown();
                                         

		//
	 	// APPLICATION DOMAIN EVENTS
		//
	   	COM_METHOD( HRESULT ) AppDomainCreationStarted( AppDomainID appDomainID );
        
	    	COM_METHOD( HRESULT ) AppDomainCreationFinished( AppDomainID appDomainID,
													     HRESULT hrStatus );
    
        COM_METHOD( HRESULT ) AppDomainShutdownStarted( AppDomainID appDomainID );

		COM_METHOD( HRESULT ) AppDomainShutdownFinished( AppDomainID appDomainID, 
        												 HRESULT hrStatus );


		//
	 	// ASSEMBLY EVENTS
		//
	   	COM_METHOD( HRESULT ) AssemblyLoadStarted( AssemblyID assemblyID );
        
    	COM_METHOD( HRESULT ) AssemblyLoadFinished( AssemblyID assemblyID,
                                                    HRESULT hrStatus );
    
        COM_METHOD( HRESULT ) AssemblyUnloadStarted( AssemblyID assemblyID );

		COM_METHOD( HRESULT ) AssemblyUnloadFinished( AssemblyID assemblyID, 
        											  HRESULT hrStatus );
		
		
		//
	 	// MODULE EVENTS
		//
	   	COM_METHOD( HRESULT ) ModuleLoadStarted( ModuleID moduleID );
        
    	COM_METHOD( HRESULT ) ModuleLoadFinished( ModuleID moduleID,
                                                  HRESULT hrStatus );
    
        COM_METHOD( HRESULT ) ModuleUnloadStarted( ModuleID moduleID );

		COM_METHOD( HRESULT ) ModuleUnloadFinished( ModuleID moduleID, 
        											HRESULT hrStatus );

		COM_METHOD( HRESULT ) ModuleAttachedToAssembly( ModuleID moduleID,
														AssemblyID assemblyID );
                
        
        //
        // CLASS EVENTS
        //
        COM_METHOD( HRESULT ) ClassLoadStarted( ClassID classID );
        
        COM_METHOD( HRESULT ) ClassLoadFinished( ClassID classID,
                                                 HRESULT hrStatus );
    
     	COM_METHOD( HRESULT ) ClassUnloadStarted( ClassID classID );

		COM_METHOD( HRESULT ) ClassUnloadFinished( ClassID classID, 
        										   HRESULT hrStatus );

		COM_METHOD( HRESULT ) FunctionUnloadStarted( FunctionID functionID );
        
        
        //
        // JIT EVENTS
        //              
        COM_METHOD( HRESULT ) JITCompilationStarted( FunctionID functionID,
                                                     BOOL fIsSafeToBlock );
                                        
        COM_METHOD( HRESULT ) JITCompilationFinished( FunctionID functionID,
        											  HRESULT hrStatus,
                                                      BOOL fIsSafeToBlock );
    
        COM_METHOD( HRESULT ) JITCachedFunctionSearchStarted( FunctionID functionID,
															  BOOL *pbUseCachedFunction );
        
		COM_METHOD( HRESULT ) JITCachedFunctionSearchFinished( FunctionID functionID,
															   COR_PRF_JIT_CACHE result );
                                                                     
        COM_METHOD( HRESULT ) JITFunctionPitched( FunctionID functionID );
        
        COM_METHOD( HRESULT ) JITInlining( FunctionID callerID,
                                           FunctionID calleeID,
                                           BOOL *pfShouldInline );

        
        //
        // THREAD EVENTS
        //
        COM_METHOD( HRESULT ) ThreadCreated( ThreadID threadID );
    
        COM_METHOD( HRESULT ) ThreadDestroyed( ThreadID threadID );

        COM_METHOD( HRESULT ) ThreadAssignedToOSThread( ThreadID managedThreadID,
                                                        DWORD osThreadID );
    

       	//
        // REMOTING EVENTS
        //                                                      

        //
        // Client-side events
        //
        COM_METHOD( HRESULT ) RemotingClientInvocationStarted();

        COM_METHOD( HRESULT ) RemotingClientSendingMessage( GUID *pCookie,
															BOOL fIsAsync );

        COM_METHOD( HRESULT ) RemotingClientReceivingReply( GUID *pCookie,
															BOOL fIsAsync );

        COM_METHOD( HRESULT ) RemotingClientInvocationFinished();

        //
        // Server-side events
        //
        COM_METHOD( HRESULT ) RemotingServerReceivingMessage( GUID *pCookie,
															  BOOL fIsAsync );

        COM_METHOD( HRESULT ) RemotingServerInvocationStarted();

        COM_METHOD( HRESULT ) RemotingServerInvocationReturned();

        COM_METHOD( HRESULT ) RemotingServerSendingReply( GUID *pCookie,
														  BOOL fIsAsync );


       	//
        // CONTEXT EVENTS
        //                                                      
    	COM_METHOD( HRESULT ) UnmanagedToManagedTransition( FunctionID functionID,
                                                            COR_PRF_TRANSITION_REASON reason );
    
        COM_METHOD( HRESULT ) ManagedToUnmanagedTransition( FunctionID functionID,
                                                            COR_PRF_TRANSITION_REASON reason );
                                                                  
                                                                        
       	//
        // SUSPENSION EVENTS
        //    
        COM_METHOD( HRESULT ) RuntimeSuspendStarted( COR_PRF_SUSPEND_REASON suspendReason );

        COM_METHOD( HRESULT ) RuntimeSuspendFinished();

        COM_METHOD( HRESULT ) RuntimeSuspendAborted();

        COM_METHOD( HRESULT ) RuntimeResumeStarted();

        COM_METHOD( HRESULT ) RuntimeResumeFinished();

        COM_METHOD( HRESULT ) RuntimeThreadSuspended( ThreadID threadid );

        COM_METHOD( HRESULT ) RuntimeThreadResumed( ThreadID threadid );


       	//
        // GC EVENTS
        //    
        COM_METHOD( HRESULT ) MovedReferences( ULONG cmovedObjectIDRanges,
                                               ObjectID oldObjectIDRangeStart[],
                                               ObjectID newObjectIDRangeStart[],
                                               ULONG cObjectIDRangeLength[] );
    
        COM_METHOD( HRESULT ) ObjectAllocated( ObjectID objectID,
                                               ClassID classID );
    
        COM_METHOD( HRESULT ) ObjectsAllocatedByClass( ULONG classCount,
                                                       ClassID classIDs[],
                                                       ULONG objects[] );
    
        COM_METHOD( HRESULT ) ObjectReferences( ObjectID objectID,
                                                ClassID classID,
                                                ULONG cObjectRefs,
                                                ObjectID objectRefIDs[] );
    
        COM_METHOD( HRESULT ) RootReferences( ULONG cRootRefs,
                                              ObjectID rootRefIDs[] );
    
        
      	//
        // EXCEPTION EVENTS
        //                                                         

        // Exception creation
        COM_METHOD( HRESULT ) ExceptionThrown( ObjectID thrownObjectID );

        // Search phase
        COM_METHOD( HRESULT ) ExceptionSearchFunctionEnter( FunctionID functionID );
    
        COM_METHOD( HRESULT ) ExceptionSearchFunctionLeave();
    
        COM_METHOD( HRESULT ) ExceptionSearchFilterEnter( FunctionID functionID );
    
        COM_METHOD( HRESULT ) ExceptionSearchFilterLeave();
    
        COM_METHOD( HRESULT ) ExceptionSearchCatcherFound( FunctionID functionID );
        
        COM_METHOD( HRESULT ) ExceptionCLRCatcherFound();

        COM_METHOD( HRESULT ) ExceptionCLRCatcherExecute();

        COM_METHOD( HRESULT ) ExceptionOSHandlerEnter( FunctionID functionID );
            
        COM_METHOD( HRESULT ) ExceptionOSHandlerLeave( FunctionID functionID );
    
        // Unwind phase
        COM_METHOD( HRESULT ) ExceptionUnwindFunctionEnter( FunctionID functionID );
    
        COM_METHOD( HRESULT ) ExceptionUnwindFunctionLeave();
        
        COM_METHOD( HRESULT ) ExceptionUnwindFinallyEnter( FunctionID functionID );
    
        COM_METHOD( HRESULT ) ExceptionUnwindFinallyLeave();
        
        COM_METHOD( HRESULT ) ExceptionCatcherEnter( FunctionID functionID,
            										 ObjectID objectID );
    
        COM_METHOD( HRESULT ) ExceptionCatcherLeave();

        
        //
		// COM CLASSIC WRAPPER
		//
        COM_METHOD( HRESULT )  COMClassicVTableCreated( ClassID wrappedClassID,
                                                        REFGUID implementedIID,
                                                        void *pVTable,
                                                        ULONG cSlots );

        COM_METHOD( HRESULT )  COMClassicVTableDestroyed( ClassID wrappedClassID,
                                                          REFGUID implementedIID,
                                                          void *pVTable );

		// Abstract members from ICorProfilerCallback2
#ifdef __ICorProfilerCallback2_INTERFACE_DEFINED__
	    COM_METHOD ( HRESULT ) SurvivingReferences( ULONG cmovedObjectIDRanges,
                                           ObjectID objectIDRangeStart[],
                                           ULONG cObjectIDRangeLength[] );
		COM_METHOD( HRESULT ) ThreadNameChanged( 
            /* [in] */ ThreadID threadId,
            /* [in] */ ULONG cchName,
            /* [in] */ WCHAR name[  ]);
		COM_METHOD( HRESULT ) FinalizeableObjectQueued( 
            /* [in] */ DWORD finalizerFlags,
            /* [in] */ ObjectID objectID);
		COM_METHOD( HRESULT )  RootReferences2( 
            /* [in] */ ULONG cRootRefs,
            /* [size_is][in] */ ObjectID rootRefIds[  ],
            /* [size_is][in] */ COR_PRF_GC_ROOT_KIND rootKinds[  ],
            /* [size_is][in] */ COR_PRF_GC_ROOT_FLAGS rootFlags[  ],
            /* [size_is][in] */ UINT_PTR rootIds[  ]);
		COM_METHOD( HRESULT )  HandleCreated(
            UINT_PTR handleId,
            ObjectID initialObjectId);
		COM_METHOD( HRESULT )  HandleDestroyed(
            UINT_PTR handleId);
		COM_METHOD( HRESULT )  GarbageCollectionStarted(
			int cGenerations,
		    BOOL generationCollected[],
			COR_PRF_GC_REASON reason);
		COM_METHOD( HRESULT )  GarbageCollectionFinished();
#endif // __ICorProfilerCallback2_INTERFACE_DEFINED__

        //
        // instantiate an instance of the callback interface
        //
        static COM_METHOD( HRESULT) CreateObject( REFIID riid, void **ppInterface );            
        
                                                                                                     
    	// used by function hooks, they have to be static
    	static void  Enter( FunctionID functionID );
		static void  Leave( FunctionID functionID );
		static void  Tailcall( FunctionID functionID );


    private:

        long m_refCount;                        
		DWORD m_dwShutdown;

}; // ProfilerCallback

extern ProfilerCallback *g_pCallbackObject;		// global reference to callback object

#endif //  __PROFILER_CALLBACK_H__

// End of File
        
        
