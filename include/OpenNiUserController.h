#pragma once
#include <XnCppWrapper.h>
#define MAX_NUM_USERS 20 
class OpenNiUserController {
public:
	OpenNiUserController( xn::UserGenerator generator );
	void getUsers();
	size_t getNumberOfUsers();
	XnSkeletonJointTransformation	getSkeletonJoint( XnUInt16 userIdx, XnSkeletonJoint jointType);
	
private:

	void registerUserCallbacks();
	xn::UserGenerator m_UserGenerator;

	//Callback Handles
	XnCallbackHandle userCbHandle;
	XnCallbackHandle calibrationStartedCbHandle;
	XnCallbackHandle calibrationCompletedCbHandle;
	XnUInt16 m_NumUsers;
	XnUserID *m_Users;
};