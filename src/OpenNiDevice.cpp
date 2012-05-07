#include "OpenNiDevice.h"
#include "_2RealUtility.hpp"
#include "_2RealTypes.h"
#include "ciOpenNiUtils.hpp"

using namespace  _2RealKinectWrapper;

OpenNiDevice::OpenNiDevice()
	:m_Context( xn::Context() ),
	 m_DeviceInfo( NodeInfoRef() ),
	 m_DeviceName("Device")
{

}

OpenNiDevice::OpenNiDevice( xn::Context context,  NodeInfoRef deviceInfo, std::string deviceName )
	:m_Context( context ),
	 m_DeviceInfo( deviceInfo ),
	 m_DeviceName( deviceName )
{

}

void OpenNiDevice::addDeviceToContext()
{
	xn::Device device;
    m_DeviceInfo->SetInstanceName( m_DeviceName.c_str() );
	
	//ToDo: There is a verified bug in the unstable version of OpenNi that prevents adding a user generator after a device.
	// for more info see:  http://groups.google.com/group/openni-dev/browse_thread/thread/96331acca1ed7ce3?pli=1
	//For this reason, we are not forcing the creation of a device node at start, but let it be handled by openni behind the scenes,
	// when a production node of the device is requested.

	checkError( m_Context.CreateProductionTree( *m_DeviceInfo, device ), " Error when creating production tree for device" );
    device.AddRef();
}

void OpenNiDevice::addGenerator( const XnPredefinedProductionNodeType &nodeType, uint32_t configureImages )
{
	if ( !hasGenerator( nodeType ) )
	{
		std::cout << "Adding generator for nodetype: " << xnNodeTypeToString(nodeType) << std::endl;
		xn::NodeInfoList nodeList;
		xn::Query query;
		query.AddNeededNode( m_DeviceInfo->GetInstanceName() );
		checkError( m_Context.EnumerateProductionTrees( nodeType, &query, nodeList, NULL ), "Error when enumerating production trees" );
		if ( nodeList.IsEmpty() )
		{
            _2RealKinectWrapper::throwError("Requested NodeType is not supported by the device");
		}
		xn::NodeInfo node = *nodeList.Begin();
		//Give a name to the generator
		std::string nodeName =  xnNodeTypeToString( nodeType ) + "_" + m_DeviceName;
		node.SetInstanceName( nodeName.c_str() );
		xn::Generator gen;
		checkError( m_Context.CreateProductionTree( node, gen ), "Error while creating production tree." );
		//ToDo: We shoud take care to release all the "AddRef's "
		gen.AddRef();
		if ( ( nodeType != XN_NODE_TYPE_DEVICE ) && ( nodeType != XN_NODE_TYPE_USER ) )
		{ 
            XnMapOutputMode mode = getRequestedOutputMode( nodeType, configureImages );
            xn::MapGenerator mapGenerator;
            getExistingProductionNode( nodeType, mapGenerator );
            checkError( mapGenerator.SetMapOutputMode( mode ), "_2Real: Error when setting outputmode \n" );
		}
	} else
	{
		std::cout << "Generator for nodetype: " << xnNodeTypeToString(nodeType) << " already exists." << std::endl;
	}
}

bool OpenNiDevice::hasGenerator( const XnPredefinedProductionNodeType &nodeType ) const
{
	xn::Query query;
	query.AddNeededNode( m_DeviceInfo->GetInstanceName() );
	xn::NodeInfoList nodeList;
	checkError( m_Context.EnumerateExistingNodes( nodeList, nodeType ), "Error Encountered while enumerating existing nodes" );
	nodeList.FilterList( m_Context, query );
	bool result = true;
	if ( nodeList.IsEmpty() )
	{
		result = false;
	}
	return result;
}

xn::NodeInfo OpenNiDevice::getNodeInfo()
{
	return *m_DeviceInfo;
}

xn::NodeInfoList OpenNiDevice::getNodeInfoList( const XnPredefinedProductionNodeType &nodeType  )
{
	xn::NodeInfoList nodeList;
	xn::Query query;
	query.AddNeededNode( m_DeviceInfo->GetInstanceName() );
	checkError( m_Context.EnumerateProductionTrees( nodeType, &query, nodeList, NULL ), "Error when enumerating production trees" );
	if ( nodeList.IsEmpty() )
	{
		_2RealKinectWrapper::throwError("Requested NodeType is not supported by the device");
	}
	return nodeList;
}

