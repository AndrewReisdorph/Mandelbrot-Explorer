#include "main.h"
#include <mpir.h>
#include "MandelShitWindow.h"
#include <iomanip>

wxIMPLEMENT_APP_CONSOLE(MandelShitApp);
using namespace std;

bool MandelShitApp::OnInit()
{

	//0.00000000000000000000383851705185637665293


	/*
	r  0.351913708220167144808117640078067779541015625
	i -0.43629490309290850883190372908115386962890625
	h  0.000000000000001347486947899281978607177734375
	*/
	mpf_set_default_prec(512);
	std::cout << "precision: " << mpf_get_default_prec() << std::endl;
	wxFrame* window = new MandelShitWindow("MandelShit");
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