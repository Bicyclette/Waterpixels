#include "window.hpp"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	Window client;
	client.show();

	return app.exec();
}