void OpenNiDevice::startGenerator( const XnPredefinedProductionNodeType &nodeType )
{
	xn::Generator generator;
	getExistingProductionNode( nodeType, generator );
	if ( !generator.IsGenerating() )
	{
		generator.StartGenerating();
	} else 
	{
		std::cout << "Nodetype: " << xnNodeTypeToString(nodeType) << " is already generating." << std::endl;
	}
}

void OpenNiDevice::stopGenerator( const XnPredefinedProductionNodeType &nodeType )
{
	xn::Generator generator;
	getExistingProductionNode( nodeType, generator );
	if ( generator.IsGenerating() )
	{
		generator.StopGenerating();
	} else 
	{
		std::cout << "Nodetype: " << xnNodeTypeToString(nodeType) << " is not generating." << std::endl;
	}
}

void OpenNiDevice::removeGenerator( const XnPredefinedProductionNodeType &nodeType )
{
	//ToDo: removeGenerator appears to not work. FixMe
	xn::Generator generator;
	getExistingProductionNode( nodeType, generator );
	generator.Release();
}

XnMapOutputMode OpenNiDevice::getRequestedOutputMode( const XnPredefinedProductionNodeType &nodeType, boost::uint32_t configureImages )
{
   XnMapOutputMode mode;
   mode.nFPS = 30;
   if ( nodeType == XN_NODE_TYPE_DEPTH )
   {
	   //configuring image size
	   if ( configureImages & IMAGE_USER_DEPTH_640X480 )
	   {
		   mode.nXRes = 640;
		   mode.nYRes = 480;
	   }
	   else if ( configureImages & IMAGE_USER_DEPTH_320X240 )
	   {
		   mode.nXRes = 320;
		   mode.nYRes = 240;
	   }
	   else if ( configureImages & IMAGE_USER_DEPTH_80X60 )
	   {
		   mode.nXRes = 80;
		   mode.nYRes = 60;
	   }
	   else //default
	   {
		   mode.nXRes = 640;
		   mode.nYRes = 480;
	   }

   } else if ( nodeType == XN_NODE_TYPE_IMAGE )
   {
	   if( configureImages & IMAGE_COLOR_1280X1024 )
	   {
		  /*
			XN_RES_QVGA
			XN_RES_VGA
			XN_RES_SXGA
			XN_RES_UXGA
			XnMapOutputMode Mode;
			pGenerator->GetMapOutputMode(Mode);
			Mode.nXRes = Resolution((XnResolution)XN_RES_UXGA).GetXResolution();
			Mode.nYRes = Resolution((XnResolution)XN_RES_UXGA).GetYResolution();
			XnStatus nRetVal = pGenerator->SetMapOutputMode(Mode);
		  */
		   mode.nXRes = 1280;
		   mode.nYRes = 1024;
	   }
	   else if( configureImages & IMAGE_COLOR_640X480 )
	   {
		   mode.nXRes = 640;
		   mode.nYRes = 480;
	   }
	   else if( configureImages & IMAGE_COLOR_320X240 )
	   {
		   mode.nXRes = 320;
		   mode.nYRes = 240;
	   }
	   else //default
	   {
		   mode.nXRes = 640;
		   mode.nYRes = 480;
	   }

   } else if ( nodeType == XN_NODE_TYPE_USER )
   {
	   if ( configureImages & IMAGE_USER_DEPTH_640X480 )
	   {
		   mode.nXRes = 640;
		   mode.nYRes = 480;
	   }
	   else if ( configureImages & IMAGE_USER_DEPTH_320X240 )
	   {
		   mode.nXRes = 320;
		   mode.nYRes = 240;
	   }
	   else if ( configureImages & IMAGE_USER_DEPTH_80X60 )
	   {
		   mode.nXRes = 80;
		   mode.nYRes = 60;
	   }
	   else //default
	   {
		   mode.nXRes = 640;
		   mode.nYRes = 480;
	   }

   } else if ( nodeType == XN_NODE_TYPE_IR )
   {
	   if ( configureImages & IMAGE_INFRARED_640X480 )
	   {
		   mode.nXRes = 640;
		   mode.nYRes = 480;
	   }
	   else if ( configureImages & IMAGE_INFRARED_320X240 )
	   {
		   mode.nXRes = 320;
		   mode.nYRes = 240;
	   }
	   else //default
	   {
		   mode.nXRes = 640;
		   mode.nYRes = 480;
	   }
   } else {

	   throwError(" Requested node type does not support an output mode ");
   }
	mode.nFPS = 30;
   return mode;
}

