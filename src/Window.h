#ifndef WINDOW_H
#define WINDOW_H
#include <SDL2/SDL.h>
#include <string>
class Window
{
  public:
    //Start SDL and TTF, create the window and renderer
    static void Init();
    //Quit SDL and TTF
    static void Quit();
    //Draw an SDL_Texture
    static void Draw(SDL_Texture *, ...);
    //Load an image
    static SDL_Texture *LoadImage(std::string file);
    //Render some text
    static SDL_Texture *RenderText(std::string, std::string, SDL_Color, int);
    //Clear window
    static void Clear();
    //Present renderer
    static void Present();
    //Get the window's box
    static SDL_Rect Box();

  private:
    static SDL_Window *mWindow;
    static SDL_Renderer *mRenderer;
    static SDL_Rect mBox;
};
#endif