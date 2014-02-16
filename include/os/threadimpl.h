/*! \file      thread.h
    \author    HiddenSeeker

    Contains crossplatform thread implementation,  
	which uses pthread on Linux and WinAPI on Windows 
*/
#pragma once

#ifdef WIN32
    #ifndef NOMINMAX
    #define NOMINMAX 
    #endif
    #include  <windows.h>
#else
    #include  <pthread.h>
#endif

namespace sad
{

class AbsractThreadExecutableFunction;

namespace os
{

/*! A platform-dependent implementation of thread, which uses WinAPI on Win32 and
	pthreads on linux
 */
class ThreadImpl
{
public:
	/*! Constructs new implementation for thread
		\param[in] f function, which should be executed when thread is run
	 */
	ThreadImpl(sad::AbsractThreadExecutableFunction * f);
	/*! Copies a thread implementation
		\param[in] o other implementation
	 */
	ThreadImpl(const sad::os::ThreadImpl & o);
	/*! Closes handle, if need to
	 */
	~ThreadImpl();
	/*! Runs a thread
		\return true if succeeded, otherwise false
	 */
	bool run();
	/*! Stops a thread
	 */
	void stop();
	/*! Blocks execution and tries to fetch result of execution of thread
	 */
	int exitCode() const;
	/*! Blocks execution and waits until thread is stopped
	 */
	void wait();
	/*! Blocks execution and waits for specified time, until thread is finished with execution,
		or time is expired
	 */
	void wait(unsigned int milliseconds);
	/*! Tests, whether thread  is finished
		\param[in] whether thread finished his job
	 */
	bool running() const;
protected:
	/*! Defines executed code
	 */
	sad::AbsractThreadExecutableFunction * m_function;
#ifdef WIN32
	/*! A handle, working with thread
	 */
	HANDLE m_handle;
#else
	/*! A handle, working with thread
	 */
	pthread_t m_handle;
#endif
private:
	/*! DO NOT USE! Not implemented
		\param[in] o other object
		\return self-reference
	 */
	sad::os::ThreadImpl & operator=(const sad::os::ThreadImpl & o);
};

}

}