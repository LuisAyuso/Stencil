
/**
 * Copyright (c) 2002-2013 Distributed and Parallel Systems Group,
 *                Institute of Computer Science,
 *               University of Innsbruck, Austria
 *
 * This file is part of the INSIEME Compiler and Runtime System.
 *
 * We provide the software of this file (below described as "INSIEME")
 * under GPL Version 3.0 on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 *
 * If you require different license terms for your intended use of the
 * software, e.g. for proprietary commercial or industrial use, please
 * contact us at:
 *                   insieme@dps.uibk.ac.at
 *
 * We kindly ask you to acknowledge the use of this software in any
 * publication or other disclosure of results by referring to the
 * following citation:
 *
 * H. Jordan, P. Thoman, J. Durillo, S. Pellegrini, P. Gschwandtner,
 * T. Fahringer, H. Moritsch. A Multi-Objective Auto-Tuning Framework
 * for Parallel Codes, in Proc. of the Intl. Conference for High
 * Performance Computing, Networking, Storage and Analysis (SC 2012),
 * IEEE Computer Society Press, Nov. 2012, Salt Lake City, USA.
 *
 * All copyright notices must be kept intact.
 *
 * INSIEME depends on several third party software packages. Please 
 * refer to http://www.dps.uibk.ac.at/insieme/license.html for details 
 * regarding third party software licenses.
 */

#pragma once






// with c++11 suport
#if __cplusplus > 199711L

#include <iostream>
#include <sstream>  
#include <fstream>
#include <vector>
#include <mutex>
#include <chrono>
#include <thread>
#include <map>
#include <iomanip>

#define STOPWATCH(name)\
	auto swt = uibk::StopWatch::start(name); 

namespace uibk{

using namespace std;

typedef chrono::high_resolution_clock   clock;
typedef	clock::time_point tTime;
typedef	tTime::duration tDuration;

/**
 * high preccission timer
 */
inline tTime getTime(){
	return clock::now();
}

/**
 * retrieve the thread that started this stopwatch. since the stopwatches are scoped
 * it should not be posible to transfer it to another thread.
 * the std:: thread id operation should be low level enough to deal with different kind
 * of shared memory programing models
 */
inline thread::id getThid(){
	return this_thread::get_id();
}

namespace {

	int fix_pid(const thread::id& thid){

		static std::map<thread::id, int> translation;
		static int count = 0;

		if(translation.find(thid) == translation.end()){
			translation[thid] = count++;
		}

		return translation[thid];
	}
}

/**
 * stopwatch class, is a singleton which allows to start and stop timers, 
 * since is centraliced in a singleton is thread safe and can store the whole program status
 */
class StopWatch{
	class swImpl;

	public:
	////////////////////////////////
	//
	class swTicket {
		StopWatch::swImpl& sw;
	public:
		mutable bool finished;
		unsigned id;

		swTicket(StopWatch::swImpl& stopWatch, int num)
		:sw(stopWatch), finished(false), id(num)
		{ }

		swTicket(const swTicket& o) =delete;

		swTicket(swTicket&& o)
		:sw(o.sw), finished(false), id(o.id)
		{
			o.finished=true;
		}

		void end(){
			if (!finished){
				sw.endTiket(*this);
				finished = true;
			}
		}

		~swTicket(){
			if (!finished){
				sw.endTiket(*this);
				finished = true;
			}
		}
	};

	private:

	class swImpl{
		
		class tRecord{
		public:
			thread::id thid;
			string name;

			tTime start;
			tTime end;

			bool updated;

			tRecord(thread::id pid, const string& n, tTime t)
			: thid(pid), name(n), start(t), updated (false)
			{ }

			void update(tTime t){
				end = t;
				assert(end > start);
				updated=true;
			}

			void dump(ostream& os, tTime globalStart)const{
				const double st = chrono::duration_cast<std::chrono::microseconds>(start-globalStart).count();
				const double nd = chrono::duration_cast<std::chrono::microseconds>(end-globalStart).count();

				assert(updated);

				os << fix_pid(thid) << "\t" << std::fixed << std::setprecision(2) << st << 
								   	   "\t" << std::fixed << std::setprecision(2) << nd << 
									   "\t" << name << endl;
			}
		};

		vector<tRecord> 	 measures;
		mutex staticLock;
		tTime  globalStart;

		public:

		swImpl()
		:	globalStart (getTime())
		{
			tRecord record(getThid(), "GlobalScope", globalStart);
			measures.insert(measures.end(), record);
		}

		~swImpl(){
			// update global scope record
			measures[0].update(getTime());
			// print status to file
			ofstream myfile;
			myfile.open ("times.sw");
			dump(myfile);
			myfile.close();
		}

		swTicket getTicket(const string& str){
			tRecord record(getThid(), str, getTime());
			staticLock.lock();
			measures.insert(measures.end(), record);
			unsigned id = measures.size()-1;
			staticLock.unlock();
			return swTicket(*this, id);
		}

		void  	 endTiket(const swTicket& swT){
			const auto x = getTime();
			staticLock.lock();
			measures[swT.id].update(x);
			staticLock.unlock();
		}

		void 	 dump(ostream& os){
			os << "pid\tstart\tend\tkind\torder" << endl;
			for (const tRecord& r : measures){
				r.dump(os, globalStart);
			}
		}

		friend class swTicket;
	} sw;


	public:

	/**
	 * generates a new stopwatch point with a given name. 
	 */
	static swTicket start(const std::string& str){
		return getInstance().sw.getTicket(str);
	}

	/**
	 * allow to compose formated names, to parametrize the string 
	 */
	template <typename T, typename... Args>
	static swTicket start(const std::string& str, const T& value, const Args&... args){
		std::stringstream ss;
		ss << str << value;
		return StopWatch::start(ss.str(), args...);
	}
	/** 
	 * stops a stopwatch ticket
	 */
	static void 	stop (swTicket& swTicket){
		swTicket.end();
	}

	/**
	 * prints the current status of all tracked stopwatches, 
	 * any non finished timer will show random end time and 0 duration
	 */
	static void printStatus(ostream& os = std::cout){
		os << "=============== STOPWATCH status =======================" << std::endl;
		getInstance().sw.dump(os);
		os << "========================================================" << std::endl;
	}

	// singleton stuff
	private:
	static StopWatch& getInstance(){
		static StopWatch singleton;
		return singleton;
	}

	StopWatch(){};
	~StopWatch(){};
};

} // uibk namespace
#else


#define STOPWATCH(name)\
	; 

#endif


