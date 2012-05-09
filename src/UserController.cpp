#include "UserController.h"
#include "ciOpenNiUtils.hpp"

namespace {
	void XN_CALLBACK_TYPE newUserCb( xn::UserGenerator& generator, XnUserID nId, void* pCookie )
	{
		xn::UserGenerator *userGenerator = static_cast<xn::UserGenerator*>(pCookie);
		userGenerator->GetSkeletonCap().RequestCalibration( nId, TRUE );
	}

	//-----------------------------------------//
	void XN_CALLBACK_TYPE lostUserCb(xn::UserGenerator& generator, XnUserID nId, void* pCookie )
	{
		std::cout << "Lost used with ID: " << nId << std::endl;
	}

	//-----------------------------------------//
	void XN_CALLBACK_TYPE userCalibrationStartedCb(xn::SkeletonCapability& capability, XnUserID nId, void* pCookie )
	{
		std::cout << "Calibrartion started for ID: " << nId << std::endl;
	}

	//-----------------------------------------//
	void XN_CALLBACK_TYPE userCalibrationCompletedCb(xn::SkeletonCapability& capability, XnUserID nId, XnCalibrationStatus eStatus, void* pCookie )
	{
		xn::UserGenerator *userGenerator = static_cast<xn::UserGenerator*>(pCookie);

		if ( eStatus == XN_CALIBRATION_STATUS_OK )
		{
			// Calibration succeeded
			std::cout << "Calibration Completed. Start Tracking User: " << nId << std::endl;
			userGenerator->GetSkeletonCap().StartTracking( nId );
		} else {
			// Calibration failed
			std::cout << "Calibration failed for user: " << nId << std::endl;

			if( eStatus==XN_CALIBRATION_STATUS_MANUAL_ABORT )
			{
				std::cout << "Manual calibration abort occured" << std::endl;
				return;
			}
		}

		userGenerator->GetSkeletonCap().RequestCalibration( nId, TRUE );
	}
}

UserController::UserController( xn::NodeInfo deviceInfo )
{

}

size_t UserController::getSkeletonData()
{

}

void UserController::getUsers()
{
	m_NumUsers = getNumberOfUsers();
	delete [] aUsers;
	aUsers  = new XnUserID[ m_NumUsers ];
	m_UserGenerator.GetUsers( aUsers, m_NumUsers );
	unsigned short 
}

size_t UserController::getNumberOfUsers()
{
	return m_UserGenerator.GetNumberOfUsers();
}

void UserController::registerUserCallbacks()
{
	if (!m_UserGenerator.IsCapabilitySupported( XN_CAPABILITY_SKELETON ) )
	{
		return;
	}
	checkError( m_UserGenerator.RegisterUserCallbacks( newUserCb, lostUserCb, (xn::UserGenerator *) &m_UserGenerator, userCbHandle ), "Cannot Register User Callbacks" );
	checkError( m_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart( userCalibrationStartedCb, (xn::UserGenerator *) &m_UserGenerator, calibrationStartedCbHandle ) , "Cannot Register User Calibration Started Callback ");
	checkError( m_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete( userCalibrationCompletedCb, (xn::UserGenerator *) &m_UserGenerator, calibrationCompletedCbHandle ), "Cannot Register User Calibration Completed Callback");
}