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

#define KINECT_STREAM_COLOR_WIDTH 1920
#define KINECT_STREAM_COLOR_HEIGHT 1080

namespace KinectManagement {

	// a struct to define a single vertex
	struct VERTEX { FLOAT X, Y, Z, W; };

	struct sVertexType
	{
		float fX;
		float fY;
		float fZ;
		float fNX;
		float fNY;
		float fNZ;
		float fU;
		float fV;
	};

	class KINECTMANAGEMENTLIBRARYAPI KinectManager
	{

	public:
		KinectManager();
		~KinectManager();

		static void Initialize(void);
		static void Update(void);
		static void Cleanup(void);

		static void* GetDataAddr(void);

	};
}