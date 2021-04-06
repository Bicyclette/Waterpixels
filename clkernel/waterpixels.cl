#define CL_HPP_TARGET_OPENCL_VERSION 200

// ########## TYPES ##########
// ###########################

typedef struct __attribute__ ((packed)) Gradient
{
	int gx;
	int gy;
}Gradient;

typedef struct __attribute__ ((packed)) labColor
{
	float l;
	float a;
	float b;
}labColor;

typedef struct __attribute__ ((packed)) rgbColor
{
	int r;
	int g;
	int b;
}rgbColor;

typedef struct __attribute__ ((packed)) labIsland
{
	labColor NW;
	labColor N;
	labColor NE;
	labColor W;
	labColor E;
	labColor SW;
	labColor S;
	labColor SE;
}labIsland;

typedef struct __attribute__ ((packed)) rgbIsland
{
	rgbColor NW;
	rgbColor N;
	rgbColor NE;
	rgbColor W;
	rgbColor E;
	rgbColor SW;
	rgbColor S;
	rgbColor SE;
}rgbIsland;

enum LAST_VISITED
{
	ROOT,
	N,
	S,
	E,
	W,
	NE,
	NW,
	SE,
	SW
};

// ########## GLOBAL CONSTANTS ##########
// ######################################

constant float3 rgbToXYZ_row1 = (float3)(0.618f, 0.117f, 0.205f);
constant float3 rgbToXYZ_row2 = (float3)(0.299f, 0.587f, 0.114f);
constant float3 rgbToXYZ_row3 = (float3)(0.0f, 0.056f, 0.944f);

constant float3 xyzToRGB_row1 = (float3)(1.876f, -0.533f, -0.343f);
constant float3 xyzToRGB_row2 = (float3)(-0.967f, 1.998f, -0.031f);
constant float3 xyzToRGB_row3 = (float3)(0.057f, -0.118f, 1.061f);

// ########## PROTOTYPES ##########
// ################################

labColor rgbToLab(rgbColor color);

rgbColor labToRgb(labColor color);

Gradient computeGradient(const int x, const int y, int index, const int width, const int height, global const unsigned char* src);

int validSeed(const int startIndex, const int endIndex, const int seedIndex, const global int* indices);

int growRegion(
		const int width,
		const int rootIndex,
		const int startIndex,
		const int endIndex,
		const global int* indices,
		const global unsigned char* gradient,
		const int minGradient,
		global bool* indexVisited);

void colorMaxRegion(
		const int width,
		const int rootIndex,
		const int startIndex,
		const int endIndex,
		const global int* indices,
		const global unsigned char* gradient,
		const int minGradient,
		global bool* indexVisited,
		global unsigned char* markers);

bool validIndex(int index, int width, int height);

// ########## WATERPIXELS FUNCTIONS ##########
// ###########################################

bool validIndex(int index, int width, int height)
{
	return index > 0 && index < (width * height * 3);
}

kernel void computeErode(
		const int gStep,
		const int width,
		const int height,
		global const unsigned char* img,
		global unsigned char* res,
		int fixedSize)
{
	size_t id = get_global_id(0);
	const int size = (fixedSize == 1) ? 2 : max(2, gStep / 16);
	int radius = size / 2;
	int baseIndex = id*3;
	int index;

	int red = 255;
	int green = 255;
	int blue = 255;

	// erode
	for(int l = -radius; l < radius; ++l)
	{
		for(int c = -radius; c < radius; ++c)
		{
			if(l != 0 && c != 0)
				continue;
			index = baseIndex + (l * width * 3) + (c * 3);
			if(validIndex(index, width, height))
			{
				if(img[index] < red)
					red = img[index];
				if(img[index+1] < green)
					green = img[index+1];
				if(img[index+2] < blue)
					blue = img[index+2];
			}
		}
	}
	res[baseIndex] = red;
	res[baseIndex+1] = green;
	res[baseIndex+2] = blue;
}

