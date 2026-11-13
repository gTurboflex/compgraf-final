#pragma once

#include "Rack.h"
#include <vector>

struct Materials {
    Texture floorTexture{TEXTURE_DIR "/floor.pgm"};
    Texture metalTexture{TEXTURE_DIR "/metal.pgm"};
    Texture wallTexture{TEXTURE_DIR "/wall.pgm"};
    Texture panelTexture{TEXTURE_DIR "/panel.pgm"};

    Material floor{floorTexture, .50f, .56f, .61f, 1.f, 28.f, .18f};
    Material wall{wallTexture, .62f, .68f, .70f, .6f, 12.f, .08f};
    Material metal{metalTexture, .25f, .32f, .38f, 1.f, 80.f, .52f};
    Material panel{panelTexture, .42f, .48f, .53f, 2.f, 44.f, .32f};
    Material equipment{wallTexture, .66f, .70f, .74f, 1.f, 32.f, .20f};
    Material diffuser{wallTexture, .85f, .92f, 1.f, .4f, 16.f, .06f};
    Material screen{wallTexture, .08f, .42f, .55f, .3f, 48.f, .25f};
};

class Scene {
public:
    Scene();
    void update(float dt);
    void draw(const Mesh& cube, const Materials& materials, Shader& lit, Shader& led,
              const Lighting& lighting, float time);

    // Input and the HUD use the same racks that are updated and drawn.
    std::vector<Rack> racks;

private:
    void drawEquipment(const Mesh& cube, const Materials& materials, Shader& lit);
    void drawCableTrays(const Mesh& cube, const Materials& materials, Shader& lit);
};
