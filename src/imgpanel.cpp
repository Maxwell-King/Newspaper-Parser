#include "../include/imgpanel.hpp"

std::string OCRregion(Pix *pixRegion, int x, int y, int w, int h) {
	tesseract::TessBaseAPI *tessApi = new tesseract::TessBaseAPI();
	tessApi->Init(NULL, "eng");
	tessApi->SetImage(pixRegion);
	std::string outText(tessApi->GetUTF8Text());
	tessApi->End();
	delete tessApi;
	pixDestroy(&pixRegion);
	return outText;
}

wxImagePanel::wxImagePanel(wxFrame *parent, wxString file, wxString date) : wxPanel(parent) {
	Bind(wxEVT_PAINT, &wxImagePanel::paintEvent, this);
	Bind(wxEVT_LEFT_DOWN, &wxImagePanel::mouseDownEvent, this);
	Bind(wxEVT_MOTION, &wxImagePanel::mouseDragEvent, this);
	Bind(wxEVT_LEFT_UP, &wxImagePanel::mouseUpEvent, this);
	image.LoadFile(file, wxBITMAP_TYPE_JPEG); 
	pixImg = pixRead(file.c_str());
	w_img = image.GetSize().x;
	h_img = image.GetSize().y;
	jsData["images"] += nlohmann::json::object({
						{"id", iImgID},
						{"width", w_img},
						{"height", h_img},
						{"file_name", file},
						{"date_captured", date}
		});
	parent->SetSize(ScaleToScreen(image.GetSize()));
	w_bmp = parent->GetClientSize().x;
	h_bmp = parent->GetClientSize().y;
	resized = wxBitmap(image.Scale(w_bmp, h_bmp));
}

void wxImagePanel::paintEvent(wxPaintEvent &evt) { // called on Refresh();
	wxPaintDC dc(this);
	render(dc);
}

void wxImagePanel::mouseDownEvent(wxMouseEvent &evt) {
	wxhghPoint = evt.GetPosition(); // assuming topleft -> bottomright
	b_mouseleftdown = true;
} 

void wxImagePanel::mouseDragEvent(wxMouseEvent &evt) {
	if (b_mouseleftdown) {
		wxcurPoint = evt.GetPosition();
		b_dragging = true;
		wxClientDC dc(this);
		render(dc);
	}
} 

void wxImagePanel::mouseUpEvent(wxMouseEvent &evt) {
	if (b_dragging) {
		wxlowPoint = evt.GetPosition();
		if (wxlowPoint.x < wxhghPoint.x) {
			int lowX = wxhghPoint.x;
			wxhghPoint.x = wxlowPoint.x;
			wxlowPoint.x = lowX;
		} 
		if (wxlowPoint.y < wxhghPoint.y) {
			int lowY = wxhghPoint.y;
			wxhghPoint.y = wxlowPoint.y;
			wxlowPoint.y = lowY;
		} 
		int xbb_img = (double) wxhghPoint.x/w_bmp * w_img;
		int ybb_img = (double) wxhghPoint.y/h_bmp * h_img;
		int wbb_img = (double) (wxlowPoint.x-wxhghPoint.x)/w_bmp * w_img;
		int hbb_img = (double) (wxlowPoint.y-wxhghPoint.y)/h_bmp * h_img;
		Box *boxRegion = boxCreate(xbb_img, ybb_img, wbb_img, hbb_img);
		Pix *pixRegion = pixClipRectangle(pixImg, boxRegion, NULL);
		wxString strRegion =	wxString::FromUTF8(OCRregion(pixRegion, xbb_img, ybb_img, wbb_img, hbb_img));
		wxDlgChoice *cpDlg = new wxDlgChoice(this, strRegion, xbb_img, ybb_img, wbb_img, hbb_img);		
		cpDlg->ShowModal();
	}
	b_mouseleftdown = false;
	b_dragging = false;
}

void wxImagePanel::render(wxDC &dc) {
	if (b_dragging) { 
		dc.DrawLine(wxhghPoint.x, wxhghPoint.y, wxcurPoint.x, wxhghPoint.y);
		dc.DrawLine(wxhghPoint.x, wxhghPoint.y, wxhghPoint.x, wxcurPoint.y);
		dc.DrawLine(wxcurPoint.x, wxcurPoint.y, wxcurPoint.x, wxhghPoint.y);
		dc.DrawLine(wxcurPoint.x, wxcurPoint.y, wxhghPoint.x, wxcurPoint.y);
	}
	dc.DrawBitmap(resized, 0, 0, false);
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

