#pragma once
#include <SDL3/SDL.h>
#include "TextRenderer.h"
#include "TextureManager.h"
#include "Subscribable.h"
#include "StateManager.h"

/*!
	\enum SelectState
	\brief Tracks which UI layer of the main menu is currently active

	\var SelectState::Main             Root menu with Play, Tutorial, and Quit buttons
	\var SelectState::DifficultySelect Overlay for choosing AI difficulty before starting a game
	\var SelectState::Tutorial         Overlay displaying paginated tutorial text
*/
enum class SelectState {
	Main,
	DifficultySelect,
	Tutorial
};

/*!
	\enum ArrowState
	\brief Tracks which tutorial pagination arrow is currently hovered

	\var ArrowState::None   No arrow is hovered
	\var ArrowState::Left   Left (previous page) arrow is hovered
	\var ArrowState::Right  Right (next page) arrow is hovered
*/
enum class ArrowState {
	None,
	Left,
	Right
};

/*!
	\class MainMenu
	\brief Renders and handles input for the main menu

	Manages three UI layers via \c SelectState: the root button list,
	a difficulty selection overlay, and a paginated tutorial overlay.
	Inherits \c Subscribable to emit \c EventButtonHovered to external
	systems such as \c AudioSystem.
*/
class MainMenu : public Subscribable {
public:
	/*!
		\brief Constructs MainMenu and initializes tutorial page content
		\param renderer     SDL renderer used for all draw calls
		\param stateManager StateManager used to push \c PlayState or pop on quit
	*/
	MainMenu(SDL_Renderer* renderer, StateManager* stateManager);

	//! Processes SDL mouse motion and click events for all active UI layers
	void handleEvent(const SDL_Event& e);

	//! Reserved for future per-frame logic; currently a no-op
	void update();

	//! Draws the background, main buttons, and any active overlay
	void render();

	//! Returns a pointer to the internal TextRenderer
	TextRenderer* getTextRenderer() { return &m_text; }

	//! Returns a pointer to the internal TextureManager
	TextureManager* getTextureManager() { return &m_textures; }

private:
	int m_hoveredButton = -1; //!< Index of the hovered main menu button, or -1 if none
	int m_hoveredOverlayButton = -1; //!< Index of the hovered difficulty button, or -1 if none
	int m_currentTutorialPage = 0; //!< Current tutorial page index
	ArrowState m_hoveredTutorialArrow = ArrowState::None; //!< Currently hovered tutorial arrow
	SelectState m_currentSelectState = SelectState::Main; //!< Active UI layer

	SDL_Renderer* m_renderer; //!< SDL renderer used for all draw calls
	TextRenderer m_text; //!< Text rendering helper
	TextureManager m_textures; //!< Texture caching and lookup
	StateManager* m_stateManager; //!< Used to transition to PlayState or pop on quit

	std::vector<std::string> m_buttonLabels; //!< Labels for the main menu buttons
	size_t m_buttonLabelsSize; //!< Cached size of \c m_buttonLabels
	std::vector<std::string> m_difficultyLabels; //!< Labels for the difficulty selection buttons
	size_t m_difficultyLabelsSize; //!< Cached size of \c m_difficultyLabels
	std::vector<std::string> m_tutorialPages; //!< Tutorial text pages, one string per page

	//! Draws the "Card Game" title centered in the upper portion of the screen
	void drawTitle();

	//! Returns the rect for the centered overlay panel used by both overlays
	SDL_FRect getOverlayRect();

	/*!
		\brief Returns the rect for a button inside the overlay panel
		\param index        Zero-based button index
		\param totalButtons Total number of buttons in the overlay (unused; layout is index-driven)
	*/
	SDL_FRect getOverlayButtonRect(int index, int totalButtons);

	/*!
		\brief Draws a dark filled button with centered cyan text
		\param text Label to display
		\param rect Button bounds
	*/
	void drawButton(const std::string& text, const SDL_FRect& rect);

	/*!
		\brief Returns the screen rect for a main menu button at the given index
		\param index Zero-based button index
	*/
	SDL_FRect getButtonRect(int index);

	/*!
		\brief Handles a main menu button click by index
		\param index 0 = Play, 1 = Tutorial, 2 = Quit
	*/
	void handleButtonClick(int index);

	//! Subscribes internally to \c EventButtonHovered to keep \c m_hoveredButton in sync
	void bindEvents();

	//! Populates \c m_tutorialPages with all tutorial text strings
	void initTutorial();

	//! Returns the rect for the left pagination arrow in the tutorial overlay
	SDL_FRect getLeftArrowRect();

	//! Returns the rect for the right pagination arrow in the tutorial overlay
	SDL_FRect getRightArrowRect();
};