#ifndef TIME_TYPES_H
#define TIME_TYPES_H

#include <chrono>
#include <thread>

namespace radar
{
	/// Types
	// Define clock
	using employed_system_clock = std::chrono::high_resolution_clock;
	// Time point type
	using time_point_t = std::chrono::time_point<employed_system_clock>;
	using time_point_us_t = uint64_t;
	using time_point_ns_t = uint64_t;
	// Duration types
	using duration_us_t = std::chrono::duration<double, std::micro>;
	// Chrono time types for time calculations
	using seconds_t = std::chrono::seconds;
	using milliseconds_t = std::chrono::milliseconds;
	using microseconds_t = std::chrono::microseconds;
	using nanoseconds_t = std::chrono::nanoseconds;

	/// Functions
	// Get system time
	inline time_point_t get_system_time() {return employed_system_clock::now();}
	// Get duration
	inline time_point_us_t get_duration_us(time_point_t start_time, time_point_t finish_time) {return static_cast<time_point_us_t>(duration_us_t(finish_time - start_time).count());}
	inline time_point_us_t get_elapsed_time_us(time_point_t start_time) {return static_cast<time_point_us_t>(duration_us_t(get_system_time() - start_time).count());}
	// Get only seconds and nanoseconds from time point to construct object like time_spec
	inline uint32_t extract_time_point_sec(time_point_t & tp) {return floor(std::chrono::duration_cast<seconds_t>(tp.time_since_epoch()).count());}
	inline uint32_t extract_time_point_nsec(time_point_t & tp) {return std::chrono::duration_cast<nanoseconds_t>(tp.time_since_epoch()).count() % 1000000000ULL;}
	// Time_point_us_t <-> time_point_t conversions
	inline time_point_us_t chrono2us(time_point_t time_point) {return std::chrono::duration_cast<microseconds_t>(time_point.time_since_epoch()).count();}
	inline time_point_ns_t chrono2ns(time_point_t time_point) {return std::chrono::duration_cast<nanoseconds_t>(time_point.time_since_epoch()).count();}
	inline time_point_t us2chrono(time_point_us_t time_point_us) {return time_point_t(microseconds_t(time_point_us));}
	// Tread sleeps
	inline void thread_sleep_ns(int delay_ns) {std::this_thread::sleep_for(nanoseconds_t(delay_ns));}
	inline void thread_sleep_us(int delay_us) {std::this_thread::sleep_for(microseconds_t(delay_us));}
	inline void thread_sleep_ms(int delay_ms) {std::this_thread::sleep_for(milliseconds_t(delay_ms));}
	inline void thread_sleep_sec(int delay_sec) {std::this_thread::sleep_for(seconds_t(delay_sec));}

	/** \brief Class TicToc
	*/
	class TicToc
	{
		static const int MAX_LABELS = 100;
	public:

		TicToc(int max_labels = MAX_LABELS) 
		{
			toc_times.resize(max_labels);
			toc_labels.resize(max_labels);
			toc_index = 0;
		}

		~TicToc() {}

		void tic() {tic_time = get_system_time();}
		time_point_ns_t toc() {return chrono2ns(get_system_time()) - chrono2ns(tic_time);}
		void toc(std::string label_str) 
		{
			toc_times.at(toc_index) = (chrono2ns(get_system_time()) - chrono2ns(tic_time));
			toc_labels.at(toc_index) = (label_str);
			toc_index++;
		}
		void print_toc()
		{
			for (unsigned int i = 0; i < toc_index; i++) {
				std::cout << toc_labels.at(i) << ": " << toc_times.at(i) << " ns" << std::endl;
			}
		}

	protected:

		time_point_t tic_time;
		unsigned int toc_index;
		//std::vector<time_point_t> tic_times;
		std::vector<time_point_ns_t> toc_times;
		std::vector<std::string> toc_labels;

	};


} // namespace radar

#endif // TIME_TYPES_H
