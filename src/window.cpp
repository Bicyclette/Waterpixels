#include "window.hpp"

Window::Window() :
	QMainWindow(),
	program("../clkernel/waterpixels.cl")
{
	img.smoothItem = nullptr;
	img.originalItem = nullptr;
	grid.hexagonGridItem = nullptr;
	grid.markersItem = nullptr;
	grid.distanceFromMarkersItem = nullptr;
	img.gradientItem = nullptr;
	img.regularizedGradientItem = nullptr;
	img.contoursItem = nullptr;

	scene = new QGraphicsScene;
	view = new QGraphicsView(scene);
	setCentralWidget(view);

	createActions();
	createMenus();
	createStatusBar();

	resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
	
	QObject::connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	QObject::connect(openImageAction, SIGNAL(triggered()), this, SLOT(openImg()));
	QObject::connect(saveDocAction, SIGNAL(triggered()), this, SLOT(saveDoc()));
	
	QObject::connect(computeGridAction, SIGNAL(triggered()), this, SLOT(computeHexagonGrid()));
	QObject::connect(computeWaterpixelsAction, SIGNAL(triggered()), this, SLOT(computeWaterpixels()));
	
	QObject::connect(showOriginalImageAction, SIGNAL(triggered()), this, SLOT(showOriginalImage()));
	QObject::connect(hideOriginalImageAction, SIGNAL(triggered()), this, SLOT(hideOriginalImage()));
	QObject::connect(showSmoothAction, SIGNAL(triggered()), this, SLOT(showSmooth()));
	QObject::connect(hideSmoothAction, SIGNAL(triggered()), this, SLOT(hideSmooth()));
	QObject::connect(showGridAction, SIGNAL(triggered()), this, SLOT(showGrid()));
	QObject::connect(hideGridAction, SIGNAL(triggered()), this, SLOT(hideGrid()));
	QObject::connect(showGradientAction, SIGNAL(triggered()), this, SLOT(showGradient()));
	QObject::connect(hideGradientAction, SIGNAL(triggered()), this, SLOT(hideGradient()));
	QObject::connect(showMarkersAction, SIGNAL(triggered()), this, SLOT(showMarkers()));
	QObject::connect(hideMarkersAction, SIGNAL(triggered()), this, SLOT(hideMarkers()));
	QObject::connect(showDistanceMarkersAction, SIGNAL(triggered()), this, SLOT(showDistanceMarkers()));
	QObject::connect(hideDistanceMarkersAction, SIGNAL(triggered()), this, SLOT(hideDistanceMarkers()));
	QObject::connect(showRegGradientAction, SIGNAL(triggered()), this, SLOT(showRegularizedGradient()));
	QObject::connect(hideRegGradientAction, SIGNAL(triggered()), this, SLOT(hideRegularizedGradient()));
	QObject::connect(showContoursAction, SIGNAL(triggered()), this, SLOT(showContours()));
	QObject::connect(hideContoursAction, SIGNAL(triggered()), this, SLOT(hideContours()));

	// grid data
	grid.step = 0;
	grid.rho = 2.0f / 3.0f;
	grid.cellCenters = 0;
}

Window::~Window()
{
	delete[] img.originalRAW;
	delete[] grid.hexagonGridRAW;
	delete[] grid.markersRAW;
	delete[] img.gradientRAW;
	delete[] img.regularizedGradientRAW;
	delete[] grid.distanceFromMarkersRAW;
}

void Window::resetImageData()
{
	if(img.originalItem != nullptr)
	{
		scene->removeItem(img.originalItem);
		img.originalItem = nullptr;
		delete[] img.originalRAW;
	}
	
	if(img.smoothItem != nullptr)
	{
		scene->removeItem(img.smoothItem);
		img.smoothItem = nullptr;
	}

	if(grid.hexagonGridItem != nullptr)
	{
		scene->removeItem(grid.hexagonGridItem);
		grid.hexagonGridItem = nullptr;
		grid.cells.clear();
		delete[] grid.hexagonGridRAW;
	}

	if(grid.markersItem != nullptr)
	{
		scene->removeItem(grid.markersItem);
		grid.markersItem = nullptr;
		delete[] grid.markersRAW;
	}

	if(img.gradientItem != nullptr)
	{
		scene->removeItem(img.gradientItem);
		img.gradientItem = nullptr;
		delete[] img.gradientRAW;
	}
	
	if(img.regularizedGradientItem != nullptr)
	{
		scene->removeItem(img.regularizedGradientItem);
		img.regularizedGradientItem = nullptr;
		delete[] img.regularizedGradientRAW;
	}
	
	if(grid.distanceFromMarkersItem != nullptr)
	{
		scene->removeItem(grid.distanceFromMarkersItem);
		grid.distanceFromMarkersItem = nullptr;
		delete[] grid.distanceFromMarkersRAW;
	}

	if(img.contoursItem != nullptr)
	{
		scene->removeItem(img.contoursItem);
		img.contoursItem = nullptr;
	}
}

void Window::createActions()
{
	openImageAction = new QAction("open image", this);
	saveDocAction = new QAction("save document", this);
	quitAction = new QAction("quit", this);
	computeGridAction = new QAction("compute hexagon grid", this);
	computeWaterpixelsAction = new QAction("generate waterpixels", this);

	computeGridAction->setEnabled(false);;
	computeWaterpixelsAction->setEnabled(false);

	showOriginalImageAction = new QAction("show original image", this);
	hideOriginalImageAction = new QAction("hide original image", this);
	showSmoothAction = new QAction("show smoothed image");
	hideSmoothAction = new QAction("hide smoothed image");
	showGridAction = new QAction("show hexagon grid", this);
	hideGridAction = new QAction("hide hexagon grid", this);
	showGradientAction = new QAction("show gradient", this);
	hideGradientAction = new QAction("hide gradient", this);
	showMarkersAction = new QAction("show markers", this);
	hideMarkersAction = new QAction("hide markers", this);
	showDistanceMarkersAction = new QAction("show distance from markers", this);
	hideDistanceMarkersAction = new QAction("hide distance from markers", this);
	showRegGradientAction = new QAction("show spatial regularized gradient", this);
	hideRegGradientAction = new QAction("hide spatial regularized gradient", this);
	showContoursAction = new QAction("show contours", this);
	hideContoursAction = new QAction("hide contours", this);
	
	showOriginalImageAction->setEnabled(false);
	hideOriginalImageAction->setEnabled(false);
	showSmoothAction->setEnabled(false);
	hideSmoothAction->setEnabled(false);
	showGridAction->setEnabled(false);
	hideGridAction->setEnabled(false);
	showGradientAction->setEnabled(false);
	hideGradientAction->setEnabled(false);
	showMarkersAction->setEnabled(false);
	hideMarkersAction->setEnabled(false);
	showDistanceMarkersAction->setEnabled(false);
	hideDistanceMarkersAction->setEnabled(false);
	showRegGradientAction->setEnabled(false);
	hideRegGradientAction->setEnabled(false);
	showContoursAction->setEnabled(false);
	hideContoursAction->setEnabled(false);
}

