// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenJoystick.cpp
//
// PURPOSE : Interface screen for joystick/gamepad settings
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "ScreenJoystick.h"
#include "ScreenMgr.h"
#include "ScreenCommands.h"
#include "GameSettings.h"
#include "InterfaceMgr.h"

#include "GameInputMgr.h"
#include "VarTrack.h"

namespace
{
	int kGap = 200;
	int kWidth = 200;
}

extern GameInputMgr* g_pGameInputMgr;
extern VarTrack g_vtGamepadMinSensitivity;
extern VarTrack g_vtGamepadMaxSensitivity;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScreenJoystick::CScreenJoystick()
{
	memset(m_nAxis,0,sizeof(m_nAxis));
	memset(m_nPOV,0,sizeof(m_nPOV));
	m_pActiveController = nullptr;

	m_pSensitivityXCtrl = nullptr;
	m_pSensitivityYCtrl = nullptr;
	m_pAxisAccelerationCtrl = nullptr;
	m_pDeadzoneXCtrl = nullptr;
	m_pDeadzoneYCtrl = nullptr;
	m_pTriggerDeadzoneCtrl = nullptr;

	m_nSensitivityX = 0;
	m_nSensitivityY = 0;
	m_nAxisAcceleration = 0;
	m_nDeadzoneLeftAnalog = 0;
	m_nDeadzoneRightAnalog = 0;
	m_nTriggerDeadzone = 0;

}

CScreenJoystick::~CScreenJoystick()
{

}

// Build the screen
LTBOOL CScreenJoystick::Build()
{
	CreateTitle(IDS_TITLE_JOYSTICK);
	
	kGap = g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOYSTICK,"ColumnWidth");
	uint8 nLarge = g_pLayoutMgr->GetScreenCustomInt(SCREEN_ID_JOYSTICK,"HeaderFontSize");

	auto vGamepads = g_pGameInputMgr->GetListOfGamepads();
	m_pActiveController = AddCycle(IDS_ACTIVE_GAMEPAD, NULL, kGap, NULL, kDefaultPos, LTFALSE);
	for (auto sGamepad : vGamepads)
	{
		m_pActiveController->AddString(sGamepad.c_str());
	}
	m_pActiveController->Enable(true);

	vGamepads.clear();

	// Gamepad sensitivity
	int nMin = int(g_vtGamepadMinSensitivity.GetFloat());
	int nMax = int(g_vtGamepadMaxSensitivity.GetFloat());

	// Axis sensitivity
	m_pSensitivityXCtrl = AddSlider(IDS_GAMEPAD_SENSITIVITY_X, IDS_HELP_GAMEPAD_SENSE, kGap, kWidth, -1, &m_nSensitivityX);
	m_pSensitivityXCtrl->SetSliderRange(nMin, nMax); // We don't want 0!
	m_pSensitivityXCtrl->SetSliderIncrement(5);
	//m_pSensitivityXCtrl->SetNumericDisplay(LTTRUE);

	m_pSensitivityYCtrl = AddSlider(IDS_GAMEPAD_SENSITIVITY_Y, IDS_HELP_GAMEPAD_SENSE, kGap, kWidth, -1, &m_nSensitivityY);
	m_pSensitivityYCtrl->SetSliderRange(nMin, nMax); // We don't want 0!
	m_pSensitivityYCtrl->SetSliderIncrement(5);
	//m_pSensitivityYCtrl->SetNumericDisplay(LTTRUE);

	// Axis accel - Disabled for now, needs a re-write
	//m_pSensitivityYCtrl = AddSlider(IDS_GAMEPAD_AXIS_ACCEL, IDS_HELP_GAMEPAD_AXIS_ACCEL, kGap, kWidth, -1, &m_nAxisAcceleration);
	//m_pSensitivityYCtrl->SetSliderRange(1, 10);
	//m_pSensitivityYCtrl->SetSliderIncrement(1);

	// Axis deadzone
	m_pSensitivityYCtrl = AddSlider(IDS_GAMEPAD_DEADZONE_X, IDS_HELP_GAMEPAD_DEADZONE, kGap, kWidth, -1, &m_nDeadzoneLeftAnalog);
	m_pSensitivityYCtrl->SetSliderRange(1, 20);
	m_pSensitivityYCtrl->SetSliderIncrement(1);

	m_pSensitivityYCtrl = AddSlider(IDS_GAMEPAD_DEADZONE_Y, IDS_HELP_GAMEPAD_DEADZONE, kGap, kWidth, -1, &m_nDeadzoneRightAnalog);
	m_pSensitivityYCtrl->SetSliderRange(1, 20);
	m_pSensitivityYCtrl->SetSliderIncrement(1);

	m_pSensitivityYCtrl = AddSlider(IDS_GAMEPAD_DEADZONE_TRIGGERS, IDS_HELP_GAMEPAD_DEADZONE, kGap, kWidth, -1, &m_nTriggerDeadzone);
	m_pSensitivityYCtrl->SetSliderRange(1, 20);
	m_pSensitivityYCtrl->SetSliderIncrement(1);

	// Make sure to call the base class
	if (! CBaseScreen::Build()) return LTFALSE;

	UseBack(LTTRUE,LTTRUE);
	return LTTRUE;

}

