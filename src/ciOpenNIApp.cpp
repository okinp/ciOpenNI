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

	ImageDataRef imgRefDev0;
	ImageDataRef imgRefDev1;

	ImageDataRef colorDev0;
	ImageDataRef colorDev1;
	
	ImageDataRef usrImgDev0;
	ImageDataRef usrImgDev1;
	
	size_t nUsers;
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
	m_KinectController->configureDevice( 0,  DEPTHIMAGE | COLORIMAGE | USERIMAGE , IMAGE_CONFIG_DEFAULT );
	//m_KinectController->startGenerator(  0, DEPTHIMAGE | COLORIMAGE | USERIMAGE, IMAGE_CONFIG_DEFAULT );
	//theContext = m_KinectController->getContext();
	
	//nUsers = 0;
}

void ciOpenNIAppApp::update()
{
	//m_KinectController->updateContext();
	//imgRefDev0 = m_KinectController->getImageData( 0, DEPTHIMAGE );
	//colorDev0  = m_KinectController->getImageData( 0, COLORIMAGE );
	////TODO: is the ghosting in the user image some kind of casting problem from 16 to 8 bit?
	//usrImgDev0 = m_KinectController->getImageData( 0, USERIMAGE );
	//
	//size_t nCurrentUsers = m_KinectController->getNumberOfUsers( 0 );
	//
	//if ( nCurrentUsers != nUsers )
	//{
	//    nUsers = nCurrentUsers;
	//    std::cout << "Number of users: " <<  nUsers << std::endl;
	//    
	//}
	//
	//
	//for (int i = 0; i< 640*480; i++)
	//{
	//    if  ( usrImgDev0[i] != 0 )
	//    {
	//        usrImgDev0[i] = 255;
	//    }
	//}
	//
	//
	////imgRefDev1 = m_KinectController->getImageData( 1, DEPTHIMAGE );
	////colorDev1  = m_KinectController->getImageData( 1, COLORIMAGE );
}

void ciOpenNIAppApp::draw()
{
	//gl::clear( Color( 0, 0, 0 ) );
 //   Channel user0( 640, 480, 640, 1, usrImgDev0.get() );
 //   Channel depth0( 640, 480, 640, 1, imgRefDev0.get() );
 //   Surface8u color0( colorDev0.get(), 640, 480, 640*3, SurfaceChannelOrder::RGB );
 //   //Channel depth1( 640, 480, 640, 1, imgRefDev1.get() );
 //   //Surface8u color1( colorDev1.get(), 640, 480, 640*3, SurfaceChannelOrder::RGB );
 //   
 //   gl::draw( gl::Texture( user0 ), ci::Rectf( 0, 0, 640, 480 ));
 //   gl::draw( gl::Texture( color0 ), ci::Rectf( 640, 0, 1280, 480 ));
 //   //gl::draw( gl::Texture( depth1 ), ci::Rectf( 0, 480, 640, 960 ));
 //   //gl::draw( gl::Texture( color1 ), ci::Rectf( 640, 480, 1280, 960));
}

void ciOpenNIAppApp::mouseDown( MouseEvent event )
{

}

CINDER_APP_BASIC( ciOpenNIAppApp, RendererGl )
