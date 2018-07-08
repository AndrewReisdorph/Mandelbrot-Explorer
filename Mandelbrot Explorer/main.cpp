#include "main.h"
#include <mpir.h>
#include "MandelbrotExplorerWindow.h"
#include <iomanip>

wxIMPLEMENT_APP_CONSOLE(MandelbrotExplorerApp);
using namespace std;

bool MandelbrotExplorerApp::OnInit()
{
	mpf_set_default_prec(512);
	wxFrame* window = new MandelbrotExplorerWindow("Mandelbrot Explorer");
	window->Show();

	return true;
}

string DurationToString(std::chrono::duration<double> Duration)
{
	auto ElapsedHours = std::chrono::duration_cast<std::chrono::hours>(Duration);
	Duration -= ElapsedHours;
	auto ElapsedMinutes = std::chrono::duration_cast<std::chrono::minutes>(Duration);
	Duration -= ElapsedMinutes;
	auto ElapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(Duration);

	std::ostringstream stream;
	stream << setfill('0') << std::setw(2) << ElapsedHours.count() << ':' <<
		std::setfill('0') << std::setw(2) << ElapsedMinutes.count() << ':' <<
		std::setfill('0') << std::setw(2) << ElapsedSeconds.count();

	return stream.str();
}