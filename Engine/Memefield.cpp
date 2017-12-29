#include "Memefield.h"
#include "SpriteCodex.h"
#include <assert.h>
#include <random>

void Memefield::Tile::spawn_meme()
{
	assert(!has_meme_);
	has_meme_ = true;
}

void Memefield::Tile::toggle_flag()
{
	// Cannot flag a revealed tile
	assert(state_ != State::kRevealed);


	if(state_ == State::kHidden)
	{
		// If hidden flag
		state_ = State::kFlagged;
	}
	else
	{
		// If flagged hide
		state_ = State::kHidden;
	}
}

void Memefield::Tile::reveal()
{
	assert(state_ != State::kRevealed);

	if (state_ == State::kHidden)
	{
		state_ = State::kRevealed;
	}
}

void Memefield::Tile::set_neighbor_meme_count(const int meme_count)
{
	assert(n_neighbor_memes_ == -1);
	n_neighbor_memes_ = meme_count;

}

bool Memefield::Tile::is_revealed() const
{
	return state_ == State::kRevealed;
}

bool Memefield::Tile::is_flagged() const
{
	return state_ == State::kFlagged;
}

bool Memefield::Tile::has_meme() const
{
	return has_meme_;
}

void Memefield::Tile::draw(Graphics & gfx, const Vei2 screen_pos, const bool fucked) const
{
	if (!fucked)
	{
		switch (state_)
		{
		case State::kHidden:
			SpriteCodex::DrawTileButton(screen_pos, gfx);
			break;
		case State::kFlagged:
			SpriteCodex::DrawTileButton(screen_pos, gfx);
			SpriteCodex::DrawTileFlag(screen_pos, gfx);
			break;
		case State::kRevealed:
			if (has_meme_)
			{
				SpriteCodex::DrawTileBomb(screen_pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTile0(screen_pos, gfx);
			}
			break;
		default:
			break;
		}
	}
	else
	{
		switch (state_)
		{
		case State::kHidden:
			if(has_meme_)
			{
				SpriteCodex::DrawTileBomb(screen_pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screen_pos, gfx);
			}
			break;
		case State::kFlagged:
			if (has_meme_)
			{
				SpriteCodex::DrawTileBomb(screen_pos, gfx);
				SpriteCodex::DrawTileFlag(screen_pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileBomb(screen_pos, gfx);
				SpriteCodex::DrawTileCross(screen_pos, gfx);
			}
			break;
		case State::kRevealed:
			if (has_meme_)
			{
				SpriteCodex::DrawTileBombRed(screen_pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTile0(screen_pos, gfx);
			}
		}
	}
}



Memefield::Memefield(const int n_memes)
{
	// Make sure we are spawning a valid number of memes
	assert(n_memes > 0 && n_memes < width * height);
	
	// Generate a random X and Y given the boundaries of the field
	std::random_device rd;
	std::mt19937 rng(rd());
	const std::uniform_int_distribution<int> x_dist(0, width - 1);
	const std::uniform_int_distribution<int> y_dist(0, height - 1);

	Vei2 new_loc;

	// For every meme the board is set to have
	for (auto i = 0; i < n_memes; i++)
	{
		// Spawn a meme at a random location if it does not already have a meme.
		new_loc.x = x_dist(rng);
		new_loc.y = y_dist(rng);
		if(!tile_at(new_loc).has_meme())
		{
			tile_at(new_loc).spawn_meme();
		}
	}

	// Set neighbor memes for every tile on the board that does not have a meme
	for (auto y = 0; y < height; y++)
	{
		for (auto x = 0; x < width; x++)
		{
			if (!tile_at({ x,y }).has_meme())
			{
				tile_at({ x,y }).set_neighbor_meme_count(count_neighbor_memes({ x,y }));
			}
		}
	}
}

void Memefield::on_reveal_click(const Vei2 & screen_pos)
{
	if (!fucked_)
	{
		const Vei2 grid_pos = screen_to_grid(screen_pos);

		// If player clicked inside the grid and the tile is not already revealed,
		// reveal the tile.
		if (tile_is_in_grid(grid_pos))
		{
			if (!tile_at(grid_pos).is_revealed())
			{
				tile_at(grid_pos).reveal();

				if (tile_at(grid_pos).has_meme())
				{
					fucked_ = true;
				}
			}
		}
	}

}

void Memefield::on_flag_click(const Vei2 & screen_pos)
{
	if (!fucked_)
	{
		const Vei2 grid_pos = screen_to_grid(screen_pos);

		// If the tile is on the grid and it is not currently revealed, toggle the flag
		if (tile_is_in_grid(grid_pos) && !tile_at(grid_pos).is_revealed())
		{
			tile_at(grid_pos).toggle_flag();
		}
	}
}

RectI Memefield::get_rect() const
{
	return RectI(0, width * tile_size, 0, height * tile_size);
}

void Memefield::draw(Graphics & gfx) const
{
	// Get the rectangle of the field
	const RectI background = get_rect();
	// Draw the background of the field
	gfx.DrawRect(background, SpriteCodex::baseColor);


	// Draw each tile on top of the background
	for (auto y = 0; y < height; y++)
	{
		for (auto x = 0; x < width; x++)
		{
			tile_at({ x,y }).draw(gfx, { x * tile_size, y * tile_size }, fucked_);
		}
	}

}

Memefield::Tile & Memefield::tile_at(const Vei2 & grid_pos)
{
	return field_[grid_pos.x + grid_pos.y * width];
}

const Memefield::Tile & Memefield::tile_at(const Vei2 & grid_pos) const
{
	return field_[grid_pos.x + grid_pos.y * width];
}

Vei2 Memefield::screen_to_grid(const Vei2 & screen_pos) const
{
	return {screen_pos.x / tile_size, screen_pos.y / tile_size};
}

int Memefield::count_neighbor_memes(const Vei2 & grid_pos)
{
	const int x_start = std::max(0, grid_pos.x - 1);
	const int y_start = std::max(0, grid_pos.y - 1);
	const int x_end = std::min(width - 1, grid_pos.x + 1);
	const int y_end = std::min(height - 1, grid_pos.y + 1);

	auto count = 0;
	for (Vei2 index = { x_start, y_start }; index.y <= y_end; index.y++)
	{
		for(index.x = x_start; index.x <= x_end; index.x++)
		{
			if(tile_at(index).has_meme())
			{
				count++;
			}
		}
	}
	return count;
}

bool Memefield::tile_is_in_grid(const Vei2 & grid_pos) const
{
	return grid_pos.x > 0 && grid_pos.x <= width - 1 &&
		grid_pos.y > 0 && grid_pos.y <= height - 1;
}
