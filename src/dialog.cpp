#include "../include/dialog.hpp"

wxDlgChoice::wxDlgChoice(wxPanel *parent, wxString text, int xbb_img, int ybb_img, int wbb_img, int hbb_img) : wxDialog(parent, wxID_ANY, wxEmptyString) {
	wxBoxSizer *bMainSizer = new wxBoxSizer(wxVERTICAL);
  bMainSizer->Add(new wxStaticText(this, wxID_ANY, text), 0, wxALIGN_CENTER | wxALL, 5);
	wxChoice *DlgChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, iNumCats, wxCatNames.data()); // add annotation 
	wxButton *DlgCopy = new wxButton(this, wxID_ANY, "Copy");
	DlgCopy->Bind(wxEVT_BUTTON,
		[=](wxCommandEvent&) {
			if (wxTheClipboard->Open()) {
				wxTheClipboard->SetData(new wxTextDataObject(text));
				wxTheClipboard->Close();
			}
			if (DlgChoice->GetSelection() != wxNOT_FOUND ) {
				if (iBboxes == 0) {
					jsData["images"] += nlohmann::json::object({
						{"id", iImgID},
						{"width", w_img},
						{"height", h_img},
						{"file_name", file},
						{"date_captured", date}
					});
				}
				jsData["annotations"] += nlohmann::json::object({
					{"id", 50*iImgID + iBboxes}, // id must be unique (atleast among other annotations
					{"iscrowd", 0},
					{"category_id", DlgChoice->GetSelection()},
					{"image_id", iImgID},
					{"area", wbb_img * hbb_img},
					{"bbox", nlohmann::json::array({xbb_img, ybb_img, wbb_img, hbb_img})}
				});
				iBboxes++;
			}
		});

  wxBoxSizer *bButtonSizer = new wxBoxSizer(wxHORIZONTAL);
  bButtonSizer->Add(DlgChoice, 1, wxBOTTOM | wxLEFT, 5); 
	bButtonSizer->AddStretchSpacer();
  bButtonSizer->Add(DlgCopy, 1, wxBOTTOM | wxRIGHT, 5);
  bMainSizer->Add(bButtonSizer, 0, wxALIGN_CENTER);
  SetSizerAndFit(bMainSizer);
}
