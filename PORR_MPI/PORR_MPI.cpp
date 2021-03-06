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
#define ROOT 0
#define BT 1
#define ET 2

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
	const int nametag = 6;
	const int nametag2 = 7;
	const int name_lentag = 8;
	const int word_counttag = 9;
	const int word_counttag2 = 10;
	int mynum, nproc;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &mynum);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	if (nproc != 3) {
		std::cerr << " wrong number of proces " + nproc << std::endl;
		return 1;
	}
	Text words_to_check_text = Text("check_image_recognition.txt");
	Controller controller = Controller(words_to_check_text);
	
	
	
	if (mynum == BT) {
		int vec_len, str_len, word_count;
		int vec_size;
		std::string path_BT;
		char buf[]="";

		//** RECV **//
		MPI_Status status;
		MPI_Probe(ROOT, nametag, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_CHAR, &str_len);
		MPI_Recv(buf, 100 , MPI_CHAR, ROOT, nametag, MPI_COMM_WORLD,&status);
		path_BT = std::string(buf, str_len);

		//** COMPUTING **//
		Text base_text = Text(path_BT);
		std::vector<std::pair<std::string, int>> count_vector= controller.repeats(base_text.get_list_of_words_from_text());
		word_count = base_text.get_list_of_words_from_text().size();

		//** SERIALIZE **//
		std::string serial_str;
		boost::iostreams::back_insert_device<std::string> inserter(serial_str);
		boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
		boost::archive::binary_oarchive send_ar(s);
		send_ar << count_vector;
		s.flush();
		vec_len = serial_str.size();
		vec_size = count_vector.size();

		//** SEND **//
		MPI_Send(&vec_len, 1, MPI_INT, ROOT, lentag, MPI_COMM_WORLD);
		MPI_Send(&vec_size, 1, MPI_INT, ROOT, sizetag, MPI_COMM_WORLD);
		MPI_Send((void *)serial_str.data(), vec_len, MPI_BYTE, ROOT, datatag, MPI_COMM_WORLD);
		MPI_Send(&word_count, 1, MPI_INT, ROOT, word_counttag, MPI_COMM_WORLD);

	}
	if (mynum == ET) {
		int vec_len, str_len,path_len,word_count;
		int vec_size;
		std::string path_ET;
		char buf[] = "";

		/*MPI_Status status2;
		MPI_Probe(ROOT, nametag2, MPI_COMM_WORLD, &status2);
		MPI_Get_count(&status2, MPI_CHAR, &str_len);*/

		//** RECV **//		
		MPI_Recv(&path_len, 1, MPI_INT, ROOT, name_lentag, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		MPI_Recv(buf, 100, MPI_CHAR, ROOT, nametag2, MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
		path_ET = std::string(buf, path_len);

		//** COMPUTING **//
		Text examined_text = Text(path_ET);
		std::vector<std::pair<std::string, int>>  count_vector = controller.repeats(examined_text.get_list_of_words_from_text());
		word_count = examined_text.get_list_of_words_from_text().size();

		//** SERIALIZE **//
		std::string serial_str;
		boost::iostreams::back_insert_device<std::string> inserter(serial_str);
		boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
		boost::archive::binary_oarchive send_ar(s);
		send_ar << count_vector;
		s.flush();
		vec_len = serial_str.size();
		vec_size = count_vector.size();

		//** SEND **//
		MPI_Send(&vec_len, 1, MPI_INT, ROOT, lentag2, MPI_COMM_WORLD);
		MPI_Send(&vec_size, 1, MPI_INT, ROOT, sizetag2, MPI_COMM_WORLD);
		MPI_Send((void *)serial_str.data(), vec_len, MPI_BYTE, ROOT, datatag2, MPI_COMM_WORLD);
		MPI_Send(&word_count, 1, MPI_INT, ROOT, word_counttag2, MPI_COMM_WORLD);
	}
	if (mynum == ROOT) {
		auto start = std::chrono::system_clock::now();

		int len_BT, len_ET,words_BT, words_ET;
		char *data_BT;
		char *data_ET;
		int  vec_size_BT, vec_size_ET;

		//**  SEND PATH  **//
		std::string path_BT("6_image.txt");
		std::string path_ET("2_image.txt");
		int path_size_ET = path_ET.size();
		
		MPI_Send(path_BT.c_str(), path_BT.size(), MPI_CHAR, BT, nametag, MPI_COMM_WORLD);
		MPI_Send(&path_size_ET, 1, MPI_INT, ET, name_lentag, MPI_COMM_WORLD);
		MPI_Send(path_ET.c_str(), path_size_ET, MPI_CHAR, ET, nametag2, MPI_COMM_WORLD);


		//**  BASE_TEXT  **//
		MPI_Recv(&len_BT, 1, MPI_INT, BT, lentag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		data_BT = new char[len_BT + 1];
		MPI_Recv(&vec_size_BT, 1, MPI_INT, BT, sizetag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		std::vector<std::pair<std::string, int>>  count_vector_BT;
		count_vector_BT.resize(vec_size_BT);

		MPI_Recv(data_BT, len_BT,MPI_BYTE,BT,datatag,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		data_BT[len_BT] = '\0';
		
		MPI_Recv(&words_BT, 1, MPI_INT, BT, word_counttag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		boost::iostreams::basic_array_source<char> device(data_BT, len_BT);
		boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
		boost::archive::binary_iarchive recv_ar(s);
		recv_ar >> count_vector_BT;

		//** EXAMINED_TEXT **//
		MPI_Recv(&len_ET, 1, MPI_INT, ET, lentag2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		data_ET = new char[len_ET + 1];
		MPI_Recv(&vec_size_ET, 1, MPI_INT, ET, sizetag2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		std::vector<std::pair<std::string, int>>  count_vector_ET;
		count_vector_ET.resize(vec_size_ET);

		MPI_Recv(data_ET, len_ET, MPI_BYTE, ET, datatag2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		data_ET[len_ET] = '\0';

		MPI_Recv(&words_ET, 1, MPI_INT, ET, word_counttag2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		boost::iostreams::basic_array_source<char> device2(data_ET, len_ET);
		boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s2(device2);
		boost::archive::binary_iarchive recv_arE(s2);
		recv_arE >> count_vector_ET;

		
		//** SEQUENTIAL PART **/
		int sum;
		sum=controller.sum_repeats( controller.minimum(vec_to_map(count_vector_BT), vec_to_map(count_vector_ET)));
		std::cout << "Ilosc slow w textBase: " << words_BT << std::endl;
		std::cout << "Ilosc slow w textExamined: " << words_ET << std::endl;
		std::cout << "Poziom podobienstwa: " << (double)sum / words_BT * 100 << "%" << std::endl;
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_seconds = end - start;

		std::cout << "czas: " << elapsed_seconds.count() << "s" << " z uzyciem "<<nproc <<" procesow \n";
	}

	
	MPI_Finalize();
	return 0;
}
