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
    addProductionGraph( XN_NODE_TYPE_DEVICE );
}

void OpenNiDevice::addProductionGraph( const XnPredefinedProductionNodeType &nodeType )
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
        //m_Context.CreateProductionTree( node, chooseGenerator( nodeType ) );
        //productionNode.AddRef();
        //GeneratorInfoPair genPair = getEmptyGeneratorInfoPair( nodeType );
       // genPair.first = &productionNode;
        //m_GeneratorPairs.push_back( genPair );
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


xn::Generator& OpenNiDevice::chooseGenerator( const XnPredefinedProductionNodeType &nodeType )
{
    switch ( nodeType )
    {
    case XN_NODE_TYPE_IMAGE:
        return m_ImageGenerator;
        break;
    case XN_NODE_TYPE_DEPTH:
        return m_DepthGenerator;
        break;
    case XN_NODE_TYPE_USER:
        return m_UserGenerator;
        break;
    case XN_NODE_TYPE_IR:
        return m_IrGenerator;
        break;
    };
    throwError("Wrong NodeType was requested");
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