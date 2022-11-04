#ifndef APP_HPP
#define APP_HPP

#include <QApplication>
#include <QtWidgets>
#include <QObject>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QDir>
#include <iostream>
#include <vector>
#include <cmath>
#include <clprogram.hpp>
#include <thread>
#include <memory>
#include <utility>
#include <limits>
#include <algorithm>
#include <queue>
#include <set>
#include <ctime>
#include <cstdlib>
#include <omp.h>

struct Image
{
	QPainter painter;

	QPixmap original;
	QPixmap smooth;
	QPixmap gradient;
	QPixmap regularizedGradient;
	QPixmap contours;
	QPixmap result;
	
	QGraphicsPixmapItem* originalItem;
	QGraphicsPixmapItem* smoothItem;
	QGraphicsPixmapItem* gradientItem;
	QGraphicsPixmapItem* regularizedGradientItem;
	QGraphicsPixmapItem* resultItem;

	unsigned char* originalRAW;
	std::unique_ptr<unsigned char[]> smoothRAW;
	unsigned char* gradientRAW;
	unsigned char* regularizedGradientRAW;
	std::unique_ptr<int[]> labelsMap;
	std::unique_ptr<unsigned char[]> contoursRAW;
	
	int width;
	int height;
	std::string name;
};

struct Grid
{
	int step;
	float rho;
	int cellCenters;

	QPixmap hexagonGrid;
	QPixmap markers;
	QPixmap distanceFromMarkers;

	QGraphicsPixmapItem* hexagonGridItem;
	QGraphicsPixmapItem* markersItem;
	QGraphicsPixmapItem* distanceFromMarkersItem;

	unsigned char* hexagonGridRAW;
	unsigned char* markersRAW;
	unsigned char* distanceFromMarkersRAW;
	std::vector<QPolygon> cells; // stride between cells is 12 => 6 points with x and y data
};

class Window : public QMainWindow
{
	Q_OBJECT

	public:
		Window();
		~Window();
		void resetImageData();

	public slots:
		void openImg();
		void saveDoc();
		void computeHexagonGrid();
		void computeWaterpixels();
		void showOriginalImage();
		void hideOriginalImage();
		void showSmooth();
		void hideSmooth();
		void showGrid();
		void hideGrid();
		void showGradient();
		void hideGradient();
		void showMarkers();
		void hideMarkers();
		void showDistanceMarkers();
		void hideDistanceMarkers();
		void showRegularizedGradient();
		void hideRegularizedGradient();
		void showContours();
		void hideContours();

	private:
		void createActions();
		void createMenus();
		void createStatusBar();
		bool loadImage(const QString & path);
		void drawCell(const int x, const int y, const int slicesX, const int slicesY, const int hexWidth);
		void computeSmooth();
		void computeLabGradient();
		void computeCellMarkers();
		void computeDistanceFromMarkers();
		void computeRegularizedGradient();

		void computeWatershed();
		void initBasins(std::vector<std::vector<int>> & basins);
		void printBasins(std::vector<std::vector<int>> & basins, bool details = false);
		bool validIndex(int index);

		QMenu* menuFile;
		QMenu* menuImage;
		QMenu* menuLayer;
		
		QAction* openImageAction;
		QAction* saveDocAction;
		QAction* quitAction;
		QAction* computeGridAction;
		QAction* computeWaterpixelsAction;
		QAction* showOriginalImageAction;
		QAction* hideOriginalImageAction;
		QAction* showSmoothAction;
		QAction* hideSmoothAction;
		QAction* showGridAction;
		QAction* hideGridAction;
		QAction* showGradientAction;
		QAction* hideGradientAction;
		QAction* showMarkersAction;
		QAction* hideMarkersAction;
		QAction* showDistanceMarkersAction;
		QAction* hideDistanceMarkersAction;
		QAction* showRegGradientAction;
		QAction* hideRegGradientAction;
		QAction* showContoursAction;
		QAction* hideContoursAction;

		QStatusBar* status;

		QGraphicsScene* scene;
		QGraphicsView* view;

		CLProgram program;

		struct Image img;
		struct Grid grid;

		double start;
		double end;
};

void computeCellMarkersThread(
				const int thr,
				const int numThreads,
				int width,
				int cellCenters,
				std::vector<int>& indices,
				std::vector<int>& indicesCount,
				unsigned char* gradient,
				unsigned char* markers);

int growRegion(
			const int width,
			const int seedIndex,
			const int startIndex,
			const int endIndex,
			std::vector<int>& indices,
			unsigned char* gradient,
			const int minGradient,
			bool indexVisited[]);

/**
 *	return index of the found seed in the indices tab
 */
int validSeed(const int startIndex, const int endIndex, const int seedIndex, std::vector<int>& indices);

void colorMaxRegion(
				const int width,
				const int seedIndex,
				const int startIndex,
				const int endIndex,
				std::vector<int>& indices,
				unsigned char* gradient,
				const int minGradient,
				unsigned char* markers,
				bool indexVisited[]);

#endif
