//
// Joy Stick�p�t�@�C��
//
#include "pch.h"
#include "ESPracerDlg.h"
#include "utilJoyStick.h"


// ���C�u���������N
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


CRITICAL_SECTION		gCsJoyStick;			// �r�������W���C�X�e�B�b�N
LPDIRECTINPUT8			lpDI = NULL;			// IDirectInput8
LPDIRECTINPUTDEVICE8	lpJoystick = NULL;		// �W���C�X�e�B�b�N�f�o�C�X
static HWND				s_hWnd = NULL;			// �E�C���h�E�n���h�����W���C�X�e�B�b�N�p
static HINSTANCE		sInstance;				// Windows �C���X�^���X


extern "C" {
	// JoyStick

	// ����������
	void InitJoyStick(HINSTANCE instance, HWND hwnd, LPVOID process)
	{
		sInstance = instance;
		s_hWnd = hwnd;

		// �r������
		InitializeCriticalSection(&gCsJoyStick);

		// Thread�N��
		AfxBeginThread(JoyStickThread, process);
	}

	// �I������
	void EndJoyStick()
	{
		DeleteCriticalSection(&gCsJoyStick);
	}

	// 1�̃f�o�C�X���ƂɌĂяo�����R�[���o�b�N�֐�
	BOOL PASCAL EnumJoyDeviceProc(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
	{
		// �W���C�X�e�B�b�N�f�o�C�X�̍쐬
		HRESULT ret = lpDI->CreateDevice(lpddi->guidInstance, &lpJoystick, NULL);
		if (FAILED(ret)) {
			lpJoystick = NULL;
			return DIENUM_STOP;
		}

		// ���̓f�[�^�`���̃Z�b�g
		ret = lpJoystick->SetDataFormat(&c_dfDIJoystick);
		if (FAILED(ret)) {
			lpJoystick->Release();
			lpJoystick = NULL;
			return DIENUM_STOP;
		}

		// �r������̃Z�b�g
		ret = lpJoystick->SetCooperativeLevel(s_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		if (FAILED(ret)) {
			lpJoystick->Release();
			lpJoystick = NULL;
			return DIENUM_STOP;
		}

		// ���͔͈͂̃Z�b�g
		DIPROPRANGE	diprg;
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMax = 1000;
		diprg.lMin = -1000;

		// X��
		diprg.diph.dwObj = DIJOFS_X;
		lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

		// Y��
		diprg.diph.dwObj = DIJOFS_Y;
		lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

		// Z��
		diprg.diph.dwObj = DIJOFS_Z;
		lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

		// RX��
		diprg.diph.dwObj = DIJOFS_RX;
		lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

		// RY��
		diprg.diph.dwObj = DIJOFS_RY;
		lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

		// RZ��
		diprg.diph.dwObj = DIJOFS_RZ;
		lpJoystick->SetProperty(DIPROP_RANGE, &diprg.diph);

		// �N����������
		lpJoystick->Poll();

		// �\�z�����Ȃ�
		//		char tmp[260];
		//		WideCharToMultiByte(CP_ACP, 0, lpddi->tszInstanceName, -1, tmp, sizeof(tmp), NULL, NULL);
		//		WideCharToMultiByte(CP_ACP, 0, lpddi->tszProductName, -1, tmp, sizeof(tmp), NULL, NULL);

		// �ŏ���1�݂̂ŏI���
		return DIENUM_STOP;			// ���̃f�o�C�X��񋓂���ɂ�DIENUM_CONTINUE��Ԃ�
	}

	BOOL GetJoyStickDevice(int mode)
	{
		HRESULT ret = DirectInput8Create(sInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&lpDI, NULL);
		if (FAILED(ret)) {
			lpDI = NULL;
		}
		else {
			ret = lpDI->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoyDeviceProc, NULL, DIEDFL_ATTACHEDONLY);
			if (FAILED(ret)) {
				lpDI->Release();
				lpDI = NULL;
			}
			else {
				return TRUE;
			}
		}

		if (lpDI == NULL) {
			if (mode == 1) {
				MessageBox(s_hWnd, _T("�W���C�X�e�B�b�N��1���ڑ�����Ă��܂���"), _T("�G���["), MB_OK | MB_ICONHAND);
			}
		}

		return FALSE;
	}

	BOOL GetJoyStick(int mode)
	{
		if (!lpJoystick) {
			// �W���C�X�e�B�b�N��1��������Ȃ�
			if (mode == 1) {
				MessageBox(s_hWnd, _T("�W���C�X�e�B�b�N��1���ڑ�����Ă��܂���"), _T("�G���["), MB_OK | MB_ICONHAND);
			}
			lpDI->Release();
			lpDI = NULL;
		}
		else {
			// �f�o�C�X���
			DIDEVCAPS dc;
			dc.dwSize = sizeof(dc);
			lpJoystick->GetCapabilities(&dc);
#if 1
			DEBUG("DIDC_ATTACHED           [%d]\n", (dc.dwFlags&DIDC_ATTACHED) ? 1 : 0);
			DEBUG("DIDC_POLLEDDEVICE       [%d]\n", (dc.dwFlags&DIDC_POLLEDDEVICE) ? 1 : 0);
			DEBUG("DIDC_EMULATED           [%d]\n", (dc.dwFlags&DIDC_EMULATED) ? 1 : 0);
			DEBUG("DIDC_FORCEFEEDBACK      [%d]\n", (dc.dwFlags&DIDC_FORCEFEEDBACK) ? 1 : 0);
			DEBUG("DIDC_FFATTACK           [%d]\n", (dc.dwFlags&DIDC_FFATTACK) ? 1 : 0);
			DEBUG("DIDC_FFFADE             [%d]\n", (dc.dwFlags&DIDC_FFFADE) ? 1 : 0);
			DEBUG("DIDC_SATURATION         [%d]\n", (dc.dwFlags&DIDC_SATURATION) ? 1 : 0);
			DEBUG("DIDC_POSNEGCOEFFICIENTS [%d]\n", (dc.dwFlags&DIDC_POSNEGCOEFFICIENTS) ? 1 : 0);
			DEBUG("DIDC_POSNEGSATURATION   [%d]\n", (dc.dwFlags&DIDC_POSNEGSATURATION) ? 1 : 0);
			DEBUG("DIDC_DEADBAND           [%d]\n", (dc.dwFlags&DIDC_DEADBAND) ? 1 : 0);
			DEBUG("DIDC_STARTDELAY         [%d]\n", (dc.dwFlags&DIDC_STARTDELAY) ? 1 : 0);
			DEBUG("DIDC_ALIAS              [%d]\n", (dc.dwFlags&DIDC_ALIAS) ? 1 : 0);
			DEBUG("DIDC_PHANTOM            [%d]\n", (dc.dwFlags&DIDC_PHANTOM) ? 1 : 0);
			DEBUG("DIDC_HIDDEN             [%d]\n", (dc.dwFlags&DIDC_HIDDEN) ? 1 : 0);/**/
#endif
																					  // ����J�n
			lpJoystick->Acquire();

			return TRUE;
		}

		return FALSE;
	}

	//
	//�@�W���C�X�e�B�b�N�̃f�[�^�擾
	//
	UINT JoyStickThread(LPVOID p)
	{
		CESPracerDlg *app = (CESPracerDlg *)p;
		DIJOYSTATE joy;
		ZeroMemory(&joy, sizeof(joy));

		// IDirectInput8�̍쐬
		if (GetJoyStickDevice(0)) {
			if (lpDI != NULL) {
				// �W���C�X�e�B�b�N�̗�
				GetJoyStick(0);
			}
		}

		while (!app->m_Terminate) {
			if (lpDI == NULL && lpJoystick == NULL) {
				if (lpDI == NULL) {
					if (lpJoystick != NULL) {
						lpJoystick->Release();
						lpJoystick = NULL;
					}

					GetJoyStickDevice(0);
				}

				if (lpDI != NULL) {
					if (lpJoystick == NULL) {
						if (GetJoyStick(0)) {
						}
					}
				}
			}

			if (lpDI != NULL && lpJoystick != NULL) {
				DIDEVCAPS dc;
				dc.dwSize = sizeof(dc);
				lpJoystick->GetCapabilities(&dc);
				if (dc.dwFlags & DIDC_POLLEDDATAFORMAT) {
					lpJoystick->Poll();
				}
				else {
					app->m_JoyStickActive = FALSE;
				}

				// �W���C�X�e�B�b�N�̓���
				HRESULT ret = lpJoystick->GetDeviceState(sizeof(joy), &joy);
				if (FAILED(ret)) {
					app->m_JoyStickActive = FALSE;
					lpJoystick->Acquire();
				}
				else {
					app->m_JoyStickActive = TRUE;
				}
			}
			else {
				app->m_JoyStickActive = FALSE;
			}

			if (app->m_JoyStickActive) {
				EnterCriticalSection(&gCsJoyStick);

				app->m_LX = joy.lX;
				app->m_LY = joy.lY;
				app->m_RX = joy.lZ;
				app->m_RY = joy.lRz;

				if (abs(app->m_LX) < 70) {
					app->m_LX = 0;
				}
				if (abs(app->m_LY) < 70) {
					app->m_LY = 0;
				}
				if (abs(app->m_RX) < 70) {
					app->m_RX = 0;
				}
				if (abs(app->m_RY) < 70) {
					app->m_RY = 0;
				}

				// L1
				app->m_bStaticL1 = (BOOL)(joy.rgbButtons[4] & 0x80);
				// R1
				app->m_bStaticR1 = (BOOL)(joy.rgbButtons[5] & 0x80);
				// L2
				if (joy.lRx > 128) {
					app->m_bStaticL2 = TRUE;
				}
				else {
					app->m_bStaticL2 = FALSE;
				}
				// R2
				if (joy.lRy > 128) {
					app->m_bStaticR2 = TRUE;
				}
				else {
					app->m_bStaticR2 = FALSE;
				}

				// Share
				app->m_bStaticShare = (BOOL)(joy.rgbButtons[8] & 0x80);
				// Option
				app->m_bStaticOption = (BOOL)(joy.rgbButtons[9] & 0x80);

				app->m_bStaticUP = FALSE;
				app->m_bStaticLeft = FALSE;
				app->m_bStaticDown = FALSE;
				app->m_bStaticRight = FALSE;
				if (joy.rgdwPOV[0] == 0) {
					// UP
					app->m_bStaticUP = TRUE;
				}
				else if (joy.rgdwPOV[0] == 4500) {
					// RIGHT + UP
					app->m_bStaticUP = TRUE;
					app->m_bStaticRight = TRUE;
				}
				else if (joy.rgdwPOV[0] == 9000) {
					// RIGHT
					app->m_bStaticRight = TRUE;
				}
				else if (joy.rgdwPOV[0] == 13500) {
					// DOWN + RIGHT
					app->m_bStaticRight = TRUE;
					app->m_bStaticDown = TRUE;
				}
				else if (joy.rgdwPOV[0] == 18000) {
					// DOWN
					app->m_bStaticDown = TRUE;
				}
				else if (joy.rgdwPOV[0] == 22500) {
					// LEFT + DOWN
					app->m_bStaticDown = TRUE;
					app->m_bStaticLeft = TRUE;
				}
				else if (joy.rgdwPOV[0] == 27000) {
					// LEFT
					app->m_bStaticLeft = TRUE;
				}
				else if (joy.rgdwPOV[0] == 31500) {
					// UP + LEFT
					app->m_bStaticLeft = TRUE;
					app->m_bStaticUP = TRUE;
				}

				app->m_Pad = joy.rgdwPOV[0];

				// Tri
				app->m_bStaticTri = (BOOL)(joy.rgbButtons[3] & 0x80);
				// Sqa
				app->m_bStaticSqa = (BOOL)(joy.rgbButtons[0] & 0x80);
				// Cir
				app->m_bStaticCir = (BOOL)(joy.rgbButtons[2] & 0x80);
				// Cross
				app->m_bStaticCross = (BOOL)(joy.rgbButtons[1] & 0x80);

				// PS
				app->m_bStaticPS = (BOOL)(joy.rgbButtons[12] & 0x80);
				// Pad
				app->m_bStaticPad = (BOOL)(joy.rgbButtons[13] & 0x80);

				LeaveCriticalSection(&gCsJoyStick);
			}
			else {
				Sleep(10);
			}

			Sleep(1);
		}

		return 0;
	}
}
