#pragma once
#include "Layer.h"

namespace mfw {
    class LayerSystem {
    public:
        ~LayerSystem();
        inline std::vector<Layer*>& getLayers() { return layers; }
        void addLayer(Layer* layer);

    private:
        std::vector<Layer*> layers;

    };
}
