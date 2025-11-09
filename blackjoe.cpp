#include <iostream>
#include <string>
#include <vector>
#include <bitset>
//#include "PAG.h" // Include Player-Automated-Game (PAG)

using namespace std;
/*
	The card values are split into an 8 bit number:
	0000 0 0 00
	 V   A T S

	The first four bits are the value of the card, e.g 3, 6, 10, etc.
	
	The last two bits represent the suit, so 00=Spades, 01=Hearts, 10=Clubs, and 11=Diamonds.
	The the other bit is used to say if the card is a face card or not, also called the tens bit, 
	it is used to set the value of face cards to 10 as per the black-jack rule-set.
	- Value may be changed if you've got the skills to find the right spot to do that in my messy code >:)
	The one after that is the ace bit, if it's 1, the card (if it is an ace) has a 
	value of 11, instead of 1 as shown by the value section. If the ace mark is removed, 
	the card will be counted with a value of 1.
*/

// Function declarations:
bitset<8> draw(vector<bitset<8>>*); 									  // Used to draw a card from the deck.
vector<bitset<8>> shuffleDeck(vector<bitset<8>>); 						  // Shuffles the deck.
string getCardName(bitset<8>); 											  // Used to get the name of a given card, e.g "Ace of Spades"
int getCardValue(bitset<8>); 											  // Used to get the value of a given card, e.g 10.
int dealersTurn(vector<bitset<8>>*, vector<bitset<8>>*);				  // The dealer algorithm resides here.
void displayLocalTable(vector<vector<bitset<8>>> hands, int, int, vector<int>); // Displays the table.
void announce(vector<int> handSums);									  // Last function to be called, announces the results.
void split(vector<bitset<8>> *hands[], vector<int>*);

// Functions with definitions:
string makeLow(string in) 												  // Used to make capital words lowercase.
{ for(int i = 0; i < (int)in.size(); i++) { in[i] = tolower(in[i]); } return in; }

