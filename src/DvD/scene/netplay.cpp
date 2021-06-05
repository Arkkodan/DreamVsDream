#include "netplay.h"

#include "scene.h"

#include "../network.h"
#include "../graphics.h"
#include "../../util/rng.h"

#include <cstring>

#include <glad/glad.h>

//NETPLAY
#ifndef NO_NETWORK
SceneNetplay::SceneNetplay() : Scene("netplay") {
	choice = 0;

	flashTimer = 0;
	flashDir = 1;
	drawShake = 0;
	barPos = 0;
	waiting = false;

	port = net::DEFAULT_PORT;
	ip = net::DEFAULT_IP;
	updateIp(false);
	updatePort(false);
}

SceneNetplay::~SceneNetplay() {
}

void SceneNetplay::think() {
	Scene::think();

	if (sys::frame % 2) {
		drawShake = !drawShake;
	}

	if (!flashDir) {
		switch (mode) {
		case net::MODE_NONE: {
			if (input(game::INPUT_A)) {
				sndSelect.play();
				if (choice) {
					mode = net::MODE_CLIENT;
				}
				else {
					mode = net::MODE_SERVER;
				}
				choice = 0;
			}
			else if (input(game::INPUT_B)) {
				sndOff.play();
				flashDir = -1;
				flashTimer = NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME / 2;
			}
			if (input(game::INPUT_UP | game::INPUT_DOWN)) {
				sndMenu.play();
				choice = !choice;
			}
		}
						   break;

		case net::MODE_SERVER: {
			if (waiting) {
				if (input(game::INPUT_A)) {
					if (net::connected) {
						sndSelect.play();
						FIGHT->gametype = SceneFight::GAMETYPE_VERSUS;
						setScene(SCENE_SELECT);
					}
				}
				else if (input(game::INPUT_B)) {
					if (!net::connected) {
						sndBack.play();
						bgm.play();
						waiting = false;
						net::stop();
					}
				}
			}
			else if (!digit) {
				if (input(game::INPUT_A)) {
					sndSelect.play();
					if (choice > 0) {
						digit = 1;
					}
					else {
						waiting = true;
						net::start(0, port);
						bgmWait.play();
					}
				}
				else if (input(game::INPUT_B)) {
					sndBack.play();
					mode = net::MODE_NONE;
					choice = 0;
				}
				if (input(game::INPUT_UP | game::INPUT_DOWN)) {
					sndMenu.play();
					choice = !choice;
				}
			}
			else {
				if (input(game::INPUT_A)) {
					//Save changes
					sndSelect.play();
					updatePort(true);
					digit = 0;
				}
				else if (input(game::INPUT_B)) {
					//Discard changes
					sndBack.play();
					updatePort(false);
					digit = 0;
				}
				if (input(game::INPUT_DIRMASK)) {
					sndMenu.play();
				}
				if (input(game::INPUT_UP)) {
					if (++portStr[digit - 1] > 9) {
						portStr[digit - 1] = 0;
					}
				}
				else if (input(game::INPUT_DOWN)) {
					if (--portStr[digit - 1] < 0) {
						portStr[digit - 1] = 9;
					}
				}
				else if (input(game::INPUT_LEFT)) {
					if (--digit < 1) {
						digit = 5;
					}
				}
				else if (input(game::INPUT_RIGHT)) {
					if (++digit > 5) {
						digit = 1;
					}
				}
			}
		}
							 break;

		case net::MODE_CLIENT: {
			if (waiting) {
				if (input(game::INPUT_A)) {
					if (net::connected) {
						sndSelect.play();
						FIGHT->gametype = SceneFight::GAMETYPE_VERSUS;
						setScene(SCENE_SELECT);
					}
				}
				else if (input(game::INPUT_B)) {
					if (!net::connected) {
						sndBack.play();
						bgm.play();
						waiting = false;
						net::stop();
					}
				}
			}
			else if (!digit) {
				if (input(game::INPUT_A)) {
					if (choice == 0) {
						waiting = true;
						audio::Music::stop();
						sndConStart.play();
						net::start(ip, port);
					}
					else if (choice == 3) {
						sndSelect.play();

						//Copy IP and port from clipboard
						char _sz_input[80];
						std::string clipboard = sys::getClipboard();
						strncpy(_sz_input, clipboard.c_str(), sizeof(_sz_input));

						char* _sz_ip = nullptr;
						char* _sz_port = nullptr;

						//Find the start of the IP and port, if they exist
						//Skip stuff until we reach a digit, that's the IP
						//Find a colon or a null, that's the port
						bool _success = true;
						int _octet_count = 0;
						for (int i = 0; ; i++) {
							if (!_sz_input[i]) {
								if (!_sz_ip) {
									_success = false;
								}
								else if (!_sz_port) {
									_sz_port = _sz_input + i;
								}
								break;
							}
							if (_sz_input[i] >= '0' && _sz_input[i] <= '9') {
								if (!_sz_ip) {
									_sz_ip = _sz_input + i;
								}
							}
							else if (_sz_input[i] == ':') {
								_sz_port = _sz_input + i + 1;
								_sz_input[i] = 0;
								break;
							}
							else if (_sz_input[i] == '.') {
								_octet_count++;
							}
							else {
								//This isn't an ip!
								_success = false;
								break;
							}
						}

						if (_success && _octet_count == 3) {
							//Update port
							port = atoi(_sz_port);
							if (port == 0) {
								port = net::DEFAULT_PORT;
							}
							updatePort(false);

							//Initialize IP
							ip = 0;

							//Parse IP (sigh)
							const char* _sz_octet = _sz_ip;
							int _i_octet = 0;
							for (int i = 0; ; i++) {
								if (_sz_ip[i] == '.' || !_sz_ip[i]) {
									_sz_ip[i] = 0;

									//Get the current octet
									int _octet = atoi(_sz_octet);
									if (_octet > 255) {
										_octet = 255;
									}
									ip |= (uint8_t)_octet << (8 * _i_octet);

									_sz_octet = _sz_ip + i + 1;
									_i_octet++;
								}
								if (_i_octet == 4) {
									break;
								}
							}
							updateIp(false);
						}
					}
					else {
						sndSelect.play();
						digit = 1;
					}
				}
				else if (input(game::INPUT_B)) {
					sndBack.play();
					mode = net::MODE_NONE;
					choice = 1;
				}
				if (input(game::INPUT_UP)) {
					sndMenu.play();
					if (--choice < 0) {
						choice = 3;
					}
				}
				else if (input(game::INPUT_DOWN)) {
					sndMenu.play();
					if (++choice > 3) {
						choice = 0;
					}
				}
			}
			else {
				if (input(game::INPUT_A)) {
					//Save changes
					sndSelect.play();
					if (choice == 1) {
						updateIp(true);
					}
					else if (choice == 2) {
						updatePort(true);
					}
					digit = 0;
				}
				else if (input(game::INPUT_B)) {
					//Discard changes
					sndBack.play();
					if (choice == 1) {
						updateIp(false);
					}
					else if (choice == 2) {
						updatePort(false);
					}
					digit = 0;
				}
				if (input(game::INPUT_DIRMASK)) {
					sndMenu.play();
				}
				if (choice == 1) {
					if (input(game::INPUT_UP)) {
						if (++ipStr[digit - 1] > 9) {
							ipStr[digit - 1] = 0;
						}
					}
					else if (input(game::INPUT_DOWN)) {
						if (--ipStr[digit - 1] < 0) {
							ipStr[digit - 1] = 9;
						}
					}
					else if (input(game::INPUT_LEFT)) {
						if (--digit < 1) {
							digit = 12;
						}
					}
					else if (input(game::INPUT_RIGHT)) {
						if (++digit > 12) {
							digit = 1;
						}
					}
				}
				else if (choice == 2) {
					if (input(game::INPUT_UP)) {
						if (++portStr[digit - 1] > 9) {
							portStr[digit - 1] = 0;
						}
					}
					else if (input(game::INPUT_DOWN)) {
						if (--portStr[digit - 1] < 0) {
							portStr[digit - 1] = 9;
						}
					}
					else if (input(game::INPUT_LEFT)) {
						if (--digit < 1) {
							digit = 5;
						}
					}
					else if (input(game::INPUT_RIGHT)) {
						if (++digit > 5) {
							digit = 1;
						}
					}
				}
			}
		}
							 break;
		}
	}

	if (!flashTimer && flashDir == 1) {
		sndOn.play();
	}

	flashTimer += flashDir;
	if (flashTimer < 0) {
		flashDir = 0;
		flashTimer = 0;
		setScene(SCENE_TITLE);
	}
	else if (flashTimer > NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME) {
		flashDir = 0;
		flashTimer = NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME;
	}

	if (flashTimer == NET_FLASH_TIME + NET_FLASH_HOLD_TIME) {
		if (flashDir == 1) {
			bgm.play();
			//graphics::setClearColor(20, 20, 20);
		}
		else {
			audio::Music::stop();
			//graphics::setClearColor(0, 0, 0);
		}
	}

	barPos--;
	if (barPos < -NET_BAR_SIZE) {
		barPos = sys::WINDOW_HEIGHT + NET_BAR_SIZE;
	}
}

