#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "OpenNiController.h"
#include "OpenNiDevice.h"
#include "_2RealTypes.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace _2RealKinectWrapper;

class ciOpenNIAppApp : public AppBasic {
  public:
    void prepareSettings( Settings* settings );
	void setup();
	void update();
	void draw();
    void mouseDown( MouseEvent event );
    OpenNiController *m_DepthController;
};

void ciOpenNIAppApp::prepareSettings( Settings* settings )
{
    // just open windows console if compiled for windows
#if defined(_WIN32) || defined(_WIN64)				
    FILE* f;
    AllocConsole();
    freopen_s( &f, "CON", "w", stdout );
#endif

  //  settings->setWindowSize( m_iScreenWidth, m_iScreenHeight );
}

void ciOpenNIAppApp::setup()
{
    m_DepthController = new OpenNiController();
    m_DepthController->initializeController();

//    (*m_DepthController[0]).addProductionGraph( XN_NODE_TYPE_DEPTH );
    (*m_DepthController)[0].addProductionGraph( XN_NODE_TYPE_DEPTH, IMAGE_CONFIG_DEFAULT );
    (*m_DepthController)[0].m_GeneratorPairs[0].first->StartGenerating();

    (*m_DepthController)[0].addProductionGraph( XN_NODE_TYPE_IMAGE, IMAGE_CONFIG_DEFAULT );
    (*m_DepthController)[0].m_GeneratorPairs[1].first->StartGenerating();

    (*m_DepthController)[0].addProductionGraph( XN_NODE_TYPE_USER, IMAGE_CONFIG_DEFAULT );
    (*m_DepthController)[0].m_GeneratorPairs[2].first->StartGenerating();


    (*m_DepthController)[1].addProductionGraph( XN_NODE_TYPE_DEPTH, IMAGE_CONFIG_DEFAULT );
    (*m_DepthController)[1].m_GeneratorPairs[0].first->StartGenerating();
    
    (*m_DepthController)[1].addProductionGraph( XN_NODE_TYPE_IMAGE, IMAGE_CONFIG_DEFAULT );
    (*m_DepthController)[1].m_GeneratorPairs[1].first->StartGenerating();
    
    (*m_DepthController)[1].addProductionGraph( XN_NODE_TYPE_USER, IMAGE_CONFIG_DEFAULT );
    (*m_DepthController)[1].m_GeneratorPairs[2].first->StartGenerating();
    
    std::vector< GeneratorInfoPair >::iterator iter;
    iter = (*m_DepthController)[0].m_GeneratorPairs.end();
    iter--;
    xn::UserGenerator *ugen = static_cast<xn::UserGenerator*>( (iter->first).get() );
    xn::PoseDetectionCapability p  = ugen->GetPoseDetectionCap();
}

void ciOpenNIAppApp::update()
{
    if ( !m_DepthController->isInitialized() && false )
    {
//        m_DepthController->initializeController();
//        m_DepthController->m_DeviceList[0].addProductionGraph( XN_NODE_TYPE_DEPTH );
//        m_DepthController->m_DeviceList[0].m_DepthGenerator.StartGenerating();

       // m_DepthController->m_DeviceList[1].addProductionGraph( XN_NODE_TYPE_USER );
       // m_DepthController->m_DeviceList[1].m_UserGenerator.StartGenerating();
        //m_DepthController->m_DeviceList[0].m_IrGenerator.StartGenerating();

    } 

    //m_DepthController->createDeviceProductionTree( 0 );
    xn::NodeInfoList user;
    xn::Context cont = m_DepthController->getContext();
    cont.EnumerateExistingNodes(user, XN_NODE_TYPE_USER );
    if ( user.IsEmpty() )
    {
        std::cout << "No User Generator " << std::endl;
    } else
    {
        std::cout << "There is a user generator" << std::endl;
    }
}

void ciOpenNIAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 

}

void ciOpenNIAppApp::mouseDown( MouseEvent event )
{

}

CINDER_APP_BASIC( ciOpenNIAppApp, RendererGl )