void Window::createMenus()
{
	menuFile = menuBar()->addMenu("File");
	menuFile->addAction(openImageAction);
	menuFile->addAction(saveDocAction);
	menuFile->addAction(quitAction);

	menuImage = menuBar()->addMenu("Image");
	menuImage->addAction(computeGridAction);
	menuImage->addAction(computeWaterpixelsAction);

	menuLayer = menuBar()->addMenu("Layer");
	menuLayer->addAction(showOriginalImageAction);
	menuLayer->addAction(hideOriginalImageAction);
	menuLayer->addAction(showSmoothAction);
	menuLayer->addAction(hideSmoothAction);
	menuLayer->addAction(showGridAction);
	menuLayer->addAction(hideGridAction);
	menuLayer->addAction(showGradientAction);
	menuLayer->addAction(hideGradientAction);
	menuLayer->addAction(showMarkersAction);
	menuLayer->addAction(hideMarkersAction);
	menuLayer->addAction(showDistanceMarkersAction);
	menuLayer->addAction(hideDistanceMarkersAction);
	menuLayer->addAction(showRegGradientAction);
	menuLayer->addAction(hideRegGradientAction);
	menuLayer->addAction(showContoursAction);
	menuLayer->addAction(hideContoursAction);
}

void Window::createStatusBar()
{
	status = new QStatusBar();
	setStatusBar(status);
	setStyleSheet("QStatusBar{border-top: 1px outset grey;}");
}

bool Window::loadImage(const QString & path)
{
	if(img.original.load(path))
	{
		std::string data = path.toStdString();
		img.name = data.substr(data.find_last_of('/') + 1, data.size());
		status->showMessage(std::string("Image: " + img.name).c_str());
		img.name = img.name.substr(0, img.name.find_first_of('.'));
		return true;
	}
	return false;
}

void Window::openImg()
{
	QString filePath = QFileDialog::getOpenFileName(this, "Open image", QString(), "Images (*.png *.jpg *.jpeg *.bmp)");
	if(!loadImage(filePath))
		QMessageBox::warning(this, "Warning", "File not loaded.");
	else
	{
		// reset
		resetImageData();

		// add image
		img.originalItem = scene->addPixmap(img.original);
		img.originalItem->setZValue(-1);

		img.width = img.original.width();
		img.height = img.original.height();

		img.originalRAW = new unsigned char[img.width * img.height * 3];
		int pixel{0};
		QImage originalImage = img.original.toImage();
		for(int y{0}; y < img.height; ++y)
		{
			for(int x{0}; x < img.width; ++x)
			{
				QColor color = originalImage.pixelColor(x, y);
				img.originalRAW[pixel++] = color.red();
				img.originalRAW[pixel++] = color.green();
				img.originalRAW[pixel++] = color.blue();
			}
		}
		
		// fill pixmaps with black transparent pixels
		img.smooth = img.original;
		img.gradient = img.original;
		grid.hexagonGrid = img.original;
		grid.markers = img.original;
		grid.distanceFromMarkers = img.original;
		img.regularizedGradient = img.original;
		img.contours = img.original;

		img.smooth.fill(QColor(0, 0, 0, 255));
		img.gradient.fill(QColor(0, 0, 0, 255));
		grid.hexagonGrid.fill(QColor(0, 0, 0, 0));
		grid.markers.fill(QColor(0, 0, 0, 0));
		grid.distanceFromMarkers.fill(QColor(0, 0, 0, 255));
		img.regularizedGradient.fill(QColor(0, 0, 0, 255));
		img.contours.fill(QColor(0, 0, 0, 0));

		// set size of unsigned char arrays
		img.smoothRAW = std::make_unique<unsigned char[]>(img.width * img.height * 3);
		img.gradientRAW = new unsigned char[img.width * img.height * 3];
		grid.hexagonGridRAW = new unsigned char[img.width * img.height * 3];
		grid.markersRAW = new unsigned char[img.width * img.height * 3];
		grid.distanceFromMarkersRAW = new unsigned char[img.width * img.height * 3];
		img.regularizedGradientRAW = new unsigned char[img.width * img.height * 3];

		// update image actions state
		computeGridAction->setEnabled(true);
		computeWaterpixelsAction->setEnabled(false);
		
		// update layer actions state
		showOriginalImageAction->setEnabled(true);
		hideOriginalImageAction->setEnabled(true);
		showSmoothAction->setEnabled(false);
		hideSmoothAction->setEnabled(false);
		showGradientAction->setEnabled(false);
		hideGradientAction->setEnabled(false);
		showGridAction->setEnabled(false);
		hideGridAction->setEnabled(false);
		showMarkersAction->setEnabled(false);
		hideMarkersAction->setEnabled(false);
		showDistanceMarkersAction->setEnabled(false);
		hideDistanceMarkersAction->setEnabled(false);
	}
}

void Window::saveDoc()
{
	// get root path
	QString root = QDir::currentPath();

	// create new folder
	QDir().mkdir(img.name.c_str());
	root = root + QString("/") + QString(img.name.c_str());

	if(!img.original.save(root + QString("/original.jpg")))
	{
		QMessageBox::warning(this, "Error", "Original image could not be saved.");
	}
	if(!img.smooth.save(root + QString("/smooth.jpg")))
	{
		QMessageBox::warning(this, "Error", "Smoothed image could not be saved.");
	}
	if(!img.gradient.save(root + QString("/gradient.jpg")))
	{
		QMessageBox::warning(this, "Error", "Gradient image could not be saved.");
	}
	if(!img.regularizedGradient.save(root + QString("/regularized_gradient.jpg")))
	{
		QMessageBox::warning(this, "Error", "Regularized gradient image could not be saved.");
	}
	if(!grid.hexagonGrid.save(root + QString("/hexagon_grid.jpg")))
	{
		QMessageBox::warning(this, "Error", "Hexagon grid image could not be saved.");
	}
	if(!grid.markers.save(root + QString("/markers.jpg")))
	{
		QMessageBox::warning(this, "Error", "Markers image could not be saved.");
	}
	if(!grid.distanceFromMarkers.save(root + QString("/distance_from_markers.jpg")))
	{
		QMessageBox::warning(this, "Error", "Distance from markers image could not be saved.");
	}
	if(!img.contours.save(root + QString("/contours.jpg")))
	{
		QMessageBox::warning(this, "Error", "Contours image could not be saved.");
	}
}

