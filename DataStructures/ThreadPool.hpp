#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <thread>
#include <chrono>
#include <cstdint>
#include <atomic>
#include <utility>
#include <vector>
#include <functional>
#include <mutex>


namespace KozyLibrary {


class ThreadPool {
private:
	struct WorkerThread;
	friend struct WorkerThread;

	using threadCntT = uint_fast16_t; // only use unsigned types
	inline static constexpr threadCntT threadCntT_MAX_VALUE = threadCntT(-2);

public:
	using voidFunc = std::function<void()>;


	/*

	*/
	ThreadPool()
    {

    }

	// TODO : add more constuctors : initializer_list with work, rvalue vector, etc.

	/*

	*/
	~ThreadPool() {
		stop();
		wait_untilStopped();
    }

	/*
		UB if it is already or still running!
		Invalid value for workerCnt gets corrected.
	*/
	void start(threadCntT workerCnt = recommended_ThreadsCnt) {
		if (workerCnt > MAX_THREADS){
			workerCnt = MAX_THREADS;
		} else if (workerCnt == 0){
			workerCnt = 1;
		}
		
		worker.clear();
		worker.reserve(workerCnt);

		while (workerCnt-- != 0){
			worker.emplace_back(WorkerThread(*this));
		}

		worker.shrink_to_fit();
		requestTerminate = false;
		isPaused = false;
		pauseCounter = 0;

		workPivot = activeWL->data();
		workEndPivot = workPivot + activeWL->size();


		for (auto& e : worker){
			e.start();
		}
		
	}

	/*
		use this if you want to explicitly terminate the ThreadPool.
		all work until now will be processed.
		UNDEFINED BEHAVIOR for any work added after this call.
		Best Practice: Expect no work after this call to be processed.

		if you want to restart, use the method restart() instead,
		so that there are no data races.
	*/
	void stop() {
		std::thread finisherThread(stopFinisher, this);
		finisherThread.detach();
	}

	

	/*
		the calling thread waits until this ThreadPool is stopped.
		May lead to an infinite loop, if the method stop() is never called at some point.
	*/
	void wait_untilStopped() const {
		while (is_running()){
			pausingWork_default();
		}
	}

	/*
		calling thread waits until all work is finished, then restarts safely. 
	*/
	void restart(threadCntT workerCnt = recommended_ThreadsCnt) {
		stop();
		wait_untilStopped();
		start(workerCnt);
	}

	/*
		returns true, if start() was succesfully executed.
		It does not matter whether the ThreadPool is paused or not.

	*/
	bool is_running() const {
		return worker.size() != 0;
	}

	/*
		pauses all workers as soon as possible.
	*/
	void pause() const {
		pauseCounter = static_cast<threadCntT>(worker.size());
	}

	/*

	*/
	void unpause() const {
		isPaused = false;
	}

	/*
		returns true if the ThreadPool is running and not paused.
		always returns false if this ThreadPool is not running.
	*/
	bool is_paused() const {
		return isPaused;
	}

	/*
		the calling thread waits until this ThreadPool is paused.
		May lead to an infinite loop, if the method pause() is never called at some point.
	*/
	inline void wait_untilPaused() const {
		while (!is_paused()){
			pausingWork_default();
		}
	}

	/*
		Represents how many concurrent threads the current machine can handle according to OS. 
		2^16 is a hard limit. 
	*/
	inline static const threadCntT MAX_THREADS { static_cast<threadCntT>((std::thread::hardware_concurrency() >= threadCntT_MAX_VALUE)? threadCntT_MAX_VALUE: std::thread::hardware_concurrency()) };
	/*
		A recommendation to how many threads should be used if there is only one ThreadPool instance being used.
	*/
	inline static const threadCntT recommended_ThreadsCnt{ static_cast<threadCntT>((MAX_THREADS <= 3)? 1 : MAX_THREADS - 2) };

	/*

	*/
	threadCntT get_workerCnt() const noexcept {
		return static_cast<threadCntT>(worker.size());
	}

	/*

	*/
	void add_Workload(voidFunc fn) {
		swapGuard.lock();
		sleepingWL->emplace_back(std::move(fn));
		swapGuard.unlock();
	}


	inline static constexpr auto pausingWork_default = []()->void {
		std::this_thread::sleep_for(std::chrono::microseconds(1));	
	};

	/*
		restarts the threadpool and then changes pausingWork function.
		this might take some time, if the Threadpool is running.
			
	*/
	template<typename voidFuncT>
	void set_pausingWork(voidFuncT&& fn = ThreadPool::pausingWork_default) {
		pausingWork = std::forward<voidFuncT>(fn);
		restart(worker.size());
	}

private:

