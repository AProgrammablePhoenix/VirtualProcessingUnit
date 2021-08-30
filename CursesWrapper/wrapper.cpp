#include <iostream>
#include <string>
#include "wrapper.hpp"

#include "../utility.h"

namespace {
	bool comn_initialized = false;

	class basic_nostream_routine {
	public:
		basic_nostream_routine(const char* inserted) {
			this->data = inserted;
		}
		const char* get() const {
			return this->data;
		}
	private:
		const char* data;
	};

	const basic_nostream_routine basic_nendl_core("\n");
	const basic_nostream_routine basic_nflush_core("");
}

namespace nstd {
	namespace base {
		void basic_nostream::init() {
			if (!comn_initialized) {
				initscr();
				scrollok(stdscr, TRUE);
				keypad(stdscr, TRUE);
				cbreak();
				echo();
				comn_initialized = true;
			}
		}
		void basic_nostream::update() {
			if (comn_initialized)
				wrefresh(stdscr);
		}
		void basic_nostream::close() {
			if (comn_initialized) {
				comn_initialized = false;
				endwin();
			}
		}
		bool basic_nostream::is_running() {
			return comn_initialized;
		}

		basic_nostream& basic_nendl(basic_nostream& nostream) {
			return nostream << basic_nendl_core.get();
		}
		basic_nostream& basic_nflush(basic_nostream& nostream) {
			wrefresh(stdscr);
			return nostream << basic_nflush_core.get();
		}

		basic_nostream& operator<<(basic_nostream& nostream, basic_nostream& (*_Pfn)(basic_nostream&)) {
			_Pfn(nostream);
			return nostream;
		}
		basic_nostream& operator<<(basic_nostream& nostream, const char* str) {
			addstr(str);
			return nostream;
		}
		basic_nostream& operator<<(basic_nostream& nostream, const std::string& str) {
			return nostream << str.c_str();
		}

		basic_nostream& operator<<(basic_nostream& nostream, char c) {
			const char temp[2] = { c, 0 };
			return nostream << temp;
		}
		basic_nostream& operator<<(basic_nostream& nostream, int n) {
			return nostream << std::to_string(n);
		}
		basic_nostream& operator<<(basic_nostream& nostream, long n) {
			return nostream << std::to_string(n);
		}
		basic_nostream& operator<<(basic_nostream& nostream, long long n) {
			return nostream << std::to_string(n);
		}
		basic_nostream& operator<<(basic_nostream& nostream, unsigned char c) {
			return nostream << (char)c;
		}
		basic_nostream& operator<<(basic_nostream& nostream, unsigned n) {
			return nostream << std::to_string(n);
		}
		basic_nostream& operator<<(basic_nostream& nostream, unsigned long n) {
			return nostream << std::to_string(n);
		}
		basic_nostream& operator<<(basic_nostream& nostream, unsigned long long n) {
			return nostream << std::to_string(n);
		}
		basic_nostream& operator<<(basic_nostream& nostream, float f) {
			return nostream << std::to_string(f);
		}
		basic_nostream& operator<<(basic_nostream& nostream, double d) {
			return nostream << std::to_string(d);
		}
		basic_nostream& operator<<(basic_nostream& nostream, long double ld) {
			return nostream << std::to_string(ld);
		}

		void basic_nistream::init() {
			if (!comn_initialized) {
				initscr();
				scrollok(stdscr, TRUE);
				keypad(stdscr, TRUE);
				cbreak();
				echo();
				comn_initialized = true;
			}
		}
		void basic_nistream::close() {
			if (comn_initialized) {
				comn_initialized = false;
				endwin();
			}
		}
		bool basic_nistream::is_running() {
			return comn_initialized;
		}

