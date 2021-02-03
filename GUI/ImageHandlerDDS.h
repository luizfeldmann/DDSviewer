#ifndef _DDS_IMAGE_HANDLER_
#define _DDS_IMAGE_HANDLER_

#include "wx/image.h"
#include "wx/versioninfo.h"

class ImageHandlerDDS: public wxImageHandler
{
public:
    inline ImageHandlerDDS()
    {
        m_name = wxT("DDS file");
        m_extension = wxT("dds");
        m_altExtensions.Add(wxT("DDS"));
        m_type = wxBITMAP_TYPE_ANY;
        m_mime = wxT("image/dds");
    }

    virtual bool LoadFile( wxImage *image, wxInputStream& stream, bool verbose=true, int index=-1 ) wxOVERRIDE;
    virtual bool SaveFile( wxImage *image, wxOutputStream& stream, bool verbose=true ) wxOVERRIDE;

protected:
    virtual bool DoCanRead( wxInputStream& stream ) wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS(ImageHandlerDDS);
};

#endif // _DDS_IMAGE_HANDLER_
