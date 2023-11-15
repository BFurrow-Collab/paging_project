#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h>

/**
 * Completed By: Bryce Furrow, Ricardo Harris
 * On: 11/14/2023
*/

/**
 * Process structure for maintaining addresses and names
 * By: Ricardo Harris
*/
struct Process {
	Process()
	: mem_size(0)
	, proc_name("") {}

	Process(unsigned int memory_size, std::string process_name)
	: mem_size(memory_size)
	, proc_name(process_name) {}

	unsigned int mem_size;
	std::string proc_name;
};

/**
 * Memory Block structure for maintaining size and processes
 * By: Bryce Furrow and Ricardo Harris
*/
struct MemBlock {
	MemBlock(unsigned int mem_request)
	: mem_size(mem_request) {
		mem_block = new bool[mem_size];
		for (unsigned int i = 0; i < mem_size; i++) {
			mem_block[i] = false;
		}
		std::map<unsigned int, Process> process_list;
	}

	~MemBlock() {
		delete[] mem_block;
	}

	unsigned int mem_size;
	std::map<unsigned int, Process> process_list;
	bool* mem_block;

	/**
	 * Placement function
 	 * By: Bryce Furrow and Ricardo Harris
	*/
	void place(Process proc, unsigned int pos) {
		for (unsigned int i = pos; i < proc.mem_size + pos; i++) {
			mem_block[i] = true;
		}

		process_list[pos] = proc;
	}

	/**
	 * First fit request function
 	 * By: Bryce Furrow and Ricardo Harris
	*/
	bool first_fit(unsigned int mem_request, std::string name) {
		bool placed = false;
		// Starting at beginning
		for (unsigned int i = 0; i < mem_size && !placed; i++) {
			// If we are at an empty block
			unsigned int stop_bit = i;
			if (!mem_block[i]) {
				// Measure out gap while before end and either we don't have the memory requested or we haven't reached occupied memory
				for (unsigned int j = i + 1; j <= mem_size && j - i <= mem_request && !mem_block[j]; j++) {
					stop_bit = j;
				}

				// If gap measured is large enough, place
				if (stop_bit - i >= mem_request) {
					Process proc(mem_request, name);
					place(proc, i);
					placed = true;
				}

				// Regardless, move up to stop bit
				i = stop_bit;
			}
		}

		return placed;
	}

	/**
 	 * Gap Finder function
   	 * @return a map of gaps in memory block
 	 * By: Bryce Furrow and Ricardo Harris
     	*/
	std::map<unsigned int, unsigned int> gap_finder() {
		// Return map with key = position of start of gap and val = size of gap
		std::map<unsigned int, unsigned int> retval;

		// for mem_block
		for (unsigned int i = 0; i < mem_size; i++) {
			unsigned int stop_bit = i;
			// If you find a gap
			if (!mem_block[i]) {
				// Measure out to where gap stops
				for (unsigned int j = i + 1; j <= mem_size && !mem_block[j]; j++) {
					stop_bit = j;
				}

				// Store gap size at start position
				retval[i] = stop_bit - i;

				// skip up to end of gap
				i = stop_bit;
			}
		}

		return retval;
	}

	/**
	 * Best fit request function
 	 * By: Bryce Furrow and Ricardo Harris
	*/
	bool best_fit(unsigned int mem_request, std::string name) {
		// Assume not placeable
		bool placeable = false;

		// Variables to record position and gap
		unsigned int place_pos = 0;
		unsigned int found_gap = 0;

		// Get map of gaps
		std::map<unsigned int, unsigned int> gaps = gap_finder();

		// For all gaps
		for (auto iter = gaps.begin(); iter != gaps.end() && !gaps.empty(); iter++) {
			// If a possible gap hasn't been found yet, record the first gap big enough
			if (!placeable && iter->second >= mem_request) {
				place_pos = iter->first;
				found_gap = iter->second;
				placeable = true;
			}
			// If a gap big enough has been found, record any gap able to hold process
			// and smaller than existing smallest gap
			else if (iter->second >= mem_request && iter->second < found_gap) {
				place_pos = iter->first;
				found_gap = iter->second;
			}
		}

		// If there was a gap big enough found, place
		if (placeable) {
			Process proc(mem_request, name);
			place(proc, place_pos);
		}

		return placeable;
	}

