#include "OpenNiController.h"
#include "_2RealUtility.hpp"

using namespace _2RealKinectWrapper;

OpenNiController::OpenNiController()
    :m_NumberOfDevices( 0 ),
     m_IsInitialized( false ),
     m_GeneratorConfig( CONFIG_DEFAULT ),
     m_ImageConfig( IMAGE_CONFIG_DEFAULT )

{

}

OpenNiController::~OpenNiController()
{
    shutdown();
}

void OpenNiController::initializeController()
{
    if ( !m_IsInitialized )
    {
        m_IsInitialized = true;
        
        OpenNiDevice::checkError( m_Context.Init(), "\n_Couldn't  Initialize devices" );
        OpenNiDevice::checkError( m_Context.EnumerateProductionTrees( XN_NODE_TYPE_DEVICE, NULL, m_DeviceInfo, NULL ), "Error when enumerating devices" );

         xn::NodeInfoList::Iterator deviceIter = m_DeviceInfo.Begin();
         for ( ; deviceIter!=m_DeviceInfo.End(); ++deviceIter )
         {
             NodeInfoRef devInfo = NodeInfoRef( new xn::NodeInfo( *deviceIter ) );
             OpenNiDevice dev( m_Context, devInfo);
             m_DeviceList.push_back( dev );
             m_NumberOfDevices+=1;
         }
         _2REAL_LOG(info) << "\n_2Real: Found and init " << (int)m_NumberOfDevices << " device[s]" << std::endl;
     }
}

xn::Context& OpenNiController::getContext()
{
    return m_Context;
}

size_t const& OpenNiController::getNumberOfConnectedDevices()
{
    return m_NumberOfDevices;
}


void OpenNiController::allignDepthImageToColor( const size_t &deviceIdx )
{

}

void OpenNiController::synchronizeImageAndDepthStreams( const size_t &deviceIdx )
{

}



bool const OpenNiController::isInitialized() 
{
    return m_IsInitialized;
}

bool OpenNiController::shutdown()
{
    //freeing memory
    if( m_IsInitialized )
    {
        m_IsInitialized = false;
        _2REAL_LOG(info) << "_2Real: Shutting down system...";
        //unlocking and stopping all generators
        for ( size_t i = 0; i < m_NumberOfDevices; ++i )
        {
            //m_DeviceList[i]->shutdown();
        }
        //releasing context object
        m_Context.StopGeneratingAll();
        m_Context.Release();

        // delete devices, no further deletes needed share_ptrs make the work
        m_DeviceList.clear();

        m_NumberOfDevices = 0;

        _2REAL_LOG(info) << "OK" << std::endl;
        return true;
    }
    _2REAL_LOG(info) << std::endl << "_2Real: System not initialized..." << std::endl;
    return false;
}