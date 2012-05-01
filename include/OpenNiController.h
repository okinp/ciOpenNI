#pragma once
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include "XnOpenNI.h"
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <iostream>
#include <string>
#include <vector>
#include "_2RealTypes.h"
#include "OpenNiDevice.h"

using namespace _2RealKinectWrapper;

class OpenNiController 
{
public:
    OpenNiController();
    ~OpenNiController();

    xn::Context& getContext();
    size_t const& getNumberOfConnectedDevices();

    void allignDepthImageToColor( const size_t &deviceIdx );
    void synchronizeImageAndDepthStreams( const size_t &deviceIdx );
    
    void initializeController();
    bool const isInitialized() ;

    //OpenNIImplementation

    //This doesn't start but adds the specified generators for the device;
    virtual bool start( size_t deviceIdx, uint32_t startGenerators, uint32_t configureImages );
    
    virtual bool startGenerator( size_t deviceIdx, uint32_t startGenerator );
    virtual bool stopGenerator( size_t deviceIdx, uint32_t stopGenerator );
    
    virtual void setMirrored( const uint32_t deviceIdx, _2RealGenerator type, bool flag );
    virtual bool isMirrored( const uint32_t deviceID, _2RealGenerator type );
    virtual bool isJointAvailable( _2RealJointType type ) const;
    virtual const bool isNewData(const uint32_t deviceID, _2RealGenerator type);
    virtual void resetSkeleton( const uint32_t deviceID, const uint32_t id );
    virtual void resetAllSkeletons();
    
    size_t const getNumberOfUsers( const size_t deviceIdx ) const;
    
    
    virtual ImageDataRef getImageData( const uint32_t deviceID, _2RealGenerator type ) const;
    
    void updateContext();
    
    OpenNiDevice& operator[]( const size_t deviceIdx );
private:
    bool m_IsInitialized;
    
    size_t           m_NumberOfDevices;
    xn::Context      m_Context;
    xn::NodeInfoList m_DeviceInfo;
    
    uint32_t  m_ImageConfig;
    uint32_t  m_GeneratorConfig;

    std::vector< OpenNiDevice > m_DeviceList;

    void checkDeviceRunning(uint8_t deviceIdx ) const;
    bool setGeneratorState( size_t deviceIdx, uint32_t requestedGenerator, bool start );
    std::vector<XnPredefinedProductionNodeType> getRequestedNodes( uint32_t startGenerators ) const; 
    virtual bool shutdown();
};