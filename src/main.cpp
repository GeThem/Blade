#include "App.h"

int main(int argc, char** argv)
{
	App app;
	AppInit(app);
	while (AppRun(app));
	return 0;
}