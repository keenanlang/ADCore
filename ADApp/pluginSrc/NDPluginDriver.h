#ifndef NDPluginDriver_H
#define NDPluginDriver_H

#include <epicsTypes.h>
#include <epicsMessageQueue.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsTime.h>

#include "asynNDArrayDriver.h"

#define NDPluginDriverArrayPortString           "NDARRAY_PORT"          /**< (asynOctet,    r/w) The port for the NDArray interface */
#define NDPluginDriverArrayAddrString           "NDARRAY_ADDR"          /**< (asynInt32,    r/w) The address on the port */
#define NDPluginDriverPluginTypeString          "PLUGIN_TYPE"           /**< (asynOctet,    r/o) The type of plugin */
#define NDPluginDriverDroppedArraysString       "DROPPED_ARRAYS"        /**< (asynInt32,    r/w) Number of dropped arrays */
#define NDPluginDriverQueueSizeString           "QUEUE_SIZE"            /**< (asynInt32,    r/w) Total queue elements */ 
#define NDPluginDriverQueueFreeString           "QUEUE_FREE"            /**< (asynInt32,    r/w) Free queue elements */
#define NDPluginDriverNumThreadsString          "NUM_THREADS"           /**< (asynInt32,    r/w) Number of threads */ 
#define NDPluginDriverEnableCallbacksString     "ENABLE_CALLBACKS"      /**< (asynInt32,    r/w) Enable callbacks from driver (1=Yes, 0=No) */
#define NDPluginDriverBlockingCallbacksString   "BLOCKING_CALLBACKS"    /**< (asynInt32,    r/w) Callbacks block (1=Yes, 0=No) */
#define NDPluginDriverProcessPluginString       "PROCESS_PLUGIN"        /**< (asynInt32,    r/w) Process plugin with last callback array */
#define NDPluginDriverExecutionTimeString       "EXECUTION_TIME"        /**< (asynFloat64,  r/o) The last execution time (milliseconds) */
#define NDPluginDriverMinCallbackTimeString     "MIN_CALLBACK_TIME"     /**< (asynFloat64,  r/w) Minimum time between calling processCallbacks 
                                                                         *  to execute plugin code */

/** Class from which actual plugin drivers are derived; derived from asynNDArrayDriver */
class epicsShareClass NDPluginDriver : public asynNDArrayDriver, public epicsThreadRunable {
public:
    NDPluginDriver(const char *portName, int queueSize, int blockingCallbacks, 
                   const char *NDArrayPort, int NDArrayAddr, int maxAddr, int numParams,
                   int maxBuffers, size_t maxMemory, int interfaceMask, int interruptMask,
                   int asynFlags, int autoConnect, int priority, int stackSize, int maxThreads=1);
    ~NDPluginDriver();

    /* These are the methods that we override from asynNDArrayDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars,
                          size_t *nActual);
    virtual asynStatus readInt32Array(asynUser *pasynUser, epicsInt32 *value,
                                        size_t nElements, size_t *nIn);
                                     
    /* These are the methods that are new to this class */
    virtual void driverCallback(asynUser *pasynUser, void *genericPointer);
    virtual void run(void);
    virtual asynStatus start(void);

protected:
    virtual void processCallbacks(NDArray *pArray);
    virtual asynStatus connectToArrayPort(void);    
    virtual asynStatus setArrayInterrupt(int connect);

protected:
    int NDPluginDriverArrayPort;
    #define FIRST_NDPLUGIN_PARAM NDPluginDriverArrayPort
    int NDPluginDriverArrayAddr;
    int NDPluginDriverPluginType;
    int NDPluginDriverDroppedArrays;
    int NDPluginDriverQueueSize;
    int NDPluginDriverQueueFree;
    int NDPluginDriverNumThreads;
    int NDPluginDriverEnableCallbacks;
    int NDPluginDriverBlockingCallbacks;
    int NDPluginDriverProcessPlugin;
    int NDPluginDriverExecutionTime;
    int NDPluginDriverMinCallbackTime;

private:
    void processTask();
    asynStatus createCallbackThreads();
    asynStatus startCallbackThreads();
    asynStatus deleteCallbackThreads();
     
    /* The asyn interfaces we access as a client */
    void *asynGenericPointerInterruptPvt_;

    /* Our data */
    int maxThreads_;
    int numThreads_;
    bool pluginStarted_;
    int threadStackSize_;
    asynUser *pasynUserGenericPointer_;          /**< asynUser for connecting to NDArray driver */
    void *asynGenericPointerPvt_;                /**< Handle for connecting to NDArray driver */
    asynGenericPointer *pasynGenericPointer_;    /**< asyn interface for connecting to NDArray driver */
    bool connectedToArrayPort_;
    std::vector<epicsThread*>pThreads_;
    epicsMessageQueue *pToThreadMsgQ_;
    epicsMessageQueue *pFromThreadMsgQ_;
    epicsTimeStamp lastProcessTime_;
    int dimsPrev_[ND_ARRAY_MAX_DIMS];
    NDArray *pInputArray_;
};

    
#endif
