#pragma once
#include <XnCppWrapper.h>
class UserController {
public:
	UserController( xn::NodeInfo deviceInfo );
	void getUsers();
	size_t getNumberOfUsers();
	XnSkeletonJointTransformation	getSkeletonData( const size_t userId, XN_SKEL_TORSO );
	

private:

	void registerUserCallbacks();
	xn::UserGenerator m_UserGenerator;

	//Callback Handles
	XnCallbackHandle userCbHandle;
	XnCallbackHandle calibrationStartedCbHandle;
	XnCallbackHandle calibrationCompletedCbHandle;
	size_t m_NumUsers;
	XnUserID *aUsers;
};