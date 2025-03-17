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
  explicit VstWindow(QWidget* parent);
	~VstWindow () noexcept override = default;

	void show () override;
	void close () override;
	void resize (Size newSize) override;
	Size getContentSize () override;

	[[nodiscard]] NativePlatformWindow getNativePlatformWindow () const override;

	Steinberg::tresult queryInterface (const Steinberg::TUID iid, void** obj) override;

};

} // Gui
} // Gj



#endif //VSTWINDOW_H
