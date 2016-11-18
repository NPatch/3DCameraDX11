// KinectManagementLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "KinectManager.h"


namespace KinectManagement {

	// This is the constructor of a class that has been exported.
	// see KinectManagementLib.h for the class definition
	KinectManager::KinectManager()
	{
		
	}

	KinectManager::~KinectManager()
	{
	}

	HRESULT KinectManager::Initialize(void)
	{
		HRESULT hr;

		hr = GetDefaultKinectSensor(&m_pKinectSensor);
		
		if (FAILED(hr)) return hr;

		if (m_pKinectSensor)
		{
			// Initialize the Kinect and get the color reader
			IColorFrameSource* pColorFrameSource = NULL;

			hr = m_pKinectSensor->Open();

			if (SUCCEEDED(hr))
			{
				hr = m_pKinectSensor->get_ColorFrameSource(&pColorFrameSource);
			}

			if (SUCCEEDED(hr))
			{
				hr = pColorFrameSource->OpenReader(&m_pColorFrameReader);
			}

			SafeRelease(pColorFrameSource);
		}

		if (!m_pKinectSensor || FAILED(hr))
		{
			//SetStatusMessage(L"No ready Kinect found!", 10000, true);
			return E_FAIL;
		}

		return hr;

	}

	void KinectManager::Update(void)
	{
		if (!m_pColorFrameReader)
		{
			return;
		}

		IColorFrame* pColorFrame = NULL;

		HRESULT hr = m_pColorFrameReader->AcquireLatestFrame(&pColorFrame);

		if (SUCCEEDED(hr))
		{
			INT64 nTime = 0;
			IFrameDescription* pFrameDescription = NULL;
			int nWidth = 0;
			int nHeight = 0;
			ColorImageFormat imageFormat = ColorImageFormat_None;

			hr = pColorFrame->get_RelativeTime(&nTime);

			if (SUCCEEDED(hr))
			{
				hr = pColorFrame->get_FrameDescription(&pFrameDescription);
			}

			if (SUCCEEDED(hr))
			{
				hr = pFrameDescription->get_Width(&nWidth);
			}

			if (SUCCEEDED(hr))
			{
				hr = pFrameDescription->get_Height(&nHeight);
			}

			if (SUCCEEDED(hr))
			{
				hr = pFrameDescription->get_BytesPerPixel(&color_frame_bytes_per_pixel);
			}

			if (SUCCEEDED(hr))
			{
				hr = pFrameDescription->get_LengthInPixels(&color_frame_length_in_pixels);
			}

			if (SUCCEEDED(hr))
			{
				hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
			}


			if (SUCCEEDED(hr))
			{



				if (imageFormat == ColorImageFormat_Rgba)
				{
					UINT nBufferSize = KINECT_STREAM_COLOR_WIDTH * KINECT_STREAM_COLOR_HEIGHT * color_frame_bytes_per_pixel;

					hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&m_pColorRGBX));
				}
				else if (m_pColorRGBX)
				{
					IFrameDescription* pRGBFrameDescription = NULL;
					pColorFrame->CreateFrameDescription(ColorImageFormat_Rgba, &pRGBFrameDescription);
					unsigned int clpp = 0;
					pRGBFrameDescription->get_BytesPerPixel(&clpp);
					unsigned int clip = 0;
					pRGBFrameDescription->get_LengthInPixels(&clip);
					color_frame_bytes_per_pixel = clpp;
					color_frame_length_in_pixels = clip;
					UINT nBufferSize = clip * clpp;
					hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, (reinterpret_cast<BYTE*>(&m_pColorRGBX)), ColorImageFormat_Rgba);
				}
				else
				{
					hr = E_FAIL;
				}
			}

			SafeRelease(pFrameDescription);
		}

		SafeRelease(pColorFrame);
	}

	void KinectManager::Cleanup(void)
	{
		// done with color frame reader
		SafeRelease(m_pColorFrameReader);

		// close the Kinect Sensor
		if (m_pKinectSensor)
		{
			m_pKinectSensor->Close();
		}

		SafeRelease(m_pKinectSensor);
	}
}