#include "OpenNiDevice.h"
#include "_2RealUtility.hpp"
#include "_2RealTypes.h"
#include "ciOpenNiUtils.hpp"

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

void OpenNiDevice::addDeviceToContext()
{
    checkError( m_Context.CreateProductionTree( *m_DeviceInfo ), " Error when creating production tree for device" );
}

void OpenNiDevice::addGenerator( const XnPredefinedProductionNodeType &nodeType, uint32_t configureImages )
{
    if ( !hasGenerator( nodeType ) )
    {
        xn::NodeInfoList nodeList;
        xn::Query query;
        query.AddNeededNode( m_DeviceInfo->GetInstanceName() );
        std::cout << m_DeviceInfo->GetInstanceName() << std::endl;
        checkError( m_Context.EnumerateProductionTrees( nodeType, &query, nodeList, NULL ), "Error when enumerating production trees" );

        if ( nodeList.IsEmpty() )
        {
            _2RealKinectWrapper::throwError("Requested NodeType is not supported by the device");
        }

        xn::NodeInfo node = *nodeList.Begin();
        xn::Generator gen;
        checkError( m_Context.CreateProductionTree( node, gen ), "Error while creating production tree." );
        
        if ( ( nodeType != XN_NODE_TYPE_DEVICE ) && ( nodeType != XN_NODE_TYPE_USER ) )
        { 
            std::cout << "Addding something" << std::endl;
            //XnMapOutputMode mode = getRequestedOutputMode( nodeType, IMAGE_CONFIG_DEFAULT );
            XnMapOutputMode mode = getRequestedOutputMode( nodeType, configureImages );
            //setting output mode
            xn::MapGenerator mGen = static_cast<xn::MapGenerator>( gen );
            checkError( mGen.SetMapOutputMode( mode ), "_2Real: Error when setting outputmode \n" );
         }
    }
}

bool OpenNiDevice::hasGenerator( const XnPredefinedProductionNodeType &nodeType ) const
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

void OpenNiDevice::startGenerator( const XnPredefinedProductionNodeType &nodeType )
{
    GeneratorRef generator = getExistingGenerator( nodeType );
    //xn::Generator 
    if (!generator )
    {
        std::cout << "Generator requested to start is invalid / doesn't exist"  <<  std::endl;
        return;
    }
    generator->StartGenerating();
}

void OpenNiDevice::stopGenerator( const XnPredefinedProductionNodeType &nodeType )
{
    GeneratorRef generator = getExistingGenerator( nodeType );
    //xn::Generator 
    if (!generator )
    {
        std::cout << "Generator requested to stop is invalid / doesn't exist"  <<  std::endl;
        return;
    }
    generator->StopGenerating();
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
    mode.nFPS = 30;
   return mode;
}

GeneratorRef OpenNiDevice::getExistingGenerator( const XnPredefinedProductionNodeType &nodeType ) const
{
//    for ( std::vector< GeneratorRef >::iterator iter = m_Generators.begin(); iter!=m_Generators.end(); ++iter )
//    {
//        if ( iter->GetIntProperty == xnNodeTypeToInt(nodeType) )
//        {
//            return *iter;
//        }
//    }
    
    
    GeneratorRef gen( new xn::Generator() );
    return gen;
}



bool OpenNiDevice::hasNewData()
{
    //Need to implement this properly
    return true;

}