kernel void computeDilation(
		const int gStep,
		const int width,
		const int height,
		global const unsigned char* img,
		global unsigned char* res,
		int fixedSize)
{
	size_t id = get_global_id(0);
	const int size = (fixedSize == 1) ? 2 : max(2, gStep / 16);
	int radius = size / 2;
	int baseIndex = id*3;
	int index;

	int red = 0;
	int green = 0;
	int blue = 0;

	// dilate
	for(int l = -radius; l < radius; ++l)
	{
		for(int c = -radius; c < radius; ++c)
		{
			if(l != 0 && c != 0)
				continue;
			index = baseIndex + (l * width * 3) + (c * 3);
			if(validIndex(index, width, height))
			{
				if(img[index] > red)
					red = img[index];
				if(img[index+1] > green)
					green = img[index+1];
				if(img[index+2] > blue)
					blue = img[index+2];
			}
		}
	}
	res[baseIndex] = red;
	res[baseIndex+1] = green;
	res[baseIndex+2] = blue;
}

kernel void computeOutline(
		const int gStep,
		const int width,
		const int height,
		global const unsigned char* img,
		global unsigned char* res)
{
	size_t id = get_global_id(0);
	const int size = 4;
	int radius = size / 2;
	int baseIndex = id*3;
	int index;

	int red = 0;
	int green = 0;
	int blue = 0;

	// dilate
	for(int l = -radius; l < radius; ++l)
	{
		for(int c = -radius; c < radius; ++c)
		{
			index = baseIndex + (l * width * 3) + (c * 3);
			if(validIndex(index, width, height))
			{
				if(img[index] > red)
					red = img[index];
				if(img[index+1] > green)
					green = img[index+1];
				if(img[index+2] > blue)
					blue = img[index+2];
			}
		}
	}
	if(red == 255 && green == 255 && blue == 255)
	{
		res[baseIndex] = 10;
		res[baseIndex+1] = 10;
		res[baseIndex+2] = 10;
	}
}

kernel void computeMarkers(
		const int width,
		global const int* indices,
		global const int* indicesCount,
		global bool* indexVisited,
		global const unsigned char* gradient,
		global unsigned char* markers)
{
	size_t id = get_global_id(0);

	int offset = 0;
	for(int i = 0; i < id; ++i)
	{
		offset = offset + indicesCount[i];
	}

	// compute min gradient in cell
	int minGradient = 255;
	int pixel;
	int pixelCount = indicesCount[id];
	for(int i = offset; i < (offset + pixelCount); ++i)
	{
		pixel = indices[i];
		if(gradient[pixel] < minGradient)
			minGradient = gradient[pixel];
	}

	// get seed giving the max area
	int maxSeed = -1;
	int maxArea = -1;
	int area;
	float coverage;
	int seedIndex;
	for(int i = offset; i < offset + pixelCount; ++i)
		indexVisited[i] = false;

	for(int i = offset; i < (offset + pixelCount); ++i)
	{
		seedIndex = indices[i];
		if(gradient[seedIndex] == minGradient && !indexVisited[i - offset])
		{
			area = growRegion(width, seedIndex, offset, offset + pixelCount, indices, gradient, minGradient, indexVisited);
			coverage = (float)(area) / (float)(pixelCount);
			if(area > maxArea)
			{
				maxArea = area;
				maxSeed = seedIndex;
				if(coverage >= 0.5f)
					break;
			}
		}
	}

	for(int i = offset; i < offset + pixelCount; ++i)
		indexVisited[i] = false;

	// color markers with highest surface extinction
	colorMaxRegion(width, maxSeed, offset, offset + pixelCount, indices, gradient, minGradient, indexVisited, markers);
}

int validSeed(const int startIndex, const int endIndex, const int seedIndex, const global int* indices)
{
	int node;
	for(int i = startIndex; i < endIndex; ++i)
	{
		if(seedIndex == indices[i])
			return i;
	}
	return -1;
}

