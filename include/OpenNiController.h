#pragma once
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include "XnOpenNI.h"
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <iostream>
#include <string>
#include <vector>
#include "OpenNiDevice.h"


class OpenNiController 
{
public:
    OpenNiController();
    ~OpenNiController();

    xn::Context getContext();
    size_t getNumberOfConnectedDevices();

    void createDeviceProductionTree( const size_t &deviceIdx );

    void allignDepthImageToColor( const size_t &deviceIdx );
    void synchronizeImageAndDepthStreams( const size_t &deviceIdx );

    void startDeviceNode( const size_t &deviceIdx, const XnPredefinedProductionNodeType  &nodeType );
    void stopDeviceNode( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType );

    void initializeController();

    bool isInitialized() const;

    OpenNiDevice operator[]( const size_t &deviceIdx )
    {
        return m_DeviceList[ deviceIdx ];
    }

    virtual ImageDataRef getImageData( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType );

    //TODO: Look for circular references ( should we be using a weak_ptr? )

    std::vector< OpenNiDevice > m_DeviceList;

private:
    bool isDeviceNodeRunning( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType ) const;
    bool containsDeviceNode( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType ) const;
    
    void checkDeviceConnected( const size_t &deviceIdx ) const;
    void checkDeviceSupportForNode( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType ) const;

    
    
    xn::Context m_Context;
    size_t m_NumberOfDevices;
    bool m_IsInitialized;
    xn::NodeInfoList m_DeviceInfo;
    
};