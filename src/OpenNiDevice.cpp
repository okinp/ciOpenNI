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

void OpenNiDevice::addProductionGraph()
{
    addProductionGraph( XN_NODE_TYPE_DEVICE, 0 );
}

void OpenNiDevice::addProductionGraph( const XnPredefinedProductionNodeType &nodeType, boost::uint32_t configureImages )
{
    if ( !hasProductionGraph( nodeType ) )
    {
        xn::NodeInfoList nodeList;
        xn::Query query;
        query.AddNeededNode( m_DeviceInfo->GetInstanceName() );
        GeneratorInfoPair gen = getEmptyGeneratorInfoPair( nodeType );
        checkError( m_Context.EnumerateProductionTrees( nodeType, &query, nodeList, NULL ), "Error when enumerating production trees" );
        if ( nodeList.IsEmpty() )
        {
            _2RealKinectWrapper::throwError("Requested NodeType is not supported by the device");
        }
        xn::NodeInfo node = *nodeList.Begin();
        xn::ProductionNode productionNode;
        m_Context.CreateProductionTree( node, *gen.first );
        m_GeneratorPairs.push_back( gen );
        
        if ( ( nodeType != XN_NODE_TYPE_DEVICE ) && ( nodeType != XN_NODE_TYPE_USER ) )
        {
            XnMapOutputMode mode = getRequestedOutputMode( nodeType, configureImages );
            //setting output mode
            xn::MapGenerator *mGen = static_cast<xn::MapGenerator*>( (gen.first).get() );
            checkError( mGen->SetMapOutputMode( mode ), "_2Real: Error when setting outputmode \n" );
        }
    }
}

bool  OpenNiDevice::hasProductionGraph()
{
    return hasProductionGraph( XN_NODE_TYPE_DEVICE );
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

// boost::shared_array<unsigned char> OpenNiDevice::getBuffer( const XnPredefinedProductionNodeType &nodeType )
// {
//     if ( nodeType == XN_NODE_TYPE_IMAGE )
//     {
// 
//     } else if ( nodeType == XN_NODE_TYPE_DEPTH )
//     {
//          
//     } else if ( nodeType == XN_NODE_TYPE_IR )
//     {
// 
//     } else if ( nodeType == XN_NODE_TYPE_USER )
//     {
// 
//     } else {
// 
//         throwError("_2Real: Requested node type does not produce image data or doesn't exist ");
//     }
// 
// }