	/**
	 * Worst fit request function
 	 * By: Bryce Furrow and Ricardo Harris
	*/
	bool worst_fit(unsigned int mem_request, std::string name) {
		// Assume not placeable
		bool placeable = false;

		// Variables to record position and gap
		unsigned int place_pos = 0;
		unsigned int found_gap = 0;

		// Get map of gaps
		std::map<unsigned int, unsigned int> gaps = gap_finder();

		// For all gaps
		for (auto iter = gaps.begin(); iter != gaps.end() && !gaps.empty(); iter++) {
			// If a possible gap hasn't been found yet, record the first gap big enough
			if (!placeable && iter->second >= mem_request) {
				place_pos = iter->first;
				found_gap = iter->second;
				placeable = true;
			}
			// If a gap big enough has been found, record any gap able to hold process
			// and bigger than the existing biggest gap
			else if (iter->second >= mem_request && iter->second > found_gap) {
				place_pos = iter->first;
				found_gap = iter->second;
			}
		}

		// If there was a gap big enough found, place
		if (placeable) {
			Process proc(mem_request, name);
			place(proc, place_pos);
		}

		return placeable;
	}

	/**
	 * Release function
 	 * By: Bryce Furrow and Ricardo Harris
	*/
	bool release(const std::string process_name) {
		// Not found yet
		bool found = false;

		// Variables for assessment
		unsigned int proc_size = 0;
		unsigned int mem_addr = 0;
		auto iter = process_list.begin();

		// While not at end of list and not found
		while (iter != process_list.end() && !found) {
			// If iter name is correct, record data and end loop
			if (iter->second.proc_name == process_name) {
				mem_addr = iter->first;
				proc_size = iter->second.mem_size;
				found = true;
			} else
				iter++;
		}

		if (found) {
			// Map doesn't like when you erase a single-entry list, so clear
			if (process_list.size() == 1)
				process_list.clear();
			else
				process_list.erase(iter);

			// Reset mem chunk
			for (unsigned int i = mem_addr; i < mem_addr + proc_size; i++)
				mem_block[i] = false;
		}

		return found;
	}

	/**
	 * Compact function. I'm sure there's a better way to do it by measuring
	 * memory density to determine a central point, do an inverted search to
	 * get blocks of unused memory by distance from that central point, then
	 * move processes from outward in outward to create a contiguous chunk.
	 * 
	 * Be my guest.
 	 * By: Bryce Furrow and Ricardo Harris
	*/
	void compact() {
		// Make queue of processes to preserve order
		std::vector<Process> temp_list;
		for (auto iter = process_list.begin(); iter != process_list.cend(); iter++) {
			// Push onto queue
			temp_list.push_back(iter->second);
		}

		// Erase map
		process_list.clear();

		// Clear memory
		for (unsigned int i = 0; i < mem_size; i++) {
			mem_block[i] = false;
		}

		// Load processes using first fit
		for (auto iter = temp_list.begin(); iter != temp_list.end(); iter++) {
			bool _empty = first_fit(iter->mem_size, iter->proc_name);
		}
	}

	/**
 	 * Print function for testing
 	 * By: Bryce Furrow and Ricardo Harris
     	*/
	void print() {
		for (auto iter = process_list.begin(); iter != process_list.end(); iter++) {
			std::cout << iter->second.proc_name << " starts at position " << iter->first << " and is size " << iter->second.mem_size << '\n';
		}

		for (unsigned int i = 0; i < mem_size; i++) {
			if (mem_block[i])
				std::cout << 'X';
			else
				std::cout << '_';
			if (i < 10)
				std::cout << "  ";
			else
				std::cout << "   ";
		}
		std::cout << '\n';
		for (unsigned int i = 0; i < mem_size; i++) {
			std::cout << i << "  ";
		}
		std::cout << '\n';
	}

