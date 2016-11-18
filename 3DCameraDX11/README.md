# 3DCameraDX11
Threaded DX11 3D Camera Frame Acquisition

This project is meant for researching and testing whether or not it's possible to put the entire MS Kinect v2 management into a separate process(also separate thread from the main thread in the Unity ecosystem) and also if it can be optimized on how we feed the gpu data.

The reason why this project was created was how wasteful, frame acquisition and texture updates from those frames, is in Unity 
using the MS unity package. The MS SDK for Kinect v2 has been wrapped for Unity, but it is really wasteful. KinectService processes data from Kinect's sensor into the gpu in order to produce Depth and Skeleton Tracking. Then we acquire the frame data in cpu side and then ,at least, rgb data go back into a texture in gpu side. Thing is, in Unity, data are passed around in data arrays without the option of passing around pointers. So there's a lot of GC activity. Some of it, we can avoid by using GCHandles but even then, apart from simple stuff, anything else forces us to cast entire arrays into other data types to be able to perform queries and analyse data etc. Also textures are updated but , for example, the rgb frame is converted in the acquisition frame into rgba format since Unity does not support yuv2 texture format. Also we are required to pass the data arrays to functions in order to upload the data to the gpu.

Mind you, there are ways to improve upon certain stuff, but you have no easy way to upload textures in a separate thread without impacting the main thread of the Unity application. And there's no SIMD.

Other areas have wasteful parts as well, like joint positions. Kinect SDK does not provide the whole skeleton hierarchy per frame due to bandwidth issues, so you have to check how many joints are provided in each frame and then check whether the joint you are looking for is present etc. If you don't mess with the code provided by the MS unity package, a readonly Dictionary<JointType,Vector4> is created per ACCESS. That means that in an application that heavily uses skeleton tracking for multiple bodies and accessing joint positions for each for various calculations, means GC chaos. 

Like I said, many of the above, can be avoided, but it amounts to lots of work and constant battle with the engine. So using separate threads/processes can alleviate some of the constraints and some of the incompatibilities for implementation and potential freedom.

Unity has made changes recently in their Rendering system and has made it possible to create a native plugin that updates GPU
resources outside Unity completely or outside the main thread.

So the idea is to create a threaded plugin that has the whole of the communication and data acquisition from the 3D camera, that
also creates and updates textures in the GPU using the frame data and sends the data as needed to the client application.

Granted, the code will be tailored to a specific scenario but it should be fairly adaptable to other scenarios by modifying this
"template". For now it is merely a proof of concept.
