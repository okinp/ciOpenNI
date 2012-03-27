#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <XnCppWrapper.h>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

typedef boost::shared_array< unsigned char >      ImageDataRef;
typedef boost::shared_ptr< xn::NodeInfo >         NodeInfoRef;
typedef boost::shared_ptr< xn::Generator >        GeneratorRef;
typedef std::pair< GeneratorRef, XnPredefinedProductionNodeType > GeneratorInfoPair;

class OpenNiDevice
{
public:
    OpenNiDevice();
    OpenNiDevice( xn::Context context,  NodeInfoRef deviceInfo );
    ~OpenNiDevice();

    void addProductionGraph();
    bool hasProductionGraph(); 

    void addProductionGraph( const XnPredefinedProductionNodeType &nodeType );
    bool hasProductionGraph( const XnPredefinedProductionNodeType &nodeType );

    xn::NodeInfo getNodeInfo();
    xn::NodeInfoList getNodeInfoList( const XnPredefinedProductionNodeType &nodeType  );

    void addGeneratorGraphs( size_t startGenerators, size_t configureImages );

    void startGenerator( const XnPredefinedProductionNodeType &nodeType );
    void stopGenerator( const XnPredefinedProductionNodeType &nodeType );
    void removeGenerator( const XnPredefinedProductionNodeType &nodeType );

    static void checkError( const XnStatus &status, const std::string &strError );

     GeneratorInfoPair getExistingGeneratorInfoPair( const XnPredefinedProductionNodeType &nodeType );
     GeneratorInfoPair getEmptyGeneratorInfoPair( const XnPredefinedProductionNodeType &nodeType ); 
    
    xn::DepthGenerator       m_DepthGenerator;
    xn::IRGenerator          m_IrGenerator;
    xn::ImageGenerator       m_ImageGenerator;
    xn::UserGenerator        m_UserGenerator;

    xn::Generator& chooseGenerator( const XnPredefinedProductionNodeType &nodeType );
    std::vector< GeneratorInfoPair > m_GeneratorPairs;

private:
    xn::Context m_Context;
    NodeInfoRef m_DeviceInfo;
    
};