ImageDataRef OpenNiDevice::getBuffer( const XnPredefinedProductionNodeType &nodeType )
{
    ImageDataRef buffer;
    if ( nodeType == XN_NODE_TYPE_IMAGE && hasGenerator( XN_NODE_TYPE_IMAGE ) )
    {
        GeneratorRef generator = getExistingGenerator( XN_NODE_TYPE_IMAGE );
        //xn::Generator generator  = *(gen.first);
        xn::ImageGenerator img = static_cast< xn::ImageGenerator >( *generator );
        xn::ImageMetaData imgMeta;
        img.GetMetaData( imgMeta );
        int xres = imgMeta.FullXRes();
        int yres = imgMeta.FullYRes();
        buffer = ImageDataRef( (unsigned char*) img.GetImageMap(), the_null_deleter()); 
    }
    else if ( nodeType == XN_NODE_TYPE_DEPTH && hasGenerator( XN_NODE_TYPE_DEPTH ) )
    {
        ImageData16Ref buffer16 = getBuffer16(XN_NODE_TYPE_DEPTH );
        buffer = ImageDataRef( new unsigned char[640*480]);
        convertImage_16_to_8(buffer16, buffer, 640*480, _2REAL_OPENNI_DEPTH_NORMALIZATION_16_TO_8 );
    } 
    else if ( nodeType == XN_NODE_TYPE_IR && hasGenerator( XN_NODE_TYPE_IR ) )
    {
        GeneratorRef irGenerator = getExistingGenerator( XN_NODE_TYPE_IR );
        xn::IRGenerator ir = static_cast< xn::IRGenerator >( *irGenerator );
        buffer = ImageDataRef( (unsigned char*) ir.GetIRMap(), the_null_deleter() );
    } 
    else if ( nodeType == XN_NODE_TYPE_USER && hasGenerator( XN_NODE_TYPE_USER ) )
    {
        GeneratorRef userGenerator = getExistingGenerator( XN_NODE_TYPE_USER );
        xn::UserGenerator user = static_cast< xn::UserGenerator >( *userGenerator );
        xn::SceneMetaData sceneMeta ;
        user.GetUserPixels( 0, sceneMeta );
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
        GeneratorRef depthGenerator = getExistingGenerator( XN_NODE_TYPE_DEPTH );
        xn::DepthGenerator depth    = static_cast< xn::DepthGenerator >( *depthGenerator );
        xn::DepthMetaData g_depthMD;
        depth.GetMetaData( g_depthMD );
        const XnDepthPixel* depthData = g_depthMD.Data();
        buffer = ImageData16Ref( (uint16_t*) depthData, the_null_deleter() );
    }
    else if ( nodeType == XN_NODE_TYPE_IR && hasGenerator( XN_NODE_TYPE_IR ) )
    {
        GeneratorRef irGenerator = getExistingGenerator( XN_NODE_TYPE_IR );
        xn::IRGenerator ir = static_cast< xn::IRGenerator >( *irGenerator );
        buffer = ImageData16Ref( (uint16_t*) ir.GetIRMap(), the_null_deleter() );
    }
//    else if ( nodeType == XN_NODE_TYPE_USER && hasProductionGraph( XN_NODE_TYPE_USER ) )
//    {
//        GeneratorInfoPair gen = getExistingGeneratorInfoPair( XN_NODE_TYPE_USER );
//        xn::Generator generator  = *(gen.first);
//        xn::UserGenerator user = static_cast< xn::UserGenerator >( generator );
//        xn::SceneMetaData sceneMeta ;
//        user.GetUserPixels( 0, sceneMeta );
//        buffer = boost::shared_array< uint16_t >( (uint16_t*) sceneMeta.Data(), the_null_deleter() ); 
//    }
    else 
    {
        throwError("_2Real: Requested node type does not produce 16bit image data or doesn't exist ");
    }
    return  buffer;
}

void OpenNiDevice::convertImage_16_to_8( const ImageData16Ref source, ImageDataRef destination, uint32_t size, const int normalizing )
{
    //iterating each pixel and writing normalized pixel data
   // std::cout << "-------------------------------------------------------------------"<< std::endl;
    for( unsigned int i=0; i<size; ++i )
    {
        destination[i] = (unsigned char) ( source[i] * ( (float)( 1 << 8 ) / normalizing ) ); 
        //normalized 16bit to 8bit
//        if (i < 100 ) {
//            std::cout << "the pixel value is: " << int(destination[i]) << std::endl;
//        }
        
    }
}

bool OpenNiDevice::generatorIsOfType( GeneratorRef generator, XnPredefinedProductionNodeType nodeType )
{
    XnUInt64 nValue;
    generator->GetIntProperty("nodeType", nValue);
    if ( nValue ==  xnNodeTypeToInt( nodeType ) ) 
    {
        return true;
    }
    return false;
}

void OpenNiDevice::setGeneratorType( GeneratorRef generator ,XnPredefinedProductionNodeType nodeType )
{
    generator->SetIntProperty ("nodeType", xnNodeTypeToInt(nodeType) );
}

int OpenNiDevice::xnNodeTypeToInt( XnPredefinedProductionNodeType nodeType )
{
    switch( nodeType ) {
        case XN_NODE_TYPE_DEPTH: return 2;
        case XN_NODE_TYPE_IMAGE: return 3;
        case XN_NODE_TYPE_IR: return 5;
        case XN_NODE_TYPE_USER: return 6;
        default: throw std::logic_error(__FILE__ ": enum En out of range");
    }
}
XnPredefinedProductionNodeType OpenNiDevice::intToXnNodeType( int nodeType )
{
    switch( nodeType ) {
        case 2: return XN_NODE_TYPE_DEPTH;
        case 3: return XN_NODE_TYPE_IMAGE;
        case 5: return XN_NODE_TYPE_IR;
        case 6: return XN_NODE_TYPE_USER;
        default: throw std::logic_error(__FILE__ ": int out of range for enum");
    }  
}
