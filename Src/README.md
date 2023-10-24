
UNICO_simulink.slx :    Simulink project of the UNICO controller. This controller can be coupled to OpenFAST
			using the OpenFAST SFunction. An example is provided in "matlab-toolbox/runFAST.m"
			To run the example, download the OpenFAST v3.5.0 SFunction, that is available at: 
			https://github.com/OpenFAST/openfast/releases/tag/v3.5.0
						
dll_controller :		In this folder a dynamic library version (.dll) of the UNICO controller is stored. 
				The .dll can be used in wind turbine simulation tools that support Bladed-style 
				control interfaces. It was tested succesfully in OpenFAST v2.4.0 and OpenFAST v3.5.0.
				OpenFAST is a simulation software distributed by NREL: https://github.com/OpenFAST
				Instructions to use and compile the .dll version of the controller are in a separate
				"readme" file within the "dll_controller" folder. 
