#ifndef INTT_EVENT_POOL_H
#define INTT_EVENT_POOL_H

#include <fun4allraw/intt_pool.h>
#include <Event/fileEventiterator.h>

#include <limits>
#include <map>
#include <string>
#include <vector>

class Packet;
class intt_pool;
class fileEventiterator;

class intt_event_pool {
public:
	intt_event_pool();
	virtual ~intt_event_pool();

	int open(std::string const&);
	int open_list(std::string const&);

	int verbosity() {return m_verbosity;}
	int verbosity(int const& v) {return m_verbosity = v;}

	int events_per_cout() {return m_evt_per_cout;}
	int events_per_cout(int const& v) {return m_evt_per_cout = 0 < v ? v : 1;}

	int next();

	virtual int set_output_file(std::string const&) {return EXIT_SUCCESS;}
	virtual int write_output_file() {return EXIT_SUCCESS;}
	virtual int analyze(Packet*) {return EXIT_SUCCESS;}
	virtual int analyze() {return EXIT_SUCCESS;}

protected:
	int num_evts() {return m_evts;}
	int print_event() {return (1 < m_verbosity && !(m_evts % m_evt_per_cout));}
	// int iValue(int const& i, std::string const& s) {return m_pool->iValue(i, s.c_str());}
	// unsigned long long lValue(int const& i, std::string const& s) {return m_pool->lValue(i, s.c_str());}

private:
	int open_next();

	int m_verbosity{0};
	int m_evt_per_cout{0};
	int m_evts{0};

	std::string m_current_file;
	std::vector<std::string> m_files_to_read;

	fileEventiterator* m_file_evt_itr{nullptr};
	// intt_pool* m_pool{nullptr};
};

#endif//INTT_EVENT_POOL_H
