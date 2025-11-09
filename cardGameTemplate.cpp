// PAY ATTENTION!!!

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <bitset>

using namespace std;
/* 
	The card values are split into an 8 bit number:
	0000 0 0 00
	 V   A T S
	
	Where the first two bits represent the suit, so 00=Spades, 01=Hearts, 10=Clubs, and 11=Diamonds.
	The next bit is used to say if the card is a face card or not, also called the tens mark, 
	used to set the value of face cards to 10 as per the black-jack rule-set.
	( May be changed if you're a fellow programmer )
	The one after that is the ace mark, if it's 1, the card (if it is an ace) has a 
	value of 11 instead of 1, as shown by the value section. If the ace mark is removed, 
	it will be counted as a 1.
	
	The last four bits are the value of the card.
*/

// Function definitions:
vector<bitset<8>> shuffleDeck(vector<bitset<8>> workingDeck); // Used to shuffle the deck.
int getCardValue(bitset<8> card); // Used to get the value of a given card, e.g 10.
string getCardName(bitset<8> card); // Used to get the name of a given card, e.g "Ace of Spades"
void displayLocalTable(bool isShowdown, vector<bitset<8>> hands[], int handCount); // Used to display the table.

// Main function.
int main(int argc, char *argv[])
{
	srand(time(NULL)); // Seed the rand function.
	
	string playerCountInput = "2";   	 	 	// Set default amount of players.
	if(argc==2)                     		 	// Check if user specified a player count.
	{ playerCountInput = argv[1]; }  	     	// Set that amount
	
	int playerCount = stoi(playerCountInput);	// Number of players (excluding dealer)
	if(playerCount > 7)						 	// Check if player count is greater than 7, if so, exit.
	{ cout << "Please keep the player count to a max. of 7.\n"; return 1; }
	
	vector<bitset<8>> deck;						// Initialize the deck.
	vector<bitset<8>> hands[playerCount+1];		// Initialize the hands of the players.

	int currentSuit = 0;   // Used to set the suit of the cards.
	int currentValue = 1;  // Used to set the values of the cards.
	
	// Assign all the values for all the cards and MAN let me tell you this was work.
	for(int i = 0; i < 52; i++) 
	{
		deck.push_back(bitset<8>((currentValue << 4)+currentSuit)); // Set the current value and suit.
		if((deck[i]>>4).to_ulong() > 10) { deck[i].set(2); }		// Check if the current card is a face card/set tens mark to 1.
		if((deck[i]>>4).to_ulong()==1) { deck[i].set(3); }			// Check if the current card is an ace/set ace mark to 1.
		if(currentValue%13==0) { currentValue=0; currentSuit++;}
		currentValue++;
	}

	// Shuffle the deck and deal the cards.
	deck = shuffleDeck(deck);
	for(int i = 0; i < playerCount+1; i++)
	{
		hands[i].push_back(deck.back()); 	// Deal one card.
		deck.pop_back();					// Remove deal card from deck.
		hands[i].push_back(deck.back());
		deck.pop_back();
	}
	
	string input;
	displayLocalTable(false, hands, stoi(playerCountInput)+1);
}
// Shuffle function.
vector<bitset<8>> shuffleDeck(vector<bitset<8>> workingDeck)
{
	for(int i = 1; i < (int)workingDeck.size(); i++) // Loop over deck.
	{
		int s1 = rand() % (int)workingDeck.size();   // Get a random integer within range of the deck's size.
		bitset<8> save;
		save = workingDeck[i];						 // Save current card.
		
		workingDeck[i] = workingDeck[s1]; 			 // Set current card to random card.
		workingDeck[s1] = save;						 // Set random card to saved card.
	}
	return workingDeck;
}
// Card-Name function.
string getCardName(bitset<8> card)
{
	int suitNum = card.to_ulong() & 0b00000011; // Apply mask to card to filter out the suit value.
	int valueNum = card.to_ulong() >> 4;		// Shift values to the left by 4 to filter out the value.
	string suit, value;
	
	switch(suitNum)
	{
		case 0: suit = "Spades"; break;	  // Set suit depending on suit value.
		case 1: suit = "Hearts"; break;
		case 2: suit = "Clubs"; break;
		case 3: suit = "Diamonds"; break;
	}
	
	if(valueNum<11&&valueNum!=1) {value = to_string(valueNum);} // Convert value to string if it isn't an ace or face card.
	else if(valueNum==1) {value = "Ace";}		// Set name of the card if it's an ace or face card.
	else if(valueNum==11) {value = "Jack";}
	else if(valueNum==12) {value = "Queen";}
	else if(valueNum==13) {value = "King";}

	return (value + " of " + suit);
}
// Card-Value function.
int getCardValue(bitset<8> card)
{
	if(card.test(3)) {return 11;} 					// Test for and return ace value.
	else if(card.test(2)) {return 10;} 				// Test for and return face card value.
	int valueNumFromSection = card.to_ulong() >> 4;
	return valueNumFromSection;						// Return value of card if it isn't an ace or face card.
}
// Display function.
void displayLocalTable(bool isShowdown, vector<bitset<8>> hands[], int handCount)
{
	cout << "------Dealer has------\n";
	(!isShowdown) ? cout << " » Hidden\n" : cout << " » " + getCardName(hands[0][0]) << endl; // Hide dealer's card unless it's his turn.

	for(int i = 1; i < (int)hands[0].size(); i++)				// Loop through the rest of the dealers cards.
	{ cout << " » " + getCardName(hands[0][i]) << endl; }		// Display the cards.
	
	for(int i = 1; i < handCount; i++)							// Loop through the player hands.
	{
		cout << "\n-----Player (" + to_string(i) + ")-----\n";  // Display the player.
		for(int k = 0; k < (int)hands[i].size(); k++) 			// Loop through the player's hand.
		{ cout << " » " + getCardName(hands[i][k]) << endl; }   // Display the cards.
	}
}
