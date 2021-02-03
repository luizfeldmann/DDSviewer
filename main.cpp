#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/dc.h>
#include <wx/dcbuffer.h>

#include "GUI/ddsGUI.h"
#include "GUI/ImageHandlerDDS.h"
#include "DDS/ddsLoader.h"

using namespace std;

class mainApp: public wxApp
{
	public:
	virtual bool OnInit();
};

wxIMPLEMENT_APP(mainApp);

class subFrame : public mainFrame
{
protected:
    wxImage bitmap;

public:
    subFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL ) :
        mainFrame(parent, id, title, pos, size, style | wxFULL_REPAINT_ON_RESIZE)
    {
        SetBackgroundStyle(wxBackgroundStyle::wxBG_STYLE_PAINT);
    }

    void SetFile(const wxString& path)
    {
        if (!bitmap.LoadFile(path))
            return;

        this->SetSize(bitmap.GetWidth(), bitmap.GetHeight());
        Refresh();
    }

    void OnPaint(wxPaintEvent& evt)
    {
        wxBufferedPaintDC dc(this); // use buffered version to avoid flicker
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        if (!bitmap.IsOk())
            return;

        int w, h;
        GetSize(&w, &h);

        wxImage picture = bitmap.Scale(w, h, wxImageResizeQuality::wxIMAGE_QUALITY_BICUBIC);
        dc.DrawBitmap(wxBitmap(picture, dc), 0, 0);
    }

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(subFrame, mainFrame)
EVT_PAINT(subFrame::OnPaint)
END_EVENT_TABLE()

bool mainApp::OnInit()
{
    wxInitAllImageHandlers();
    wxImage::AddHandler(new ImageHandlerDDS());

    subFrame* frame = new subFrame(NULL);
    frame->Show(true);

    if (argc == 2)
    {
        frame->SetTitle(argv[1]);
        frame->SetFile(argv[1]);
    }

    SetTopWindow(frame);
    return true;
}
