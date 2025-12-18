//
// Created by ns on 12/7/24.
//

#ifndef GJAUDIOMIXERPLUGINSVST3HOSTAPP_H
#define GJAUDIOMIXERPLUGINSVST3HOSTAPP_H

#include "public.sdk/source/vst/hosting/pluginterfacesupport.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "public.sdk/source/vst/utility/stringconvert.h"
#include <map>
#include <memory>
#include <string>
#include <iostream>

#include "./AttributeList.h"
#include "./HostMessage.h"
#include "./audiohost/source/audiohost.h"


namespace Gj {
namespace Audio {
namespace Plugins {
namespace Vst3 {
namespace Host {
using namespace Steinberg;
using namespace Steinberg::Vst;

class App : public IHostApplication {
public:
  App();

  virtual ~App() noexcept { FUNKNOWN_DTOR}

  tresult terminate();

  //--- IHostApplication ---------------
  tresult PLUGIN_API getName(String128 name) override;

  tresult PLUGIN_API createInstance(TUID cid, TUID _iid, void** obj) override;

  DECLARE_FUNKNOWN_METHODS
  PlugInterfaceSupport* getPlugInterfaceSupport() const { return mPlugInterfaceSupport; }

private:
  IPtr<PlugInterfaceSupport> mPlugInterfaceSupport;
};
} // Host
} // Vst3
} // Plugins
} // Audio
} // Gj

#endif //GJAUDIOMIXERPLUGINSVST3HOSTAPP_H
