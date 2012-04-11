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
    boost::shared_array<unsigned char> imgRefDev0;
    boost::shared_array<unsigned char> imgRefDev1;
    
    boost::shared_array<unsigned char> colorDev0;
    boost::shared_array<unsigned char> colorDev1;
};

void ciOpenNIAppApp::prepareSettings( Settings* settings )
{
    // just open windows console if compiled for windows
#if defined(_WIN32) || defined(_WIN64)				
    FILE* f;
    AllocConsole();
    freopen_s( &f, "CON", "w", stdout );
#endif
    settings->setWindowSize( 1280, 960 );
}

void ciOpenNIAppApp::setup()
{
    m_KinectController = new OpenNiController();
    m_KinectController->initializeController();
    m_KinectController->start( 0, DEPTHIMAGE | COLORIMAGE, IMAGE_CONFIG_DEFAULT );
    m_KinectController->start( 1, DEPTHIMAGE | COLORIMAGE, IMAGE_CONFIG_DEFAULT );
    m_KinectController->startGenerator(0, DEPTHIMAGE | COLORIMAGE );
    m_KinectController->startGenerator(1, DEPTHIMAGE | COLORIMAGE );
    
    theContext = m_KinectController->getContext();
}

void ciOpenNIAppApp::update()
{
    m_KinectController->updateContext();
    imgRefDev0 = m_KinectController->getImageData( 0, DEPTHIMAGE );
    colorDev0  = m_KinectController->getImageData( 0, COLORIMAGE );
    imgRefDev1 = m_KinectController->getImageData( 1, DEPTHIMAGE );
    colorDev1  = m_KinectController->getImageData( 1, COLORIMAGE );
}

void ciOpenNIAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    Channel depth0( 640, 480, 640, 1, imgRefDev0.get() );
    Surface8u color0( colorDev0.get(), 640, 480, 640*3, SurfaceChannelOrder::RGB );
    Channel depth1( 640, 480, 640, 1, imgRefDev1.get() );
    Surface8u color1( colorDev1.get(), 640, 480, 640*3, SurfaceChannelOrder::RGB );
    
    gl::draw( gl::Texture( depth0 ), ci::Rectf( 0, 0, 640, 480 ));
    gl::draw( gl::Texture( color0 ), ci::Rectf( 640, 0, 1280, 480 ));
    gl::draw( gl::Texture( depth1 ), ci::Rectf( 0, 480, 640, 960 ));
    gl::draw( gl::Texture( color1 ), ci::Rectf( 640, 480, 1280, 960));
}

void ciOpenNIAppApp::mouseDown( MouseEvent event )
{

}

CINDER_APP_BASIC( ciOpenNIAppApp, RendererGl )
