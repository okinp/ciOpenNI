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

    xn::Context& getContext();
    size_t const& getNumberOfConnectedDevices();

    void allignDepthImageToColor( const size_t &deviceIdx );
    void synchronizeImageAndDepthStreams( const size_t &deviceIdx );

    void initializeController();
    bool const isInitialized() ;
   // virtual ImageDataRef getImageData( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType );

    //TODO: Look for circular references ( should we be using a weak_ptr? )

    OpenNiDevice& operator[]( const size_t &deviceIdx )
    {
        return m_DeviceList[ deviceIdx ];
    }

private:
    

    virtual bool shutdown();
    
    xn::Context m_Context;
    xn::NodeInfoList m_DeviceInfo;
    size_t m_NumberOfDevices;

    bool m_IsInitialized;
    uint32_t  m_GeneratorConfig;
    uint32_t  m_ImageConfig;
    
    std::vector< OpenNiDevice > m_DeviceList;
};