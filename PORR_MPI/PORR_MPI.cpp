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

std::map<std::string, int> vec_to_map(std::vector<std::pair<std::string, int>> vec) {
	std::map<std::string, int> map;
	for (auto it = vec.begin(); it != vec.end(); ++it)
		map[it->first] = it->second;
	return map;
}
int main(int argc, char **argv) {
	const int lentag = 0;
	const int datatag = 1;
	const int sizetag = 2;
	const int lentag2 = 3;
	const int datatag2 = 4;
	const int sizetag2 = 5;

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
		int len;
		int vec_size;
		Text base_text = Text("C:\\Users\\Lenovo\\source\\repos\\PORR_MPI\\x64\\Debug\\6_image.txt");
		std::vector<std::pair<std::string, int>>  count_vector= controller.repeats(base_text.get_list_of_words_from_text());
	
		std::string serial_str;
		boost::iostreams::back_insert_device<std::string> inserter(serial_str);
		boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
		boost::archive::binary_oarchive send_ar(s);
		send_ar << count_vector;
		s.flush();
		len = serial_str.size();
		vec_size = count_vector.size();

		MPI_Send(&len, 1, MPI_INT, 0, lentag, MPI_COMM_WORLD);
		MPI_Send(&vec_size, 1, MPI_INT, 0, sizetag, MPI_COMM_WORLD);
		MPI_Send((void *)serial_str.data(), len, MPI_BYTE, 0, datatag, MPI_COMM_WORLD);
	}
	if (mynum == 2) {
		int len;
		int vec_size;
		Text examined_text = Text("C:\\Users\\Lenovo\\source\\repos\\PORR_MPI\\x64\\Debug\\2_image.txt");
		std::vector<std::pair<std::string, int>>  count_vector = controller.repeats(examined_text.get_list_of_words_from_text());

		std::string serial_str;
		boost::iostreams::back_insert_device<std::string> inserter(serial_str);
		boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
		boost::archive::binary_oarchive send_ar(s);
		send_ar << count_vector;
		s.flush();
		len = serial_str.size();
		vec_size = count_vector.size();

		MPI_Send(&len, 1, MPI_INT, 0, lentag2, MPI_COMM_WORLD);
		MPI_Send(&vec_size, 1, MPI_INT, 0, sizetag2, MPI_COMM_WORLD);
		MPI_Send((void *)serial_str.data(), len, MPI_BYTE, 0, datatag2, MPI_COMM_WORLD);
	}
	if (mynum == 0) {
		int len_BT, len_ET;
		const int clen = 819; //dla obu 819 ponieważ data wymaga const - nie mozna jej przekzywac przez MPI_Send
		char data_BT[clen + 1];
		char data_ET[clen + 1];
		int  vec_size_BT, vec_size_ET;

		//**  BASE_TEXT  **//
		MPI_Recv(&len_BT, 1, MPI_INT, 1, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&vec_size_BT, 1, MPI_INT, 1, sizetag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		std::vector<std::pair<std::string, int>>  count_vector_BT;
		count_vector_BT.resize(vec_size_BT);

		MPI_Recv(data_BT, len_BT,MPI_BYTE,1,datatag,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		data_BT[clen] = '\0';
		
		boost::iostreams::basic_array_source<char> device(data_BT, clen);
		boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
		boost::archive::binary_iarchive recv_ar(s);
		recv_ar >> count_vector_BT;

		//** EXAMINED_TEXT **//
		MPI_Recv(&len_ET, 1, MPI_INT, 2, lentag2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&vec_size_ET, 1, MPI_INT, 2, sizetag2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		std::vector<std::pair<std::string, int>>  count_vector_ET;
		count_vector_ET.resize(vec_size_ET);

		MPI_Recv(data_ET, len_ET, MPI_BYTE, 2, datatag2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		data_ET[clen] = '\0';

		boost::iostreams::basic_array_source<char> device2(data_ET, clen);
		boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s2(device2);
		boost::archive::binary_iarchive recv_arE(s2);
		recv_arE >> count_vector_ET;

		//for (auto it = count_vector_ET.begin(); it != count_vector_ET.end(); ++it) {
		//		std::cout << it->first << " " << it->second << std::endl;}

		//** SEQUENTIAL PART **/
		int sum;
		sum=controller.sum_repeats( controller.minimum(vec_to_map(count_vector_BT), vec_to_map(count_vector_ET)));
		std::cout << sum << std::endl;
	}

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
		
	std::cout << mynum<< "\n";
	std::cout << nproc << "\n";
	std::cout << "czas: " << elapsed_seconds.count() << "s\n";
	MPI_Finalize();
	return 0;
}