uint32 CScreenJoystick::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_UPDATE:
		{
			UpdateData();

		
			break;
		}
	default:
		return CBaseScreen::OnCommand(dwCommand,dwParam1,dwParam2);

	}
	return 1;
};



// Change in focus
void CScreenJoystick::OnFocus(LTBOOL bFocus)
{
	CUserProfile *pProfile = g_pProfileMgr->GetCurrentProfile();
	if (bFocus)
	{
		pProfile->SetJoystick();
		for (int axis = 0; axis < g_pProfileMgr->GetNumAxis(); axis++)
		{
			m_nAxis[axis] = pProfile->m_nAxis[axis];
		}
		for (int POV = 0; POV < g_pProfileMgr->GetNumPOV(); POV++)
		{
			m_nPOV[POV] = pProfile->m_nPOV[POV];
		}

		// Get our active controller, and set our selected index
		char szActiveController[128] = "";
		GetConsoleString("GamepadName", szActiveController, "");
		for (int i = 0; i < m_pActiveController->GetNumStrings(); i++)
		{
			auto pString = m_pActiveController->GetString(i);
			auto sString = pString->GetText();

			if (stricmp(szActiveController, sString) == 0)
			{
				m_pActiveController->SetSelIndex(i);
			}
		}

		m_nSensitivityX = pProfile->m_nGamepadSensitivityX;
		m_nSensitivityY = pProfile->m_nGamepadSensitivityY;

		m_nAxisAcceleration = pProfile->m_nAxisAcceleration;

		m_nDeadzoneLeftAnalog = pProfile->m_nDeadzoneLeftAnalog;
		m_nDeadzoneRightAnalog = pProfile->m_nDeadzoneRightAnalog;
		m_nTriggerDeadzone = pProfile->m_nTriggerDeadzone;

	
        UpdateData(LTFALSE);
	}
	else
	{
		UpdateData();

		for (int axis = 0; axis < g_pProfileMgr->GetNumAxis(); axis++)
		{
			pProfile->m_nAxis[axis] = m_nAxis[axis];
		};
		for (int POV = 0; POV < g_pProfileMgr->GetNumPOV(); POV++)
		{
			pProfile->m_nPOV[POV] = m_nPOV[POV];
		};

		// Get our selected index string, and set it as the active controller
		auto pString = m_pActiveController->GetString(m_pActiveController->GetSelIndex());
		auto sString = pString->GetText();
		g_pGameInputMgr->SetGamepad(sString);

		pProfile->m_nGamepadSensitivityX = m_nSensitivityX;
		pProfile->m_nGamepadSensitivityY = m_nSensitivityY;

		pProfile->m_nAxisAcceleration = m_nAxisAcceleration;

		pProfile->m_nDeadzoneLeftAnalog = m_nDeadzoneLeftAnalog;
		pProfile->m_nDeadzoneRightAnalog = m_nDeadzoneRightAnalog;
		pProfile->m_nTriggerDeadzone = m_nTriggerDeadzone;

		pProfile->ApplyJoystick();
		pProfile->Save();
	}
	CBaseScreen::OnFocus(bFocus);
}