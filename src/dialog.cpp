#include "../include/dialog.hpp"

int iBboxes = 0;

wxDlgChoice::wxDlgChoice(wxPanel *parent, wxString text, int xbb_img, int ybb_img, int wbb_img, int hbb_img) : wxDialog(parent, wxID_ANY, wxEmptyString) {
	wxBoxSizer *bMainSizer = new wxBoxSizer(wxVERTICAL);
	bMainSizer->Add(new wxStaticText(this, wxID_ANY, text), 0, wxALIGN_CENTER | wxALL, 5);
	wxChoice *DlgChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, js_data.categories_size, js_data.categories_list.data()); // add annotation 
	wxButton *DlgCopy = new wxButton(this, wxID_ANY, "Copy");
	DlgCopy->Bind(wxEVT_BUTTON,
		[=](wxCommandEvent&) {
			if (wxTheClipboard->Open()) {
				wxTheClipboard->SetData(new wxTextDataObject(text));
				wxTheClipboard->Close();
			}
			if (DlgChoice->GetSelection() != wxNOT_FOUND ) {
				if (iBboxes == 0) {
					add_image(date, file, h_img, img_id, w_img);
				}
				add_annotation(wbb_img * hbb_img, xbb_img, ybb_img, wbb_img, hbb_img, js_data.cat_map[DlgChoice->GetString(DlgChoice->GetSelection())], 50*img_id + iBboxes, img_id);
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