	/*
		pauses the workers and when they are synchronized, as in all paused, lets them terminate
	*/
	static void stopFinisher(ThreadPool* poolPtr) {
		ThreadPool& pool = *poolPtr;
		
		pool.pause();
		pool.requestTerminate = true;
		pool.wait_untilPaused();
		pool.unpause();

		pool.worker.clear();
	}


	struct WorkerThread {

		template<typename voidFuncT>
		WorkerThread(
			ThreadPool& arg_threadpool,
			std::thread&& arg_executionThread, 
			voidFuncT&& arg_pausingWorkCopy
		):
			threadpool(arg_threadpool),
			executionThread(std::move(arg_executionThread)),
			pausingWorkCopy(std::forward<voidFuncT>(arg_pausingWorkCopy))
		{

		}

		WorkerThread(ThreadPool& pool): WorkerThread(pool, std::thread{}, pool.pausingWork)
		{

		}

		WorkerThread(const WorkerThread& cpy): WorkerThread(cpy.threadpool, std::thread{}, cpy.pausingWorkCopy)
		{

		}

		WorkerThread(WorkerThread&& mv): WorkerThread(mv.threadpool, std::move(mv.executionThread), std::move(mv.pausingWorkCopy))
		{

		}

		~WorkerThread() {
			if (executionThread.joinable()){ 
				executionThread.join();
			}
		}

		/*

		*/
		void start() {
			executionThread = std::move(std::thread(work, this));
			
		}

		inline void stop() {
			threadpool.stop();
		}

		inline void pause() {
			threadpool.pause();
		}

		inline void unpause() {
			threadpool.unpause();
		}

		/*
			needs to be paused, else UB
		*//*
		template<typename voidFuncT>
		void set_pausingWork(voidFuncT&& fn = ThreadPool::pausingWork_default) {
			pausingWorkCopy = std::forward<voidFuncT>(fn);
		}
*/
		/*

		*/
		static void work(WorkerThread* worker) {
			WorkerThread& wref = *worker;
			ThreadPool& pool = wref.threadpool;
			static std::mutex workerGuard{};
			threadCntT pauseID = 0;

			while (!pool.requestTerminate){
				while (pool.pauseCounter == 0){

					workerGuard.lock();	
					if (pool.workPivot == pool.workEndPivot){
						workerGuard.unlock();

						if (pool.sleepingWL->empty() && pool.pauseCounter == 0){
							wref.pausingWorkCopy();
						} else {
							static std::atomic<threadCntT> workerCounter {0};
							if (++workerCounter == 1){
								pool.activeWL->clear();

								pool.swapGuard.lock();
								std::swap(pool.activeWL, pool.sleepingWL);
								pool.swapGuard.unlock();

								workerGuard.lock();
								pool.workPivot = pool.activeWL->data();
								pool.workEndPivot = pool.workPivot + pool.activeWL->size();
								workerGuard.unlock();
								workerCounter = 0;
							}
						}

					} else {
						voidFunc workValue = std::move(*pool.workPivot++);
						workerGuard.unlock();
						
						workValue();
					}
					
				}

				pauseID = --pool.pauseCounter;
				if (pauseID == 0){
					pool.isPaused = true;
				} else {
					while (pool.pauseCounter != 0){
						wref.pausingWorkCopy();
					}
				}

				while (pool.isPaused){
					wref.pausingWorkCopy();
				}
				

			}

			if (pauseID == 0){ // one thread finishes remaining work
				
				for (auto workPtr = pool.workPivot; pool.workPivot != pool.workEndPivot; workPtr = ++pool.workPivot){ // processing active work queue
					(*workPtr)();
				}
				pool.activeWL->clear();

				pool.swapGuard.lock();
				for (auto& e : (*pool.sleepingWL)){ // processing most recent work. ANY WORK ADDED AFTER IS IGNORED 
					e();
				}
				pool.sleepingWL->clear();
				pool.swapGuard.unlock();


			} 
			
		}

		bool operator==(const WorkerThread& rhs) const {
			return this == &rhs;
		}
		inline bool operator!=(const WorkerThread& rhs) const {
			return !(*this == rhs);
		}
		
		bool operator==(const std::thread& rhs) const {
			return executionThread.get_id() == rhs.get_id();
		}
		inline bool operator!=(const std::thread& rhs) const {
			return !(*this == rhs);
		}

		std::thread executionThread;
		voidFunc pausingWorkCopy;
		ThreadPool& threadpool;
	};

	std::vector<WorkerThread> worker{};

	std::vector<voidFunc> workload[2]{};
	std::vector<voidFunc> * activeWL{workload}, * sleepingWL{workload +1};
	voidFunc* workPivot{nullptr}, * workEndPivot{nullptr};

	voidFunc pausingWork {pausingWork_default};

	std::mutex swapGuard{};

	mutable std::atomic<uint_fast32_t> pauseCounter{0};
	mutable std::atomic<bool> isPaused{false};
	std::atomic<bool> requestTerminate{false};

};


}

#endif