void OpenNiDevice::getExistingProductionNode( const XnPredefinedProductionNodeType &nodeType, xn::ProductionNode& productionNode )
{
    std::string nodeName =  xnNodeTypeToString( nodeType ) + "_" + m_DeviceName;
	checkError( m_Context.GetProductionNodeByName(nodeName.c_str(), productionNode), " Requested production node has not been created" );
}

bool OpenNiDevice::hasNewData( const XnPredefinedProductionNodeType &nodeType )
{
	xn::Generator generator;
	getExistingProductionNode( nodeType, generator );
	XnBool newData = generator.IsDataNew();
	if ( newData )
		return true;
	return false;
}

ImageDataRef OpenNiDevice::getBuffer( const XnPredefinedProductionNodeType &nodeType )
{
	ImageDataRef buffer;
	if ( nodeType == XN_NODE_TYPE_IMAGE && hasGenerator( XN_NODE_TYPE_IMAGE ) )
	{
		xn::ImageGenerator imageGenerator;
		getExistingProductionNode(XN_NODE_TYPE_IMAGE, imageGenerator );
		xn::ImageMetaData imgMeta;
		imageGenerator.GetMetaData( imgMeta );
		//int xres = imgMeta.FullXRes();
		//int yres = imgMeta.FullYRes();
		buffer = ImageDataRef( (unsigned char*) imageGenerator.GetImageMap(), the_null_deleter()); 
	}
	else if ( nodeType == XN_NODE_TYPE_DEPTH && hasGenerator( XN_NODE_TYPE_DEPTH ) )
	{
		ImageData16Ref buffer16 = getBuffer16(XN_NODE_TYPE_DEPTH );
		buffer = ImageDataRef( new unsigned char[640*480]);
		convertImage_16_to_8(buffer16, buffer, 640*480, _2REAL_OPENNI_DEPTH_NORMALIZATION_16_TO_8 );
	} 
	else if ( nodeType == XN_NODE_TYPE_IR && hasGenerator( XN_NODE_TYPE_IR ) )
	{
		xn::IRGenerator irGenerator;
		getExistingProductionNode(XN_NODE_TYPE_IR, irGenerator );
		buffer = ImageDataRef( (unsigned char*) irGenerator.GetIRMap(), the_null_deleter() );
	} 
	else if ( nodeType == XN_NODE_TYPE_USER && hasGenerator( XN_NODE_TYPE_USER ) )
	{
		xn::UserGenerator userGenerator;
		getExistingProductionNode( XN_NODE_TYPE_USER, userGenerator );
		xn::SceneMetaData sceneMeta ;
		userGenerator.GetUserPixels( 0, sceneMeta );
		buffer = ImageDataRef( (unsigned char*) sceneMeta.Data(), the_null_deleter() );
	} 
	else 
	{
		throwError("_2Real: Requested node type does not produce image data or doesn't exist ");
	}
	return  buffer;
}

ImageData16Ref OpenNiDevice::getBuffer16( const XnPredefinedProductionNodeType &nodeType )
{
	ImageData16Ref buffer;//( new uint16_t[640*480]);
	if ( nodeType == XN_NODE_TYPE_DEPTH && hasGenerator( XN_NODE_TYPE_DEPTH ) )
	{
		xn::DepthGenerator depthGenerator;
		getExistingProductionNode( XN_NODE_TYPE_DEPTH, depthGenerator );
		xn::DepthMetaData g_depthMD;
		depthGenerator.GetMetaData( g_depthMD );
		const XnDepthPixel* depthData = g_depthMD.Data();
		buffer = ImageData16Ref( (uint16_t*) depthData, the_null_deleter() );
	}
	else if ( nodeType == XN_NODE_TYPE_IR && hasGenerator( XN_NODE_TYPE_IR ) )
	{
		xn::IRGenerator irGenerator;
		getExistingProductionNode( XN_NODE_TYPE_IR, irGenerator );
		buffer = ImageData16Ref( (uint16_t*) irGenerator.GetIRMap(), the_null_deleter() );
	}
	else 
	{
		throwError("_2Real: Requested node type does not produce 16bit image data or doesn't exist ");
	}
	return  buffer;
}

