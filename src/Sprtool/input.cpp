#include "../DvD/globals.h"
#include "../DvD/input.h"
#include "../DvD/fighter.h"
#include "../DvD/graphics.h"

extern game::Fighter fighter;
extern int frame;
extern int anim;

namespace input {
util::Vector screenOffset;
util::Vector screenSize;

void resize(int w, int h) {
	if(h == 0) {
		h = 1;
	}

	int x = w;
	int y = h;

	float ratio1 = 1.0 * w / h;
	const float ratio2 = 1.0 * globals::WINDOW_WIDTH / globals::WINDOW_HEIGHT;
	if(ratio1 > ratio2) {
		x = h * ratio2;
	} else if(ratio1 < ratio2) {
		y = w / ratio2;
	}

	screenSize.x = x;
	screenSize.y = y;
	screenOffset.x = (w - x) / 2;
	screenOffset.y = (h - y) / 2;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(screenOffset.x, screenOffset.y, x, y);
	glOrtho(0, globals::WINDOW_WIDTH, globals::WINDOW_HEIGHT, 0, 0, 1);
	glMatrixMode(GL_MODELVIEW);
}

bool blackBG = true;

util::Vector mousePos;
bool mouse1Down = false;
sprite::HitBox* selectBox = nullptr;
bool selectBoxAttack = false;
util::Vector selectBoxOffset;

bool selectAll = false;

bool copyBoxes = false;
int copyFrame = -1;

void moveAll(int x, int y) {
	for(int i = 0; i < fighter.sprites[frame].hitBoxes.size; i++) {
		fighter.sprites[frame].hitBoxes.boxes[i].pos.x += x;
		fighter.sprites[frame].hitBoxes.boxes[i].pos.y += y;
	}
	for(int i = 0; i < fighter.sprites[frame].aHitBoxes.size; i++) {
		fighter.sprites[frame].aHitBoxes.boxes[i].pos.x += x;
		fighter.sprites[frame].aHitBoxes.boxes[i].pos.y += y;
	}
}

void keyPress(int key, bool press) {
	static int shift = 0;
	static int ctrl = 0;

	if(press)
		switch(key) {
		//Invert screen colors
		case SDLK_q:
			if(blackBG) {
				graphics::setClearColor(255, 255, 255);
			} else {
				graphics::setClearColor(0, 0, 0);
			}
			blackBG = !blackBG;
			break;

		//COPY AND PASTE
		case SDLK_c:
			if(ctrl) {
				copyFrame = frame;
				if(selectAll) {
					copyBoxes = true;
				} else {
					copyBoxes = false;
				}
			}
			break;

		case SDLK_v:
			if(ctrl) {
				if(copyFrame == frame || copyFrame == -1) {
					return;
				}

				//Copy everything
				if(copyBoxes) {
					delete [] fighter.sprites[frame].hitBoxes.boxes;
					delete [] fighter.sprites[frame].aHitBoxes.boxes;
					fighter.sprites[frame].hitBoxes.init(fighter.sprites[copyFrame].hitBoxes.size);
					fighter.sprites[frame].aHitBoxes.init(fighter.sprites[copyFrame].aHitBoxes.size);

					for(int i = 0; i < fighter.sprites[frame].hitBoxes.size; i++) {
						fighter.sprites[frame].hitBoxes.boxes[i] = fighter.sprites[copyFrame].hitBoxes.boxes[i];
					}
					for(int i = 0; i < fighter.sprites[frame].aHitBoxes.size; i++) {
						fighter.sprites[frame].aHitBoxes.boxes[i] = fighter.sprites[copyFrame].aHitBoxes.boxes[i];
					}
				} else {
					fighter.sprites[frame].x = fighter.sprites[copyFrame].x;
					fighter.sprites[frame].y = fighter.sprites[copyFrame].y;
				}
			}
			break;

		//New hitbox
		case SDLK_s:
			if(ctrl) {
				fighter.saveSpr();
			} else {
				selectBox = fighter.sprites[frame].hitBoxes.newHitbox();
				selectBoxAttack = false;
			}
			break;

		//New attack hitbox
		case SDLK_a:
			if(ctrl) {
				selectBox = nullptr;
				selectAll = !selectAll;
			} else {
				selectBox = fighter.sprites[frame].aHitBoxes.newHitbox();
				selectBoxAttack = true;
			}
			break;

		//Delete hitbox
		case SDLK_x:
			if(selectBoxAttack) {
				fighter.sprites[frame].aHitBoxes.deleteHitbox(selectBox);
			} else {
				fighter.sprites[frame].hitBoxes.deleteHitbox(selectBox);
			}
			selectBox = nullptr;
			break;

		//Deselect
		case SDLK_d:
			selectBox = nullptr;
			break;

		//Select previous hitbox
		case SDLK_p: {
			selectAll = false;
			if(selectBox) {
				int i = 0;
				for(; i < fighter.sprites[frame].hitBoxes.size; i++)
					if(fighter.sprites[frame].hitBoxes.boxes + i == selectBox) {
						if(i == 0) {
							if(fighter.sprites[frame].aHitBoxes.size) {
								selectBox = fighter.sprites[frame].aHitBoxes.boxes + (fighter.sprites[frame].aHitBoxes.size-1);
								selectBoxAttack = true;
							} else {
								selectBox = fighter.sprites[frame].hitBoxes.boxes + (fighter.sprites[frame].hitBoxes.size-1);
							}
						} else {
							selectBox = fighter.sprites[frame].hitBoxes.boxes + (i - 1);
						}
						break;
					}
				if(i == fighter.sprites[frame].hitBoxes.size) {
					for(i = 0; i < fighter.sprites[frame].aHitBoxes.size; i++)
						if(fighter.sprites[frame].aHitBoxes.boxes + i == selectBox) {
							if(i == 0) {
								if(fighter.sprites[frame].hitBoxes.size) {
									selectBox = fighter.sprites[frame].hitBoxes.boxes + (fighter.sprites[frame].hitBoxes.size-1);
									selectBoxAttack = false;
								} else {
									selectBox = fighter.sprites[frame].aHitBoxes.boxes + (fighter.sprites[frame].aHitBoxes.size-1);
								}
							} else {
								selectBox = fighter.sprites[frame].aHitBoxes.boxes + (i - 1);
							}
							break;
						}
				}
			} else {
				if(fighter.sprites[frame].hitBoxes.size) {
					selectBox = fighter.sprites[frame].hitBoxes.boxes;
					selectBoxAttack = false;
				} else if(fighter.sprites[frame].aHitBoxes.size) {
					selectBox = fighter.sprites[frame].aHitBoxes.boxes;
					selectBoxAttack = true;
				}
			}
		}
		break;

		//Select next hitbox
		case SDLK_n: {
			selectAll = false;
			if(selectBox) {
				int i = 0;
				for(; i < fighter.sprites[frame].hitBoxes.size; i++)
					if(fighter.sprites[frame].hitBoxes.boxes + i == selectBox) {
						if(i == fighter.sprites[frame].hitBoxes.size-1) {
							if(fighter.sprites[frame].aHitBoxes.size) {
								selectBox = fighter.sprites[frame].aHitBoxes.boxes;
								selectBoxAttack = true;
							} else {
								selectBox = fighter.sprites[frame].hitBoxes.boxes;
							}
						} else {
							selectBox = fighter.sprites[frame].hitBoxes.boxes + (i + 1);
						}
						break;
					}
				if(i == fighter.sprites[frame].hitBoxes.size) {
					for(i = 0; i < fighter.sprites[frame].aHitBoxes.size; i++)
						if(fighter.sprites[frame].aHitBoxes.boxes + i == selectBox) {
							if(i == fighter.sprites[frame].aHitBoxes.size-1) {
								if(fighter.sprites[frame].hitBoxes.size) {
									selectBox = fighter.sprites[frame].hitBoxes.boxes;
									selectBoxAttack = false;
								} else {
									selectBox = fighter.sprites[frame].aHitBoxes.boxes;
								}
							} else {
								selectBox = fighter.sprites[frame].aHitBoxes.boxes + (i + 1);
							}
							break;
						}
				}
			} else {
				if(fighter.sprites[frame].hitBoxes.size) {
					selectBox = fighter.sprites[frame].hitBoxes.boxes;
					selectBoxAttack = false;
				} else if(fighter.sprites[frame].aHitBoxes.size) {
					selectBox = fighter.sprites[frame].aHitBoxes.boxes;
					selectBoxAttack = true;
				}
			}
		}
		break;

		case SDLK_h:
		case SDLK_LEFT:
			if(selectBox || selectAll) {
				if(shift) {
					if(ctrl) {
						selectBox->size.x -= 10;
					} else {
						selectBox->size.x--;
					}
					if(selectBox->size.x <= 0) {
						selectBox->size.x = 1;
					}
				} else {
					if(selectAll) {
						if(ctrl) {
							moveAll(-10, 0);
						} else {
							moveAll(-1, 0);
						}
					} else {
						if(ctrl) {
							selectBox->pos.x -= 10;
						} else {
							selectBox->pos.x--;
						}
					}
				}
			} else {
				if(ctrl) {
					fighter.sprites[frame].x += 10;
				} else {
					fighter.sprites[frame].x++;
				}
			}
			break;

		case SDLK_l:
		case SDLK_RIGHT:
			if(selectBox || selectAll) {
				if(shift) {
					if(ctrl) {
						selectBox->size.x += 10;
					} else {
						selectBox->size.x++;
					}
				} else {
					if(selectAll) {
						if(ctrl) {
							moveAll(10, 0);
						} else {
							moveAll(1, 0);
						}
					} else {
						if(ctrl) {
							selectBox->pos.x += 10;
						} else {
							selectBox->pos.x++;
						}
					}
				}
			} else {
				if(ctrl) {
					fighter.sprites[frame].x -= 10;
				} else {
					fighter.sprites[frame].x--;
				}
			}
			break;

		case SDLK_k:
		case SDLK_UP:
			if(selectBox || selectAll) {
				if(shift) {
					if(ctrl) {
						selectBox->size.y += 10;
					} else {
						selectBox->size.y++;
					}
				} else {
					if(selectAll) {
						if(ctrl) {
							moveAll(0, 10);
						} else {
							moveAll(0, 1);
						}
					} else {
						if(ctrl) {
							selectBox->pos.y += 10;
						} else {
							selectBox->pos.y++;
						}
					}
				}
			} else {
				if(ctrl) {
					fighter.sprites[frame].y -= 10;
				} else {
					fighter.sprites[frame].y--;
				}
			}
			break;

		case SDLK_j:
		case SDLK_DOWN:
			if(selectBox || selectAll) {
				if(shift) {
					if(ctrl) {
						selectBox->size.y -= 10;
					} else {
						selectBox->size.y--;
					}
					if(selectBox->size.y <= 0) {
						selectBox->size.y = 1;
					}
				} else {
					if(selectAll) {
						if(ctrl) {
							moveAll(0, -10);
						} else {
							moveAll(0, -1);
						}
					} else {
						if(ctrl) {
							selectBox->pos.y -= 10;
						} else {
							selectBox->pos.y--;
						}
					}
				}
			} else {
				if(ctrl) {
					fighter.sprites[frame].y += 10;
				} else {
					fighter.sprites[frame].y++;
				}
			}
			break;

		case SDLK_o:
			//case SDLK_PageUp:
			selectBox = nullptr;
			selectAll = false;
			if(ctrl) {
				if(frame <= fighter.nSprites-1 - 10) {
					frame += 10;
				} else {
					frame = fighter.nSprites-1;
				}
			} else if(frame < fighter.nSprites-1) {
				frame++;
			}
			break;

		case SDLK_i:
			//case SDLK_PageDown:
			selectBox = nullptr;
			selectAll = false;
			if(ctrl) {
				if(frame >= 10) {
					frame -= 10;
				} else {
					frame = 0;
				}
			} else if(frame) {
				frame--;
			}
			break;

		case SDLK_LSHIFT:
			shift |= 1;
			break;

		case SDLK_RSHIFT:
			shift |= 2;
			break;

		case SDLK_LCTRL:
			ctrl |= 1;
			break;

		case SDLK_RCTRL:
			ctrl |= 2;
			break;
		}
	else
		switch (key) {
		case SDLK_LSHIFT:
			shift &= ~1;
			break;

		case SDLK_RSHIFT:
			shift &= ~2;
			break;

		case SDLK_LCTRL:
			ctrl &= ~1;
			break;

		case SDLK_RCTRL:
			ctrl &= ~2;
			break;
		}
}

void mouseMove(int x, int y) {
	//Calculates mouse pos based on size of screen
	float ratio = 1.0f;
	if(screenOffset.x) {
		ratio = (float)globals::WINDOW_HEIGHT / screenSize.y;
	} else if(screenOffset.y) {
		ratio = (float)globals::WINDOW_WIDTH / screenSize.x;
	}
	mousePos.x = (x - screenOffset.x) * ratio;
	mousePos.y = (y - screenOffset.y) * ratio;
	if(mousePos.x < 0) {
		mousePos.x = 0;
	}
	if(mousePos.x >= globals::WINDOW_WIDTH) {
		mousePos.x = globals::WINDOW_WIDTH - 1;
	}
	if(mousePos.y < 0) {
		mousePos.y = 0;
	}
	if(mousePos.y >= globals::WINDOW_HEIGHT) {
		mousePos.y = globals::WINDOW_HEIGHT - 1;
	}

	//Do stuff if a hitbox is selected
	if(mouse1Down && selectBox) {
		selectBox->pos.x = (mousePos.x + selectBoxOffset.x) / 2;
		selectBox->pos.y = (globals::FLIP(mousePos.y) + selectBoxOffset.y) / 2;
	}
}

void mousePress(int key, bool press) {
	if(press)
		switch(key) {
		case 0: {
			mouse1Down = true;
			int i = 0;
			for(; i < fighter.sprites[frame].hitBoxes.size; i++)
				if(fighter.sprites[frame].hitBoxes.boxes[i].collidePoint(mousePos.x - globals::WINDOW_WIDTH / 2, mousePos.y + globals::EDIT_OFFSET)) {
					selectAll = false; //TODO fix this
					selectBox = fighter.sprites[frame].hitBoxes.boxes + i;
					selectBoxAttack = false;
					selectBoxOffset.x = selectBox->pos.x - mousePos.x;
					selectBoxOffset.y = selectBox->pos.y - globals::FLIP(mousePos.y);
					break;
				}
			if(i == fighter.sprites[frame].hitBoxes.size) {
				selectBox = nullptr;
			}
			if(!selectBox) {
				for(i = 0; i < fighter.sprites[frame].aHitBoxes.size; i++)
					if(fighter.sprites[frame].aHitBoxes.boxes[i].collidePoint(mousePos.x - globals::WINDOW_WIDTH / 2, mousePos.y + globals::EDIT_OFFSET)) {
						selectAll = false; //TODO fix this
						selectBox = fighter.sprites[frame].aHitBoxes.boxes + i;
						selectBoxAttack = true;
						selectBoxOffset.x = selectBox->pos.x - mousePos.x;
						selectBoxOffset.y = selectBox->pos.y - globals::FLIP(mousePos.y);
						break;
					}
				if(i == fighter.sprites[frame].aHitBoxes.size) {
					selectBox = nullptr;
					selectAll = false;
				}
			}
		}
		break;
		case 1:
			break;
		}
	else
		switch(key) {
		case 0:
			mouse1Down = false;
			break;
		}
}
}