void Window::computeWaterpixels()
{
	// smooth image
	computeSmooth();
	// compute lab gradient
	computeLabGradient();
	// compute cell markers
	computeCellMarkers();
	// compute distance from markers
	computeDistanceFromMarkers();
	// compute spatial regularization of the gradient
	computeRegularizedGradient();
	// watershed
	computeWatershed();
}

void Window::showOriginalImage()
{
	img.originalItem->show();
}

void Window::hideOriginalImage()
{
	img.originalItem->hide();
}

void Window::showSmooth()
{
	img.smoothItem->show();
}

void Window::hideSmooth()
{
	img.smoothItem->hide();
}

void Window::showGrid()
{
	grid.hexagonGridItem->show();
}

void Window::hideGrid()
{
	grid.hexagonGridItem->hide();
}

void Window::showGradient()
{
	img.gradientItem->show();
}

void Window::hideGradient()
{
	img.gradientItem->hide();
}

void Window::showMarkers()
{
	grid.markersItem->show();
}

void Window::hideMarkers()
{
	grid.markersItem->hide();
}

void Window::showDistanceMarkers()
{
	grid.distanceFromMarkersItem->show();
}

void Window::hideDistanceMarkers()
{
	grid.distanceFromMarkersItem->hide();
}

void Window::showRegularizedGradient()
{
	img.regularizedGradientItem->show();
}

void Window::hideRegularizedGradient()
{
	img.regularizedGradientItem->hide();
}

void Window::showContours()
{
	img.contoursItem->show();
}

void Window::hideContours()
{
	img.contoursItem->hide();
}

void Window::computeSmooth()
{
	cl::Context context = program.getContext();
	cl::CommandQueue queue = program.getCommandQueue();
	cl::Kernel erodeKernel = program.getErodeKernel();
	cl::Kernel dilationKernel = program.getDilationKernel();
	
	// reset smooth image data
	img.smooth.fill(QColor(0, 0, 0, 255));

	// prepare data for first erosion
	const int nbElems{img.width * img.height * 3};
	cl::Buffer originalImage(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nbElems * sizeof(unsigned char), img.originalRAW);
	cl::Buffer resBuffer(context, CL_MEM_READ_WRITE, nbElems * sizeof(unsigned char));

	// set erode kernel parameters
	erodeKernel.setArg(0, grid.step);
	erodeKernel.setArg(1, img.width);
	erodeKernel.setArg(2, img.height);
	erodeKernel.setArg(3, originalImage);
	erodeKernel.setArg(4, resBuffer);
	erodeKernel.setArg(5, 0);

	// launch kernel on the compute device
	queue.enqueueNDRangeKernel(erodeKernel, cl::NullRange, img.width * img.height, cl::NullRange);
	// get result back to host
	queue.enqueueReadBuffer(resBuffer, CL_TRUE, 0, nbElems * sizeof(unsigned char), img.smoothRAW.get());
	
	// set dilation kernel parameters
	originalImage = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nbElems * sizeof(unsigned char), img.smoothRAW.get());
	dilationKernel.setArg(0, grid.step);
	dilationKernel.setArg(1, img.width);
	dilationKernel.setArg(2, img.height);
	dilationKernel.setArg(3, originalImage);
	dilationKernel.setArg(4, resBuffer);
	dilationKernel.setArg(5, 0);

	// launch kernel on the compute device
	queue.enqueueNDRangeKernel(dilationKernel, cl::NullRange, img.width * img.height, cl::NullRange);
	// get result back to host
	queue.enqueueReadBuffer(resBuffer, CL_TRUE, 0, nbElems * sizeof(unsigned char), img.smoothRAW.get());
	
	// set dilation kernel parameters
	originalImage = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nbElems * sizeof(unsigned char), img.smoothRAW.get());
	dilationKernel.setArg(0, grid.step);
	dilationKernel.setArg(1, img.width);
	dilationKernel.setArg(2, img.height);
	dilationKernel.setArg(3, originalImage);
	dilationKernel.setArg(4, resBuffer);
	dilationKernel.setArg(5, 0);

	// launch kernel on the compute device
	queue.enqueueNDRangeKernel(dilationKernel, cl::NullRange, img.width * img.height, cl::NullRange);
	// get result back to host
	queue.enqueueReadBuffer(resBuffer, CL_TRUE, 0, nbElems * sizeof(unsigned char), img.smoothRAW.get());

	// set erode kernel parameters
	originalImage = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nbElems * sizeof(unsigned char), img.smoothRAW.get());
	erodeKernel.setArg(0, grid.step);
	erodeKernel.setArg(1, img.width);
	erodeKernel.setArg(2, img.height);
	erodeKernel.setArg(3, originalImage);
	erodeKernel.setArg(4, resBuffer);
	erodeKernel.setArg(5, 0);

	// launch kernel on the compute device
	queue.enqueueNDRangeKernel(erodeKernel, cl::NullRange, img.width * img.height, cl::NullRange);
	// get result back to host
	queue.enqueueReadBuffer(resBuffer, CL_TRUE, 0, nbElems * sizeof(unsigned char), img.smoothRAW.get());

	// rewrite image
	img.painter.begin(&img.smooth);
	int index;
	int red;
	int green;
	int blue;
	for(int y{0}; y < img.height; ++y)
	{
		for(int x{0}; x < img.width; ++x)
		{
			index = 3 * (y * img.width + x);

			red = img.smoothRAW[index];
			green = img.smoothRAW[index+1];
			blue = img.smoothRAW[index+2];

			img.painter.setPen(QColor(red, green, blue, 255));
			img.painter.drawPoint(x, y);
		}
	}
	img.painter.end();
	
	// set smooth item
	if(img.smoothItem != nullptr)
	{
		scene->removeItem(img.smoothItem);
		img.smoothItem = nullptr;
	}
	img.smoothItem = scene->addPixmap(img.smooth);
	img.smoothItem->setZValue(1);

	// update layer actions state
	showSmoothAction->setEnabled(true);
	hideSmoothAction->setEnabled(true);
}

