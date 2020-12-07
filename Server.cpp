#include <iostream>
#include <mutex>
#include <vector>
#include <string>
#include "socket.h"
#include "Blockable.h"
#include "socketserver.h"
#include "thread.h"
using std::cout;
using std::endl;
struct player
{
	std::string name;
	int life;
};


void terminationThread(bool *terminate, Communication::SocketServer *server, std::vector<Communication::Socket *> *socket,const int id){
	std::string choice;
	std::cout<<"Server Online! INPUT : done to close SERVER"<<std::endl;
	while(*terminate){
		std::cin>>choice;
		if(choice.compare("done")==0){
			*terminate = false;
		}
	}
	for(int i = 0; i<socket->size() ; i++){
		socket->at(i)->Write(Communication::ByteArray("Sever Closed , Good Bye!"));
		socket->at(i)->Close();
		std::cout<<"Now close ALL CLIENT."<< i << "..."<<std::endl;
		delete socket->at(i);
	}
	server->Shutdown();
}

void communicationThread(bool *server_running, Communication::Socket *player1, Communication::Socket *player2,player data1,player data2, const int id){
	player1->Write(Communication::ByteArray("go"));
	player2->Write(Communication::ByteArray("go"));
	Communication::ByteArray namebuff;
	player1->Read(namebuff);
	data1.name = namebuff.ToString();

	cout << "The server has received response from new client and signed in his name as "+ data1.name + "." <<endl;
	player2->Read(namebuff);
	data2.name = namebuff.ToString();
	cout << "The server has received response from new client and signed in his name as "+ data2.name + "." <<endl;
	bool thread_running = true;
	player1->Write(Communication::ByteArray("BATTLE start! Please enter the action you want to make(OA, SA, or S):"));
	player2->Write(Communication::ByteArray("BATTLE start! Please enter the action you want to make(OA, SA, or S):"));
	cout << "Battle Start!" << endl;  
	while (thread_running && *server_running)
	{
		int write,read;
		Communication::ByteArray receivedMsg;
		std::string player1Response,player2Response	,player1Buffer,player2Buffer;
		int player1Choice,player2Choice;

		/* Get a message from player 1 */
		read = player1->Read(receivedMsg);
		player1Response = receivedMsg.ToString();
		/* Get a message from player 2 */
		read = player2->Read(receivedMsg);
		player2Response = receivedMsg.ToString();

		if(player1Response == "quit"){
			player2->Write(Communication::ByteArray("The server is terminated. Please try another time to enter the game.\n"));
			player2->Write(Communication::ByteArray("YOU WIN! Do you want to start another match? (Y/N):\n"));
			break;
		}else if (player2Response == "quit"){
			player1->Write(Communication::ByteArray("The server is terminated. Please try another time to enter the game.\n"));
			player1->Write(Communication::ByteArray("YOU WIN! Do you want to start another match? (Y/N):\n"));
			break;
		}


		if(player1Response == "OA" && player2Response!="S"){
			data2.life--;
		}else if(player1Response == "SA" && player2Response!="S"){
			data2.life-= 5;
		}

		if(player2Response == "OA" && player1Response!="S"){
			data1.life--;
		}else if(player2Response == "SA" && player1Response!="S"){
			data1.life-= 5;
		}
		
		if (data2.life <= 0){
			player1->Write(Communication::ByteArray("YOU WIN! Do you want to start another match? (Y/N):"));
			cout << "Match is over. Winner is "  << data2.name << endl;
			player2->Write(Communication::ByteArray("YOU LOSE! Do you want to start another match? (Y/N):"));
			thread_running = false;
		}else if (data1.life <= 0){
			player2->Write(Communication::ByteArray("YOU WIN! Do you want to start another match? (Y/N):"));
			cout << "Match is over. Winner is "  << data1.name << endl;
			player1->Write(Communication::ByteArray("YOU LOSE! Do you want to start another match? (Y/N):"));
			thread_running = false;
		}
		if (thread_running){
			std::string temp;
			temp = "Attack finished. You currently have " +  std::to_string(data1.life) + "HP. Please enter your next action (OA, SA, or S):";
			player1->Write(Communication::ByteArray(temp));
			temp.clear();
			temp = "Attack finished. You currently have " +  std::to_string(data2.life) + "HP. Please enter your next action (OA, SA, or S):";
			player2->Write(Communication::ByteArray(temp));
			cout << data1.name << "has " << data1.life << " HP, ";
			cout << data2.name << "has " << data2.life << " HP.";
		}
	}
	
	/* Terminate thread */
	player1->Close();
	player2->Close();
	
}

int main(){
	/* Create a Socket Server to accept connections */
	Communication::SocketServer *serverSocket;
	serverSocket = new Communication::SocketServer(1337);
	std::vector <std::thread *> threadVector;
	std::vector <Communication::Socket *>socketVector;
	bool runing = true;
	int threadId = 0;
	/* Begin termination thread */
	std::thread* new_thread = new std::thread(terminationThread, &runing, serverSocket, &socketVector,threadId);
	threadVector.push_back(new_thread);
	threadId++;
	
	while(runing){
		try{
			/* Create a regular Socket to communicate with client */
			Communication::Socket *player1 = new Communication::Socket(serverSocket->Accept());
			cout << "There is a new player connected to the server." << endl;
			player player1_data;
			player1_data.life = 20;

			player player2_data;
			player2_data.life = 20;
			socketVector.push_back(player1);
			Communication::Socket *player2 = new Communication::Socket(serverSocket->Accept());
			cout << "There is a new player connected to the server." << endl;
			socketVector.push_back(player2);
			
			
			/* Spawn a thread for the client that connected */
			std::thread* new_thread = new std::thread(communicationThread, &runing, player1,player2,player1_data,player1_data,threadId);
			threadVector.push_back(new_thread);
			threadId++;
		}catch(int e){
			if(e == 2){
				break;
			}
			else{
				continue;
			}
		}		
	}	

	for(int i = 0 ; i < threadVector.size() ; i++){
		threadVector.at(i)->join();
		std::cout<<"Now terminating thread "<< i << "..."<<std::endl;
		delete threadVector.at(i);
	}
	std::cout<<"Server terminated! Good Bye!"<<std::endl;
	return 0;
}