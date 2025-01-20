#include "GPUInformation.h"

void GPUInformation::InitializeAGSLib()
{
	AGSContext* agsContext = nullptr;
	AGSGPUInfo gpuInfo = {};
	AGSConfiguration config = {};
	if (agsInitialize(AGS_CURRENT_VERSION, &config, &agsContext, &gpuInfo) == AGS_SUCCESS)
	{
		printf("\nAGS Library initialized: v%d.%d.%d\n", AMD_AGS_VERSION_MAJOR, AMD_AGS_VERSION_MINOR, AMD_AGS_VERSION_PATCH);
		printf("-----------------------------------------------------------------\n");

		printf("Radeon Software Version:   %s\n", gpuInfo.radeonSoftwareVersion);
		printf("Driver Version:            %s\n", gpuInfo.driverVersion);
		printf("-----------------------------------------------------------------\n");
		PrintDisplayInfo(gpuInfo);
		printf("-----------------------------------------------------------------\n");


		if (agsDeInitialize(agsContext) != AGS_SUCCESS)
		{
			printf("Failed to cleanup AGS Library\n");
		}
	}
	else
	{
		printf("Failed to initialize AGS Library\n");
	}

	printf("\ndone\n");
}

void GPUInformation::PrintDisplayInfo(const AGSGPUInfo& gpuInfo)
{
	for (int gpuIndex = 0; gpuIndex < gpuInfo.numDevices; gpuIndex++)
	{
		const AGSDeviceInfo& device = gpuInfo.devices[gpuIndex];

		printf("\n---------- Device %d%s, %s\n", gpuIndex, device.isPrimaryDevice ? " [primary]" : "", device.adapterString);

		printf("Vendor id:   0x%04X (%s)\n", device.vendorId, getVendorName(device.vendorId));
		printf("Device id:   0x%04X\n", device.deviceId);
		printf("Revision id: 0x%04X\n\n", device.revisionId);

		const char* asicFamily[] =
		{
			"unknown",
			"Pre GCN",
			"GCN Gen1",
			"GCN Gen2",
			"GCN Gen3",
			"GCN Gen4",
			"Vega",
			"RDNA",
			"RDNA2",
			"RDNA3"
		};

		static_assert(_countof(asicFamily) == AGSDeviceInfo::AsicFamily_Count, "asic family table out of date");

		if (device.vendorId == 0x1002)
		{
			char wgpInfo[256] = {};
			if (device.asicFamily >= AGSDeviceInfo::AsicFamily_RDNA)
			{
				sprintf_s(wgpInfo, ", %d WGPs", device.numWGPs);
			}

			printf("Architecture: %s, %s%s%d CUs%s, %d ROPs\n", asicFamily[device.asicFamily], device.isAPU ? "(APU), " : "", device.isExternal ? "(External), " : "", device.numCUs, wgpInfo, device.numROPs);
			printf("    core clock %d MHz, memory clock %d MHz\n", device.coreClock, device.memoryClock);
			printf("    %.1f Tflops\n", device.teraFlops);
			printf("local memory: %d MBs (%.1f GB/s), shared memory: %d MBs\n\n", (int)(device.localMemoryInBytes / (1024 * 1024)), (float)device.memoryBandwidth / 1024.0f, (int)(device.sharedMemoryInBytes / (1024 * 1024)));
		}

		printf("\n");

		if (device.eyefinityEnabled)
		{
			printf("SLS grid is %d displays wide by %d displays tall\n", device.eyefinityGridWidth, device.eyefinityGridHeight);
			printf("SLS resolution is %d x %d pixels%s\n", device.eyefinityResolutionX, device.eyefinityResolutionY, device.eyefinityBezelCompensated ? ", bezel-compensated" : "");
		}
		else
		{
			printf("Eyefinity not enabled on this device\n");
		}

		printf("\n");

		for (int i = 0; i < device.numDisplays; i++)
		{
			const AGSDisplayInfo& display = device.displays[i];

			printf("\t---------- Display %d %s----------------------------------------\n", i, display.isPrimaryDisplay ? "[primary]" : "---------");

			printf("\tdevice name: %s\n", display.displayDeviceName);
			printf("\tmonitor name: %s\n\n", display.name);

			printf("\tMax resolution:             %d x %d, %.1f Hz\n", display.maxResolutionX, display.maxResolutionY, display.maxRefreshRate);
			printf("\tCurrent resolution:         %d x %d, Offset (%d, %d), %.1f Hz\n", display.currentResolution.width, display.currentResolution.height, display.currentResolution.offsetX, display.currentResolution.offsetY, display.currentRefreshRate);
			printf("\tVisible resolution:         %d x %d, Offset (%d, %d)\n\n", display.visibleResolution.width, display.visibleResolution.height, display.visibleResolution.offsetX, display.visibleResolution.offsetY);

			printf("\tchromaticity red:           %f, %f\n", display.chromaticityRedX, display.chromaticityRedY);
			printf("\tchromaticity green:         %f, %f\n", display.chromaticityGreenX, display.chromaticityGreenY);
			printf("\tchromaticity blue:          %f, %f\n", display.chromaticityBlueX, display.chromaticityBlueY);
			printf("\tchromaticity white point:   %f, %f\n\n", display.chromaticityWhitePointX, display.chromaticityWhitePointY);

			printf("\tluminance: [min, max, avg]  %f, %f, %f\n", display.minLuminance, display.maxLuminance, display.avgLuminance);

			printf("\tscreen reflectance diffuse  %f\n", display.screenDiffuseReflectance);
			printf("\tscreen reflectance specular %f\n\n", display.screenSpecularReflectance);

			if (display.HDR10)
				printf("\tHDR10 supported\n");

			if (display.dolbyVision)
				printf("\tDolby Vision supported\n");

			if (display.freesync)
				printf("\tFreesync supported\n");

			if (display.freesyncHDR)
				printf("\tFreesync HDR supported\n");

			printf("\n");

			if (display.eyefinityInGroup)
			{
				printf("\tEyefinity Display [%s mode] %s\n", display.eyefinityInPortraitMode ? "portrait" : "landscape", display.eyefinityPreferredDisplay ? " (preferred display)" : "");

				printf("\tGrid coord [%d, %d]\n", display.eyefinityGridCoordX, display.eyefinityGridCoordY);
			}

			printf("\tlogical display index: %d\n", display.logicalDisplayIndex);
			printf("\tADL adapter index: %d\n\n", display.adlAdapterIndex);

			printf("\n");
		}
	}
}

const char* GPUInformation::getVendorName(int vendorId)
{
	switch (vendorId)
	{
	case 0x1002: return "AMD";
	case 0x8086: return "INTEL";
	case 0x10DE: return "NVIDIA";
	default: return "unknown";
	}
}
