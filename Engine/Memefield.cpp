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

void Memefield::Tile::set_neighbor_meme_count(int meme_count)
{
	assert(n_neighbor_memes_ == -1);

}

bool Memefield::Tile::is_revealed()
{
	return state_ == State::kRevealed;
}

bool Memefield::Tile::is_flagged()
{
	return state_ == State::kFlagged;
}

bool Memefield::Tile::has_meme()
{
	return has_meme_;
}

void Memefield::Tile::draw(Graphics & gfx, const Vei2 screen_pos) const
{
	switch(state_)
	{
	case State::kHidden:
		SpriteCodex::DrawTileButton(screen_pos, gfx);
		break;
	case State::kFlagged:
		SpriteCodex::DrawTileButton(screen_pos, gfx);
		SpriteCodex::DrawTileFlag(screen_pos, gfx);
		break;
	case State::kRevealed:
		if(has_meme_)
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

Memefield::Memefield(const int n_memes)
{
	assert(n_memes > 0 && n_memes < width * height);

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
}

void Memefield::draw(Graphics & gfx) const
{
	for(auto y = 0; y < height; y++)
	{
		for(auto x = 0; x < width; x++)
		{
			tile_at({ x,y }).draw(gfx, {x * 16, y * 16});
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
