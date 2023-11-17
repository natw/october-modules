#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
  pluginInstance = p;

  p->addModel(modelSSG);
  p->addModel(modelRandom);
  p->addModel(modelNCOM);
  // Any other plugin initialization may go here.
  // As an alternative, consider lazy-loading assets and lookup tables when your
  // module is created to reduce startup times of Rack.
}