void SceneNetplay::draw() const {
	Scene::draw();

	int speed = (flashDir + 1) / 2 + 1;

	if (flashTimer > NET_FLASH_TIME) {
		float xscale = 1.0f;
		if (flashTimer < NET_FLASH_TIME + NET_FLASH_HOLD_TIME + NET_FADE_TIME / speed) {
			float add = (1.0f - (flashTimer - NET_FLASH_TIME - NET_FLASH_HOLD_TIME) / (float)(NET_FADE_TIME / speed));
			add = add * add * add;
			xscale += add;
		}

		graphics::setScale(NET_SCALE * xscale, NET_SCALE);
		imgLogo.draw(sys::WINDOW_WIDTH / 2 - imgLogo.w * NET_SCALE * xscale / 2, sys::WINDOW_HEIGHT / 4 - imgLogo.h * NET_SCALE / 2 + drawShake);

		//Main menu
		switch (mode) {
		case net::MODE_NONE: {
			if (menuFont.exists()) {
				Font::setScale(NET_SCALE * xscale, NET_SCALE);
				menuFont.drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE * xscale / 2), sys::WINDOW_HEIGHT / 3 * 2 + drawShake, "Server");
				Font::setScale(NET_SCALE * xscale, NET_SCALE);
				menuFont.drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE * xscale / 2), sys::WINDOW_HEIGHT / 3 * 2 + 32 + drawShake, "Client");
			}
			graphics::setScale(xscale, NET_SCALE);
			imgCursor.draw(sys::WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE * xscale / 2), sys::WINDOW_HEIGHT / 3 * 2 + 32 * choice + drawShake);
		}
						   break;

		case net::MODE_SERVER: {
			if (waiting) {
				if (menuFont.exists()) {
					Font::setScale(NET_SCALE);
					if (net::connected) {
						menuFont.drawText(sys::WINDOW_WIDTH / 2 - (23 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "Connection established!");
					}
					else if (net::running) {
						menuFont.drawText(sys::WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "Waiting...");
					}
					else {
						menuFont.drawText(sys::WINDOW_WIDTH / 2 - (18 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "An error occurred.");
					}
				}
			}
			else {
				if (menuFont.exists()) {
					Font::setScale(NET_SCALE);
					menuFont.drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 + drawShake, "Start");
					Font::setScale(NET_SCALE);
					menuFont.drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 + 32 + drawShake, "Port:");
					for (int i = 0; i < 5; i++) {
						Font::setScale(NET_SCALE);
						menuFont.drawChar(sys::WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) + i * 8 * NET_SCALE, sys::WINDOW_HEIGHT / 3 * 2 - ((i + 1 == digit) ? 4 : 0) + 32 + drawShake, portStr[i] + '0');
					}
					if (!digit) {
						graphics::setScale(xscale, NET_SCALE);
						imgCursor.draw(sys::WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 + 32 * choice + drawShake);
					}
				}
			}
		}
							 break;

		case net::MODE_CLIENT: {
			if (waiting) {
				if (menuFont.exists()) {
					Font::setScale(NET_SCALE);
					if (net::connected) {
						menuFont.drawText(sys::WINDOW_WIDTH / 2 - (23 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "Connection established!");
					}
					else if (net::running) {
						menuFont.drawText(sys::WINDOW_WIDTH / 2 - (13 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "Connecting...");
					}
					else {
						menuFont.drawText(sys::WINDOW_WIDTH / 2 - (18 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 - 8 * NET_SCALE / 2 + drawShake, "An error occurred.");
					}
				}
			}
			else {
				if (menuFont.exists()) {
					Font::setScale(NET_SCALE);
					menuFont.drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 + drawShake, "Start");
					Font::setScale(NET_SCALE);
					menuFont.drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 + 32 + drawShake, "IP:");
					for (int i = 0; i < 4; i++) {
						for (int j = 0; j < 3; j++) {
							Font::setScale(NET_SCALE);
							menuFont.drawChar(sys::WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) + (i * 4 + j) * 8 * NET_SCALE, sys::WINDOW_HEIGHT / 3 * 2 - ((choice == 1 && (i * 3 + j) + 1 == digit) ? 4 : 0) + 32 + drawShake, ipStr[i * 3 + j] + '0');
						}
						if (i < 3) {
							Font::setScale(NET_SCALE);
							menuFont.drawChar(sys::WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) + (i * 4 + 3) * 8 * NET_SCALE, sys::WINDOW_HEIGHT / 3 * 2 + 32 + drawShake, '.');
						}
					}
					Font::setScale(NET_SCALE);
					menuFont.drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 + 32 * 2 + drawShake, "Port:");
					for (int i = 0; i < 5; i++) {
						Font::setScale(NET_SCALE);
						menuFont.drawChar(sys::WINDOW_WIDTH / 2 + (8 * 2 * NET_SCALE / 2) + i * 8 * NET_SCALE, sys::WINDOW_HEIGHT / 3 * 2 - ((choice == 2 && i + 1 == digit) ? 4 : 0) + 32 * 2 + drawShake, portStr[i] + '0');
					}
					if (!digit) {
						graphics::setScale(xscale, NET_SCALE);
						imgCursor.draw(sys::WINDOW_WIDTH / 2 - (10 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 + 32 * choice + drawShake);
					}
					Font::setScale(NET_SCALE);
					menuFont.drawText(sys::WINDOW_WIDTH / 2 - (8 * 8 * NET_SCALE / 2), sys::WINDOW_HEIGHT / 3 * 2 + 32 * 3 + drawShake, "Copy from clipboard");
				}
			}
		}
							 break;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
		glVertex3f(sys::WINDOW_WIDTH, barPos, 0);
		glVertex3f(0, barPos, 0);
		glColor4f(1.0f, 1.0f, 1.0f, 0.1f);
		glVertex3f(0, barPos + NET_BAR_SIZE, 0);
		glVertex3f(sys::WINDOW_WIDTH, barPos + NET_BAR_SIZE, 0);
		glEnd();

		graphics::setRender(Image::Render::ADDITIVE);
		imgStatic.draw(-util::roll(sys::WINDOW_WIDTH), -util::roll(sys::WINDOW_HEIGHT));
		graphics::setRender(Image::Render::MULTIPLY);
		imgScanlines.draw(0, 0);
	}

	if (flashDir) {
		int xoff = (flashTimer * flashTimer * flashTimer) / 5;
		int yoff = 1;
		if (flashTimer > NET_FLASH_TIME / 2) {
			yoff = flashTimer - NET_FLASH_TIME / 2;
			yoff = (yoff * yoff * yoff) / 5;
		}
		float alpha = 1.0f;
		if (flashTimer > NET_FLASH_TIME + NET_FLASH_HOLD_TIME) {
			xoff = sys::WINDOW_WIDTH;
			yoff = sys::WINDOW_WIDTH;
			alpha = 1.0f - (flashTimer - NET_FLASH_TIME - NET_FLASH_HOLD_TIME) / (float)(NET_FADE_TIME / speed);
		}
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, 0);
		glColor4f(1, 1, 1, alpha);
		glBegin(GL_QUADS);
		glVertex3f(sys::WINDOW_WIDTH / 2 - xoff, sys::WINDOW_HEIGHT / 2 - yoff, 0);
		glVertex3f(sys::WINDOW_WIDTH / 2 - xoff, sys::WINDOW_HEIGHT / 2 + yoff, 0);
		glVertex3f(sys::WINDOW_WIDTH / 2 + xoff, sys::WINDOW_HEIGHT / 2 + yoff, 0);
		glVertex3f(sys::WINDOW_WIDTH / 2 + xoff, sys::WINDOW_HEIGHT / 2 - yoff, 0);
		glEnd();
	}
}

