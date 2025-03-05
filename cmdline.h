#include <wx/wxprec.h>
#include <wx/cmdline.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class MyApp: public wxApp
{
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
    { wxCMD_LINE_PARAM, NULL, NULL, "image filename",
      wxCMD_LINE_VAL_STRING , wxCMD_LINE_OPTION_MANDATORY },

    { wxCMD_LINE_NONE }
  };

DECLARE_APP(MyApp)
