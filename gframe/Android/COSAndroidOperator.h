// Copyright (C) 2020 Edoardo Lolletti

#ifndef __C_OS_ANDROID_OPERATOR_H_INCLUDED__
#define __C_OS_ANDROID_OPERATOR_H_INCLUDED__

#ifdef __ANDROID__

#include <IOSOperator.h>

namespace irr {

class CIrrDeviceLinux;

//! The Operating system operator provides operation system specific methods and information.
class COSAndroidOperator : public IOSOperator {
public:

	COSAndroidOperator();

	//! returns the current operation system version as string.
	virtual const core::stringc& getOperatingSystemVersion() const _IRR_OVERRIDE_;

	//! copies text to the clipboard
	virtual void copyToClipboard(const wchar_t* text) const _IRR_OVERRIDE_;

	//! gets text from the clipboard
	//! \return Returns 0 if no string is in there.
	virtual const wchar_t* getTextFromClipboard() const _IRR_OVERRIDE_;

	//! gets the processor speed in megahertz
	//! \param Mhz:
	//! \return Returns true if successful, false if not
	virtual bool getProcessorSpeedMHz(u32* MHz) const _IRR_OVERRIDE_;

	//! gets the total and available system RAM in kB
	//! \param Total: will contain the total system memory
	//! \param Avail: will contain the available memory
	//! \return Returns true if successful, false if not
	virtual bool getSystemMemory(u32* Total, u32* Avail) const _IRR_OVERRIDE_;

private:

	core::stringc OperatingSystem;

};

} // __C_OS_ANDROID_OPERATOR_H_INCLUDED__
#endif //__ANDROID__

#endif