// Main function : Where I reside.
int main(int argc, char *argv[])
{
	string playerCountInput = "2";   	 				// Set default amount of players.

	if(argc==2)                     		 			// Check if user specified a player count.
	{ playerCountInput = argv[1]; }  	 				// Set that amount.
	int playerCount = stoi(playerCountInput);			// Number of players (excluding dealer)
	if(playerCount > 7)						 			// Check if player count is greater than 7, if so, exit.
	{ cout << "Please keep the player count to a max. of 7.\n"; return 1; }

	srand(time(NULL)); 									// Seed the rand function.
	int i, k, sum;
	int suit = 0;   									// Used to set the suit of the cards.
	int value = 1;  									// Used to set the values of the cards.
	int turn = playerCount;
	string input;										// Current input command.
	vector<int> splitIndexes;
	vector<int> handSums;								// Sum of the values of all hands. Very handsum name.
	vector<bitset<8>> deck;						    	// Initialize the deck.
	vector<vector<bitset<8>>> hands;					// Initialize the hands of the players.

	// Assign all the values for all the cards along with their suit.
	for(i = 0; i < 52; i++) 
	{
		deck.push_back(bitset<8>((value << 4)+suit));	   // Set the current value and suit.
		if((deck[i]>>4).to_ulong()>10) { deck[i].set(2); } // Check if the current card is a face card & set tens bit to 1.
		if((deck[i]>>4).to_ulong()==1) { deck[i].set(3); } // Check if the current card is an ace & set ace bit to 1.
		if(value%13==0) { value=0; suit++;} value++;	   // Reset value count, and increment suit count.
	}

	// Used for debugging, sets all the cards in the deck to the ace of spades. WARNING: Comment out the code above.
	/*for(i = 0; i < 52; i++) 
	{deck.push_back(bitset<8>("00011000"));}*/

	deck = shuffleDeck(deck); 				// Shuffle the deck.	
	for(i = 0; i < playerCount+1; i++)
	{
		hands.push_back(vector<bitset<8>>{draw(&deck), draw(&deck)});	// Deal cards.
		sum = 0;
		for(k = 0; k < (int)hands[i].size(); k++)	// Calculate premature hand values of all players.
		{ sum += getCardValue(hands[i][k]); }
		handSums.push_back(sum);					// Store said values.
	}

	while(turn>=0)		// Game loop - the turn variable only decrements once a player stands or busts.
	{
		if(turn==0) 	// Check if it's the dealer's turn, aka last turn, aka final turn.
		{
			handSums[0] = dealersTurn(&deck, &hands[0]);		// Summon the dealer algorithm and let it do its magic (Call to the dealer function)
			displayLocalTable(hands, playerCount+1, turn, handSums);	// Display the game table.
			announce(handSums); break;									// Announce the results and exit the game.
		}
		sum = 0;
		for(k = 0; k < (int)hands[turn].size(); k++)					// Loop through all cards of the current player.
		{
			sum += getCardValue(hands[turn][k]);						// Add the current card's value to the value of the hand.
			if(sum>21&&hands[turn][k].test(3))							// Test for aces and if necessary set their value to 1.
			{ hands[turn][k].reset(3); sum -= 10; }
		}
		if(sum==21) { turn--; continue; }								// Check if player has a hand value of 21 - in which case standing is forced.
		handSums[turn] = sum;											// Add the value of the hand to the vector that holds all hand values.

		displayLocalTable(hands, playerCount+1, turn, handSums);		// Display the game table.
		cout << "\nHit or Stand? : ";									// Take input from the current player.
		getline(cin, input);
		input = makeLow(input);											// Convert the input to lowercase so I don't have 49 if statements (2^5 + 2^4 + 1)
																		// Excersice: find out how to get this equation.

		if(input.compare("hit")!=0&&input.compare("stand")!=0&&input.compare("exit")!=0&&input.compare("split")!=0&&input.compare("h")!=0) // Huge guard clause.
		{ cout << "Invalid entry, try again!\n"; continue; }
		else if(input.compare("stand")==0) { turn--; continue; }							// In case of stand (s).
		else if(input.compare("exit")==0) { return 0; }										// In case of exit.
		else if(input.compare("split")==0)
		{
			
		}
		// In case of hit:
		hands[turn].push_back(draw(&deck));													// Draw new card.
		handSums[turn] += getCardValue(hands[turn].back());									// Add its value to the sum of the current hand in the vector.

		for(k = 0; k < (int)hands[turn].size()&&handSums[turn]>21; k++)						// Correct the value of any aces if needed.
		{ if(hands[turn][k].test(3)) { hands[turn][k].reset(3); handSums[turn] -= 10; } }

		if(handSums[turn]>=21) { turn--; }		// If player busted (or has 21), cancel their turn BY FORCE!
	}
}
// Draw funtion : Used to draw a card from the deck, and thereby remove it from the deck.
bitset<8> draw(vector<bitset<8>> *deck)
{
	bitset<8> drawCard = deck->back();	// Save the last card in the deck.
	deck->pop_back();					// Delete last card in the deck.
	return drawCard;	// IT'S THE RETURN KEYWORD, WHY TF DO YOU NEED TO READ THIS COMMENT?!
}
// Shuffle function : Used to shuffle the deck.
vector<bitset<8>> shuffleDeck(vector<bitset<8>> workingDeck)
{
	for(int i = 1; i < (int)workingDeck.size(); i++) 	  			// Loop over deck.
	{
		int subject = rand() % (int)workingDeck.size();   			// Get a random integer within range of the deck's size.
		bitset<8> save = workingDeck[i], save2 = workingDeck[i-1];	// Save current cards.
		
		workingDeck[i] = workingDeck[subject];			  			// Set current card to random card.
		workingDeck[subject] = save;					  			// Set random card into saved card.

		if(subject+1<(int)workingDeck.size())			  			// Perform additional shuffle if possible.
		{ workingDeck[i-1] = workingDeck[subject+1]; workingDeck[subject+1] = save2; }
	}
	return workingDeck;	// IT'S THE RETURN KEYWORD, WHY TF DO YOU NEED TO READ THIS COMMENT?!
}
// Card-Name function : Used to get the name of a given card.
string getCardName(bitset<8> card)
{
	int suitNum = card.to_ulong() & 0b00000011; // Apply mask to card to filter out the suit value.
	int valueNum = card.to_ulong() >> 4;		// Shift bits to the left by 4 to filter out the value of the card.
	string suit, value;
	switch(suitNum)
	{
		case 0: suit = "Spades"; break;	  // Set suit depending on suit value.
		case 1: suit = "Hearts"; break;
		case 2: suit = "Clubs"; break;
		case 3: suit = "Diamonds"; break;
	}
	if(valueNum<11&&valueNum!=1) {value = to_string(valueNum);} // Convert value to string if it isn't an ace or face card.
	else if(valueNum==1) {value = "Ace";}	// Set name of the card if it's an ace or face card.
	else if(valueNum==11) {value = "Jack";}
	else if(valueNum==12) {value = "Queen";}
	else if(valueNum==13) {value = "King";}
	return (value + " of " + suit);	// IT'S THE RETURN KEYWORD, WHY TF DO YOU NEED TO READ THIS COMMENT?!
}
// Card-Value function : Used to get the value of a given card.
int getCardValue(bitset<8> card)
{
	if(card.test(3)) {return 11;}					// Return 11 immediately if the ace bit is flipped.
	else if(card.test(2)) {return 10;}				// Return 11 immediately if the tens bit is flipped.
	int valueNumFromSection = card.to_ulong() >> 4;	// Get the value if the card isn't an ace or face card.
	return valueNumFromSection;	// IT'S THE RETURN KEYWORD, WHY TF DO YOU NEED TO READ THIS COMMENT?!
}
// Dealers turn function : The dealer algorithm resides here.
int dealersTurn(vector<bitset<8>> *deck, vector<bitset<8>> *hand)
{
	int sum = 0, i, k;
	for(i = 0; i < (int)hand->size(); i++)					// Calculate premature sum.
	{ sum += getCardValue(hand->at(i)); }
	if(sum>21) { hand->back().reset(3); sum -= 10; }		// Solve edge case where the dealer gets two aces in the start.

	while (sum<17)											// Loop 'hit' action while the hand value is less than 17.
	{
		hand->push_back(draw(deck));						// Draw new card.
		sum += getCardValue(hand->back());					// Add it's value to the sum.

		// Check if it's an ace and the sum over 21, if so, set the value of an ace in had back to 1.
		for(k = 0; k<(int)hand->size() && sum>21; k++)
		{ if(hand->at(k).test(3)) { hand->at(k).reset(3); sum -= 10; } }
	}
	return sum;	// IT'S THE RETURN KEYWORD, WHY TF DO YOU NEED TO READ THIS COMMENT?!
}
// Display function : Used to display the table (all the hands and cards) ever turn.
void displayLocalTable(vector<vector<bitset<8>>> hands, int handCount, int turn, vector<int> handSums)
{
	system("clear");
	if(handSums[0]>21&&turn==0) { cout << " >>> >> Dealer (busted) << <<<\n"; }						// Check if dealer busted.
	else { cout << " >>> >> Dealer << <<<\n"; }
	(turn!=0) ? cout << "     Hidden (\?\?)\n" : cout << "     " + getCardName(hands[0][0]) + "\n";	// Check if it is the final turn.

	for(int i = 1; i < (int)hands[0].size(); i++)							// Loop through and print the rest of the dealer's hand.
	{ cout << "     " + getCardName(hands[0][i]) + "\n"; }
	if(turn==0) { cout << "      Value of " + to_string(handSums[0]) + "\n"; }// Print hand value if it's the final turn.

	for(int i = 1; i < handCount; i++)										// Loop through all other players.
	{
		if(i==turn) { continue; }											// Skip current player since they have a special display.
		if(handSums[i]>21) 
		{ cout << "\n  >> > Player " + to_string(i) + " (busted) < <<\n"; } // Check if current player busted.
		else { cout << "\n  >> > Player " + to_string(i) + " < <<\n"; }		// Print player and number.
		for(int k = 0; k < (int)hands[i].size(); k++)						// Loop through and print the player's hand.
		{ cout << "     " + getCardName(hands[i][k]) + "\n"; }
		cout << "      Value of " + to_string(handSums[i]) + "\n";			// Print hand value of that player.
	}
	if(turn==0) { return; }													// Do not show current player if it's the last turn.

	cout << "\n >> Current player (" + to_string(turn) + ") <<\n";			// Print current player.
	for(int i = 0; i < (int)hands[turn].size(); i++)						// Loop through and print the player's hand.
	{ cout << "      " + getCardName(hands[turn][i]) + "\n"; }
	cout << "       Value of " + to_string(handSums[turn]) + "\n";			// Print hand value of the current player.
}
// Announce function : Used to announce the results of the game.
void announce(vector<int> handSums)
{
	int biggest = 0, sBiggest = 0, index = 0;
	for(int i = 0; i < (int)handSums.size(); i++)	// Loop through all the hand values to find biggest and second biggest value.
	{ 
		if(handSums[i]>21) { continue; }			// Skip value if it's greater than 21.
		if(handSums[i]>=biggest) 					// If the current value is greater than the one previously considered biggest.
		{
			index = i;								// Set the index to current value for single-winner detection.
			if(biggest>sBiggest) { sBiggest = biggest; } // If the biggest > second biggest, set the second biggest to the biggest.
			biggest = handSums[i];					// Set the biggest to the current value.
		}
		else if(handSums[i]>sBiggest) { sBiggest = handSums[i]; } // If the current value > the second biggest, set second biggest to current.
	}
	if(sBiggest==biggest) { cout << "\nThe game ended wearing a tie!\n"; }	// Check if the game ended in a tie.
	else if(index==0) { cout << "\nThe dealer wins!\n"; }					// Check if the game was won by the dealer (index = 0)
	else { cout << "\nPlayer " + to_string(index) + " wins!\n"; }			// In any other case one of the players has won.
}
// Split function : Splits the hand of the current player.
/*
			Here we are *rolls eyes and credits*


		Program made by Elija Abraham



		Idea by Elija Abraham
		

		Made with the help of Elija Abraham's genius

		With the resources of Elibraham studios

		Sound designs by Elija Abraham Inc.

		Graphical illustrations made by Elija & Co.

		Jeremy.

		Programmers: Elija Abrahams (plural), 
		that guy in the corner whom I like to whip if he doesn't do my code for me, and Elija Abraham clone #331 - suck it #144!

		Head of developement:	/ .. \
								\ -- /

		Body of developement:    /|\
						   	    / | \
							      |
								 / \
							    /   \

		Special thanks to: my bus without whom I wouldn't be where I am. My candy who always knows
		how to make salty jokes come across as sweet. And finally, my spine, without whom I wouldn't be standing as tall as I am.

		Not so special thanks to: the ocean, you're just too salty. The critics. And also my parents, for giving 
		non-consentual birth to me >:(

		Made in micro, a software for NEERDDSSS!!!


		Made in memory of Lisa and in the interest of food.	
*/
