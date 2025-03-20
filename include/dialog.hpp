#include "jsdata.hpp"

#include <cstring>
#include <iostream>
#include <fstream>

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/clipbrd.h>

class wxDlgChoice : public wxDialog {
public:
	wxDlgChoice(wxPanel *parent, wxString text, int xbb_img, int ybb_img, int wbb_img, int hbb_img);
	wxStaticText *DlgBlockTxt;
	wxBoxSizer *DlgButtonSizer;
	wxBoxSizer *DlgMainSizer;
	wxChoice *DlgChoice;
	wxButton *DlgCopy;
};

