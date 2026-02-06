//
// Created by ns on 3/16/25.
//

#ifndef VSTWINDOW_H
#define VSTWINDOW_H

#include <QWidget>
#include <QWindow>
#include <QGridLayout>
#include <QResizeEvent>

#include "public.sdk/samples/vst-hosting/editorhost/source/platform/iwindow.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"

#include "../../../../../Logging.h"
#include "../../../../../types/AtomicStr.h"
#include "../../../../../types/Types.h"

namespace Gj {
namespace Gui {
namespace Mixer {
using namespace Steinberg::Vst::EditorHost;

class VstWindow final : public QWidget, public IWindow {
	ChannelIndex channelIndex;
	PluginIndex pluginIndex;
	AtomicStr name;
	QGridLayout grid;
	std::function<void(Coord, Coord)> vst3ResizeFunc;


	void setStyle();

	void setupGrid();

public:
	explicit VstWindow(QWidget* parent, ChannelIndex channelIndex, PluginIndex pluginIndex, const AtomicStr& name);

	void setVst3ResizeFunc(const std::function<void(Coord, Coord)>& func) { vst3ResizeFunc = func; }

	~VstWindow() noexcept override;

	void show() override;

	void close() override;

	void resize(Size newSize) override {
		// todo
	}

	void resizeEvent(QResizeEvent* event) override;

	Size getContentSize() override;

	[[nodiscard]] NativePlatformWindow getNativePlatformWindow() const override;

	Steinberg::tresult queryInterface(const Steinberg::TUID iid, void** obj) override;
};
} // Mixer
} // Gui
} // Gj


#endif //VSTWINDOW_H
