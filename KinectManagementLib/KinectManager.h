#pragma once
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the KINECTMANAGEMENTLIB_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// KINECTMANAGEMENTLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef KINECTMANAGEMENTLIB_EXPORTS
#define KINECTMANAGEMENTLIB_API __declspec(dllexport)
#else
#define KINECTMANAGEMENTLIB_API __declspec(dllimport)
#endif

#ifndef __KINECTMANAGER_H_INCLUDED__   // if x.h hasn't been included yet...
#define __KINECTMANAGER_H_INCLUDED__   //   #define this so the compiler knows it has been included

#include <Kinect.h>

#define KINECT_STREAM_COLOR_WIDTH 1920
#define KINECT_STREAM_COLOR_HEIGHT 1080

namespace KinectManagement {

	// a struct to define a single vertex
	struct VERTEX { FLOAT X, Y, Z, W; };

	struct COLOR { FLOAT R, G, B, A; };

	struct sVertexType
	{
		float fX;
		float fY;
		float fZ;
		float fU;
		float fV;
	};


	class KINECTMANAGEMENTLIB_API KinectManager
	{
		// Current Kinect
		IKinectSensor* m_pKinectSensor;
		// Color reader
		IColorFrameReader* m_pColorFrameReader;


	public:
		COLOR m_pColorRGBX[KINECT_STREAM_COLOR_WIDTH*KINECT_STREAM_COLOR_HEIGHT];
		IFrameDescription *m_pColorFrameDescription;
		unsigned int color_frame_length_in_pixels;
		unsigned int color_frame_bytes_per_pixel;
		//VERTEX m_pColorRGBX[KINECT_STREAM_COLOR_WIDTH*KINECT_STREAM_COLOR_HEIGHT];
		
		KinectManager();
		~KinectManager();

		HRESULT Initialize(void);
		void Update(void);
		void Cleanup(void);
	};
}
#endif 