int growRegion(
		const int width,
		const int rootIndex,
		const int startIndex,
		const int endIndex,
		global const int* indices,
		global const unsigned char* gradient,
		const int minGradient,
		global bool* indexVisited)
{
	// array of seeds
	int seedIndex[262144];
	seedIndex[0] = rootIndex;
	int seeds = 1;

	int node;
	int N; int S; int E; int W;
	int NE; int NW; int SE; int SW;

	int area = 0;
	int offset = 0;

	while(seeds > 0)
	{
		for(int i = 0; i < seeds; ++i)
		{
			int index = seedIndex[i];
			node = validSeed(startIndex, endIndex, index, indices);
			if(node != -1 && gradient[index] == minGradient)
			{
				// surface increase
				area++;

				// mark seed as visited
				indexVisited[node] = true;

				// get surrounding nodes
				N = index - (width*3);
				S = index + (width*3);
				E = index + 3;
				W = index - 3;
				NE = index - (width*3) + 3;
				NW = index - (width*3) - 3;
				SE = index + (width*3) + 3;
				SW = index + (width*3) - 3;

				// rewrite seedIndex array
				node = validSeed(startIndex, endIndex, N, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = N;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, S, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = S;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, E, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = E;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, W, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = W;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, NE, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = NE;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, NW, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = NW;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, SE, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = SE;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, SW, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = SW;
					offset = offset + 1;
				}
			}
		}
		seeds = offset;
		offset = 0;
	}

	return area;
}

void colorMaxRegion(
		const int width,
		const int rootIndex,
		const int startIndex,
		const int endIndex,
		global const int* indices,
		global const unsigned char* gradient,
		const int minGradient,
		global bool* indexVisited,
		global unsigned char* markers)
{
	// array of seeds
	int seedIndex[262144];
	seedIndex[0] = rootIndex;
	int seeds = 1;

	int node;
	int N; int S; int E; int W;
	int NE; int NW; int SE; int SW;

	int offset = 0;

	while(seeds > 0)
	{
		for(int i = 0; i < seeds; ++i)
		{
			int index = seedIndex[i];
			node = validSeed(startIndex, endIndex, index, indices);
			if(node != -1 && gradient[index] == minGradient)
			{
				// color seed
				markers[index + 1] = 255;

				// mark seed as visited
				indexVisited[node] = true;

				// get surrounding nodes
				N = index - (width*3);
				S = index + (width*3);
				E = index + 3;
				W = index - 3;
				NE = index - (width*3) + 3;
				NW = index - (width*3) - 3;
				SE = index + (width*3) + 3;
				SW = index + (width*3) - 3;

				// rewrite seedIndex array
				node = validSeed(startIndex, endIndex, N, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = N;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, S, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = S;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, E, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = E;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, W, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = W;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, NE, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = NE;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, NW, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = NW;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, SE, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = SE;
					offset = offset + 1;
				}
				node = validSeed(startIndex, endIndex, SW, indices);
				if(node != -1 && !indexVisited[node])
				{
					seedIndex[offset] = SW;
					offset = offset + 1;
				}
			}
		}
		seeds = offset;
		offset = 0;
	}
}

kernel void computeDistanceFromMarkers(
		const int width,
		const int height,
		const float gridStep,
		global const int* markersIndices,
		const int markersCount,
		global unsigned char* distanceFromMarkers)
{
	size_t id = get_global_id(0);
	int x = id % width;
	int y = (id / width) % height;

	float dist = FLT_MAX;

	for(int i = 0; i < markersCount; ++i)
	{
		int index = markersIndices[i];
		int markerX = index % width;
		int markerY = (index / width) % height;

		int deltaX = markerX - x;
		int deltaY = markerY - y;

		dist = min(dist, sqrt( (float)(deltaX * deltaX) + (float)(deltaY * deltaY) ));
	}

	dist = dist * (2.0f/gridStep);
	dist = min(255.0f, dist * 64.0f);

	int res = (int)(dist);

	distanceFromMarkers[id*3] = res;
	distanceFromMarkers[id*3+1] = res;
	distanceFromMarkers[id*3+2] = res;
}

