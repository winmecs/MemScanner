#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>

#define maps_template "/proc/%ld/maps"
#define mem_tempplate "/proc/%ld/mem"

void print_usage()
{
	std::cout<<"Usage: sudo ./main PID\n where:\n -PID is the pid of the target process"<<std::endl;
	return;
}

int main(int argc, char const *argv[])
{
	long int pid;
	if (argc != 2){
		print_usage();
		return 0;
	}
	pid = std::stoi(argv[1]);
	if (pid<0){
		print_usage();
		return 0;
	}

	std::cout<<"Please enter the string you want to search"<<std::endl;
	std::string search_string;
	getline(std::cin,search_string);

	while(search_string.compare("")==0){
	    std::cout<<"Please entet a valid string"<<std::endl;
		getline(std::cin,search_string);	     
	} 

	std::cout<<"Please enter the new string you want to get"<<std::endl;
	std::string new_string;
	getline(std::cin,new_string);

	//open the maps and mem files of the process

	char *maps_filename =(char*)malloc(std::strlen(argv[1]) + std::strlen(maps_template));
	char *mem_filename = (char*)malloc(std::strlen(argv[1]) + std::strlen(mem_tempplate));

	snprintf(maps_filename,std::strlen(argv[1]) + std::strlen(maps_template),maps_template,pid);
	snprintf(mem_filename,std::strlen(argv[1]) + std::strlen(maps_template),mem_tempplate,pid);

	std::cout<<"maps file: "<<maps_filename<<std::endl;
	std::cout<<"mem file: "<<mem_filename<<std::endl;

	// open maps file
	std::ifstream maps_file;
	maps_file.open(maps_filename,std::ios::in);
	if(!maps_file){
		std::cout<<"Can not open maps file"<<std::endl;
		return 0;
	}

	std::string addr, perm, offset, device, inode;

	bool has_heap = false;
	// find the heap
	for (std::string maps_line; getline(maps_file,maps_line);){
		if (maps_line.find("[heap]") != std::string::npos){
			has_heap = true;
			std::cout << "find heap:" << std::endl << maps_line << std::endl;
			std::istringstream iss(maps_line);
			iss >> addr >> perm >> offset >> device >> inode;
		}
	}
	if(!has_heap){
		std::cout << maps_filename << " does not have heap" << std::endl;
		maps_file.close();
		return 0;
	}

	std::cout << "addresses: " << addr << std::endl << "persimission: "<< perm << std::endl << "offset: " << offset << std::endl;

	//check if there is read and write permission
	if (perm[0] != 'r' || perm [1] != 'w')
	{
		std::cout << maps_filename << " does not have read/write permission" << std::endl;
		maps_file.close();
		return 0;
	}

	std::size_t split_pos = addr.find("-");

	if (split_pos == std::string::npos || split_pos == addr.size() - 1)
	{
		std::cout << "Wrong addr format" << std::endl;
		maps_file.close();
		return 0;
	}

	// get the start and end address 
	// std::string start_addr, end_addr;
	unsigned long long start_addr, end_addr, length;
	
	// start_addr.append(addr, 0, split_pos);
	start_addr = std::stoull(addr.substr(0,split_pos), nullptr, 16);
	// end_addr.append(addr, split_pos+1, std::string::npos);
	end_addr = std::stoull(addr.substr(split_pos+1, std::string::npos), nullptr, 16);

	// std::cout << "Start addr: " << start_addr << "\tend_addr: " << end_addr << std::endl;

	//open mem file
	std::fstream mem_file;
	mem_file.open(mem_filename,std::ios::in | std::ios::out | std::ios::binary);
	if(!mem_file){
		std::cout<<"Can not open mem file"<<std::endl;
		maps_file.close();
		return 0;
	}

	//read heap 
	mem_file.seekg(start_addr, mem_file.beg);
	length = end_addr - start_addr;

	std::string heap;
	heap.reserve(length);
	while(length-- > 0) {
	    heap += mem_file.get();
	}

	// find the search string
	std::size_t string_pos  = heap.find(search_string);
	if (string_pos == std::string::npos)
	{
		std::cout << "Can not find the string" << std::endl;
		maps_file.close();
		mem_file.close();
		return 0;
	}
	std::cout << "Found " << search_string << " at 0x" << std::hex << string_pos + start_addr << std::endl;

	// write the new string
	mem_file.seekp(start_addr + string_pos);
	mem_file << new_string;

	// write empty char
	mem_file.write("\x00", 1);

	std::cout << "Complete!" << std::endl;

	// close files
    maps_file.close();
    mem_file.close();	

	return 0;
}