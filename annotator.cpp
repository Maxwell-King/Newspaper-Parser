#include "imgpanel.h"

#include <wx/wxprec.h>
#include <wx/cmdline.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class wxAnnotatorApp: public wxApp
{
  wxFrame *frame;
  wxImagePanel *drawPanel;
public:
  // from wxApp
  virtual bool OnInit();
  virtual int OnExit();
  virtual int OnRun();
  virtual void OnInitCmdLine(wxCmdLineParser& parser);
  virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
};

static const wxCmdLineEntryDesc g_cmdLineDesc [] =
  {
    { wxCMD_LINE_PARAM, NULL, NULL, "image_filename",
      wxCMD_LINE_VAL_STRING , wxCMD_LINE_OPTION_MANDATORY },
	{ wxCMD_LINE_PARAM, NULL, NULL, "image_date",
      wxCMD_LINE_VAL_STRING , wxCMD_LINE_OPTION_MANDATORY },
    { wxCMD_LINE_NONE }
  };

// annotator /home/MKing23/Pictures/test/img22.jpg
// annotator C:\Users\danie\Pictures\test\img22.jpg
bool wxAnnotatorApp::OnInit()
{
  if (!wxApp::OnInit())
    return false;
  
  std::cerr << "Starting annotator..." << std::endl;
  return true;
}

int wxAnnotatorApp::OnExit()
{
	//
  return 0;
}

int wxAnnotatorApp::OnRun()
{
  int exitcode = wxApp::OnRun();
  return exitcode;
}

void wxAnnotatorApp::OnInitCmdLine(wxCmdLineParser& parser)
{
  parser.SetDesc(g_cmdLineDesc);
}

bool wxAnnotatorApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
  wxInitAllImageHandlers();
            
  frame = new wxFrame(NULL, wxID_ANY, wxT("Annotator"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
  drawPanel = new wxImagePanel(frame, parser.GetParam(0), wxBITMAP_TYPE_JPEG);
  // wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  // sizer->Add(drawPanel, 1, wxFIXED_MINSIZE);
  // frame->SetSizer(sizer);
  frame->Show();
  return true;
}

IMPLEMENT_APP(wxAnnotatorApp)
