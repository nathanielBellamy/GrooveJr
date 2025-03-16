//
// Created by ns on 3/16/25.
//

#ifndef VST3WINDOW_H
#define VST3WINDOW_H

#include <QWidget>

#include "../../../audio/effects/vst3/host/editorhost/source/platform/iwindow.h";

namespace Gj {
namespace Gui {

using namespace Steinberg::Vst::EditorHost;

class VST3Window : public QWidget, public IWindow {

public:
	~VST3Window () noexcept = default;

	void show ();
	void close ();
	void resize (Size newSize);
	Size getContentSize ();

	NativePlatformWindow getNativePlatformWindow () const;

	Steinberg::tresult queryInterface (const Steinberg::TUID iid, void** obj);

};

} // Gui
} // Gj



#endif //VST3WINDOW_H
