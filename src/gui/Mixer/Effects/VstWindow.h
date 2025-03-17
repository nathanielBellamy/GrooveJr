//
// Created by ns on 3/16/25.
//

#ifndef VSTWINDOW_H
#define VSTWINDOW_H

#include <QWidget>

#include "../../../audio/effects/vst3/host/editorhost/source/platform/iwindow.h"

namespace Gj {
namespace Gui {

using namespace Steinberg::Vst::EditorHost;

class VstWindow : public QWidget, public IWindow {

public:
  VstWindow(QWidget* parent);
	~VstWindow () noexcept = default;

	void show ();
	void close ();
	void resize (Size newSize);
	Size getContentSize ();

	NativePlatformWindow getNativePlatformWindow () const;

	Steinberg::tresult queryInterface (const Steinberg::TUID iid, void** obj);

};

} // Gui
} // Gj



#endif //VSTWINDOW_H
