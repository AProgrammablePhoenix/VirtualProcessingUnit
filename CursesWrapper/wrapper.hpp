#pragma once

#include <iostream>
#include <curses.h>

namespace nstd {
	namespace base {
		class basic_nostream {
		public:
			void init();
			void update();
			void close();

			bool is_running();
		};

		static basic_nostream ncout;

		basic_nostream& basic_nendl(basic_nostream& nostream);
		basic_nostream& basic_nflush(basic_nostream& nostream);

		basic_nostream& operator<<(basic_nostream& nostream, basic_nostream& (*_Pfn)(basic_nostream&));
		basic_nostream& operator<<(basic_nostream& nostream, const char* str);
		basic_nostream& operator<<(basic_nostream& nostream, const std::string& str);

		basic_nostream& operator<<(basic_nostream& nostream, char c);
		basic_nostream& operator<<(basic_nostream& nostream, int n);
		basic_nostream& operator<<(basic_nostream& nostream, long n);
		basic_nostream& operator<<(basic_nostream& nostream, long long n);
		basic_nostream& operator<<(basic_nostream& nostream, unsigned char c);
		basic_nostream& operator<<(basic_nostream& nostream, unsigned n);
		basic_nostream& operator<<(basic_nostream& nostream, unsigned long n);
		basic_nostream& operator<<(basic_nostream& nostream, unsigned long long n);
		basic_nostream& operator<<(basic_nostream& nostream, float f);
		basic_nostream& operator<<(basic_nostream& nostream, double d);
		basic_nostream& operator<<(basic_nostream& nostream, long double ld);

		class basic_nistream {
		public:
			void init();
			void close();

			bool is_running();
		};

		static basic_nistream ncin;

		basic_nistream& operator>>(basic_nistream& nistream, std::string& str);
		basic_nistream& operator>>(basic_nistream& nistream, char& c);
		basic_nistream& operator>>(basic_nistream& nistream, int& n);
		basic_nistream& operator>>(basic_nistream& nistream, long& n);
		basic_nistream& operator>>(basic_nistream& nistream, long long& n);
		basic_nistream& operator>>(basic_nistream& nistream, unsigned char& c);
		basic_nistream& operator>>(basic_nistream& nistream, unsigned& n);
		basic_nistream& operator>>(basic_nistream& nistream, unsigned long& n);
		basic_nistream& operator>>(basic_nistream& nistream, unsigned long long& n);
		basic_nistream& operator>>(basic_nistream& nistream, float& f);
		basic_nistream& operator>>(basic_nistream& nistream, double& d);
		basic_nistream& operator>>(basic_nistream& nistream, long double& ld);

	}

	class nostream : public virtual base::basic_nostream {
	public:
		nostream();
		~nostream();

		void clear();
	};

	nostream& operator<<(nstd::nostream& nostream, nstd::nostream& (*_Pfn)(nstd::nostream&));
	base::basic_nostream& operator<<(base::basic_nostream& nostream, nstd::nostream& (*_Pfn)(base::basic_nostream&));

	nostream& n_rawendl(base::basic_nostream& nostream);
	nostream& nflush(base::basic_nostream& nostream);
	nostream& nendl(base::basic_nostream& nostream);

	nostream& n_rawendl(nstd::nostream& nostream);
	nostream& nflush(nstd::nostream& nostream);
	nostream& nendl(nstd::nostream& nostream);

	static nostream ncout;

	class nistream : public virtual base::basic_nistream {
	public:
		nistream();
		~nistream();
	};

	static nistream ncin;
}