	/**
	 * Status Report function
 	 * By: Bryce Furrow and Ricardo Harris
	*/
	void status_report() {
		if (process_list.empty()) {
			std::cout << "Addresses [0] . . .\n";
			return;
		}

		// Create report map
		std::map<unsigned int, std::string> report;

		// Add gaps to report
		std::map<unsigned int, unsigned int> gaps = gap_finder();
		for (auto iter = gaps.begin(); iter != gaps.end() && !gaps.empty(); iter++) {
			report[iter->first] = "Unused";
		}

		// Add processes to report
		for (auto iter = process_list.begin(); iter != process_list.end() && !process_list.empty(); iter++) {
			report[iter->first] = "Process " + iter->second.proc_name;
		}

		// Process list has p1 and p2, which means report has p1 and p2

		// print report
		for (auto iter = report.begin(); iter != report.end(); iter++) {
			auto copy_iter = iter;
			copy_iter++;
			if (copy_iter != report.end())
				std::cout << "Addresses [" << iter->first << ':' << copy_iter->first - 1 << "] " << iter->second << '\n';
			else if (iter->second == "Unused")
				std::cout << "Addresses [" << iter->first << "] . . .\n";
			else
				std::cout << "Addresses [" << iter->first << ':' << mem_size - 1 << "] " << iter->second << '\n';
		}
	}
};




// DO NOT TOUCH BELOW. IT IS COMPLETE.




/**
 * Allocator function. Main program.
 * By: Ricardo Harris
*/
void allocator(MemBlock mem_block);

/**
 * Command parser.
 * @return vector of substrings (command and arguments)
 * By: Ricardo Harris
*/
std::vector<std::string> parse(std::string& input);

/**
 * Process controller to call functions based on command
 * By: Ricardo Harris
*/
void controller(const std::vector<std::string>& cmd_args, MemBlock& mem_block);

/**
 * Commands
 * By: Ricardo Harris
*/
void request(const std::vector<std::string>& cmd_args, MemBlock& mem_block);
void release(const std::vector<std::string>& cmd_args, MemBlock& mem_block);
void compact(const std::vector<std::string>& cmd_args, MemBlock& mem_block);
void status_report(const std::vector<std::string>& cmd_args, MemBlock& mem_block);

int main(int argc, char *argv[]) {
	// If too few or too many arguments
	if (argc != 2) {
		std::cout << "Improper number of arguments.\n";
		return 1;
	}

	// Try uint from arg, else err
	unsigned int mem_count;
	try {
		mem_count = std::stoi(argv[1]);
	} catch (...) {
		std::cout << "Improper argument. Argument should be a positive integer.";
		return 1;
	}

	// Make memory block, begin program
	MemBlock mem_block(mem_count);
	allocator(mem_block);

	return 0;
}

void allocator(MemBlock mem_block) {
	// Run state
	bool run = true;
	std::string input;

	// While state
	while (run) {
		std::cout << "allocator> ";

		// Grab input and parse
		getline(std::cin, input);
		std::vector<std::string> cmd_args = parse(input);

		// If quit, end, else call controller
		if (cmd_args[0] == "X" || cmd_args[0] == "x")
			run = false;
		else
			controller(cmd_args, mem_block);
	}
}

std::vector<std::string> parse(std::string& input) {
	// Split string at spaces, pushing into vector
	std::istringstream str(input);
	std::string arg;
	std::vector<std::string> cmd_args;

	// While new word
	while (str >> arg)
		cmd_args.push_back(arg);

	// Reset input
	input = "";
	return cmd_args;
}

