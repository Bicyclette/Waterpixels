#ifndef CLPROGRAM_HPP
#define CLPROGRAM_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <utility>
#include <algorithm>

#define CL_HPP_TARGET_OPENCL_VERSION 210
#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/cl2.hpp>

class CLProgram
{
	public:

		CLProgram(const std::string & file);
		cl::Kernel & getErodeKernel();
		cl::Kernel & getDilationKernel();
		cl::Kernel & getOutlineKernel();
		cl::Kernel & getGradientKernel();
		cl::Kernel & getMarkersKernel();
		cl::Kernel & getDistanceKernel();
		cl::CommandQueue & getCommandQueue();
		cl::Context & getContext();

	private:

		std::vector<cl::Device> devices;
		cl::Device device;
		cl::Context context;
		cl::CommandQueue queue;
		cl::Program program;
		cl::Kernel erodeKernel;
		cl::Kernel dilationKernel;
		cl::Kernel outlineKernel;
		cl::Kernel gradientKernel;
		cl::Kernel markersKernel;
		cl::Kernel distanceKernel;
};

#endif
