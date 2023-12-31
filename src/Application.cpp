#include "Application.h"

Application::Application(const char *_title, int _width, int _height)
    : title(_title), width(_width), height(_height)
{
}

Application::~Application()
{
    for (int i = 0; i < 10; i++)
    {
        free(scenes[i]);
    }
    free(scenes);
    destroyApplication();
}

void Application::destroyApplication()
{
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Initialize the application with given flags
// default value set to 0.
void Application::initApplication(Uint32 flags)
{
    // Init SDL.
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("Error initializing SDL.\n"
               "SDL_Error: %s\n",
               SDL_GetError());
    }

    // Create window
    SDL_Window *win = SDL_CreateWindow(title.c_str(),
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       width, height, flags);

    if (win == NULL)
    {
        printf("Error initializing window.\n"
               "SDL_Error: %s\n",
               SDL_GetError());
    }

    // Construct SDL renderer object
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, render_flags);

    if (rend == NULL)
    {
        printf("SDL Renderer could not be initialized!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
    }

    // Initialize font engine
    int ttf = TTF_Init();
    if (ttf != 0)
    {
        printf("SDL2_ttf could not be initialized!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
    }

    // Initialize SDL2_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
    {
        printf("SDL2_mixer could not be initialized!\n"
               "SDL_Error: %s\n",
               SDL_GetError());
    }

    Mix_AllocateChannels(16);

    window = win;
    renderer = rend;
    _isRunning = true;
    scenes = (Scene **)malloc(sizeof(Scene *) * 10);
}

void Application::tick()
{
    currentTime = SDL_GetTicks();

    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        getFrameEvents().push_back(event);
    }

    handleEvents();
    currentScene->pollEvents();
    currentScene->update();
    currentScene->render(renderer);
    SDL_RenderPresent(renderer);

    getFrameEvents().clear();

    SDL_Delay(1000 / 60);
}

// Handles events to keep track mouse position and
// whether or not SDL_Quit event is triggered.
void Application::handleEvents()
{
    for (SDL_Event event : getFrameEvents())
    {
        // update mouse position on mouse updates
        if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
            SDL_GetMouseState(&mouseX, &mouseY);

        if (event.type == SDL_QUIT)
            this->setShouldClose(true);
    }
}

void Application::setCurrentScene(Scene *scene)
{
    if (currentScene != NULL)
    {
        currentScene->cleanUp();
        currentScene = NULL;
    }

    // TODO: Implement better error checking
    if (!scene->init())
    {
        std::cout << "Failed to initialize new scene!" << std::endl;
        this->setShouldClose(true);
    }

    currentScene = scene;
}

void Application::setCurrentSceneById(Uint8 id)
{
    setCurrentScene(scenes[id]);
}

// Both grabs and hides the mouse depending on the current state.
void Application::toggleMouse()
{
    SDL_SetWindowMouseGrab(window, mouseGrabbed ? SDL_FALSE : SDL_TRUE);
    SDL_ShowCursor(mouseHidden ? SDL_TRUE : SDL_FALSE);

    mouseGrabbed = !mouseGrabbed;
    mouseHidden = !mouseHidden;
}

// Grabs/ungrabs the mouse depending on state value.
void Application::grabMouse(bool state)
{
    mouseGrabbed = state;
    SDL_SetWindowMouseGrab(window, state ? SDL_TRUE : SDL_FALSE);
}

// Releases the mouse and shows it.
void Application::hideMouse(bool state)
{
    mouseHidden = state;
    SDL_ShowCursor(state ? SDL_FALSE : SDL_TRUE);
}