void Window::computeLabGradient()
{
	cl::Context context = program.getContext();
	cl::CommandQueue queue = program.getCommandQueue();
	cl::Kernel gradientKernel = program.getGradientKernel();
	
	// reset imageGradient data
	img.gradient.fill(QColor(0, 0, 0, 255));

	// prepare data
	const int nbElems{img.width * img.height * 3};
	cl::Buffer originalImage(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nbElems * sizeof(unsigned char), img.smoothRAW.get());
	cl::Buffer gradientImage(context, CL_MEM_READ_WRITE, nbElems * sizeof(unsigned char));

	// set kernel parameters
	gradientKernel.setArg(0, img.width);
	gradientKernel.setArg(1, img.height);
	gradientKernel.setArg(2, originalImage);
	gradientKernel.setArg(3, gradientImage);

	// launch kernel on the compute device
	queue.enqueueNDRangeKernel(gradientKernel, cl::NullRange, img.width * img.height, cl::NullRange);

	// get result back to host
	queue.enqueueReadBuffer(gradientImage, CL_TRUE, 0, nbElems * sizeof(unsigned char), img.gradientRAW);

	// rewrite image
	img.painter.begin(&img.gradient);
	int index;
	int red;
	int green;
	int blue;
	for(int y{0}; y < img.height; ++y)
	{
		for(int x{0}; x < img.width; ++x)
		{
			if(y >= 1)
				index = 3 * (y * img.width + x);
			else
				index = 3 * x;

			red = img.gradientRAW[index];
			green = img.gradientRAW[index+1];
			blue = img.gradientRAW[index+2];

			img.painter.setPen(QColor(red, green, blue, 255));
			img.painter.drawPoint(x, y);
		}
	}
	img.painter.end();
	
	// set gradient item
	if(img.gradientItem != nullptr)
	{
		scene->removeItem(img.gradientItem);
		img.gradientItem = nullptr;
	}
	img.gradientItem = scene->addPixmap(img.gradient);
	img.gradientItem->setZValue(1);

	// update layer actions state
	showGradientAction->setEnabled(true);
	hideGradientAction->setEnabled(true);
}

void Window::computeHexagonGrid()
{
	// ask for grid step
	grid.step = QInputDialog::getInt(this, "Grid step", "Enter a grid step value", 40, 0);

	// reset imageGrid data
	grid.hexagonGrid.fill(QColor(0, 0, 0, 0));
	grid.cells.clear();

	// set grid data
	int hexagonWidth = static_cast<int>(grid.step + 2 * (cos(M_PI / 3.0) * grid.step));
	int slicesX = img.width / (hexagonWidth / 2);
	int slicesY = img.height / (hexagonWidth / 2);
	grid.cellCenters = slicesX * slicesY;

	img.painter.begin(&grid.hexagonGrid);
	img.painter.setPen(QColor(7, 48, 138, 255));

	for(int y{0}; y < slicesY; ++y)
		for(int x{0}; x < slicesX; ++x)
			drawCell(x, y, slicesX, slicesY, hexagonWidth);
	img.painter.end();

	int pixel{0};
	QImage image = grid.hexagonGrid.toImage();
	for(int y{0}; y < img.height; ++y)
	{
		for(int x{0}; x < img.width; ++x)
		{
			QColor pixelColor = image.pixelColor(x, y);
			if(pixelColor == Qt::black)
				image.setPixelColor(x, y, QColor(0, 0, 0, 0));

			grid.hexagonGridRAW[pixel++] = pixelColor.red();
			grid.hexagonGridRAW[pixel++] = pixelColor.green();
			grid.hexagonGridRAW[pixel++] = pixelColor.blue();
		}
	}
	grid.hexagonGrid = grid.hexagonGrid.fromImage(image);

	// set hexagon grid item
	if(grid.hexagonGridItem != nullptr)
	{
		scene->removeItem(grid.hexagonGridItem);
		grid.hexagonGridItem = nullptr;
	}
	grid.hexagonGridItem = scene->addPixmap(grid.hexagonGrid);
	grid.hexagonGridItem->setZValue(2);

	// enable waterpixels computation
	computeWaterpixelsAction->setEnabled(true);
	
	// update layer actions state
	showGridAction->setEnabled(true);
	hideGridAction->setEnabled(true);
}

void Window::drawCell(const int x, const int y, const int slicesX, const int slicesY, const int hexWidth)
{
	int baseOffset = hexWidth / 2 + hexWidth / 4;
	QPoint offset;
	if(x % 2  == 0)
		offset = QPoint(x * baseOffset, y * hexWidth);
	else
		offset = QPoint(x * baseOffset, y * hexWidth + hexWidth/2);

	QPoint points[6] =
	{
		QPoint(hexWidth / 2, 0) + offset,
		QPoint(hexWidth / 4, -hexWidth / 2) + offset,
		QPoint(-hexWidth / 4, -hexWidth / 2) + offset,
		QPoint(-hexWidth / 2, 0) + offset,
		QPoint(-hexWidth / 4, hexWidth / 2) + offset,
		QPoint(hexWidth / 4, hexWidth / 2) + offset
	};

	QPoint center = points[0] + points[1] + points[2] + points[3] + points[4] + points[5];
	center /= 6.0f;
	
	QPoint innerPoints[6] =
	{
		center + grid.rho * (points[0] - center),
		center + grid.rho * (points[1] - center),
		center + grid.rho * (points[2] - center),
		center + grid.rho * (points[3] - center),
		center + grid.rho * (points[4] - center),
		center + grid.rho * (points[5] - center)
	};

	QBrush brush;
	brush.setColor(QColor(7, 48, 138, 255));
	brush.setStyle(Qt::SolidPattern);
	img.painter.setBrush(brush);

	img.painter.drawPolygon(points, 6);

	brush.setColor(Qt::black);
	img.painter.setBrush(brush);
	
	img.painter.drawPolygon(innerPoints, 6);

	// update grid cells data
	QPolygon polygon(6);
	for(int i{0}; i < 6; ++i)
		polygon.setPoint(i, innerPoints[i]);
	grid.cells.push_back(polygon);
}