void controller(const std::vector<std::string>& cmd_args, MemBlock& mem_block) {
	// If rq, request
	if (cmd_args[0] == "RQ" ||
		cmd_args[0] == "Rq" ||
		cmd_args[0] == "rQ" ||
		cmd_args[0] == "rq")
		request(cmd_args, mem_block);

	// Else if rl, release
	else if (cmd_args[0] == "RL" ||
		cmd_args[0] == "Rl" ||
		cmd_args[0] == "rL" ||
		cmd_args[0] == "rl")
		release(cmd_args, mem_block);

	// else if c, compact
	else if (cmd_args[0] == "C" ||
		cmd_args[0] == "c")
		compact(cmd_args, mem_block);

	// else if stat, print status report
	else if (cmd_args[0] == "STAT" ||
		cmd_args[0] == "sTAT" ||
		cmd_args[0] == "StAT" ||
		cmd_args[0] == "STaT" ||
		cmd_args[0] == "STAt" ||
		cmd_args[0] == "stAT" ||
		cmd_args[0] == "sTaT" ||
		cmd_args[0] == "StaT" ||
		cmd_args[0] == "sTAt" ||
		cmd_args[0] == "StAt" ||
		cmd_args[0] == "STat" ||
		cmd_args[0] == "staT" ||
		cmd_args[0] == "stAt" ||
		cmd_args[0] == "sTat" ||
		cmd_args[0] == "Stat" ||
		cmd_args[0] == "stat")
		status_report(cmd_args, mem_block);

	// else an improper function call
	else
		std::cout << "Improper function call. Please limit calls to:\n"
			<< "'RQ' for request\n"
			<< "'RL' for release\n"
			<< "'C' for compact\n"
			<< "'STAT' for status report\n";
}

void request(const std::vector<std::string>& cmd_args, MemBlock& mem_block) {
	// If command wrong size
	if (cmd_args.size() != 4) {
		std::cout << "Improper request call. Please format your request as:\n"
			<< "'RQ [process name] [memory size] [function call]'.\n";
		return;
	}

	// Get process name
	std::string process_name = cmd_args[1];

	// Try for process request size
	unsigned int mem_size;
	try {
		mem_size = stoi(cmd_args[2]);
	} catch (...) {
		std::cout << "Improper memory size. Please request a positive integer.\n";
		return;
	}

	// Sort function call
	bool success = false;
	if (cmd_args[3] == "F" || cmd_args[3] == "f")
		success = mem_block.first_fit(mem_size, process_name);
	else if (cmd_args[3] == "B" || cmd_args[3] == "b")
		success = mem_block.best_fit(mem_size, process_name);
	else if (cmd_args[3] == "W" || cmd_args[3] == "w")
		success = mem_block.worst_fit(mem_size, process_name);
	else {
		std::cout << "Improper request function type. Please use:\n"
			<< "'F' for first fit\n"
			<< "'B' for best fit\n"
			<< "'W' for worst fit\n";
		return;
	}

	if (success)
		std::cout << "Memory request was successful!\n";
	else
		std::cout << "Memory request was unsuccessful. Memory full.\n";
}

void release(const std::vector<std::string>& cmd_args, MemBlock& mem_block) {
	// If command wrong size
	if (cmd_args.size() != 2) {
		std::cout << "Improper release call. Please format your request as:\n"
			<< "'RL [process name]'.\n";
		return;
	}

	if (mem_block.release(cmd_args[1]))
		std::cout << "Memory released successfully!\n";
	else 
		std::cout << "Process not found in memory.\n";
}

void compact(const std::vector<std::string>& cmd_args, MemBlock& mem_block) {
	// If command wrong size
	if (cmd_args.size() != 1) {
		std::cout << "Improper compact call. Please just call 'C'.\n";
		return;
	}

	mem_block.compact();
}
void status_report(const std::vector<std::string>& cmd_args, MemBlock& mem_block) {
	// If command wrong size
	if (cmd_args.size() != 1) {
		std::cout << "Improper status report call. Please just call 'STAT'.\n";
		return;
	}

	mem_block.status_report();
}
