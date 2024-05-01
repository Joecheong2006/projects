#include "UiLayerSystem.h"
#include "mfwlog.h"

namespace mfw {
    UiLayerSystem::~UiLayerSystem() {
        for (auto& layer : layers) {
            delete layer;
        }
    }

    void UiLayerSystem::addLayer(UiLayer* layer) {
        layers.push_back(layer);
        layers.back()->OnStart();
    }
}
