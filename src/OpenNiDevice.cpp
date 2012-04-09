#include "OpenNiDevice.h"
#include "_2RealUtility.hpp"
#include "_2RealTypes.h"


using namespace  _2RealKinectWrapper;
OpenNiDevice::OpenNiDevice()
    :m_Context( xn::Context() ),
     m_DeviceInfo( NodeInfoRef() )
{

}

OpenNiDevice::OpenNiDevice( xn::Context context,  NodeInfoRef deviceInfo )
    :m_Context( context ),
     m_DeviceInfo( deviceInfo )
{

}

OpenNiDevice::~OpenNiDevice()
{

}

void OpenNiDevice::addDeviceToContext()
{
    checkError( m_Context.CreateProductionTree( *m_DeviceInfo ), " Error when creating production tree for device" );
}

void OpenNiDevice::addProductionGraph( const XnPredefinedProductionNodeType &nodeType, boost::uint32_t configureImages )
{
    if ( !hasProductionGraph( nodeType ) )
    {
        xn::NodeInfoList nodeList;
        xn::Query query;
        query.AddNeededNode( m_DeviceInfo->GetInstanceName() );
        std::cout << m_DeviceInfo->GetInstanceName() << std::endl;
        GeneratorInfoPair gen = getEmptyGeneratorInfoPair( nodeType );
        checkError( m_Context.EnumerateProductionTrees( nodeType, &query, nodeList, NULL ), "Error when enumerating production trees" );

        if ( nodeList.IsEmpty() )
        {
            _2RealKinectWrapper::throwError("Requested NodeType is not supported by the device");
        }

        xn::NodeInfo node = *nodeList.Begin();
        xn::ProductionNode productionNode;
        checkError( m_Context.CreateProductionTree( node, *gen.first ), "Error while creating production tree." );
        m_GeneratorPairs.push_back( gen );

//         
//         //
//         if ( ( nodeType != XN_NODE_TYPE_DEVICE ) && ( nodeType != XN_NODE_TYPE_USER ) )
//         {
//             std::cout << "Addding something" << std::endl;
//             XnMapOutputMode mode = getRequestedOutputMode( nodeType, IMAGE_CONFIG_DEFAULT );
//             //setting output mode
//             xn::MapGenerator *mGen = static_cast<xn::MapGenerator*>( (gen.first).get() );
//             checkError( mGen->SetMapOutputMode( mode ), "_2Real: Error when setting outputmode \n" );
//         }
    }
}


