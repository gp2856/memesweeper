/****************************************************************************************** 
 *	Chili DirectX Framework Version 16.07.20											  *	
 *	Game.cpp																			  *
 *	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
 *																						  *
 *	This file is part of The Chili DirectX Framework.									  *
 *																						  *
 *	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
 *	it under the terms of the GNU General Public License as published by				  *
 *	the Free Software Foundation, either version 3 of the License, or					  *
 *	(at your option) any later version.													  *
 *																						  *
 *	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
 *	GNU General Public License for more details.										  *
 *																						  *
 *	You should have received a copy of the GNU General Public License					  *
 *	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
 ******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
#include "SpriteCodex.h"

Game::Game(MainWindow& wnd)
	:
	wnd(wnd),
	gfx(wnd),
	field_(gfx.GetRect().GetCenter(), 40),
	snd_fucked_(L"spayed.wav")
{
}

void Game::Go()
{
	gfx.BeginFrame();	
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	while(!wnd.mouse.IsEmpty())
	{
		const Mouse::Event e = wnd.mouse.Read();

		// Process events only if we are memeing
		if (field_.get_state() == memefield::state::memeing)
		{
			if (e.GetType() == Mouse::Event::Type::LPress)
			{
				const Vei2 mouse_pos = wnd.mouse.GetPos();
				if (field_.get_rect().Contains(mouse_pos))
				{
					field_.on_reveal_click(mouse_pos);
				}
				if(field_.get_state() == memefield::state::fucked)
				{
					snd_fucked_.Play();
				}
			}

			if (e.GetType() == Mouse::Event::Type::RPress)
			{
				const Vei2 mouse_pos = wnd.mouse.GetPos();

				if (field_.get_rect().Contains(mouse_pos))
				{
				field_.on_flag_click(mouse_pos);
				}
			}
		}
	}
}

void Game::ComposeFrame()
{
	
	field_.draw(gfx);
	if (field_.check_win())
	{
		SpriteCodex::DrawWin(gfx.GetRect().GetCenter(), gfx);
	}
}
