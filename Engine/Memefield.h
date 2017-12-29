#pragma once
#include "Graphics.h"
#include "Vei2.h"

class Memefield
{
private:
	class Tile
	{
	public:
		enum class State
		{
			kHidden,
			kFlagged,
			kRevealed
		};
	public:
		// Tile management
		void spawn_meme();
		void toggle_flag();
		void reveal();
		void set_neighbor_meme_count(const int meme_count);

		// Flags
		bool is_revealed() const;
		bool is_flagged() const;
		bool has_meme() const;

		// Misc
		void draw(Graphics& gfx, const Vei2 screen_pos, const bool fucked) const;

	private:
		bool has_meme_ = false;
		int n_neighbor_memes_ = -1;
		State state_ = State::kHidden;
	};

public:
	Memefield(int n_memes);
	
	// Mouse Events
	void on_reveal_click(const Vei2& screen_pos);
	void on_flag_click(const Vei2& screen_pos);

	// Misc
	RectI get_rect() const;
	void draw(Graphics& gfx) const;

private:
	Tile & tile_at(const Vei2& grid_pos);
	const Tile& tile_at(const Vei2& grid_pos) const;
	Vei2 screen_to_grid(const Vei2& screen_pos) const;
	int count_neighbor_memes(const Vei2& grid_pos);
	bool tile_is_in_grid(const Vei2& grid_pos) const;

private:
	// Dimensions of the grid
	static constexpr int width = 20;
	static constexpr int height = 14;
	static constexpr int tile_size = 16;

	// Tile array
	Tile field_[width*height];

	// Flags
	bool fucked_ = false;
};