void Window::computeCellMarkers()
{
	cl::Context context = program.getContext();
	cl::CommandQueue queue = program.getCommandQueue();
	cl::Kernel markersKernel = program.getMarkersKernel();
	
	// reset grid markers data
	grid.markers.fill(QColor(0, 0, 0, 0));

	// prepare data
	const int nbElems{img.width * img.height * 3};
	std::vector<int> indices; // indices list of pixels which are in a cell (black area)
	std::vector<int> indicesCount; // indices count per cell

	QPoint p;
	int index;
	int indicesInCell{0};
	for(int i{0}; i < grid.cellCenters; ++i)
	{
		for(int y{0}; y < img.height; ++y)
		{
			for(int x{0}; x < img.width; ++x)
			{
				if(y >= 1)
					index = 3 * (y * img.width + x);
				else
					index = 3 * x;
				QPolygon cell = grid.cells.at(i);
				p.setX(x);
				p.setY(y);
				if(cell.containsPoint(p, Qt::OddEvenFill))
				{
					indicesInCell++;
					indices.push_back(index);
				}
			}
		}
		indicesCount.push_back(indicesInCell);
		indicesInCell = 0;
	}
/*
	cl::Buffer indicesBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, indices.size() * sizeof(int), indices.data());
	cl::Buffer indicesCountBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, indicesCount.size() * sizeof(int), indicesCount.data());
	cl::Buffer indexVisitedBuffer(context, CL_MEM_READ_WRITE, indices.size() * sizeof(bool));
	cl::Buffer gradientBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nbElems * sizeof(unsigned char), img.gradientRAW);
	cl::Buffer markersBuffer(context, CL_MEM_READ_WRITE, nbElems * sizeof(unsigned char));

	// GPU PROCESS ##########

	// set kernel parameters
	markersKernel.setArg(0, img.width);
	markersKernel.setArg(1, indicesBuffer);
	markersKernel.setArg(2, indicesCountBuffer);
	markersKernel.setArg(3, indexVisitedBuffer);
	markersKernel.setArg(4, gradientBuffer);
	markersKernel.setArg(5, markersBuffer);

	// launch kernel on the compute device
	queue.enqueueNDRangeKernel(markersKernel, cl::NullRange, grid.cellCenters, cl::NullRange);

	// get result back to host
	queue.enqueueReadBuffer(markersBuffer, CL_TRUE, 0, nbElems * sizeof(unsigned char), grid.markersRAW);
*/
	// CPU PROCESS ##########

    // get max number of threads available on the machine
	int thrCount = std::thread::hardware_concurrency();
    std::vector<std::unique_ptr<std::thread>> threads;
	threads.reserve(thrCount);

	// dispatch
	for(int t{0}; t < thrCount; ++t)
	{
		threads.push_back(
						std::make_unique<std::thread>(
								computeCellMarkersThread,
								t,
								thrCount,
								img.width,
								grid.cellCenters,
								std::ref(indices),
								std::ref(indicesCount),
								img.gradientRAW,
								grid.markersRAW
								)
						);
		threads.at(t)->join();
	}

	// rewrite image
	img.painter.begin(&grid.markers);
	int red;
	int green;
	int blue;
	for(int y{0}; y < img.height; ++y)
	{
		for(int x{0}; x < img.width; ++x)
		{
			if(y >= 1)
				index = 3 * (y * img.width + x);
			else
				index = 3 * x;

			red = grid.markersRAW[index];
			green = grid.markersRAW[index+1];
			blue = grid.markersRAW[index+2];

			if(red == 0 && green == 0 && blue == 0)
				img.painter.setPen(QColor(0, 0, 0, 0));
			else
				img.painter.setPen(QColor(red, green, blue, 255));
			img.painter.drawPoint(x, y);
		}
	}
	img.painter.end();
	
	// set markers item
	if(grid.markersItem != nullptr)
	{
		scene->removeItem(grid.markersItem);
		grid.markersItem = nullptr;
	}
	grid.markersItem = scene->addPixmap(grid.markers);
	grid.markersItem->setZValue(4);

	// update layer actions state
	showMarkersAction->setEnabled(true);
	hideMarkersAction->setEnabled(true);
}

void computeCellMarkersThread(
				const int thr,
				const int numThreads,
				int width,
				int cellCenters,
				std::vector<int>& indices,
				std::vector<int>& indicesCount,
				unsigned char* gradient,
				unsigned char* markers)
{
    int stride{cellCenters / numThreads};
    int id{thr * stride};
	int lastId{id + stride};
	if(thr == (numThreads-1) && (cellCenters % numThreads) == 1)
		lastId++;
	for(; id < lastId; ++id)
	{
		int offset = 0;
		for(int i = 0; i < id; ++i)
		{
			offset = offset + indicesCount[i];
		}

		// compute min gradient in cell
		int minGradient = 255;
		int pixel;
		int pixelCount = indicesCount.at(id);
		for(int i = offset; i < (offset + pixelCount); ++i)
		{
			pixel = indices.at(i);
			if(gradient[pixel] < minGradient)
				minGradient = gradient[pixel];
		}

		// get seed giving the max area
		int maxSeed = -1;
		int maxArea = -1;
		int area;
		float coverage;
		int seedIndex;
		bool indexVisited[pixelCount];
		for(int i{0}; i < pixelCount; ++i)
			indexVisited[i] = false;

		for(int i = offset; i < (offset + pixelCount); ++i)
		{
			seedIndex = indices.at(i);
			if(gradient[seedIndex] == minGradient && !indexVisited[i - offset])
			{
				area = growRegion(width, seedIndex, offset, offset + pixelCount, indices, gradient, minGradient, indexVisited);
				coverage = static_cast<float>(area) / static_cast<float>(pixelCount);
				if(area > maxArea)
				{
				    maxArea = area;
					maxSeed = seedIndex;
					if(coverage >= 0.5f)
						break;
				}
			}
		}

		for(int i{0}; i < pixelCount; ++i)
			indexVisited[i] = false;

		// color markers with highest surface extinction
		colorMaxRegion(width, maxSeed, offset, offset + pixelCount, indices, gradient, minGradient, markers, indexVisited);
	}
}

