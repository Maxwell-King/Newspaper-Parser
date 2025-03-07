#include "imgpanel.h"

#include <wx/wxprec.h>
#include <wx/cmdline.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

static void openJSON(nlohmann::json& jsData, const std::string strDataSet) {
  std::filebuf fb;
	if (fb.open (strDataSet, std::ios::in)) {
		std::istream is(&fb);
		jsData = nlohmann::json::parse(is);
	}
}

class wxAnnotatorApp: public wxApp
{
  wxFrame *frame;
  wxImagePanel *drawPanel;
	std::vector<wxString> DlgOpts;
	wxString file;
	wxString date;
	nlohmann::json jsData;

	tesseract::TessBaseAPI *tessApi = new tesseract::TessBaseAPI();
	Pix *pixImg;

	int innerW, innerH, imgW, imgH, iImgID, iAnnID, iButtons = 0, iNumCats = 0;
	std::string jsFilename;

public:
  virtual bool OnInit();
  virtual int OnExit();
  virtual int OnRun();
  virtual void OnInitCmdLine(wxCmdLineParser& parser);
  virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
	
	void chooseJSON();
	void OCRimage();
	void addButtons();

};


static const wxCmdLineEntryDesc g_cmdLineDesc [] = {
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
	
	wxInitAllImageHandlers();
            
  frame = new wxFrame(NULL, wxID_ANY, wxT("Annotator"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
	drawPanel = new wxImagePanel(frame, file);
	imgW = drawPanel->image.GetSize().x;
	imgH = drawPanel->image.GetSize().y;

	innerW = frame->GetClientSize().x;
	innerH = frame->GetClientSize().y;

	chooseJSON();
	// OCRimage();
	// addButtons();
  
  frame->Show();
	
  return true;
}

void wxAnnotatorApp::chooseJSON() {
	nlohmann::json jsTrnData, jsValData;
	openJSON(jsValData,"val_20_percent.json");
	openJSON(jsTrnData,"train_80_percent.json");
	int iTrnSz = jsTrnData["images"].size();
  int iValSz = jsValData["images"].size();
  int iImgSz = iTrnSz + iValSz;
	iImgID = iImgSz+1;
	iAnnID = iImgID+3000;
  double dTrnPrct = (double) iTrnSz/iImgSz;
  double dValPrct = (double) iValSz/iImgSz;
	for (nlohmann::json::const_iterator jsIt = jsValData["categories"].cbegin(); 
				jsIt != jsValData["categories"].end();
				++jsIt) {
					std::cerr << (*jsIt)["name"] << std::endl;
					DlgOpts.push_back(nlohmann::to_string((*jsIt)["name"]));
					iNumCats++;
	}

	if (dValPrct < 0.2) { //add to val set. keep 80/20 ratio
    jsData = jsValData;
    jsFilename = "val_20_percent.json";
  }
  else { //add to train set
    jsData = jsTrnData;
    jsFilename = "train_80_percent.json";
  }
	jsData["images"] += nlohmann::json::object({
            {"id", iImgID},
            {"width", imgW},
            {"height", imgH},
            {"file_name", file},
            {"date_captured", date}
    });
}

void wxAnnotatorApp::OCRimage() {
	if (tessApi->Init(NULL, "eng")) {
      fprintf(stderr, "Could not initialize tesseract.\n");
      exit(1);
  }
	pixImg = pixRead(file.c_str());
	tessApi->SetImage(pixImg);
  tessApi->SetPageSegMode(tesseract::PSM_AUTO);
  tessApi->Recognize(0);
}

void wxAnnotatorApp::addButtons() {
  tesseract::ResultIterator* riIter = tessApi->GetIterator();
  tesseract::PageIteratorLevel pilLevel = tesseract::RIL_PARA;
  if (riIter != 0) {
    do {
			wxString wxstrBlock = wxString::FromUTF8(riIter->GetUTF8Text(pilLevel));
      int x1, y1, x2, y2;
      riIter->BoundingBox(pilLevel, &x1, &y1, &x2, &y2);
			int bboxW = x2-x1;
      int bboxH = y2-y1;
      wxPoint *cpPos = new wxPoint( (double) x1/imgW * innerW, (double) y1/imgH * innerH);
      wxSize *cpSz = new wxSize( (double) bboxW/imgW * innerW, (double) bboxH/imgH * innerH);
			wxButton *cpButton = new wxButton(drawPanel, wxID_ANY, wxEmptyString, *cpPos, *cpSz);
			//std::cerr << x1 << SEPERATOR << imgW << SEPERATOR << innerW <<std::endl;
			cpButton->SetBitmap(wxBitmapBundle(drawPanel->resized.GetSubBitmap(wxRect(*cpPos, *cpSz))));
			cpButton->Bind(wxEVT_BUTTON,
			  [=](wxCommandEvent&) {
					wxDialog *cpDialog = new wxDialog(drawPanel, wxID_ANY, wxEmptyString);
					wxStaticText *DlgBlockTxt = new wxStaticText(cpDialog, wxID_ANY, wxstrBlock);
					wxBoxSizer *DlgButtonSizer = new wxBoxSizer(wxHORIZONTAL);
					wxBoxSizer *DlgMainSizer = new wxBoxSizer(wxVERTICAL);
					
					wxChoice *DlgChoice = new wxChoice(cpDialog, wxID_ANY, wxDefaultPosition, wxDefaultSize, iNumCats, DlgOpts.data()); // add annotation 
					wxButton *DlgCopy = new wxButton(cpDialog, wxID_ANY, "Copy");
					DlgCopy->Bind(wxEVT_BUTTON,
						[=](wxCommandEvent&) {
							if (wxTheClipboard->Open())	{
									wxTheClipboard->SetData(new wxTextDataObject(wxstrBlock));
									wxTheClipboard->Close();
							}
							// get dlgchoice state, add to json
							if (DlgChoice->GetSelection() != wxNOT_FOUND ) {
								jsData["annotations"] += nlohmann::json::object({
									{"id", iAnnID++},
									{"iscrowd", 0},
									{"category_id", DlgChoice->GetSelection()},
									{"image_id", iImgID},
									{"area", bboxW * bboxH},
									{"bbox", nlohmann::json::array({x1, y1,	bboxW, bboxH})}
								});
								
								//std::cerr << jsData.dump(-1) << std::endl;
							}
						});
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

int wxAnnotatorApp::OnExit()
{
	std::ofstream oDataSet(jsFilename, std::ofstream::trunc);
  oDataSet << jsData.dump(-1) << std::endl;
  std::cerr << "Contents saved to " << jsFilename << std::endl;
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
	file = parser.GetParam(0);
	date = parser.GetParam(1);

  return true;
}

IMPLEMENT_APP(wxAnnotatorApp)