void SceneNetplay::reset() {
	Scene::reset();
	choice = 0;
	mode = net::MODE_NONE;
	digit = 0;
	flashDir = 1;
	flashTimer = 0;
	barPos = sys::WINDOW_HEIGHT / 3 * 2;
	waiting = false;
}

void SceneNetplay::updateIp(bool toint) {
	if (toint) {
		ip = 0;
		for (int i = 0; i < 4; i++) {
			int n = 0;
			for (int j = 0; j < 3; j++) {
				n *= 10;
				n += ipStr[i * 3 + j];
			}
			if (n > 255) {
				n = 255;
			}
			ip |= (uint8_t)n << (8 * i);
		}
		if (!ip) {
			ip = net::DEFAULT_IP;
		}
		updateIp(false);
	}
	else {
		int n1 = ip;
		for (int i = 0; i < 4; i++) {
			int n = n1 & 0xFF;
			for (int j = 0; j < 3; j++) {
				ipStr[i * 3 + (2 - j)] = n % 10;
				n /= 10;
			}
			n1 >>= 8;
		}
	}
}

void SceneNetplay::updatePort(bool toint) {
	if (toint) {
		int newport = 0;
		for (int i = 0; i < 5; i++) {
			newport *= 10;
			newport += portStr[i];
		}
		if (newport > 65535) {
			newport = 65535;
		}
		if (newport == 0) {
			newport = net::DEFAULT_PORT;
		}
		port = newport;
		updatePort(false);
	}
	else {
		int p = port;
		for (int i = 0; i < 5; i++) {
			portStr[4 - i] = p % 10;
			p /= 10;
		}
	}
}

