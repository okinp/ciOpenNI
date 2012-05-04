#include "OpenNiController.h"
#include <sstream>
#include "_2RealUtility.hpp"
#include "_2RealImageSource.hpp"
#include "ciOpenNiUtils.hpp"

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
		
		checkError( m_Context.Init(), "\n_Couldn't  Initialize context" );
		checkError( m_Context.EnumerateProductionTrees( XN_NODE_TYPE_DEVICE, NULL, m_DeviceInfo, NULL ), "Error when enumerating devices" );

		xn::NodeInfoList::Iterator deviceIter = m_DeviceInfo.Begin();

		for ( ; deviceIter!=m_DeviceInfo.End(); ++deviceIter )
		{
			std::stringstream deviceName;
			deviceName << "Device_" << m_NumberOfDevices;
			NodeInfoRef devInfo = NodeInfoRef( new xn::NodeInfo( *deviceIter ) );
			OpenNiDevice dev( m_Context, devInfo,deviceName.str() );
			m_DeviceList.push_back( dev );
			dev.addDeviceToContext();
			m_NumberOfDevices+=1;
		}
		_2REAL_LOG(info) << "\n_2Real: Found and init " << (int)m_NumberOfDevices << " device[s]" << std::endl;
	}
}

void OpenNiController::configureDevice( size_t deviceIdx, uint32_t startGenerators, uint32_t configureImages )
{
	checkDeviceIndex( deviceIdx );
	m_GeneratorConfig = startGenerators;
	m_ImageConfig     = configureImages;
	
	std::vector<XnPredefinedProductionNodeType> requestedNodes =  getRequestedNodes( m_GeneratorConfig );
	for ( std::vector<XnPredefinedProductionNodeType>::iterator iter = requestedNodes.begin(); iter!=requestedNodes.end(); ++iter ) 
	{
		m_DeviceList[ deviceIdx ].addGenerator( *iter, m_ImageConfig );
	}
}

void OpenNiController::startGenerator( size_t deviceIdx, uint32_t configureGenerators )
{
	setGeneratorState( deviceIdx, configureGenerators, true );
}

void OpenNiController::stopGenerator( size_t deviceIdx, uint32_t configureGenerators )
{
   setGeneratorState( deviceIdx, configureGenerators, false );
}

void OpenNiController::setGeneratorState( size_t deviceIdx, uint32_t requestedGenerator, bool start )
{
	checkDeviceIndex( deviceIdx );
	//Check if generator exists
	std::vector<XnPredefinedProductionNodeType> requestedNodes = getRequestedNodes( requestedGenerator );
	
	if ( requestedNodes.size() == 0 ) 
	{
		return;
	}
	
	for ( std::vector<XnPredefinedProductionNodeType>::iterator iter = requestedNodes.begin(); iter!=requestedNodes.end(); ++iter ) 
	{
		if ( start ){
			m_DeviceList[ deviceIdx ].startGenerator(*iter);
		} else {
			m_DeviceList[ deviceIdx ].stopGenerator(*iter);
		}
		
	}
}

bool const OpenNiController::isInitialized() 
{
	return m_IsInitialized;
} 

xn::Context& OpenNiController::getContext()
{
	return m_Context;
}

size_t const& OpenNiController::getNumberOfConnectedDevices()
{
	return m_NumberOfDevices;
}

std::vector<XnPredefinedProductionNodeType> OpenNiController::getRequestedNodes( uint32_t startGenerators ) const
{
	std::vector<XnPredefinedProductionNodeType> XnRequestedNodeSet;
	
	if(  ( startGenerators & COLORIMAGE )  &&  ( startGenerators & INFRAREDIMAGE )  )
	{
		_2REAL_LOG(warn) << "_2Real: Cannot have color and infrared generators at same time!" << std::endl;
	}
	
	if( startGenerators & COLORIMAGE )
	{
		XnRequestedNodeSet.push_back( XN_NODE_TYPE_IMAGE );
	}
	
	if( startGenerators & DEPTHIMAGE )
	{
		XnRequestedNodeSet.push_back( XN_NODE_TYPE_DEPTH );
	}
	
	if( !( startGenerators & COLORIMAGE ) && startGenerators & INFRAREDIMAGE )
	{
		XnRequestedNodeSet.push_back( XN_NODE_TYPE_IR );
	}
	
	if( startGenerators & USERIMAGE || startGenerators & USERIMAGE_COLORED )
	{
		XnRequestedNodeSet.push_back( XN_NODE_TYPE_USER );
	}
	return XnRequestedNodeSet;
}


void OpenNiController::shutdown()
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
		return;
	}
	_2REAL_LOG(info) << std::endl << "_2Real: System not initialized..." << std::endl;
}

OpenNiDevice& OpenNiController::operator[]( const size_t deviceIdx  )
{
	return m_DeviceList[ deviceIdx ];
}