int growRegion(
		const int width,
		const int seedIndex,
		const int startIndex,
		const int endIndex,
		std::vector<int>& indices,
		unsigned char* gradient,
		const int minGradient,
		bool indexVisited[])
{
	bool correctSeed = false;
	int node;
	for(int i{startIndex}; i < endIndex; ++i)
	{
		if(seedIndex == indices.at(i))
		{
			correctSeed = true;
			node = i;
			break;
		}
	}

	if(correctSeed && !indexVisited[node - startIndex])
	{
		// visit node
		indexVisited[node - startIndex] = true;

		// check if node fills condition
		if(gradient[seedIndex] != minGradient)
			return 0;

		int north = growRegion(width, seedIndex-(width*3), startIndex, endIndex, indices, gradient, minGradient, indexVisited);
		int south = growRegion(width, seedIndex+(width*3), startIndex, endIndex, indices, gradient, minGradient, indexVisited);
		int east = growRegion(width, seedIndex+3, startIndex, endIndex, indices, gradient, minGradient, indexVisited);
		int west = growRegion(width, seedIndex-3, startIndex, endIndex, indices, gradient, minGradient, indexVisited);
		int northEast = growRegion(width, seedIndex-(width*3)+3, startIndex, endIndex, indices, gradient, minGradient, indexVisited);
		int northWest = growRegion(width, seedIndex-(width*3)-3, startIndex, endIndex, indices, gradient, minGradient, indexVisited);
		int southEast = growRegion(width, seedIndex+(width*3)+3, startIndex, endIndex, indices, gradient, minGradient, indexVisited);
		int southWest = growRegion(width, seedIndex+(width*3)-3, startIndex, endIndex, indices, gradient, minGradient, indexVisited);

		return 1 + north + south + east + west + southEast + southWest + northEast + northWest;
	}
	else
	{
		return 0;
	}
}

int validSeed(const int startIndex, const int endIndex, const int seedIndex, std::vector<int>& indices)
{
	int node;
	for(int i{startIndex}; i < endIndex; ++i)
	{
		if(seedIndex == indices.at(i))
			return i;
	}
	return -1;
}

int growRegionIt(
		const int width,
		const int rootIndex,
		const int startIndex,
		const int endIndex,
		std::vector<int>& indices,
		unsigned char* gradient,
		const int minGradient,
		bool indexVisited[])
{
	// array of seeds (FIFO)
	std::queue<int> seedIndex;
	seedIndex.push(rootIndex);

	int area{0};
	int node;
	int N; int S; int E; int W;
	int NE; int NW; int SE; int SW;

	while(seedIndex.size() > 0)
	{
		int index = seedIndex.front();
		node = validSeed(startIndex, endIndex, index, indices);
		if(node != -1 && gradient[index] == minGradient)
		{
			// surface increase
			area++;
				
			// mark seed as visited
			indexVisited[node - startIndex] = true;

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
			if(node != -1 && !indexVisited[node - startIndex])
				seedIndex.push(N);
			node = validSeed(startIndex, endIndex, S, indices);
			if(node != -1 && !indexVisited[node - startIndex])
				seedIndex.push(S);
			node = validSeed(startIndex, endIndex, E, indices);
			if(node != -1 && !indexVisited[node - startIndex])
				seedIndex.push(E);
			node = validSeed(startIndex, endIndex, W, indices);
			if(node != -1 && !indexVisited[node - startIndex])
				seedIndex.push(W);
			node = validSeed(startIndex, endIndex, NE, indices);
			if(node != -1 && !indexVisited[node - startIndex])
				seedIndex.push(NE);
			node = validSeed(startIndex, endIndex, NW, indices);
			if(node != -1 && !indexVisited[node - startIndex])
				seedIndex.push(NW);
			node = validSeed(startIndex, endIndex, SE, indices);
			if(node != -1 && !indexVisited[node - startIndex])
				seedIndex.push(SE);
			node = validSeed(startIndex, endIndex, SW, indices);
			if(node != -1 && !indexVisited[node - startIndex])
				seedIndex.push(SW);
		}
		seedIndex.pop();
	}
	return area;
}

void colorMaxRegion(
		const int width,
		const int seedIndex,
		const int startIndex,
		const int endIndex,
		std::vector<int>& indices,
		unsigned char* gradient,
		const int minGradient,
		unsigned char* markers,
		bool indexVisited[])
{
	bool correctSeed = false;
	int node;
	for(int i{startIndex}; i < endIndex; ++i)
	{
		if(seedIndex == indices.at(i))
		{
			correctSeed = true;
			node = i;
			break;
		}
	}

	if(correctSeed && !indexVisited[node - startIndex])
	{
		// visit node
		indexVisited[node - startIndex] = true;
		
		// check if node fills condition
		if(gradient[seedIndex] != minGradient)
			return;

		// color green
		markers[seedIndex + 1] = 255;

		colorMaxRegion(width, seedIndex-(width*3), startIndex, endIndex, indices, gradient, minGradient, markers, indexVisited);
		colorMaxRegion(width, seedIndex+(width*3), startIndex, endIndex, indices, gradient, minGradient, markers, indexVisited);
		colorMaxRegion(width, seedIndex+3, startIndex, endIndex, indices, gradient, minGradient, markers, indexVisited);
		colorMaxRegion(width, seedIndex-3, startIndex, endIndex, indices, gradient, minGradient, markers, indexVisited);
		colorMaxRegion(width, seedIndex-(width*3)+3, startIndex, endIndex, indices, gradient, minGradient, markers, indexVisited);
		colorMaxRegion(width, seedIndex-(width*3)-3, startIndex, endIndex, indices, gradient, minGradient, markers, indexVisited);
		colorMaxRegion(width, seedIndex+(width*3)+3, startIndex, endIndex, indices, gradient, minGradient, markers, indexVisited);
		colorMaxRegion(width, seedIndex+(width*3)-3, startIndex, endIndex, indices, gradient, minGradient, markers, indexVisited);
	}
}

