#include "imgpanel.hpp"

#include <cstring>
#include <iostream>
#include <fstream>

#include <wx/wxprec.h>
#include <wx/cmdline.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class wxAnnotatorApp: public wxApp {
	wxFrame *frame;
	wxImagePanel *drawPanel;
	wxString file;
	wxString date;
public:
	virtual bool OnInit();
	virtual int OnExit();
	virtual int OnRun();
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
};

static const wxCmdLineEntryDesc g_cmdLineDesc [] = {
	{ wxCMD_LINE_PARAM, NULL, NULL, "image_filename",
		wxCMD_LINE_VAL_STRING , wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_PARAM, NULL, NULL, "image_date",
		wxCMD_LINE_VAL_STRING , wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_NONE }
};