bool OpenNiDevice::hasProductionGraph( const XnPredefinedProductionNodeType &nodeType )
{
    xn::Query query;
    query.AddNeededNode( m_DeviceInfo->GetInstanceName() );
    xn::NodeInfoList nodeList;
    checkError( m_Context.EnumerateExistingNodes( nodeList, nodeType ), "Error Encountered while enumerating existing nodes" );
    nodeList.FilterList( m_Context, query );
    if ( nodeList.IsEmpty() )
    {
        return false;
    }
    return true;
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

void OpenNiDevice::addGeneratorGraphs( boost::uint32_t startGenerators, boost::uint32_t configureImages )
{
    if( startGenerators & COLORIMAGE )
    {
        xn::NodeInfoList list = getNodeInfoList( XN_NODE_TYPE_IMAGE );
        //addProductiongGraph( nodeInfo, XN_NODE_TYPE_IMAGE );
    }
 
    if( startGenerators & DEPTHIMAGE )
    {
        xn::NodeInfoList  list = getNodeInfoList( XN_NODE_TYPE_DEPTH );
        //addProductionGraph( nodeInfo, XN_NODE_TYPE_DEPTH );
    }

    if( startGenerators & COLORIMAGE && startGenerators & INFRAREDIMAGE )
        _2REAL_LOG(warn) << "_2Real: Cannot start color and infraredgenerator at same time!" << std::endl;


    if( !( startGenerators & COLORIMAGE ) && startGenerators & INFRAREDIMAGE )
    {
        xn::NodeInfoList list = getNodeInfoList( XN_NODE_TYPE_IR );
        //addProductionGraph( nodeInfo, XN_NODE_TYPE_IR );
    }        

    if( startGenerators & USERIMAGE || startGenerators & USERIMAGE_COLORED )
    {
        xn::NodeInfoList list = getNodeInfoList( XN_NODE_TYPE_USER );
        //addProductionGraph( nodeInfo, XN_NODE_TYPE_USER );
    }
}   


void OpenNiDevice::startGenerator( const XnPredefinedProductionNodeType &nodeType )
{
//    GeneratorInfoPair genInfo = getExistingGeneratorInfoPairForNodeType( nodeType );
//    if ( genInfo.second != XN_NODE_TYPE_INVALID )
//    {
//        std::cout << "Invalid Generator Requested"  <<  std::endl;
//        return;
//    }
//    genInfo.first->StartGenerating();
}

void OpenNiDevice::stopGenerator( const XnPredefinedProductionNodeType &nodeType )
{
 //   GeneratorInfoPair genInfo = getExistingGeneratorInfoPairForNodeType( nodeType );
 //   if ( genInfo.second != XN_NODE_TYPE_INVALID )
 //   {
 //       std::cout << "Generator requested is either invalid or doesn't exist"  <<  std::endl;
 //       return;
 //   }
 //   genInfo.first->StopGenerating();
}

void OpenNiDevice::removeGenerator( const XnPredefinedProductionNodeType &nodeType )
{

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
   return mode;
}

GeneratorInfoPair OpenNiDevice::getExistingGeneratorInfoPair( const XnPredefinedProductionNodeType &nodeType ) 
{
    for ( std::vector< GeneratorInfoPair>::iterator iter = m_GeneratorPairs.begin(); iter!=m_GeneratorPairs.end(); ++iter )
     {
         if ( iter->second == nodeType )
         {
             return *iter;
         }
     }

     GeneratorInfoPair genInfo;
     genInfo.first  = GeneratorRef( new xn::Generator() );
     genInfo.second = XN_NODE_TYPE_INVALID;
     return genInfo;
 }
 
GeneratorInfoPair OpenNiDevice::getEmptyGeneratorInfoPair( const XnPredefinedProductionNodeType &nodeType ) 
{
     GeneratorInfoPair genInfo;
     switch ( nodeType )
     {
     case XN_NODE_TYPE_IMAGE:
         genInfo.first  =  GeneratorRef( new xn::ImageGenerator() );
         genInfo.second = nodeType;
         break;
     case XN_NODE_TYPE_DEPTH:
         genInfo.first  = GeneratorRef( new xn::DepthGenerator() );
         genInfo.second = nodeType;
         break;
     case XN_NODE_TYPE_USER:
         genInfo.first  = GeneratorRef( new xn::UserGenerator() );
         genInfo.second = nodeType;
         break;
     case XN_NODE_TYPE_IR:
         genInfo.first  = GeneratorRef( new xn::IRGenerator() );
         genInfo.second = nodeType;
         break;
     default:
         genInfo.first  = GeneratorRef( new xn::Generator() );
         genInfo.second = XN_NODE_TYPE_INVALID;
     }
     return genInfo;
}

void OpenNiDevice::checkError( const XnStatus &status, const std::string &strError )
{
    if ( status != XN_STATUS_OK )
    {
        std::cout << strError << " " << xnGetStatusString( status ) << std::endl;
    }
}

bool OpenNiDevice::hasNewData()
{
    //Need to implement this properly
    return true;

}

boost::shared_array<unsigned char> OpenNiDevice::getBuffer( const XnPredefinedProductionNodeType &nodeType )
{
    boost::shared_array< unsigned char > buffer;//( new unsigned char[640*480] );
    if ( nodeType == XN_NODE_TYPE_IMAGE && hasProductionGraph( XN_NODE_TYPE_IMAGE ) )
    {
        GeneratorInfoPair gen = getExistingGeneratorInfoPair( XN_NODE_TYPE_IMAGE );
        xn::Generator generator  = *(gen.first);
        xn::ImageGenerator img = static_cast< xn::ImageGenerator >( generator );
        ImageMetaData imgMeta;
        img.GetMetaData( imgMeta );
        int xres = imgMeta.FullXRes();
        int yres = imgMeta.FullYRes();
        buffer = boost::shared_array<unsigned char >( new unsigned char[ xres * yres ], null_deleter() );
        //buffer = boost::shared_array< unsigned char>( (unsigned char*) img.GetImageMap(), null_deleter()); 
    }
    else if ( nodeType == XN_NODE_TYPE_DEPTH && hasProductionGraph( XN_NODE_TYPE_DEPTH ) )
    {
        boost::shared_array<uint16_t> buffer16 = getBuffer16(XN_NODE_TYPE_DEPTH );
        boost::shared_array<unsigned char>
        convertImage_16_to_8(buffer16, buffer, 640*480, _2REAL_OPENNI_DEPTH_NORMALIZATION_16_TO_8 );
        //buffer = boost::shared_array< unsigned char>( (unsigned char*) depth.GetDepthMap(), null_deleter()); 
    } 

    else if ( nodeType == XN_NODE_TYPE_IR && hasProductionGraph( XN_NODE_TYPE_IR ) )
    {
        GeneratorInfoPair gen = getExistingGeneratorInfoPair( XN_NODE_TYPE_IR );
        xn::Generator generator  = *(gen.first);
        xn::IRGenerator ir = static_cast< xn::IRGenerator >( generator );
        buffer = boost::shared_array< unsigned char>( (unsigned char*) ir.GetIRMap(), null_deleter() );
    } 
    else if ( nodeType == XN_NODE_TYPE_USER && hasProductionGraph( XN_NODE_TYPE_USER ) )
    {
        GeneratorInfoPair gen = getExistingGeneratorInfoPair( XN_NODE_TYPE_IR );
        //xn::UserGenerator user = static_cast< xn::UserGenerator >( gen );
        //buffer = boost::shared_array< unsigned char>( (unsigned char*) user.GetUserMap() );
    } 
    else 
    {

        throwError("_2Real: Requested node type does not produce image data or doesn't exist ");
    }
    return  buffer;
}

boost::shared_array<uint16_t> OpenNiDevice::getBuffer16( const XnPredefinedProductionNodeType &nodeType )
{
    boost::shared_array<uint16_t> buffer;
    if ( nodeType == XN_NODE_TYPE_DEPTH && hasProductionGraph( XN_NODE_TYPE_DEPTH ) )
    {
        GeneratorInfoPair gen = getExistingGeneratorInfoPair( XN_NODE_TYPE_DEPTH );
        xn::Generator generator = *(gen.first);
        xn::DepthGenerator depth = static_cast< xn::DepthGenerator >( generator );
        buffer = boost::shared_array< uint16_t >( (uint16_t*) depth.GetDepthMap(), null_deleter());
    }
    else if ( nodeType == XN_NODE_TYPE_IR && hasProductionGraph( XN_NODE_TYPE_IR ) )
    {
        GeneratorInfoPair gen = getExistingGeneratorInfoPair( XN_NODE_TYPE_IR );
        xn::Generator generator  = *(gen.first);
        xn::IRGenerator ir = static_cast< xn::IRGenerator >( generator );
        buffer = boost::shared_array< uint16_t >( (uint16_t*) ir.GetIRMap(), null_deleter() );
    }
    else 
    {

        throwError("_2Real: Requested node type does not produce 16bit image data or doesn't exist ");
    }
    return  buffer;
}

void OpenNiDevice::convertImage_16_to_8( const boost::shared_array<uint16_t> source, boost::shared_array<unsigned char> destination, uint32_t size, const int normalizing )
{
    //iterating each pixel and writing normalized pixel data
    for( unsigned int i=0; i<size; ++i )
        destination[i] = (unsigned char) ( source[i] * ( (float)( 1 << 8 ) / normalizing ) ); //normalized 16bit to 8bit
}
