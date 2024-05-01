#include "mfwpch.h"
#pragma once

#include "UiLayer.h"

namespace mfw {
    class UiLayerSystem {
    public:
        ~UiLayerSystem();
        inline std::vector<UiLayer*>& getLayers() { return layers; }
        void addLayer(UiLayer* layer);

    private:
        std::vector<UiLayer*> layers;

    };
}