		basic_nistream& operator>>(basic_nistream& nistream, std::string& str) {
#if defined(ISWIN)
			str.clear();
			int ch = getch();
			size_t n_chrs = 0;
			size_t max_chrs = 0;

			while ((char)ch != '\n' && ch != PADENTER) {
				switch (ch) {
					case PADPLUS:
						str.push_back('+');
						waddch(stdscr, '+');
						++n_chrs;
						++max_chrs;
						break;
					case PADMINUS:
						str.push_back('-');
						waddch(stdscr, '-');
						++n_chrs;
						++max_chrs;
						break;
					case PADSTAR:
						str.push_back('*');
						waddch(stdscr, '*');
						++n_chrs;
						++max_chrs;
						break;
					case PADSLASH:
						str.push_back('/');
						waddch(stdscr, '/');
						++max_chrs;
						++n_chrs;
						break;
					case KEY_BACKSPACE:
					case 0x8:
					case 0x7F:
						if (n_chrs) {
							wdelch(stdscr);
							str.pop_back();
							--n_chrs;
							--max_chrs;
						}
						else {
							int x, y;
							getyx(stdscr, y, x);
							wmove(stdscr, y, ++x);
						}
						break;
					case KEY_LEFT:
						if (n_chrs) {
							int x, y;
							getyx(stdscr, y, x);
							wmove(stdscr, y, --x);
							--n_chrs;
						}
						break;
					case KEY_RIGHT:
						if (n_chrs < max_chrs) {
							int x, y;
							getyx(stdscr, y, x);
							wmove(stdscr, y, ++x);
							++n_chrs;
						}
						break;
					default:
						str.push_back((char)ch);
						++n_chrs;
						++max_chrs;
						break;
				}
				wrefresh(stdscr);
				ch = getch();
			}

			if (ch == PADENTER)
				nstd::ncout << nstd::nendl;

			return nistream;
#else
			char temp[256] = { 0 };
			getnstr(temp, 256);

			str = std::string(temp);
			return nistream;
#endif
		}
		basic_nistream& operator>>(basic_nistream& nistream, char& c) {
			c = (char)getch();
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, int& n) {
			std::string raw;
			nistream >> raw;

			n = std::stoi(raw);
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, long& n) {
			std::string raw;
			nistream >> raw;

			n = std::stol(raw);
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, long long& n) {
			std::string raw;
			nistream >> raw;

			n = std::stoll(raw);
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, unsigned char& c) {
			c = (unsigned char)getch();
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, unsigned& n) {
			std::string raw;
			nistream >> raw;

			n = (unsigned)std::stoul(raw);
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, unsigned long& n) {
			std::string raw;
			nistream >> raw;

			n = std::stoul(raw);
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, unsigned long long& n) {
			std::string raw;
			nistream >> raw;

			n = std::stoull(raw);
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, float& f) {
			std::string raw;
			nistream >> raw;

			f = std::stof(raw);
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, double& d) {
			std::string raw;
			nistream >> raw;

			d = std::stod(raw);
			return nistream;
		}
		basic_nistream& operator>>(basic_nistream& nistream, long double& ld) {
			std::string raw;
			nistream >> raw;

			ld = std::stold(raw);
			return nistream;
		}
	}

	nostream::nostream() { 
		nstd::base::ncout.init();
	}
	nostream::~nostream() {
		nstd::base::ncout.close();
	}

	void nostream::clear() {
		wclear(stdscr);
		*this << nflush;
	}

	nostream& operator<<(nostream& nostream, const char* str) {
		printw(str);
		return nostream;
	}
	nostream& operator<<(nostream& nostream, const std::string& str) {
		return nostream << str.c_str();
	}

	nostream& operator<<(nstd::nostream& nostream, nstd::nostream& (*_Pfn)(nstd::nostream&)) {
		_Pfn(nostream);
		return nostream;
	}
	base::basic_nostream& operator<<(base::basic_nostream& nostream, nstd::nostream& (*_Pfn)(base::basic_nostream&)) {
		_Pfn(nostream);
		return nostream;
	}

	nostream& n_rawendl(base::basic_nostream& nostream) {
		nostream << base::basic_nendl;
		return nstd::ncout;
	}
	nostream& nflush(base::basic_nostream& nostream) {
		nostream << base::basic_nflush;
		return nstd::ncout;
	}
	nostream& nendl(base::basic_nostream& nostream) {
		nostream << n_rawendl << nflush;
		return nstd::ncout;
	}

	nostream& n_rawendl(nstd::nostream& nostream) {
		nostream << base::basic_nendl;
		return nostream;
	}
	nostream& nflush(nstd::nostream& nostream) {
		nostream << base::basic_nflush;
		return nostream;
	}
	nostream& nendl(nstd::nostream& nostream) {
		return nostream << n_rawendl << nflush;
	}

	nistream::nistream() {
		nstd::base::ncin.init();
	}
	nistream::~nistream() {
		nstd::base::ncin.close();
	}
}