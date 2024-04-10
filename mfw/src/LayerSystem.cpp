#include "mfwpch.h"
#include "LayerSystem.h"
#include "mfwlog.h"

namespace mfw {
    LayerSystem::~LayerSystem() {
        for (auto& layer : layers) {
            delete layer;
        }
    }

    void LayerSystem::addLayer(Layer* layer) {
        layers.push_back(layer);
        layers.back()->OnStart();
    }
}
