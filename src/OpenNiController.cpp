#include "OpenNiController.h"
#include "_2RealUtility.hpp"

OpenNiController::OpenNiController()
    :m_NumberOfDevices( 0 ),
     m_IsInitialized( false )

{

}

OpenNiController::~OpenNiController()
{

}

void OpenNiController::initializeController()
{
    if ( !m_IsInitialized )
    {
        m_IsInitialized = true;
        
        OpenNiDevice::checkError( m_Context.Init(), "\n_Couldn't  Initialize devices" );
        OpenNiDevice::checkError( m_Context.EnumerateProductionTrees( XN_NODE_TYPE_DEVICE, NULL, m_DeviceInfo, NULL ), "Error when enumerating devices" );

        size_t numDevs = 0;

         xn::NodeInfoList::Iterator deviceIter = m_DeviceInfo.Begin();
         for ( ; deviceIter!=m_DeviceInfo.End(); ++deviceIter )
         {
             NodeInfoRef devInfo = NodeInfoRef( new xn::NodeInfo( *deviceIter ) );
             OpenNiDevice dev( m_Context, devInfo);
             m_DeviceList.push_back( dev );
             m_NumberOfDevices+=1;
         }
         std::cout << "The number of connected devices is: " << m_NumberOfDevices << std::endl;
     }
}

xn::Context OpenNiController::getContext()
{
    return m_Context;
}

size_t OpenNiController::getNumberOfConnectedDevices()
{
    return m_NumberOfDevices;
}

void OpenNiController::createDeviceProductionTree( const size_t &deviceIdx )
{
    std::cout << "----------- Device " << deviceIdx << "------------" << std::endl;
    OpenNiDevice::checkError( m_Context.CreateProductionTree( m_DeviceList[ deviceIdx ].getNodeInfo() ), "_2Real: Error when creating production tree for device\n" );
}

//void openNiController::addAllDevicesToContext()
//{
//
//}

void OpenNiController::allignDepthImageToColor( const size_t &deviceIdx )
{

}

void OpenNiController::synchronizeImageAndDepthStreams( const size_t &deviceIdx )
{

}

void OpenNiController::startDeviceNode( const size_t &deviceIdx, const XnPredefinedProductionNodeType &imgType )
{

}

void OpenNiController::stopDeviceNode( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType )
{
    //Check if the production tree of the device is created


    //Check if the device supports the required generator


    //Check if the required generator is alreaady running


    //Stop the generator
}

ImageDataRef OpenNiController::getImageData( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType )
{




    //1. Is device connected?
    //2. Does node type support image data?

    //3. Is the generator running?

    //4. Grab data



    return ImageDataRef();
}

bool OpenNiController::isDeviceNodeRunning( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType ) const
{
    return false;
}

bool OpenNiController::containsDeviceNode( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType ) const
{
    return false;
}

bool OpenNiController::isInitialized() const
{
    return m_IsInitialized;
}


void OpenNiController::checkDeviceSupportForNode( const size_t &deviceIdx, const XnPredefinedProductionNodeType &nodeType ) const
{

}