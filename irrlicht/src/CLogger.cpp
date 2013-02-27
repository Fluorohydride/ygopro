// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CLogger.h"

namespace irr
{

	CLogger::CLogger(IEventReceiver* r)
		: LogLevel(ELL_INFORMATION), Receiver(r)
	{
		#ifdef _DEBUG
		setDebugName("CLogger");
		#endif
	}

	//! Returns the current set log level.
	ELOG_LEVEL CLogger::getLogLevel() const
	{
		return LogLevel;
	}

	//! Sets a new log level.
	void CLogger::setLogLevel(ELOG_LEVEL ll)
	{
		LogLevel = ll;
	}

	//! Prints out a text into the log
	void CLogger::log(const c8* text, ELOG_LEVEL ll)
	{
		if (ll < LogLevel)
			return;

		if (Receiver)
		{
			SEvent event;
			event.EventType = EET_LOG_TEXT_EVENT;
			event.LogEvent.Text = text;
			event.LogEvent.Level = ll;
			if (Receiver->OnEvent(event))
				return;
		}

		os::Printer::print(text);
	}


	//! Prints out a text into the log
	void CLogger::log(const c8* text, const c8* hint, ELOG_LEVEL ll)
	{
		if (ll < LogLevel)
			return;

		core::stringc s = text;
		s += ": ";
		s += hint;
		log (s.c_str(), ll);
	}

	//! Prints out a text into the log
	void CLogger::log(const wchar_t* text, ELOG_LEVEL ll)
	{
		if (ll < LogLevel)
			return;

		core::stringc s = text;
		log(s.c_str(), ll);
	}


	//! Prints out a text into the log
	void CLogger::log(const wchar_t* text, const wchar_t* hint, ELOG_LEVEL ll)
	{
		if (ll < LogLevel)
			return;

		core::stringc s1 = text;
		core::stringc s2 = hint;
		log(s1.c_str(), s2.c_str(), ll);
	}

	//! Prints out a text into the log
	void CLogger::log(const c8* text, const wchar_t* hint, ELOG_LEVEL ll)
	{
		if (ll < LogLevel)
			return;

		core::stringc s2 = hint;
		log( text, s2.c_str(), ll);
	}

	//! Sets a new event receiver
	void CLogger::setReceiver(IEventReceiver* r)
	{
		Receiver = r;
	}


} // end namespace irr

