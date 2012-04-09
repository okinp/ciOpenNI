#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
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
    OpenNiController *m_KinectController;
    xn::Context theContext;
};

void ciOpenNIAppApp::prepareSettings( Settings* settings )
{
    // just open windows console if compiled for windows
#if defined(_WIN32) || defined(_WIN64)				
    FILE* f;
    AllocConsole();
    freopen_s( &f, "CON", "w", stdout );
#endif

}

void ciOpenNIAppApp::setup()
{
    m_KinectController = new OpenNiController();
    m_KinectController->initializeController();
    m_KinectController->start( 0, COLORIMAGE | DEPTHIMAGE | USERIMAGE, IMAGE_CONFIG_DEFAULT );
    theContext = m_KinectController->getContext();
   // m_KinectController->start( 1, COLORIMAGE | DEPTHIMAGE | USERIMAGE, IMAGE_CONFIG_DEFAULT );
}

void ciOpenNIAppApp::update()
{
    if ( !m_KinectController->isInitialized() && false )
    {

    }

    xn::NodeInfoList depthList;
    theContext.EnumerateExistingNodes( depthList );

    
}

void ciOpenNIAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
    theContext.WaitNoneUpdateAll();
    boost::shared_array<unsigned char> imgRef = m_KinectController->getImageData( 0, DEPTHIMAGE, true, 0 );
    Channel depth( 640, 480, 640, 1, imgRef.get() );
    gl::draw( gl::Texture( depth ) );
}

void ciOpenNIAppApp::mouseDown( MouseEvent event )
{

}

CINDER_APP_BASIC( ciOpenNIAppApp, RendererGl )
