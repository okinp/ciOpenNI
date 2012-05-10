#include "OpenNiUserController.h"
#include "ciOpenNiUtils.hpp"

namespace {
	void XN_CALLBACK_TYPE newUserCb( xn::UserGenerator& generator, XnUserID nId, void* pCookie )
	{
		xn::UserGenerator *userGenerator = static_cast<xn::UserGenerator*>(pCookie);
		userGenerator->GetSkeletonCap().RequestCalibration( nId, TRUE );
		std::cout << "New user with ID: " << nId << std::endl;
	}

	//-----------------------------------------//
	void XN_CALLBACK_TYPE lostUserCb(xn::UserGenerator& generator, XnUserID nId, void* pCookie )
	{
		std::cout << "Lost user with ID: " << nId << std::endl;
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
} //End unnamed namespace

OpenNiUserController::OpenNiUserController( xn::UserGenerator generator )
	:m_UserGenerator( generator ),
	 m_Users( new XnUserID[ MAX_NUM_USERS ] ),
	 m_NumUsers( 0 )
{
	registerUserCallbacks();
}

XnSkeletonJointTransformation OpenNiUserController::getSkeletonJoint( XnUInt16 userIdx, XnSkeletonJoint jointType  )
{
	//getUsers();
	try
	{
		XnSkeletonJointTransformation requestedJoint;
		m_UserGenerator.GetSkeletonCap().GetSkeletonJoint( m_Users[userIdx], jointType, requestedJoint );
		return requestedJoint;
	}
	catch ( std::exception& e  )
	{
		std::cout << "Could not get skeleton joint." << std::endl;
	}
}

void OpenNiUserController::getUsers()
{

	try
	{
		m_NumUsers = MAX_NUM_USERS;
		m_UserGenerator.GetUsers( m_Users, m_NumUsers );
		std::cout << "this is working" << std::endl;
	}
	catch ( std::exception& e  )
	{
		std::cout << "Could not update available Users" << std::endl;
	}
}

size_t OpenNiUserController::getNumberOfUsers()
{
	return m_UserGenerator.GetNumberOfUsers();
}

void OpenNiUserController::registerUserCallbacks()
{
	if (!m_UserGenerator.IsCapabilitySupported( XN_CAPABILITY_SKELETON ) )
	{
		return;
	}
	checkError( m_UserGenerator.RegisterUserCallbacks( newUserCb, lostUserCb, (xn::UserGenerator *) &m_UserGenerator, userCbHandle ), "Cannot Register User Callbacks" );
	checkError( m_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart( userCalibrationStartedCb, (xn::UserGenerator *) &m_UserGenerator, calibrationStartedCbHandle ) , "Cannot Register User Calibration Started Callback ");
	checkError( m_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete( userCalibrationCompletedCb, (xn::UserGenerator *) &m_UserGenerator, calibrationCompletedCbHandle ), "Cannot Register User Calibration Completed Callback");
}