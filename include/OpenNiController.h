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

typedef std::vector< OpenNiDevice > DeviceContainer;
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
	bool const isInitialized();

	void configureDevice( size_t deviceIdx, uint32_t startGenerators, uint32_t configureImages );
	
	void startGenerator( size_t deviceIdx, uint32_t configureGenerators );
	void stopGenerator( size_t deviceIdx, uint32_t configureGenerators );
	
	void setMirrored( const uint32_t deviceIdx, _2RealGenerator type, bool flag );
	bool isMirrored( const uint32_t deviceID, _2RealGenerator type );
	bool isJointAvailable( _2RealJointType type ) const;
	void resetSkeleton( const uint32_t deviceID, const uint32_t id );
	void resetAllSkeletons();
	
	size_t const getNumberOfUsers( const size_t deviceIdx ) const;
	ImageDataRef getImageData( const uint32_t deviceID, _2RealGenerator type ) const;
	
	void updateContext();
	
	OpenNiDevice& operator[]( const size_t deviceIdx );

private:
	xn::Context			m_Context;
	xn::NodeInfoList	m_DeviceInfo;
	DeviceContainer		m_DeviceList;
	size_t				m_NumberOfDevices;

	bool				m_IsInitialized;

	uint32_t			m_ImageConfig;
	uint32_t			m_GeneratorConfig;

	void checkDeviceIndex(uint8_t deviceIdx ) const;
	void setGeneratorState( size_t deviceIdx, uint32_t requestedGenerator, bool start );
	std::vector<XnPredefinedProductionNodeType> getRequestedNodes( uint32_t startGenerators ) const; 
	void shutdown();
};