#ifndef __QY_HTREAD_H__
#define __QY_HTREAD_H__

#include "qyutil/qyconfig.h"
#include "qyutil/qyctypes.h"

DEFINE_NAMESPACE(qy)

enum eThreadEvent
{
    kTRunning = 1, ///< runing ...
    kTStoped = 2, ///< call stop
};

enum eThreadResult
{
    kTResNone = 1,
    kTResExit = 2,
    kTResHook = 3, ///< thread hook,and Need to call QyThread::waitUp()
};

// Object that will be passed by the spawned thread when it enters the callback
// function.
#define QyThreadObj void*

// Callback function that the spawned thread will enter once spawned.
// A return value of false is interpreted as that the function has no
// more work to do and that the thread can be released.
typedef int (*QyThreadRunFunction)(QyThreadObj, eThreadEvent e);

enum eThreadPriority {
    kLowPriority = 1,
    kNormalPriority = 2,
    kHighPriority = 3,
    kHighestPriority = 4,
    kRealtimePriority = 5
};

#if defined(WIN32)
typedef unsigned int QyThreadId;
#else
#include <pthread.h>
typedef pthread_t    QyThreadId;
#endif

const int kThreadMaxNameLength = 64;

class QYUTIL_API QyThread {
public:
    virtual ~QyThread() {};
    
    // Factory method. Constructor disabled.
    //
    // func        Pointer to a, by user, specified callback function.
    // obj         Object associated with the thread. Passed in the callback
    //             function.
    // prio        Thread priority. May require root/admin rights.
    // thread_name  NULL terminated thread name, will be visable in the Windows
    //             debugger.
    static QyThread* createThread(QyThreadRunFunction func,
                                  QyThreadObj obj,
                                  eThreadPriority prio = kNormalPriority,
                                  const char* thread_name = 0);
    
    // Get the current thread's kernel thread ID.
    static QyThreadId currentThreadId();
    
    // Non blocking termination of the spawned thread. Note that it is not safe
    // to delete this class until the spawned thread has been reclaimed.
    virtual void setNotAlive() = 0;
    
    // Tries to spawns a thread and returns true if that was successful.
    // Additionally, it tries to set thread priority according to the priority
    // from when CreateThread was called. However, failure to set priority will
    // not result in a false return value.
    // TODO(henrike): add a function for polling whether priority was set or
    //                not.
    virtual bool start() = 0;
    
    // Stops the spawned thread and waits for it to be reclaimed with a timeout
    // of two seconds. Will return false if the thread was not reclaimed.
    // Multiple tries to Stop are allowed (e.g. to wait longer than 2 seconds).
    // It's ok to call Stop() even if the spawned thread has been reclaimed.
    virtual bool stop(int msec = 2000) = 0;
    
    virtual void waitUp() = 0;
    // Sets the threads CPU affinity. CPUs are listed 0 - (number of CPUs - 1).
    // The numbers in processor_numbers specify which CPUs are allowed to run the
    // thread. processor_numbers should not contain any duplicates and elements
    // should be lower than (number of CPUs - 1). amount_of_processors should be
    // equal to the number of processors listed in processor_numbers.
    virtual bool setAffinity(const int* processor_numbers,
                             const unsigned int amount_of_processors) {
        return false;
    }
    
    virtual QyThreadId threadId() const = 0;
};

END_NAMESPACE(qy)

#endif  // __QY_HTREAD_H__
