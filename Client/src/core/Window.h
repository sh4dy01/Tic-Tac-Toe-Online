#pragma once
#include <SFML/Graphics.hpp>

class Window
{
public:
    Window();
    ~Window();

#pragma region Lifecycle

    /// <summary>
    /// Creates (or recreates) a window with the given title and dimensions.
    /// </summary>
    void Create(const char* title, unsigned int width, unsigned int height);
    /// <summary>
    /// Polls events and calls callbacks.
    /// </summary>
    void PollEvents();
    /// <summary>
    /// Returns whether the window is open.
    /// </summary>
    bool IsOpen() const { return m_Window->isOpen(); }
    /// <summary>
    /// Returns if the window is focused.
    /// </summary>
    static bool const IsFocused() { return m_IsFocused; }

#pragma endregion

#pragma region Rendering

    /// <summary>
    /// Clears the window and render all the drawable objects.
    /// </summary>
    void Render();
    /// <summary>
    /// Sets the clear color of the window.
    /// </summary>
    void SetClearColor(const sf::Color& color) { m_ClearColor = color; }
    /// <summary>
    /// Registers a drawable object to be rendered.
    /// </summary>
    void RegisterDrawable(sf::Drawable* drawable) { m_Drawables.push_back(drawable); }
    /// <summary>
    /// Unregisters a drawable object.
    /// </summary>
    void UnregisterDrawable(sf::Drawable* drawable) { m_Drawables.erase(std::remove(m_Drawables.begin(), m_Drawables.end(), drawable), m_Drawables.end()); }
    /// <summary>
    /// Returns all the registered drawable objects.
    /// </summary>
    std::vector<sf::Drawable*>& GetDrawables() { return m_Drawables; }

#pragma endregion

#pragma region Other

    sf::Vector2f GetCenter() const { return sf::Vector2f(m_Window->getSize().x * 0.5f, m_Window->getSize().y *0.5f); }
    int GetWidth() const { return m_Window->getSize().x; }
    int GetHeight() const { return m_Window->getSize().y; }
    static sf::RenderWindow& GetWindow() { return *m_Window; }

#pragma endregion

private:

    static bool m_IsFocused;
    static sf::RenderWindow* m_Window;
    sf::Color m_ClearColor;
    std::vector<sf::Drawable*> m_Drawables;

};