kernel void computeLabGradient(const int width, const int height, global const unsigned char* src, global unsigned char* dest)
{
	size_t id = get_global_id(0);
	int x = id % width;
	int y = (id / width) % height;

	Gradient grad = computeGradient(x, y, id*3, width, height, src);
	float gradient = sqrt((float)(grad.gx * grad.gx) + (float)(grad.gy * grad.gy));
	int g = (int)(gradient);

	dest[id*3] = g;
	dest[id*3+1] = g;
	dest[id*3+2] = g;
}

labColor rgbToLab(rgbColor color)
{
	const float xn = dot(rgbToXYZ_row1, (float3)(255.0f, 255.0f, 255.0f));
	const float yn = dot(rgbToXYZ_row2, (float3)(255.0f, 255.0f, 255.0f));
	const float zn = dot(rgbToXYZ_row3, (float3)(255.0f, 255.0f, 255.0f));
	
	float red = (float)(color.r);
	float green = (float)(color.g);
	float blue = (float)(color.b);
	float3 rgb = (float3)(red, green, blue);

	float x = dot(rgbToXYZ_row1, rgb);
	float y = dot(rgbToXYZ_row2, rgb);
	float z = dot(rgbToXYZ_row3, rgb);

	float xXN = x / xn; float fXXN = (xXN > 0.008856) ? cbrt(xXN) : 7.7787f * xXN + (16.0f/116.0f);
	float yYN = y / yn; float fYYN = (yYN > 0.008856) ? cbrt(yYN) : 7.7787f * yYN + (16.0f/116.0f);
	float zZN = z / zn; float fZZN = (zZN > 0.008856) ? cbrt(zZN) : 7.7787f * zZN + (16.0f/116.0f);

	float l;
	if(yYN > 0.008856)
		l = 116.0f * cbrt(yYN) - 16.0f;
	else
		l = 903.3f * yYN;

	float a = 500.0f * (fXXN - fYYN);
	float b = 200.0f * (fYYN - fZZN);

	labColor res;
	res.l = l;
	res.a = a;
	res.b = b;

	return res;
}

rgbColor labToRgb(labColor color)
{
	const float xn = dot(rgbToXYZ_row1, (float3)(255.0f, 255.0f, 255.0f));
	const float yn = dot(rgbToXYZ_row2, (float3)(255.0f, 255.0f, 255.0f));
	const float zn = dot(rgbToXYZ_row3, (float3)(255.0f, 255.0f, 255.0f));
	
	float l = color.l;
	float a = color.a;
	float b = color.b;

	float y = (l <= 8) ? yn * (l / 903.3f) : yn * pow(((l + 16.0f)/116.0f), 3);
	
	float t;
	float ft;
	
	t = (a / 500.0f) + ((l + 16.0f) / 116.0f);
	ft = (t > 0.207f) ? pow(t, 3) : (116.0f * t - 16.0f) / 903.3f;
	float x = xn * ft;

	t = ((l + 16.0f) / 116.0f) - (b / 200.0f);
	ft = (t > 0.207f) ? pow(t, 3) : (116.0f * t - 16.0f) / 903.3f;
	float z = zn * ft;

	float3 xyz = (float3)(x, y, z);

	float red = dot(xyzToRGB_row1, xyz);
	float green = dot(xyzToRGB_row2, xyz);
	float blue = dot(xyzToRGB_row3, xyz);

	rgbColor res;
	res.r = (int)(red);
	res.g = (int)(green);
	res.b = (int)(blue);

	return res;
}

