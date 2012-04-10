#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <XnCppWrapper.h>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

//sensor reaches up to 10 meters in OpenNI
#define _2REAL_OPENNI_DEPTH_NORMALIZATION_16_TO_8 10000

typedef boost::shared_array< unsigned char >      ImageDataRef;
typedef boost::shared_ptr< xn::NodeInfo >         NodeInfoRef;
typedef boost::shared_ptr< xn::Generator >        GeneratorRef;
//We don't need this. We could use the SetIntPorperty and set it to xn::XnPredefinedProductionNodeType
typedef std::pair< GeneratorRef, XnPredefinedProductionNodeType > GeneratorInfoPair;

class OpenNiDevice
{
 public:
    OpenNiDevice();
    OpenNiDevice( xn::Context context,  NodeInfoRef deviceInfo );
    ~OpenNiDevice();

    void addDeviceToContext();

    void addProductionGraph( const XnPredefinedProductionNodeType &nodeType, boost::uint32_t configureImages );
    bool hasProductionGraph( const XnPredefinedProductionNodeType &nodeType );

    xn::NodeInfo getNodeInfo();
    xn::NodeInfoList getNodeInfoList( const XnPredefinedProductionNodeType &nodeType  );

    void addGeneratorGraphs( boost::uint32_t startGenerators, boost::uint32_t configureImages );

    void startGenerator( const XnPredefinedProductionNodeType &nodeType );
    void stopGenerator( const XnPredefinedProductionNodeType &nodeType );
    
    void removeGenerator( const XnPredefinedProductionNodeType &nodeType );

    bool hasNewData();

    //boost::shared_array<uint16_t>						getDepthBuffer_16bit();
    boost::shared_array<uint16_t>                       getBuffer16( const XnPredefinedProductionNodeType &nodeType );
    boost::shared_array<unsigned char>					getBuffer( const XnPredefinedProductionNodeType &nodeType );

    static void checkError( const XnStatus &status, const std::string &strError );

    GeneratorInfoPair getExistingGeneratorInfoPair( const XnPredefinedProductionNodeType &nodeType );
    GeneratorInfoPair getEmptyGeneratorInfoPair( const XnPredefinedProductionNodeType &nodeType ); 
    
    std::vector< GeneratorInfoPair > m_GeneratorPairs;

    XnMapOutputMode getRequestedOutputMode( const XnPredefinedProductionNodeType &nodeType, boost::uint32_t configureImages );
       
private:
    xn::Context m_Context;
    NodeInfoRef m_DeviceInfo;
    void convertImage_16_to_8( const boost::shared_array<uint16_t> source, boost::shared_array<unsigned char> destination, uint32_t size, const int normalizing );


   
    
};