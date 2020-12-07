#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include "socket.h"
#include "Blockable.h"
#include "socketserver.h"
#include "thread.h"
using std::cin;
using std::cout;
using std::endl;
int inArray(std::string userInput, std::vector<int > &times)
{
	if (userInput.empty())
	{
		return -1;
	}
	std::string validChoices[4] = {"OA", "SA", "S", "quit"};
	for (int i = 0; i < 4; ++i)
	{
		if (userInput.compare(validChoices[i]) == 0)
		{
			if (times[i] > 0){
				times[i]--;
				return i;
			}
		}
	}
	return -1;
}
bool isIn(std::string a, std::string b)
{
	std::string::size_type position;
	position = a.find(b);
	if (position != a.npos)
	{
		return true;
	}
	return false;
}

int main()
{

	cout << "Welcome to battle game paradise! In here, you will use your brain to battle with another player. Now I have to tell you the rules of this game:" << endl;
	cout << "1. You will have 20 HP at the beginning, if you lose your entire HP, You lose the game, vice versa." << endl;
	cout << "2. There are three different actions you can make during battle. You can use Ordinary Attack (shortcut: OA; Damage: 1; Number of Usage: no limit), Super Attack (shortcut: SA; Damage: 5; Number of Usage: 3), and Shield (shortcut: S; Number of Usage: 3) to block attack." << endl;
	cout << "3. If you want to surround the game and quit the connection to the server, you have to enter ‘quit’." << endl;
	cout << "Please wait for connection to another player, the battle will be start soon." << endl;
	bool is_continue_game = true;
	std::string name = "";
	while (is_continue_game)
	{
		Communication::Socket mySocket("127.0.0.1", 1337);
		int open, write, read, choice = -1, win = 0, loss = 0, tie = 0;
		std::string myMessage;
		bool terminate = true;
		std::string validChoices[4] = {"OA", "SA", "S", "quit"};
		/* Open the socket */
		std::vector<int> times = {50,3,3,5};
		open = mySocket.Open();
		Communication::ByteArray msgIn;
		while (terminate)
		{
			/* Initial read to synchronize players */
			read = mySocket.Read(msgIn);
			if (msgIn.ToString().compare("go") != 0)
			{
				terminate = false;
				break;
			}
			if (name.size() == 0)
			{
				std::cout << "Enter the account name you want to be use in this battle game:";
				std::cin >> name;
				write = mySocket.Write(Communication::ByteArray(name));
			}
			else
			{
				write = mySocket.Write(Communication::ByteArray(name));
			}
			while ((read = mySocket.Read(msgIn)) > 0)
			{
				cout << msgIn.ToString() << endl;
				if (msgIn.ToString() == "Sever Closed , Good Bye!"){
					return 0;
				} else if (isIn(msgIn.ToString(), "(Y/N)"))
				{
					terminate = false;
					std::string Y;
					cin >> Y;
					if (Y != "Y")
					{
						is_continue_game = false;
						break;
					}
				}
				else if (isIn(msgIn.ToString(), "(OA, SA, or S)"))
				{
					/* Write to the socket */
					while (choice == -1)
					{
						std::cin >> myMessage;
						choice = inArray(myMessage,times);
						if (choice == -1)
						{
							cout << "Please check your input, and enter again: " << endl;
						}
					}
					write = mySocket.Write(Communication::ByteArray(validChoices[choice]));
					if (choice == 3){
						is_continue_game = false;
						break;
					}
					choice = -1;
				}
				
			}
		}
		/* Close the socket when we are done */
		mySocket.Close();
	}
	std::cout << "Good Bye!" << std::endl;
	return 0;
}
