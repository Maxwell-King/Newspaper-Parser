#include <cstring>

#include <wx/wx.h>
#include <wx/sizer.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "nlohmann/json.hpp"

class wxImagePanel : public wxPanel {
  //buttons here?
public:
  wxImagePanel(wxFrame *parent, wxString file, wxBitmapType format);

  int dcX, dcY, iButtons;
  wxImage image;
	Pix *pixImg;
	tesseract::TessBaseAPI *tessApi = new tesseract::TessBaseAPI();
  wxBitmap resized;

  void paintEvent(wxPaintEvent &evt);
  void render(wxDC& dc);
  void addButtons();
  wxSize ScaleToScreen(wxSize sz);  // longest side scaled to fit screen (1080P) maintaing aspect ratio

  DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxImagePanel, wxPanel)

EVT_PAINT(wxImagePanel::paintEvent)

END_EVENT_TABLE()

wxImagePanel::wxImagePanel(wxFrame *parent, const wxString file, const wxBitmapType format) : wxPanel(parent) {
  // wxstrFile = file;
	// load json C:/Users/danie/Pictures/train_80_percent.json
	// load json C:/Users/danie/Pictures/val_20_percent.json
	if (tessApi->Init(NULL, "eng")) {
      fprintf(stderr, "Could not initialize tesseract.\n");
      exit(1);
  }
	pixImg = pixRead(file.c_str());
	tessApi->SetImage(pixImg);
  tessApi->SetPageSegMode(tesseract::PSM_AUTO);
  tessApi->Recognize(0);
	// add image
	image.LoadFile(file, format); 
  parent->SetSize(ScaleToScreen(image.GetSize()));
  dcX = -1, dcY = -1;
}

void wxImagePanel::paintEvent(wxPaintEvent &evt) {
  wxPaintDC dc(this);
  render(dc);
}

void wxImagePanel::render(wxDC &dc) {
  if (dcX == -1 && dcY == -1) {
    dc.GetSize(&dcX, &dcY);
    resized = wxBitmap(image.Scale(dcX, dcY));
    addButtons();
  }
  dc.DrawBitmap(resized, 0, 0, false);
}

void wxImagePanel::addButtons() {
  tesseract::ResultIterator* riIter = tessApi->GetIterator();
  tesseract::PageIteratorLevel pilLevel = tesseract::RIL_PARA;
  if (riIter != 0) {
    do {
			std::string strBlock = riIter->GetUTF8Text(pilLevel);
      int x1, y1, x2, y2;
      riIter->BoundingBox(pilLevel, &x1, &y1, &x2, &y2);
			int w = x2-x1;
      int h = y2-y1;
      wxPoint *cpPos = new wxPoint( (double) x1/image.GetSize().x * dcX, (double) y1/image.GetSize().y * dcY);
      wxSize *cpSz = new wxSize( (double) w/image.GetSize().x * dcX, (double) h/image.GetSize().y * dcY);

			wxButton *cpButton = new wxButton(this, wxID_ANY, wxEmptyString, *cpPos, *cpSz);
			cpButton->SetBitmap(wxBitmapBundle(resized.GetSubBitmap(wxRect(*cpPos, *cpSz))));
			cpButton->Bind(wxEVT_BUTTON,
			  [=](wxCommandEvent&) {
					wxDialog *cpDialog = new wxDialog(this, wxID_ANY, wxEmptyString);
					wxBoxSizer *DlgButtonSizer = new wxBoxSizer(wxHORIZONTAL);
					wxBoxSizer *DlgMainSizer = new wxBoxSizer(wxVERTICAL);

					wxString DlgOpts[] = {"HEADLINE", "BYLINE"};
					wxChoice *DlgChoice = new wxChoice(cpDialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, 2, DlgOpts); // add annotation 
					wxButton *DlgCopy = new wxButton(cpDialog, wxID_ANY, "Copy");
					wxStaticText *DlgBlockTxt = new wxStaticText(cpDialog, wxID_ANY, wxString::FromUTF8(strBlock));
					DlgButtonSizer->Add(DlgChoice, 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT | wxRIGHT, 15);
					DlgButtonSizer->Add(DlgCopy, 0, wxALIGN_BOTTOM | wxBOTTOM | wxRIGHT | wxLEFT, 15);
					wxSizer *DlgLineSizer = cpDialog->CreateSeparatedSizer(DlgButtonSizer);
					DlgLineSizer->Add(DlgBlockTxt, 0, wxALIGN_CENTER);

					cpDialog->SetSizerAndFit(DlgLineSizer);
					cpDialog->ShowModal();
			  });
    } while (riIter->Next(pilLevel));
  }
  tessApi->End();
  delete tessApi;
}

wxSize wxImagePanel::ScaleToScreen(wxSize sz) { // longest side scaled to fit screen (1080P) maintaing aspect ratio
  wxSize szNewSz = sz;
  double dNewW = sz.x/1920.f;
  double dNewH = sz.y/1080.f;
  
  if (dNewW > 1.0f || dNewH > 1.0f) {
    double dMaxF = dNewW >= dNewH ? dNewW : dNewH;
    szNewSz.x = round((double) szNewSz.x/dMaxF);
    szNewSz.y = round((double) szNewSz.y/dMaxF);
  }
  return szNewSz;
}

