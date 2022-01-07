#pragma once
#include "./ConfigManager.h"
#include <rapidconfig.h>

RCCreateSection(MainConfig, {
	RCAddSection(Window, {
		RCAddNumberArray(Dimensions, { 10,10,300,200 });
		RCAddNumber(LastOpen, -1);
	});
	RCAddSection(Interface, {
		RCAddBoolean(ShowTime, true);
	});
});

extern MainConfig config;