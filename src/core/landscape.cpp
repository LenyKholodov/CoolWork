#include <landscape.h>
#include "draw_stuff.h"

#define NO_ACCUMULATION_MODE 0
#define ONE_LEVEL_ACCUMULATION_MODE 1
#define ACCUMULATION_MODE 2

struct Landscape::Impl {     //описание реализации класса Landscape
  Bitmap height_map;         //карта высот рельефа 
};

Landscape::Landscape ()
{
  impl = new Impl;

  impl->height_map.width = 10;
  impl->height_map.height = 10;
  impl->height_map.data.resize (100, 0);
}

Landscape::~Landscape ()
{
  delete impl;
}

vec2i Landscape::GetDimensions ()
{
  return vec2i (impl->height_map.width, impl->height_map.height);
}

void Landscape::PencilExtrusion (int radius, int x, int y, float flow, int mode, struct Bitmap* old_map)
{
  if (flow > 100) flow = 100;
  if (flow < -100) flow = -100;

  if (x > impl->height_map.width) x = impl->height_map.width;
  if (x < 0) x = 0;
  if (y > impl->height_map.height) y = impl->height_map.height;
  if (y < 0) y = 0;

  for (int i = max(0, y - radius); i < min(impl->height_map.height, y + radius); i++)
    for (int j = max(0, x - radius); j < min(impl->height_map.width, x + radius); j++)
    {
      switch (mode)
      {
        case NO_ACCUMULATION_MODE:        impl->height_map.data[i * impl->height_map.width + j] = flow / 100.f; break;
        case ONE_LEVEL_ACCUMULATION_MODE: impl->height_map.data[i * impl->height_map.width + j] = old_map->data[i * impl->height_map.width + j] + flow / 100.f; break;
        case ACCUMULATION_MODE: 	  impl->height_map.data[i * impl->height_map.width + j] += flow / 100.f; break;   
      }
    }
}

void Landscape::BrushExtrusion (int radius, int x, int y, float flow, int mode, struct Bitmap* old_map)
{
  if (flow > 100) flow = 100;
  if (flow < -100) flow = -100;

  if (x > impl->height_map.width) x = impl->height_map.width;
  if (x < 0) x = 0;
  if (y > impl->height_map.height) y = impl->height_map.height;
  if (y < 0) y = 0;

  float len, extr_coeff;
  for (int i = max(0, y - radius); i < min(impl->height_map.height, y + radius); i++)
    for (int j = max(0, x - radius); j < min(impl->height_map.width, x + radius); j++)
    {
      len = length (vec2f ((float)x - j, (float)y - i));
      
      if (abs(len) > radius) continue;
      
      extr_coeff = (1.f - pow((len / (float)radius), 2));
      
      switch (mode)
      {
        case NO_ACCUMULATION_MODE:        impl->height_map.data[i * impl->height_map.width + j] = flow / 100.f * extr_coeff; break;
        case ONE_LEVEL_ACCUMULATION_MODE:
        {
          impl->height_map.data[i * impl->height_map.width + j] += flow / 100.f * extr_coeff;
          if (flow > 0)
            impl->height_map.data[i * impl->height_map.width + j] = min(impl->height_map.data[i * impl->height_map.width + j], old_map->data[i * impl->height_map.width + j] + flow / 100.f);
          else
            impl->height_map.data[i * impl->height_map.width + j] = max(impl->height_map.data[i * impl->height_map.width + j], old_map->data[i * impl->height_map.width + j] + flow / 100.f);
          break;
        }
        case ACCUMULATION_MODE:           impl->height_map.data[i * impl->height_map.width + j] += flow / 100.f * extr_coeff; break;
      }
    }
}

void Landscape::resize (size_t width,size_t height)
{
  impl->height_map.width  = width;
  impl->height_map.height = height;
  impl->height_map.data.resize (width * height, 0);
}

void Landscape::GetHeightMap (HeightMap& map)
{
  map.map    = &impl->height_map.data [0];
  map.width  = impl->height_map.width;
  map.height = impl->height_map.height;
}

Bitmap Landscape::GetHeightMap ()
{
  return impl->height_map;
}