void SceneNetplay::parseLine(Parser& parser) {
	if (parser.is("BGM", 2)) { //Override superclass
		bgm.createFromFile("", getResource(parser.getArg(1), Parser::EXT_MUSIC));
		bgmWait.createFromFile("", getResource(parser.getArg(2), Parser::EXT_MUSIC));
	}
	else if (parser.is("SFX_CONNECT", 2)) {
		sndConStart.createFromFile(getResource(parser.getArg(1), Parser::EXT_SOUND));
		sndConSuccess.createFromFile(getResource(parser.getArg(2), Parser::EXT_SOUND));
	}
	else if (parser.is("LOGO", 1)) {
		imgLogo.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
	}
	else if (parser.is("SCANLINES", 2)) {
		imgScanlines.createFromFile(getResource(parser.getArg(1), Parser::EXT_IMAGE));
		imgStatic.createFromFile(getResource(parser.getArg(2), Parser::EXT_IMAGE));
	}
	else if (parser.is("MENU", 2)) {
		menuFont.createFromFile(getResource(parser.getArg(1), Parser::EXT_FONT));
		imgCursor.createFromFile(getResource(parser.getArg(2), Parser::EXT_IMAGE));
	}
	else if (parser.is("SFX_TV", 2)) {
		//Sounds
		sndOn.createFromFile(getResource(parser.getArg(1), Parser::EXT_SOUND));
		sndOff.createFromFile(getResource(parser.getArg(2), Parser::EXT_SOUND));
	}
	else {
		Scene::parseLine(parser);
	}
}
#endif
