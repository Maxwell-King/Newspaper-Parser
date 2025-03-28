#include "../include/annotator.hpp"

wxString file;
wxString date;

bool wxAnnotatorApp::OnInit() {
	if (!wxApp::OnInit()) {
		return false;
	}
	std::cerr << "Starting annotator..." << std::endl;
	
	wxInitAllImageHandlers();
	frame = new wxFrame(NULL, wxID_ANY, wxT("Annotator"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
	js_choose();
	drawPanel = new wxImagePanel(frame);
	frame->Show();
	
	return true;
}

int wxAnnotatorApp::OnExit() {
	js_close();
	return 0;
}

int wxAnnotatorApp::OnRun() {
	int exitcode = wxApp::OnRun();
	return exitcode;
}

void wxAnnotatorApp::OnInitCmdLine(wxCmdLineParser& parser) {
	parser.SetDesc(g_cmdLineDesc);
}

bool wxAnnotatorApp::OnCmdLineParsed(wxCmdLineParser& parser) {
	file = parser.GetParam(0);
	date = parser.GetParam(1);
	return true;
}

IMPLEMENT_APP(wxAnnotatorApp)
