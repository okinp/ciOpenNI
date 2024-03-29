#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <XnCppWrapper.h>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <memory>
#include "OpenNiUserController.h"

//sensor reaches up to 10 meters in OpenNI
#define _2REAL_OPENNI_DEPTH_NORMALIZATION_16_TO_8 10000

typedef boost::shared_array< unsigned char > ImageDataRef;
typedef boost::shared_array< uint16_t >      ImageData16Ref;
typedef boost::shared_ptr< xn::NodeInfo >    NodeInfoRef;
typedef boost::shared_ptr< xn::Generator >   GeneratorRef;

//We don't need this. We could use the SetIntPorperty and set it to xn::XnPredefinedProductionNodeType
typedef std::pair< GeneratorRef, XnPredefinedProductionNodeType > GeneratorInfoPair;

class OpenNiDevice
{
 public:
	OpenNiDevice();
	OpenNiDevice( xn::Context context,  NodeInfoRef deviceInfo, std::string deviceName );
	
	void addDeviceToContext();

	void addGenerator( const XnPredefinedProductionNodeType &nodeType, uint32_t configureImages );
	void removeGenerator( const XnPredefinedProductionNodeType &nodeType );

	bool hasGenerator( const XnPredefinedProductionNodeType &nodeType ) const;
	void startGenerator( const XnPredefinedProductionNodeType &nodeType );
	void stopGenerator( const XnPredefinedProductionNodeType &nodeType );
	
	bool hasNewData( const XnPredefinedProductionNodeType &nodeType );

	XnSkeletonJointTransformation	getSkeletonJoint( XnUInt16 userIdx, XnSkeletonJoint jointType );
	size_t getNumberOfUsers() const;
	
	ImageDataRef       getBuffer( const XnPredefinedProductionNodeType &nodeType );
	ImageData16Ref     getBuffer16( const XnPredefinedProductionNodeType &nodeType );

	void convertRealWorldToProjective( XnUInt32 count, 		const XnPoint3D  	aRealWorld[], XnPoint3D  	aProjective[] );
	void convertProjectiveToRealWorld( XnUInt32 count, 		const XnPoint3D  	aProjective[], XnPoint3D  	aRealWorld[] );

	xn::NodeInfo     getNodeInfo();
	xn::NodeInfoList getNodeInfoList( const XnPredefinedProductionNodeType &nodeType  );
    
	void getExistingProductionNode( const XnPredefinedProductionNodeType &nodeType, xn::ProductionNode& productionNode );

    static std::string xnNodeTypeToString( const XnPredefinedProductionNodeType& nodeType );
private:
	mutable xn::Context m_Context;
	boost::shared_ptr< OpenNiUserController > m_UserControllerRef;
	std::string m_DeviceName;
	NodeInfoRef m_DeviceInfo;

	void convertImage_16_to_8( const boost::shared_array<uint16_t> source, boost::shared_array<unsigned char> destination, uint32_t size, const int normalizing );
	XnMapOutputMode getRequestedOutputMode( const XnPredefinedProductionNodeType &nodeType, uint32_t configureImages );


};