Gradient computeGradient(const int x, const int y, int index, const int width, const int height, global const unsigned char* src)
{
	rgbIsland rgbMap;
	labIsland labMap;
	
	if(x == 0 && y == 0)
	{
		rgbMap.NW.r = 0; rgbMap.NW.g = 0; rgbMap.NW.b = 0;
		rgbMap.N.r = 0; rgbMap.N.g = 0; rgbMap.N.b = 0;
		rgbMap.NE.r = 0; rgbMap.NE.g = 0; rgbMap.NE.b = 0;
		rgbMap.W.r = 0; rgbMap.W.g = 0; rgbMap.W.b = 0;
		rgbMap.SW.r = 0; rgbMap.SW.g = 0; rgbMap.SW.b = 0;

		rgbMap.E.r = src[index + 3];
		rgbMap.E.g = src[index + 4];
		rgbMap.E.b = src[index + 5];
		
		rgbMap.S.r = src[index + (3 * width)];
		rgbMap.S.g = src[index + (3 * width) + 1];
		rgbMap.S.b = src[index + (3 * width) + 2];
		
		rgbMap.SE.r = src[index + (3 * width) + 3];
		rgbMap.SE.g = src[index + (3 * width) + 4];
		rgbMap.SE.b = src[index + (3 * width) + 5];
	}
	else if(x == (width - 1) && y == 0)
	{
		rgbMap.NW.r = 0; rgbMap.NW.g = 0; rgbMap.NW.b = 0;
		rgbMap.N.r = 0; rgbMap.N.g = 0; rgbMap.N.b = 0;
		rgbMap.NE.r = 0; rgbMap.NE.g = 0; rgbMap.NE.b = 0;
		rgbMap.E.r = 0; rgbMap.E.g = 0; rgbMap.E.b = 0;
		rgbMap.SE.r = 0; rgbMap.SE.g = 0; rgbMap.SE.b = 0;

		rgbMap.W.r = src[index - 3];
		rgbMap.W.g = src[index - 2];
		rgbMap.W.b = src[index - 1];
		
		rgbMap.S.r = src[index + (3 * width)];
		rgbMap.S.g = src[index + (3 * width) + 1];
		rgbMap.S.b = src[index + (3 * width) + 2];
		
		rgbMap.SW.r = src[index + (3 * width) - 3];
		rgbMap.SW.g = src[index + (3 * width) - 2];
		rgbMap.SW.b = src[index + (3 * width) - 1];
	}
	else if(x == 0 && y == (height - 1))
	{
		rgbMap.NW.r = 0; rgbMap.NW.g = 0; rgbMap.NW.b = 0;
		rgbMap.W.r = 0; rgbMap.W.g = 0; rgbMap.W.b = 0;
		rgbMap.SE.r = 0; rgbMap.SE.g = 0; rgbMap.SE.b = 0;
		rgbMap.S.r = 0; rgbMap.S.g = 0; rgbMap.S.b = 0;
		rgbMap.SW.r = 0; rgbMap.SW.g = 0; rgbMap.SW.b = 0;

		rgbMap.E.r = src[index + 3];
		rgbMap.E.g = src[index + 4];
		rgbMap.E.b = src[index + 5];
		
		rgbMap.N.r = src[index - (3 * width)];
		rgbMap.N.g = src[index - (3 * width) + 1];
		rgbMap.N.b = src[index - (3 * width) + 2];
		
		rgbMap.NE.r = src[index - (3 * width) + 3];
		rgbMap.NE.g = src[index - (3 * width) + 4];
		rgbMap.NE.b = src[index - (3 * width) + 5];
	}
	else if(x == (width - 1) && y == (height - 1))
	{
		rgbMap.SW.r = 0; rgbMap.NW.g = 0; rgbMap.NW.b = 0;
		rgbMap.S.r = 0; rgbMap.N.g = 0; rgbMap.N.b = 0;
		rgbMap.SE.r = 0; rgbMap.NE.g = 0; rgbMap.NE.b = 0;
		rgbMap.E.r = 0; rgbMap.W.g = 0; rgbMap.W.b = 0;
		rgbMap.NE.r = 0; rgbMap.SW.g = 0; rgbMap.SW.b = 0;

		rgbMap.W.r = src[index - 3];
		rgbMap.W.g = src[index - 2];
		rgbMap.W.b = src[index - 1];
		
		rgbMap.N.r = src[index - (3 * width)];
		rgbMap.N.g = src[index - (3 * width) + 1];
		rgbMap.N.b = src[index - (3 * width) + 2];
		
		rgbMap.NW.r = src[index - (3 * width) - 3];
		rgbMap.NW.g = src[index - (3 * width) - 2];
		rgbMap.NW.b = src[index - (3 * width) - 1];
	}
	else
	{
		rgbMap.NW.r = src[index - (3 * width) - 3];
		rgbMap.NW.g = src[index - (3 * width) - 2];
		rgbMap.NW.b = src[index - (3 * width) - 1];
		
		rgbMap.N.r = src[index - (3 * width)];
		rgbMap.N.g = src[index - (3 * width) + 1];
		rgbMap.N.b = src[index - (3 * width) + 2];
		
		rgbMap.NE.r = src[index - (3 * width) + 3];
		rgbMap.NE.g = src[index - (3 * width) + 4];
		rgbMap.NE.b = src[index - (3 * width) + 5];
		
		rgbMap.W.r = src[index - 3];
		rgbMap.W.g = src[index - 2];
		rgbMap.W.b = src[index - 1];
		
		rgbMap.E.r = src[index + 3];
		rgbMap.E.g = src[index + 4];
		rgbMap.E.b = src[index + 5];
		
		rgbMap.SW.r = src[index + (3 * width) - 3];
		rgbMap.SW.g = src[index + (3 * width) - 2];
		rgbMap.SW.b = src[index + (3 * width) - 1];
		
		rgbMap.S.r = src[index + (3 * width)];
		rgbMap.S.g = src[index + (3 * width) + 1];
		rgbMap.S.b = src[index + (3 * width) + 2];
		
		rgbMap.SE.r = src[index + (3 * width) + 3];
		rgbMap.SE.g = src[index + (3 * width) + 4];
		rgbMap.SE.b = src[index + (3 * width) + 5];
	}

	labMap.NW = rgbToLab(rgbMap.NW);
	labMap.N = rgbToLab(rgbMap.N);
	labMap.NE = rgbToLab(rgbMap.NE);
	labMap.W = rgbToLab(rgbMap.W);
	labMap.E = rgbToLab(rgbMap.E);
	labMap.SW = rgbToLab(rgbMap.SW);
	labMap.S = rgbToLab(rgbMap.S);
	labMap.SE = rgbToLab(rgbMap.SE);

	float labGx = labMap.NW.l -1 * labMap.NE.l +2 * labMap.W.l -2 * labMap.E.l + labMap.SW.l - labMap.SE.l;
	labGx = labGx + labMap.NW.a -1 * labMap.NE.a +2 * labMap.W.a -2 * labMap.E.a + labMap.SW.a - labMap.SE.a;
	labGx = labGx + labMap.NW.b -1 * labMap.NE.b +2 * labMap.W.b -2 * labMap.E.b + labMap.SW.b - labMap.SE.b;

	float labGy = labMap.NW.l +2 * labMap.N.l + labMap.NE.l -1 * labMap.SW.l -2 * labMap.S.l -1 * labMap.SE.l;
	labGy = labGy + labMap.NW.a +2 * labMap.N.a + labMap.NE.a -1 * labMap.SW.a -2 * labMap.S.a -1 * labMap.SE.a;
	labGy = labGy + labMap.NW.b +2 * labMap.N.b + labMap.NE.b -1 * labMap.SW.b -2 * labMap.S.b -1 * labMap.SE.b;

	Gradient g;
	g.gx = (int)(labGx);
	g.gy = (int)(labGy);

	return g;
}
