#pragma once
#include <Windows.h>
#include <iostream>
#include "amd_ags.h"

// this class is used to output various gpu information for debug purposes

class GPUInformation
{
public:
	void InitializeAGSLib();


private:
	void PrintDisplayInfo(const AGSGPUInfo& gpuInfo);
	const char* getVendorName(int vendorId);
};

