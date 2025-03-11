#include "../include/dialog.hpp"

wxDlgChoice::wxDlgChoice(wxPanel *parent, wxString text, int xbb_img, int ybb_img, int wbb_img, int hbb_img) : wxDialog(parent, wxID_ANY, wxEmptyString) {
	wxStaticText *DlgBlockTxt = new wxStaticText(this, wxID_ANY, text);
	wxBoxSizer *DlgButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *DlgMainSizer = new wxBoxSizer(wxVERTICAL);
	wxChoice *DlgChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, iNumCats, wxCatNames.data()); // add annotation 
	wxButton *DlgCopy = new wxButton(this, wxID_ANY, "Copy");
	DlgCopy->Bind(wxEVT_BUTTON,
			[=](wxCommandEvent&) {
				if (wxTheClipboard->Open()) {
					wxTheClipboard->SetData(new wxTextDataObject(text));
					wxTheClipboard->Close();
				}
				if (DlgChoice->GetSelection() != wxNOT_FOUND ) {
					jsData["annotations"] += nlohmann::json::object({
						{"id", 50*iImgID + iBboxes}, // id must be unique 
						{"iscrowd", 0},
						{"category_id", DlgChoice->GetSelection()},
						{"image_id", iImgID},
						{"area", wbb_img * hbb_img},
						{"bbox", nlohmann::json::array({xbb_img, ybb_img, wbb_img, hbb_img})}
					});
					iBboxes++;
				}
			});
	DlgButtonSizer->Add(DlgChoice, 0, wxALIGN_BOTTOM | wxBOTTOM | wxLEFT | wxRIGHT, 15);
	DlgButtonSizer->Add(DlgCopy, 0, wxALIGN_BOTTOM | wxBOTTOM | wxRIGHT | wxLEFT, 15);
	wxSizer *DlgLineSizer = this->CreateSeparatedSizer(DlgButtonSizer);
	DlgLineSizer->Add(DlgBlockTxt, 0, wxALIGN_CENTER);
	this->SetSizerAndFit(DlgLineSizer);
}