void Window::computeDistanceFromMarkers()
{
	cl::Context context = program.getContext();
	cl::CommandQueue queue = program.getCommandQueue();
	cl::Kernel distanceKernel = program.getDistanceKernel();
	
	// reset imageGradient data
	grid.distanceFromMarkers.fill(QColor(0, 0, 0, 255));

	// prepare data
	const int nbElems{img.width * img.height * 3};

	std::vector<int> markersIndices;
	for(int i{0}; i < img.width * img.height; ++i)
	{
		if(grid.markersRAW[i*3+1] == 255)
			markersIndices.push_back(i);
	}

	int markersCount = markersIndices.size();

	cl::Buffer markersBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, markersCount * sizeof(int), markersIndices.data());
	cl::Buffer distanceBuffer(context, CL_MEM_READ_WRITE, nbElems * sizeof(unsigned char));

	// set kernel parameters
	distanceKernel.setArg(0, img.width);
	distanceKernel.setArg(1, img.height);
	distanceKernel.setArg(2, static_cast<float>(grid.step));
	distanceKernel.setArg(3, markersBuffer);
	distanceKernel.setArg(4, markersCount);
	distanceKernel.setArg(5, distanceBuffer);

	// launch kernel on the compute device
	queue.enqueueNDRangeKernel(distanceKernel, cl::NullRange, img.width * img.height, cl::NullRange);

	// get result back to host
	queue.enqueueReadBuffer(distanceBuffer, CL_TRUE, 0, nbElems * sizeof(unsigned char), grid.distanceFromMarkersRAW);

	// rewrite image
	img.painter.begin(&grid.distanceFromMarkers);
	int index;
	int red;
	int green;
	int blue;
	for(int y{0}; y < img.height; ++y)
	{
		for(int x{0}; x < img.width; ++x)
		{
			if(y >= 1)
				index = 3 * (y * img.width + x);
			else
				index = 3 * x;

			red = grid.distanceFromMarkersRAW[index];
			green = grid.distanceFromMarkersRAW[index+1];
			blue = grid.distanceFromMarkersRAW[index+2];

			img.painter.setPen(QColor(red, green, blue, 255));
			img.painter.drawPoint(x, y);
		}
	}
	img.painter.end();
	
	// set distance from markers item
	if(grid.distanceFromMarkersItem != nullptr)
	{
		scene->removeItem(grid.distanceFromMarkersItem);
		grid.distanceFromMarkersItem = nullptr;
	}
	grid.distanceFromMarkersItem = scene->addPixmap(grid.distanceFromMarkers);
	grid.distanceFromMarkersItem->setZValue(3);

	// update layer actions state
	showDistanceMarkersAction->setEnabled(true);
	hideDistanceMarkersAction->setEnabled(true);
}

void Window::computeRegularizedGradient()
{
	// reset regularized gradient data
	img.regularizedGradient.fill(QColor(0, 0, 0, 255));

	// combine
	int index;
	int red;
	int green;
	int blue;
	for(int y{0}; y < img.height; ++y)
	{
		for(int x{0}; x < img.width; ++x)
		{
			if(y >= 1)
				index = 3 * (y * img.width + x);
			else
				index = 3 * x;
			
			red = std::min(255, img.gradientRAW[index] + grid.distanceFromMarkersRAW[index]);
			green = std::min(255, img.gradientRAW[index+1] + grid.distanceFromMarkersRAW[index+1]);
			blue = std::min(255, img.gradientRAW[index+2] + grid.distanceFromMarkersRAW[index+2]);

			img.regularizedGradientRAW[index] = red;
			img.regularizedGradientRAW[index+1] = green;
			img.regularizedGradientRAW[index+2] = blue;
		}
	}

	// rewrite image
	img.painter.begin(&img.regularizedGradient);
	for(int y{0}; y < img.height; ++y)
	{
		for(int x{0}; x < img.width; ++x)
		{
			if(y >= 1)
				index = 3 * (y * img.width + x);
			else
				index = 3 * x;

			red = img.regularizedGradientRAW[index];
			green = img.regularizedGradientRAW[index+1];
			blue = img.regularizedGradientRAW[index+2];

			img.painter.setPen(QColor(red, green, blue, 255));
			img.painter.drawPoint(x, y);
		}
	}
	img.painter.end();
	
	// set regularized gradient item
	if(img.regularizedGradientItem != nullptr)
	{
		scene->removeItem(img.regularizedGradientItem);
		img.regularizedGradientItem = nullptr;
	}
	img.regularizedGradientItem = scene->addPixmap(img.regularizedGradient);
	img.regularizedGradientItem->setZValue(5);

	// update layer actions state
	showRegGradientAction->setEnabled(true);
	hideRegGradientAction->setEnabled(true);
}

// #####################
// ##### WATERSHED #####
// #####################

void Window::printBasins(std::vector<std::vector<int>> & basins, bool details)
{
	for(int i{0}; i < basins.size(); ++i)
	{
		std::cout << "basin " << i + 1 << " got " << basins.at(i).size() << " points\n";
		if(details)
		{
			std::vector<int> & b = basins.at(i);
			for(int j{0}; j < b.size(); ++j)
				std::cout << b.at(j) << ", ";
			std::cout << std::endl;
		}
	}
}

void Window::initBasins(std::vector<std::vector<int>> & basins)
{
	QPolygon cell;
	QPoint p;
	int index;
	for(int i{0}; i < grid.cells.size(); ++i)
	{
		cell = grid.cells.at(i);
		basins.push_back(std::vector<int>());
		for(int y{0}; y < img.height; ++y)
		{
			for(int x{0}; x < img.width; ++x)
			{
				p.setX(x); p.setY(y);
				if(cell.containsPoint(p, Qt::OddEvenFill))
				{
					index = (y >= 1) ? 3*(y*img.width+x) : 3*x ;
					if(grid.markersRAW[index+1] == 255)
						basins.at(i).push_back(index);
				}
			}
		}
	}
	
	// clean empty basins
	for(int i{0}; i < basins.size(); ++i)
	{
		std::vector<int> & b = basins.at(i);
		if(b.size() == 0)
		{
			basins.erase(basins.begin() + i);
			i--;
		}
	}
}

bool Window::validIndex(int index)
{
	return index > 0 && index < (img.width * img.height * 3);
}

