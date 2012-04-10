#pragma once
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include "XnOpenNI.h"
#include <XnLog.h>
#include <XnCppWrapper.h>
#include <iostream>
#include <string>
#include <vector>
#include "_2RealTypes.h"
#include "OpenNiDevice.h"

using namespace _2RealKinectWrapper;

class OpenNiController 
{
public:
    OpenNiController();
    ~OpenNiController();

    xn::Context& getContext();
    size_t const& getNumberOfConnectedDevices();

    void allignDepthImageToColor( const size_t &deviceIdx );
    void synchronizeImageAndDepthStreams( const size_t &deviceIdx );
    
    void initializeController();
    bool const isInitialized() ;

    //OpenNIImplementation

    //This doesn't start but adds the specified generators for the device;
    virtual bool start( size_t deviceIdx, uint32_t startGenerators, uint32_t configureImages );
    
    virtual bool startGenerator( size_t deviceIdx, uint32_t startGenerator );
    virtual bool stopGenerator( size_t deviceIdx, uint32_t stopGenerator );
    
    virtual void setMirrored( const uint32_t deviceIdx, _2RealGenerator type, bool flag );
    virtual bool isMirrored( const uint32_t deviceID, _2RealGenerator type );
    virtual bool isJointAvailable( _2RealJointType type ) const;
    virtual const bool isNewData(const uint32_t deviceID, _2RealGenerator type);
    virtual void resetSkeleton( const uint32_t deviceID, const uint32_t id );
    virtual void resetAllSkeletons();

    
    
    virtual boost::shared_array<unsigned char> getImageData( const uint32_t deviceID, _2RealGenerator type );
    
    void updateContext();
    
    OpenNiDevice& operator[]( const size_t deviceIdx );
private:
    bool m_IsInitialized;
    
    size_t           m_NumberOfDevices;
    xn::Context      m_Context;
    xn::NodeInfoList m_DeviceInfo;
    
    uint32_t  m_ImageConfig;
    uint32_t  m_GeneratorConfig;

    std::vector< OpenNiDevice > m_DeviceList;

    void checkDeviceRunning(uint8_t deviceIdx ) const;
    bool setGeneratorState( size_t deviceIdx, uint32_t requestedGenerator, bool start );
    std::vector<XnPredefinedProductionNodeType> getRequestedNodes( uint32_t startGenerators ); 
    virtual bool shutdown();
    


    
    //virtual boost::shared_array<uint16_t> getImageDataDepth16Bit( const uint32_t deviceID, bool waitAndBlock=false );

    //virtual uint32_t getBytesPerPixel( _2RealGenerator type ) const

    //virtual uint32_t getImageWidth( const uint32_t deviceID, _2RealGenerator type )
    //virtual uint32_t getImageHeight( const uint32_t deviceID, _2RealGenerator type )

    //virtual const _2RealVector3f getJointWorldPosition( const uint32_t deviceID, const uint8_t userID, _2RealJointType type )
    //virtual const _2RealPositionsVector3f& getSkeletonWorldPositions( const uint32_t deviceID, const uint8_t userID )
    //virtual const _2RealVector3f getJointScreenPosition( const uint32_t deviceID, const uint8_t userID, _2RealJointType type )
    //virtual const _2RealPositionsVector3f& getSkeletonScreenPositions( const uint32_t deviceID, const uint8_t userID )
    //virtual const _2RealOrientationsMatrix3x3& getSkeletonWorldOrientations( const uint32_t deviceID, const uint8_t userID )
    //virtual const _2RealMatrix3x3 getJointWorldOrientation( const uint32_t deviceID, const uint8_t userID, _2RealJointType type )
    //virtual const _2RealJointConfidence getSkeletonJointConfidence(const uint32_t deviceID, const uint8_t userID, _2RealJointType type)
    //virtual const _2RealJointConfidences getSkeletonJointConfidences(const uint32_t deviceID, const uint8_t userID)
    //virtual const uint32_t getNumberOfUsers( const uint32_t deviceID ) const
    //virtual const uint32_t getNumberOfSkeletons( const uint32_t deviceID ) const
    //virtual void setAlignColorDepthImage( const uint32_t deviceID, bool flag )
    //virtual bool hasFeatureJointOrientation() const
    //virtual bool restart()
    //virtual void convertProjectiveToWorld( const uint32_t deviceID, const uint32_t coordinateCount, const _2RealVector3f* inProjective, _2RealVector3f* outWorld )
    //virtual void convertWorldToProjective( const uint32_t deviceID, const uint32_t coordinateCount, const _2RealVector3f* inWorld, _2RealVector3f* outProjective )
    //virtual bool setMotorAngle(int deviceID, int angle)
    //virtual int getMotorAngle(int deviceID)
    //virtual const _2RealVector3f getUsersWorldCenterOfMass(const uint32_t deviceID, const uint8_t userID)
    //virtual const _2RealVector3f getUsersScreenCenterOfMass(const uint32_t deviceID, const uint8_t userID)
    //virtual void setLogLevel(_2RealLogLevel iLevel)
    //virtual void setLogOutputStream(std::ostream* outStream)
    //void checkError( XnStatus status, std::string strError ) const  
};