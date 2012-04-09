#include "OpenNiController.h"
#include "_2RealUtility.hpp"
#include "_2RealImageSource.hpp"

#define _2REAL_NUMBER_OF_JOINTS 24

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
             dev.addDeviceToContext();
             m_NumberOfDevices+=1;
         }
         _2REAL_LOG(info) << "\n_2Real: Found and init " << (int)m_NumberOfDevices << " device[s]" << std::endl;
     }
}
// NO
void OpenNiController::addDeviceToContext( const size_t &deviceIdx )
{
    if ( deviceIdx < m_NumberOfDevices )
    {
        m_DeviceList[ deviceIdx ].addDeviceToContext();
    } else {
        throwError("Device index out of range");
    }
}
// YES
bool OpenNiController::start( size_t deviceIdx, boost::uint32_t startGenerators, boost::uint32_t configureImages )
{
    checkDeviceRunning( deviceIdx );
    if( startGenerators & COLORIMAGE )
    {
        m_DeviceList[ deviceIdx ].addProductionGraph( XN_NODE_TYPE_IMAGE, configureImages );
        //XnMapOutputMode mode = m_DeviceList[ deviceIdx ].getRequestedOutputMode( XN_NODE_TYPE_IMAGE, configureImages );
    }
    if( startGenerators & DEPTHIMAGE )
    {
        m_DeviceList[ deviceIdx ].addProductionGraph( XN_NODE_TYPE_DEPTH, configureImages );
        //XnMapOutputMode mode = m_DeviceList[ deviceIdx ].getRequestedOutputMode( XN_NODE_TYPE_DEPTH, configureImages );
    }

    if( ( m_GeneratorConfig & COLORIMAGE ) && ( m_GeneratorConfig & INFRAREDIMAGE ) )
        _2REAL_LOG(warn) << "_2Real: Cannot start color and infrared generator at same time!" << std::endl;

    if( !( m_GeneratorConfig & COLORIMAGE ) && m_GeneratorConfig & INFRAREDIMAGE )
    {
        m_DeviceList[ deviceIdx ].addProductionGraph( XN_NODE_TYPE_IR, configureImages );
        //XnMapOutputMode mode = m_DeviceList[ deviceIdx ].getRequestedOutputMode( XN_NODE_TYPE_IR, configureImages );
    }
    if( m_GeneratorConfig & USERIMAGE || m_GeneratorConfig & USERIMAGE_COLORED )
    {
        m_DeviceList[ deviceIdx ].addProductionGraph( XN_NODE_TYPE_USER, configureImages );
        //XnMapOutputMode mode = m_DeviceList[ deviceIdx ].getRequestedOutputMode( XN_NODE_TYPE_USER, configureImages );
    }
    return true;
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

OpenNiDevice& OpenNiController::operator[]( const size_t deviceIdx  )
{
    return m_DeviceList[ deviceIdx ];
}

void OpenNiController::setMirrored( const uint32_t deviceIdx, _2RealGenerator type, bool flag )
{
    checkDeviceRunning( deviceIdx );
    GeneratorInfoPair gen;
    switch( type )
    {
        case COLORIMAGE:
            if( !m_DeviceList[ deviceIdx ].hasProductionGraph( XN_NODE_TYPE_IMAGE ) )
            {
                _2REAL_LOG(warn) << "_2Real: Cannot set mirror capability due to non activated color generator..." << std::endl;
                return;
            }
            gen = m_DeviceList[ deviceIdx ].getExistingGeneratorInfoPair( XN_NODE_TYPE_IMAGE );
            OpenNiDevice::checkError( (gen.first)->GetMirrorCap().SetMirror( flag ), "Error when trying to set mirroring for color image\n" );
            break;
        case DEPTHIMAGE:
            if( !m_DeviceList[ deviceIdx ].hasProductionGraph( XN_NODE_TYPE_DEPTH ) )
            {
                _2REAL_LOG(warn) << "_2Real: Cannot set mirror capability due to non activated depth generator..." << std::endl;
                return;
            }
            gen = m_DeviceList[ deviceIdx ].getExistingGeneratorInfoPair( XN_NODE_TYPE_DEPTH );
            OpenNiDevice::checkError( (gen.first)->GetMirrorCap().SetMirror( flag ), "Error when trying to set mirroring for depth image\n" );
            break;
        case INFRAREDIMAGE:
            if( !m_DeviceList[ deviceIdx ].hasProductionGraph( XN_NODE_TYPE_IR ) )
            {
                _2REAL_LOG(warn) << "_2Real: Cannot set mirror capability due non activated infrared generator..." << std::endl;
                return;
            }
            gen = m_DeviceList[ deviceIdx ].getExistingGeneratorInfoPair( XN_NODE_TYPE_IR );
            OpenNiDevice::checkError( (gen.first)->GetMirrorCap().SetMirror( flag ), "Error when trying to set mirroring for IR image\n" );
            break;
        case USERIMAGE_COLORED:
        case USERIMAGE:
            break;
        default:
            throwError( "_2RealKinectWrapper::setMirrored() Error: Wrong type of generator assigned?!" );
    }
}

bool OpenNiController::isMirrored( const uint32_t deviceIdx, _2RealGenerator type )
{
    checkDeviceRunning( deviceIdx );
    GeneratorInfoPair gen;
    bool flag = true;
    switch( type )
    {
        case COLORIMAGE:
            if( !m_DeviceList[ deviceIdx ].hasProductionGraph( XN_NODE_TYPE_IMAGE ) )
            {
                _2REAL_LOG(warn) << "_2Real: Cannot check mirroring state on a non activated color generator..." << std::endl;
                return false;
            }
            gen = m_DeviceList[ deviceIdx ].getExistingGeneratorInfoPair( XN_NODE_TYPE_IMAGE );
            flag = (gen.first)->GetMirrorCap().IsMirrored();
            break;
        case DEPTHIMAGE:
            if( !m_DeviceList[ deviceIdx ].hasProductionGraph( XN_NODE_TYPE_DEPTH ) )
            {
                _2REAL_LOG(warn) << "_2Real: Cannot check mirroring state on a non activated depth generator..." << std::endl;
                return false;
            }
            gen = m_DeviceList[ deviceIdx ].getExistingGeneratorInfoPair( XN_NODE_TYPE_DEPTH );
            flag = (gen.first)->GetMirrorCap().IsMirrored();
            break;
        case INFRAREDIMAGE:
            if( !m_DeviceList[ deviceIdx ].hasProductionGraph( XN_NODE_TYPE_IR ) )
            {
                _2REAL_LOG(warn) << "_2Real: Cannot check mirroring state on a non activated infrared generator..." << std::endl;
                return false;
            }
            gen = m_DeviceList[ deviceIdx ].getExistingGeneratorInfoPair( XN_NODE_TYPE_IR );
            flag = (gen.first)->GetMirrorCap().IsMirrored();
            break;
        case USERIMAGE_COLORED:
        case USERIMAGE:
            break;
        default:
            throwError( "_2RealKinectWrapper::setMirrored() Error: Wrong type of generator assigned?!" );
    }
    return flag;
}

void OpenNiController::checkDeviceRunning( uint8_t deviceIdx ) const
{
    //checking id
    if ( deviceIdx > m_NumberOfDevices - 1 )
    {
        throwError("_2Real: Error, deviceID out of bounds!\n");
    }
}

bool OpenNiController::isJointAvailable( _2RealJointType type ) const
{
    if( int( type ) < 0 || int( type ) > _2REAL_NUMBER_OF_JOINTS - 1 )
        throwError( "_2Real: isJointAvailable() error, joint id out of bounds!" );

    switch( int( type ) )
    {
    case JOINT_LEFT_COLLAR:
    case JOINT_LEFT_WRIST:
    case JOINT_LEFT_FINGERTIP:
    case JOINT_RIGHT_COLLAR:
    case JOINT_RIGHT_WRIST:
    case  JOINT_RIGHT_FINGERTIP:
    case JOINT_LEFT_ANKLE:
    case JOINT_RIGHT_ANKLE:
    case JOINT_WAIST:
        return false;
    default:
        return true;
    }
}

const bool OpenNiController::isNewData( const uint32_t deviceID, _2RealGenerator type )
{
    checkDeviceRunning( deviceID );
    return m_DeviceList[ deviceID ].hasNewData( );
}

void OpenNiController::resetSkeleton( const uint32_t deviceID, const uint32_t id )
{
    checkDeviceRunning( deviceID );

//     if ( m_Device[deviceID]->m_UserGenerator.isGenerating() )
//     {
//        // m_Devices[deviceID]->m_UserGenerator.forceResetUser( id );
//     }
}

void OpenNiController::resetAllSkeletons()
{
    for ( uint32_t i = 0; i < m_NumberOfDevices; ++i )
    {
//         if ( m_Devices[ i ]->m_UserGenerator.isGenerating() )
//         {
//          //   m_Devices[ i ]->m_UserGenerator.forceResetUsers();
//         }
    }
}

boost::shared_array<unsigned char> OpenNiController::getImageData( const uint32_t deviceID, _2RealGenerator type, bool waitAndBlock, const uint8_t userId )
{
    checkDeviceRunning( deviceID );

    bool colorUserImage = false;
    //refreshing user data
    //boost::shared_ptr< OpenNIDevice > device = m_Devices[deviceID];
    OpenNiDevice device = m_DeviceList[ deviceID ];
    switch( type )
    {
    case COLORIMAGE:
        if ( !device.hasProductionGraph( XN_NODE_TYPE_IMAGE)  )
        {
            _2REAL_LOG(warn) << "_2Real: getImageData() color generator is not activated! Cannot fetch image..." << std::endl;
            return boost::shared_array<unsigned char>();
        }
        //fetching new image data
        std::cout << " image data requested" << std::endl;
        return device.getBuffer( XN_NODE_TYPE_IMAGE );
        break;
    case DEPTHIMAGE:
        if( !device.hasProductionGraph( XN_NODE_TYPE_DEPTH ) )
        {
            _2REAL_LOG(warn) << "_2Real: getImageData() depth generator is not activated! Cannot fetch image..." << std::endl;
            return boost::shared_array<unsigned char>();
        }
        //std::cout << " depth data requested" << std::endl;
        return device.getBuffer( XN_NODE_TYPE_DEPTH );
        break;
    case INFRAREDIMAGE:
        if( !device.hasProductionGraph( XN_NODE_TYPE_IR ) )
        {
            _2REAL_LOG(warn) << "_2Real: getImageData() infrared generator is not activated! Cannot fetch image..." << std::endl;
            return boost::shared_array<unsigned char>();
        }
        std::cout << " ir data requested" << std::endl;
        return device.getBuffer( XN_NODE_TYPE_IR );
        break;
    case USERIMAGE_COLORED:
        colorUserImage = true;
        break;
    case USERIMAGE:
        return boost::shared_array<unsigned char>();
//         {
//             if( !device.hasProductionGraph( XN_NODE_TYPE_USER )
//             {
//                 _2REAL_LOG(warn) << "_2Real: getImageData() user generator is not activated! Cannot fetch image..." << std::endl;
//                 return boost::shared_array<unsigned char>();
//             }
//             if( colorUserImage )
//                 return device->getUserColorImageBuffer();
//             else
//                 return device->getUserImageBuffer();
//         }
        break;
    default:
        throwError( "_2Real: getImageData() Error: Wrong type of generator assigned?!" );
    }
    return boost::shared_array<unsigned char>();
}

// boost::shared_array<uint16_t> OpenNiController::getImageDataDepth16Bit( const uint32_t deviceID, bool waitAndBlock/*=false */ )
// {
//     checkDeviceRunning(deviceID);
//     if( !m_Devices[deviceID]->GetOpenNIDepthGenerator().IsValid() )
//     {
//         _2REAL_LOG(warn) << "_2Real: getDepthImageData16Bit() depth generator is not activated! Cannot fetch image..." << std::endl;
//         return boost::shared_array<uint16_t>();
//     }
// 
//     return m_Devices[deviceID]->getDepthBuffer_16bit();
// }