void OpenNiDevice::convertImage_16_to_8( const ImageData16Ref source, ImageDataRef destination, uint32_t size, const int normalizing )
{
	for( unsigned int i=0; i<size; ++i )
	{
		destination[i] = (unsigned char) ( source[i] * ( (float)( 1 << 8 ) / normalizing ) ); 	
	}
}

void OpenNiDevice::convertRealWorldToProjective( XnUInt32 count, 		const XnPoint3D  	aRealWorld[], XnPoint3D  	aProjective[] )
{
	xn::DepthGenerator depthGen;
	if ( !hasGenerator(XN_NODE_TYPE_DEPTH) )
	{
		//Lets create the depthGenerator
	} else {
		//Lets use the already existing one
	}
	depthGen.ConvertRealWorldToProjective(count, aRealWorld, aProjective );
}

void OpenNiDevice::convertProjectiveToRealWorld( XnUInt32 count, 		const XnPoint3D  	aProjective[], XnPoint3D  	aRealWorld[] )
{
	xn::DepthGenerator depthGen;
	if ( !hasGenerator(XN_NODE_TYPE_DEPTH) )
	{
		//Lets create the depthGenerator
	} else {
		//Lets use the already existing one
	}
	depthGen.ConvertProjectiveToRealWorld(count, aProjective, aRealWorld );
}


std::string OpenNiDevice::xnNodeTypeToString( const XnPredefinedProductionNodeType& nodeType )
{
	std::string nodeTypeString;
	switch( nodeType ) {
	case XN_NODE_TYPE_INVALID:
		nodeTypeString = "XN_NODE_TYPE_INVALID";
		break;
	case XN_NODE_TYPE_DEVICE:
		nodeTypeString = "XN_NODE_TYPE_DEVICE";
		break;
	case XN_NODE_TYPE_DEPTH:
		nodeTypeString = "XN_NODE_TYPE_DEPTH";
		break;
	case XN_NODE_TYPE_IMAGE:
		nodeTypeString = "XN_NODE_TYPE_IMAGE";
		break;
	case XN_NODE_TYPE_AUDIO:
		nodeTypeString = "XN_NODE_TYPE_AUDIO";
		break;
	case XN_NODE_TYPE_IR:
		nodeTypeString = "XN_NODE_TYPE_IR";
		break;
	case XN_NODE_TYPE_USER:
		nodeTypeString = "XN_NODE_TYPE_USER";
		break;
	case XN_NODE_TYPE_RECORDER:
		nodeTypeString = "XN_NODE_TYPE_RECORDER";
		break;
	case XN_NODE_TYPE_PLAYER:
		nodeTypeString = "XN_NODE_TYPE_PLAYER";
		break;
	case XN_NODE_TYPE_GESTURE:
		nodeTypeString = "XN_NODE_TYPE_GESTURE";
		break;
	case XN_NODE_TYPE_SCENE:
		nodeTypeString = "XN_NODE_TYPE_SCENE";
		break;
	case XN_NODE_TYPE_HANDS:
		nodeTypeString = "XN_NODE_TYPE_HANDS";
		break;
	case XN_NODE_TYPE_CODEC:
		nodeTypeString = "XN_NODE_TYPE_CODEC";
		break;
	case XN_NODE_TYPE_PRODUCTION_NODE:
		nodeTypeString = "XN_NODE_TYPE_PRODUCTION_NODE";
		break;
	case XN_NODE_TYPE_GENERATOR:
		nodeTypeString = "XN_NODE_TYPE_GENERATOR";
		break;
	case XN_NODE_TYPE_MAP_GENERATOR:
		nodeTypeString = "XN_NODE_TYPE_MAP_GENERATOR";
		break;
	case XN_NODE_TYPE_SCRIPT:
		nodeTypeString = "XN_NODE_TYPE_SCRIPT";
		break;
	default: 
		throw std::logic_error(__FILE__ ": enum En out of range");
		break;
	}
	return nodeTypeString;
}

