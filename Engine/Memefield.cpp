#include "memefield.h"
#include "SpriteCodex.h"
#include <assert.h>
#include <random>

void memefield::Tile::spawn_meme()
{
	assert(!has_meme_);
	has_meme_ = true;
}

void memefield::Tile::toggle_flag()
{
	// Cannot flag a revealed tile
	assert(state_ != state::revealed);


	if(state_ == state::hidden)
	{
		// If hidden flag
		state_ = state::flagged;
	}
	else
	{
		// If flagged hide
		state_ = state::hidden;
	}
}

void memefield::Tile::reveal()
{
	assert(state_ != state::revealed);

	if (state_ == state::hidden)
	{
		state_ = state::revealed;
	}
}

void memefield::Tile::set_neighbor_meme_count(const int meme_count)
{
	assert(n_neighbor_memes_ == -1);
	n_neighbor_memes_ = meme_count;

}

bool memefield::Tile::is_revealed() const
{
	return state_ == state::revealed;
}

bool memefield::Tile::is_flagged() const
{
	return state_ == state::flagged;
}

bool memefield::Tile::has_meme() const
{
	return has_meme_;
}

void memefield::Tile::draw(Graphics & gfx, const Vei2 screen_pos, const memefield::state field_state) const
{
	if (field_state != memefield::state::fucked)
	{
		switch (state_)
		{
		case state::hidden:
			SpriteCodex::DrawTileButton(screen_pos, gfx);
			break;
		case state::flagged:
			SpriteCodex::DrawTileButton(screen_pos, gfx);
			SpriteCodex::DrawTileFlag(screen_pos, gfx);
			break;
		case state::revealed:
			if (has_meme_)
			{
				SpriteCodex::DrawTileBomb(screen_pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileNum(screen_pos, gfx, n_neighbor_memes_);
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
		case state::hidden:
			if(has_meme_)
			{
				SpriteCodex::DrawTileBomb(screen_pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileButton(screen_pos, gfx);
			}
			break;
		case state::flagged:
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
		case state::revealed:
			if (has_meme_)
			{
				SpriteCodex::DrawTileBombRed(screen_pos, gfx);
			}
			else
			{
				SpriteCodex::DrawTileNum(screen_pos, gfx, n_neighbor_memes_);
			}
		}
	}

}



memefield::memefield(const Vei2& center, const int n_memes)
	:
	top_left_(center - Vei2(width * SpriteCodex::tileSize, height * SpriteCodex::tileSize) / 2)
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

void memefield::on_reveal_click(const Vei2 & screen_pos)
{
	if (state_ == state::memeing)
	{
		assert(screen_pos.x >= top_left_.x && screen_pos.y >= top_left_.y);
		const Vei2 grid_pos = screen_to_grid(screen_pos);

		// If the tile is not already revealed,
		// reveal the tile.
		if (!tile_at(grid_pos).is_revealed() && !tile_at(grid_pos).is_flagged())
		{
			tile_at(grid_pos).reveal();

			if (tile_at(grid_pos).has_meme())
			{
				state_ = state::fucked;
			}
			else if(check_win())
			{
				state_ = state::winrar;
			}
		}
	}

}

void memefield::on_flag_click(const Vei2 & screen_pos)
{
	if (state_ == state::memeing)
	{
		const Vei2 grid_pos = screen_to_grid(screen_pos);

		// If the tile is not currently revealed, toggle the flag
		if (!tile_at(grid_pos).is_revealed())
		{
			tile_at(grid_pos).toggle_flag();
		}
	}
}

RectI memefield::get_rect() const
{
	return RectI(top_left_, width * SpriteCodex::tileSize, height * SpriteCodex::tileSize);
}

void memefield::draw(Graphics & gfx) const
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
			tile_at({ x,y }).draw(gfx, { x * tile_size + top_left_.x, y * tile_size + top_left_.y }, state_);
		}
	}
}

memefield::state memefield::get_state() const
{
	return state_;
}

memefield::Tile & memefield::tile_at(const Vei2 & grid_pos)
{
	return field_[grid_pos.x + grid_pos.y * width];
}

const memefield::Tile & memefield::tile_at(const Vei2 & grid_pos) const
{
	return field_[grid_pos.x + grid_pos.y * width];
}

Vei2 memefield::screen_to_grid(const Vei2 & screen_pos) const
{
	// Because the grid starts at (0,0), we want to calculate the grid_pos as if
	// it were starting at screen position (0,0).  To do this, we subtract the origin (offset)
	// from the screen position, then divide by the tile size
	return {(screen_pos.x - top_left_.x) / tile_size, (screen_pos.y - top_left_.y) / tile_size };
}

int memefield::count_neighbor_memes(const Vei2 & grid_pos)
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

bool memefield::check_win()
{
	// Win condition: Every tile that does not have a meme is revealed

	// For every tile in the grid
	for (const auto t: field_)
	{
		if ((t.has_meme() && !t.is_flagged()) ||
			(!t.has_meme() && !t.is_revealed()))
		{
			return false;
		}
	}

	// Return true if we finish looping through the grid and we do not find
	// any unrevealed tiles that aren't memes
	return true;
}