void Window::computeWatershed()
{
	// queue
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> q;

	// pixels in queue
	std::unique_ptr<bool[]> inQueue{std::make_unique<bool[]>(img.width * img.height)};

	// init queue, basins and labels map
	std::vector<int> neighbours;
	std::vector<std::vector<int>> basins;
	initBasins(basins);
	
	img.contoursRAW = std::make_unique<unsigned char[]>(img.width * img.height * 3);
	img.labelsMap = std::make_unique<int[]>(img.width * img.height);

	int index;
	int label = 1;
	for(int i{0}; i < basins.size(); ++i)
	{
		std::vector<int> & b = basins.at(i);
		index = b.at(0);
		img.labelsMap[index/3] = label;

		neighbours.clear();
		neighbours.push_back(index - (img.width*3));
		neighbours.push_back(index + (img.width*3));
		neighbours.push_back(index + 3);
		neighbours.push_back(index - 3);
		neighbours.push_back(index - (img.width*3) + 3);
		neighbours.push_back(index - (img.width*3) - 3);
		neighbours.push_back(index + (img.width*3) + 3);
		neighbours.push_back(index + (img.width*3) - 3);

		for(int j{0}; j < neighbours.size(); ++j)
		{
			int n = neighbours.at(j);
			if(validIndex(n))
				q.push(std::pair<int, int>(img.regularizedGradientRAW[n], n));
			
		}
		label++;
	}

	// flood
	while(!q.empty())
	{
		// extract top pixel
		std::pair<int, int> pixel = q.top();
		q.pop();
		index = pixel.second;

		// get 8 neighbours
		neighbours.clear();
		neighbours.push_back(index - (img.width*3));
		neighbours.push_back(index + (img.width*3));
		neighbours.push_back(index + 3);
		neighbours.push_back(index - 3);
		neighbours.push_back(index - (img.width*3) + 3);
		neighbours.push_back(index - (img.width*3) - 3);
		neighbours.push_back(index + (img.width*3) + 3);
		neighbours.push_back(index + (img.width*3) - 3);

		int writeLabel{true};
		int nLabel{0};
		for(int i{0}; i < neighbours.size(); ++i)
		{
			int n = neighbours.at(i);
			if(validIndex(n))
			{
				if(img.labelsMap[n/3] != 0)
				{
					if(nLabel == 0)
						nLabel = img.labelsMap[n/3];
					else if(img.labelsMap[n/3] != nLabel)
						writeLabel = false;
				}
				else
				{
					if(inQueue[n/3] == false)
					{
						inQueue[n/3] = true;
						q.push(std::pair<int, int>(img.regularizedGradientRAW[n], n));
					}
				}
			}
		}
		if(writeLabel)
			img.labelsMap[index/3] = nLabel;
	}

	// write contours map
	for(int i{0}; i < (img.width * img.height); ++i)
	{
		if(img.labelsMap[i] == 0)
		{
			img.contoursRAW[i*3] = 255;
			img.contoursRAW[i*3+1] = 255;
			img.contoursRAW[i*3+2] = 255;
		}
	}

	// dilate borders
	cl::Context context = program.getContext();
	cl::CommandQueue queue = program.getCommandQueue();
	cl::Kernel dilationKernel = program.getDilationKernel();
	cl::Kernel outlineKernel = program.getOutlineKernel();
	
	// prepare data for dilation
	const int nbElems{img.width * img.height * 3};
	cl::Buffer contoursImage(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nbElems * sizeof(unsigned char), img.contoursRAW.get());
	cl::Buffer resBuffer(context, CL_MEM_READ_WRITE, nbElems * sizeof(unsigned char));

	// set dilation kernel parameters
	dilationKernel.setArg(0, grid.step);
	dilationKernel.setArg(1, img.width);
	dilationKernel.setArg(2, img.height);
	dilationKernel.setArg(3, contoursImage);
	dilationKernel.setArg(4, resBuffer);
	dilationKernel.setArg(5, 1);

	// launch kernel on the compute device
	queue.enqueueNDRangeKernel(dilationKernel, cl::NullRange, img.width * img.height, cl::NullRange);
	// get result back to host
	std::unique_ptr<unsigned char[]> borders = std::make_unique<unsigned char[]>(img.width * img.height * 3);
	queue.enqueueReadBuffer(resBuffer, CL_TRUE, 0, nbElems * sizeof(unsigned char), borders.get());

	// rewrite contours map
	for(int i{0}; i < (img.width * img.height); ++i)
	{
		img.contoursRAW[i*3] = borders[i*3];
		img.contoursRAW[i*3+1] = borders[i*3];
		img.contoursRAW[i*3+2] = borders[i*3];
	}

	contoursImage = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, nbElems * sizeof(unsigned char), img.contoursRAW.get());
	
	// set outline kernel parameters
	outlineKernel.setArg(0, grid.step);
	outlineKernel.setArg(1, img.width);
	outlineKernel.setArg(2, img.height);
	outlineKernel.setArg(3, contoursImage);
	outlineKernel.setArg(4, resBuffer);

	// launch kernel on the compute device
	queue.enqueueNDRangeKernel(outlineKernel, cl::NullRange, img.width * img.height, cl::NullRange);
	// get result back to host
	std::unique_ptr<unsigned char[]> outline = std::make_unique<unsigned char[]>(img.width * img.height * 3);
	queue.enqueueReadBuffer(resBuffer, CL_TRUE, 0, nbElems * sizeof(unsigned char), outline.get());
	
	// rewrite contours map
	for(int i{0}; i < (img.width * img.height); ++i)
	{
		if(borders[i*3] == 255)
		{
			img.contoursRAW[i*3] = borders[i*3];
			img.contoursRAW[i*3+1] = borders[i*3];
			img.contoursRAW[i*3+2] = borders[i*3];
		}
		else if(outline[i*3] == 10)
		{
			img.contoursRAW[i*3] = outline[i*3];
			img.contoursRAW[i*3+1] = outline[i*3];
			img.contoursRAW[i*3+2] = outline[i*3];
		}
	}

	// rewrite image
	img.painter.begin(&img.contours);
	int red;
	int green;
	int blue;
	for(int y{0}; y < img.height; ++y)
	{
		for(int x{0}; x < img.width; ++x)
		{
			index = 3 * (y * img.width + x);
			red = img.originalRAW[index];
			green = img.originalRAW[index+1];
			blue = img.originalRAW[index+2];
			if(img.contoursRAW[index] == 255)
				img.painter.setPen(QColor(255, 255, 255, 255));
			else if(img.contoursRAW[index] == 10)
				img.painter.setPen(QColor(10, 10, 10, 255));
			else
				img.painter.setPen(QColor(red, green, blue, 255));
			img.painter.drawPoint(x, y);
		}
	}
	img.painter.end();
	
	// set contours item
	if(img.contoursItem != nullptr)
	{
		scene->removeItem(img.contoursItem);
		img.contoursItem = nullptr;
	}
	img.contoursItem = scene->addPixmap(img.contours);
	img.contoursItem->setZValue(6);

	// update layer actions state
	showContoursAction->setEnabled(true);
	hideContoursAction->setEnabled(true);
}

bool dichotomicSearch(int value, std::vector<int> & elements)
{
	bool found{false};
	int start{0};
	int end{elements.size() - 1};
	int pivot{(start + end) / 2};
	int e;
	
	while(start != end && (end - start) > 1)
	{
		pivot = (start + end) / 2;
		e = elements.at(pivot);
		if(e == value)
		{
			found = true;
			break;
		}
		else if(e < value)
			start = pivot;
		else
			end = pivot;
	}

	if(start == end && elements.at(start) == value)
		found = true;
	else if((end - start) == 1)
	{
		if(elements.at(start) == value || elements.at(end) == value)
			found = true;
	}
	return found;
}
