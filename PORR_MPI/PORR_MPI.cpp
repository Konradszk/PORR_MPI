// PORR_MPI.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>
#include <chrono>
#include "Controller.h"
#include "mpi.h"
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
int main(int argc, char **argv) {
	const int lentag = 0;
	const int datatag = 1;
	const int sizetag = 2;

	int mynum, nproc, source;
	auto start = std::chrono::system_clock::now();
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mynum);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	if (nproc != 3) {
		std::cerr << " wrong number of proces " + nproc << std::endl;
		return 1;
	}
	Text words_to_check_text = Text("C:\\Users\\Lenovo\\source\\repos\\PORR_MPI\\x64\\Debug\\check_image_recognition.txt");
	Controller controller = Controller(words_to_check_text);
	source = 0;
	
	
	//MPI_Bcast(&controller, nproc, MPI_BYTE, source, MPI_COMM_WORLD);
	if (mynum == 1) {
		Text base_text = Text("C:\\Users\\Lenovo\\source\\repos\\PORR_MPI\\x64\\Debug\\6_image.txt");
		std::vector<std::pair<std::string, int>>  count_vector= controller.repeats(base_text.get_list_of_words_from_text());
		//for (auto it = count_vector.begin(); it != count_vector.end(); ++it) {
		//	std::cout << it->first << " " << it->second << std::endl;}
		

		std::string serial_str;
		boost::iostreams::back_insert_device<std::string> inserter(serial_str);
		boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
		boost::archive::binary_oarchive send_ar(s);
		send_ar << count_vector;
		s.flush();
		int len = serial_str.size();
		int vec_size = count_vector.size();

		MPI_Send(&len, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD);
		MPI_Send(&vec_size, 1, MPI_INT, 0, sizetag, MPI_COMM_WORLD);

		MPI_Send((void *)serial_str.data(), len, MPI_BYTE, 0, datatag, MPI_COMM_WORLD);
	}
	if (mynum == 2) {
		Text examined_text = Text("C:\\Users\\Lenovo\\source\\repos\\PORR_MPI\\x64\\Debug\\2_image.txt");
		//controller.repeats(base_text.get_list_of_words_from_text());
	}
	if (mynum == 0) {
		int len;
		const int clen = 819;
		char data[clen + 1];
		int  vec_size;

		MPI_Recv(&len, 1, MPI_INT, 1, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&vec_size, 1, MPI_INT, 1, sizetag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		std::vector<std::pair<std::string, int>>  count_vector;
		count_vector.resize(vec_size);
		
		
		MPI_Recv(data,len,MPI_BYTE,1,datatag,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		data[clen] = '\0';
		
		boost::iostreams::basic_array_source<char> device(data, clen);
		boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
		boost::archive::binary_iarchive recv_ar(s);

		recv_ar >> count_vector;

		for (auto it = count_vector.begin(); it != count_vector.end(); ++it) {
				std::cout << it->first << " " << it->second << std::endl;}
	}
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
		
	std::cout << mynum<< "\n";
	std::cout << nproc << "\n";
	std::cout << "czas: " << elapsed_seconds.count() << "s\n";
	MPI_Finalize();
	return 0;
}