void OpenNiController::allignDepthImageToColor( const size_t &deviceIdx )
{
	
}

void OpenNiController::synchronizeImageAndDepthStreams( const size_t &deviceIdx )
{
	
}

void OpenNiController::setMirrored( const uint32_t deviceIdx, _2RealGenerator type, bool flag )
{
	checkDeviceIndex( deviceIdx );
	std::vector<XnPredefinedProductionNodeType> requestedNodes = getRequestedNodes( type );
	
	if ( requestedNodes.size() == 0 )
	{
		throwError( "_2Real::setMirrored() Error: wrong type of generator provided!");
	} else if ( requestedNodes.size() > 1 ) {
		_2REAL_LOG(warn) << "_2Real: setMirrored() doesn't accept combinations of _2RealGenerator types..." << std::endl;
		return;
	} 
	
	if ( requestedNodes[0] == XN_NODE_TYPE_USER )
		return;
	
	if( !m_DeviceList[ deviceIdx ].hasGenerator( requestedNodes[0] ) )
	{
		_2REAL_LOG(warn) << "_2Real: Cannot set mirror capability due to non activated generator..." << std::endl;
		return;
	}
	//GeneratorRef gen = m_DeviceList[ deviceIdx ].getExistingGenerator( requestedNodes[0] );
	//checkError( gen->GetMirrorCap().SetMirror( flag ), "Error when trying to set mirroring for image\n" );
}


bool OpenNiController::isMirrored( const uint32_t deviceIdx, _2RealGenerator type )
{
	checkDeviceIndex( deviceIdx );
	bool flag = true;
	std::vector<XnPredefinedProductionNodeType> requestedNodes = getRequestedNodes( type );
	
	if ( requestedNodes.size() == 0 )
	{
		throwError( "_2Real::setMirrored() Error: wrong type of generator provided!");
	} else if ( requestedNodes.size() > 1 ) {
		_2REAL_LOG(warn) << "_2Real: isMirrored() doesn't accept combinations of _2RealGenerator types..." << std::endl;
		return false;
	} 
	
	
	if( !m_DeviceList[ deviceIdx ].hasGenerator( requestedNodes[0] ) )
	{
		_2REAL_LOG(warn) << "_2Real: Cannot check mirror capability due to non activated generator..." << std::endl;
		return false;
	}
	
	if ( requestedNodes[0] != XN_NODE_TYPE_USER )
	{
	   // GeneratorRef gen = m_DeviceList[ deviceIdx ].getExistingGenerator( requestedNodes[0] );
	   // flag = gen->GetMirrorCap().IsMirrored();
	}
	return flag;
}

size_t const OpenNiController::getNumberOfUsers( const size_t deviceIdx ) const
{
	checkDeviceIndex( deviceIdx );
	if( !m_DeviceList[ deviceIdx ].hasGenerator( XN_NODE_TYPE_USER ) )
	{
		_2REAL_LOG(warn) << "_2Real: Cannot get number of users. User generator is not active..." << std::endl;
		return 0;
	}
//    GeneratorRef gen = m_DeviceList[ deviceIdx ].getExistingGenerator( XN_NODE_TYPE_USER );
 //   xn::Generator generator  = *gen;
  //  xn::UserGenerator user = static_cast< xn::UserGenerator >( generator );
  //  return user.GetNumberOfUsers();
	return 0;
}

ImageDataRef OpenNiController::getImageData( const uint32_t deviceID, _2RealGenerator type ) const
{
	checkDeviceIndex( deviceID );
	
	OpenNiDevice device = m_DeviceList[ deviceID ];
	
	std::vector<XnPredefinedProductionNodeType> requestedNodes = getRequestedNodes( type );
	
	if ( requestedNodes.size() > 1 )
	{
		std::cout << "The size is: " << requestedNodes.size() << std::endl; 
		throwError( "_2Real:: getImageData() Error: wrong type of generator provided!");
	} else if ( requestedNodes.size() == 0 )
	{
		throwError( "_2Real:: getImageData() Error: WTF!");
	}
	
	if ( !device.hasGenerator( requestedNodes[0] )  )
	{
		_2REAL_LOG(warn) << "_2Real: getImageData()  Generator is not activated! Cannot fetch image..." << std::endl;
		return boost::shared_array<unsigned char>(); //NULL shared array
	}
	boost::shared_array<unsigned char> imageBuffer = device.getBuffer( requestedNodes[0] );
	return imageBuffer;
}

void OpenNiController::updateContext()
{
	m_Context.WaitAnyUpdateAll();
}

void OpenNiController::checkDeviceIndex( uint8_t deviceIdx ) const
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

void OpenNiController::resetSkeleton( const uint32_t deviceID, const uint32_t id )
{
	checkDeviceIndex( deviceID );

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


