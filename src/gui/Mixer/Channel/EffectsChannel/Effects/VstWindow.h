//
// Created by ns on 3/16/25.
//

#ifndef VSTWINDOW_H
#define VSTWINDOW_H

#include <QWidget>
#include <QWindow>
#include <QGridLayout>

#include "public.sdk/samples/vst-hosting/editorhost/source/platform/iwindow.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"

#include "../../../../../Logging.h"

namespace Gj {
namespace Gui {

using namespace Steinberg::Vst::EditorHost;

class VstWindow final : public QWidget, public IWindow {

private:
	int channelIndex;
	int effectIndex;
	std::string name;
	QGridLayout grid;
	void setStyle();
	void setupGrid();

public:
  explicit VstWindow(QWidget* parent, int channelIndex, int effectIndex, std::string name);
	~VstWindow () noexcept override;

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
