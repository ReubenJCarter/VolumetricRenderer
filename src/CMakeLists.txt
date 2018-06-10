
set (volumetricRendererSrc 
		Util.cpp
		Main.cpp
		Image3D.cpp
		MainWindow.cpp
		HelperWidgets.cpp
		ControlPanel.cpp
		RenderViewport.cpp
		TestGenerateVolume.cpp
		SampleMappingEditor.cpp
		ImageSettings.cpp
		
		Renderer/Texture3D.cpp
		Renderer/TextureCube.cpp
		Renderer/Texture1D.cpp
		Renderer/MeshObject.cpp
		Renderer/Object3D.cpp
		Renderer/TextureVolumeObject.cpp
		Renderer/RayVolumeObject.cpp
		Renderer/PhotonVolumeObject.cpp
		Renderer/CameraObject.cpp
		Renderer/CameraControl.cpp
		Renderer/AxisObject.cpp
		
		IO/Image3DFromDicomFile.cpp
		IO/Image3DFromTIFFFile.cpp
		IO/Image3DFromPNGFile.cpp
		IO/Image3DFromDevilFile.cpp
		IO/Image3DFromNRRDFile.cpp
)

add_executable(VolumetricRenderer ${volumetricRendererSrc})

target_link_libraries(VolumetricRenderer Qt5::Core Qt5::Widgets Qt5::OpenGL)

target_link_libraries(VolumetricRenderer dcmdata dcmimgle dcmimage )

target_link_libraries(VolumetricRenderer tiff)

target_link_libraries(VolumetricRenderer libpng16)

target_link_libraries(VolumetricRenderer DevIL)

target_link_libraries(VolumetricRenderer teem)

target_link_libraries(VolumetricRenderer opencv_world341d)

install(TARGETS VolumetricRenderer
		RUNTIME
		DESTINATION bin
		COMPONENT MainApplication)	