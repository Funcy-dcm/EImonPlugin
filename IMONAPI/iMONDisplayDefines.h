#ifndef __IMON_DISPLAY_API_DEFINES_H__
#define __IMON_DISPLAY_API_DEFINES_H__

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//	Enumerations

/**DSPResult
@brief	These enumeration values represent the returned result for iMON Display API function calls.\n
			All iMON Display API function calls return one of this result values.\n
			For meaning of each result, refer the comment of each line below*/
enum DSPResult
{
	DSP_SUCCEEDED = 0,				//// Function Call Succeeded Without Error
	DSP_E_FAIL,						//// Unspecified Failure
	DSP_E_OUTOFMEMORY,				//// Failed to Allocate Necessary Memory
	DSP_E_INVALIDARG,				//// One or More Arguments Are Not Valid
	DSP_E_NOT_INITED,				//// API is Not Initialized
	DSP_E_POINTER,					//// Pointer is Not Valid

	DSP_S_INITED = 0x1000,			//// API is Initialized
	DSP_S_NOT_INITED,				//// API is Not Initialized
	DSP_S_IN_PLUGIN_MODE,			//// API Can Control iMON Display (Display Plug-in Mode)
	DSP_S_NOT_IN_PLUGIN_MODE,		//// API Can't Control iMON Display
};


/**DSPNInitResult
@brief	These enumeration values represent the result status for requesting Display Plug-in Mode to iMON.\n
			iMON Display API notifies one of this result values to the caller application after requesting Display Plug-in Mode to iMON.\n
			For more information, refer the comment of each line below*/
enum DSPNInitResult
{
	DSPN_SUCCEEDED = 0,				//// Display Plug-in Mode is Initialized Successfully
	DSPN_ERR_IN_USED = 0x0100,		//// Display Plug-in is Already Used by Other Application
	DSPN_ERR_HW_DISCONNECTED,		//// iMON HW is Not Connected
	DSPN_ERR_NOT_SUPPORTED_HW,		//// The Connected iMON HW doesn't Support Display Plug-in
	DSPN_ERR_PLUGIN_DISABLED,		//// Display Plug-in Mode Option is Disabled
	DSPN_ERR_IMON_NO_REPLY,			//// The Latest iMON is Not Installed or iMON Not Running
	DSPN_ERR_UNKNOWN = 0x0200,		//// Unknown Failure
};


/**DSPType
@brief	These enumeration values represent display type.\n
			Currently iMON Display API supports VFD and LCD products.*/
enum DSPType
{
	DSPN_DSP_NONE	= 0,
	DSPN_DSP_VFD	= 0x01,			//// VFD products
	DSPN_DSP_LCD	= 0x02,			//// LCD products
};


/**DSPNotifyCode
@brief	These enumeration values represent the notification codes.\n
			iMON Display API will send or post message to the caller application.\n
			The caller application should assign the message and the winodw handle to receivce message with IMON_Display_Init fucntion.\n
			These enumeration values are used with WPARAM parameter of the message.\n 
			For more information, see the explanation of each notification code below*/
enum DSPNotifyCode
{
	/**DSPNM_PLUGIN_SUCCEED
	@brief	When API succeeds to get the control for the display, API will post caller-specified message with DSPNM_PLUGIN_SUCCEED as WPARAM parameter.\n
				LPARAM represents DSPType. This value can be 0x01 (VFD), 0x02 (LCD) or 0x03 (VFD+LCD).*/
	DSPNM_PLUGIN_SUCCEED = 0,

	/**DSPNM_PLUGIN_FAILED
	@brief	When API fails to get the control for the display, API will post caller-specified message with DSPNM_PLUGIN_FAILED as WPARAM parameter.\n
				LPARAM represents error code with DSPNResult.*/
	DSPNM_PLUGIN_FAILED,

	/**DSPNM_IMON_RESTARTED
	@brief	When iMON starts, API will post caller-specified message with DSPNM_IMON_RESTARTED as WPARAM parameter.\n
				LPARAM represents DSPType. This value can be 0 (No Display), 0x01 (VFD), 0x02 (LCD) or 0x03 (VFD+LCD).*/
	DSPNM_IMON_RESTARTED,

	/**DSPNM_IMON_CLOSED
	@brief	When iMON closed, API will post caller-specified message with DSPNM_IMON_CLOSED as WPARAM parameter.\n
				LPARAM is not used.*/
	DSPNM_IMON_CLOSED,

	/**DSPNM_HW_CONNECTED
	@brief	When iMON HW newly connected, API will post caller-specified message with DSPNM_HW_CONNECTED as WPARAM parameter.\n
				LPARAM represents DSPType. This value can be 0 (No Display), 0x01 (VFD), 0x02 (LCD) or 0x03 (VFD+LCD).*/
	DSPNM_HW_CONNECTED,

	/**DSPNM_HW_DISCONNECTED
	@brief	When iMON HW disconnected, API will post caller-specified message with DSPNM_HW_DISCONNECTED as WPARAM parameter.\n
				LPARAM is DSPNResult value, DSPN_ERR_HW_DISCONNECTED.*/
	DSPNM_HW_DISCONNECTED,


	/**DSPNM_LCD_TEXT_SCROLL_DONE
	@brief	When iMON LCD finishes scrolling Text, API will post caller-specified message with DSPNM_LCD_TEXT_SCROLL_DONE as WPARAM parameter.\n
				The caller application may need to know when text scroll is finished, for sending next text.\n
				LPARAM is not used.*/
	DSPNM_LCD_TEXT_SCROLL_DONE = 0x1000,
};

//////////////////////////////////////////////////
//////////////////////////////////////////////////
//	Structure

/**DspEqData
@brief	This structure contains Equalizer data for 16 bands. 
@param	BandData    It represents Equalizer data for 16 bands. Its range is from 0 to 100.*/
typedef struct DspEqData
{
	int BandData[16];

} DSPEQDATA, *PDSPEQDATA;

#endif	//__IMON_DISPLAY_API_DEFINES_H__