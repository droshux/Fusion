#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-use-anyofallof"
#include <iostream>
#include <array>
#include <utility>
#include <queue>
#include <algorithm>
#include <random>

#define clrScreen "\033[2J\033[1;1H"

using namespace std;

template<class T>
int getIndex(T (&v)[], T &K, const unsigned int vSize)
{
    for (int i = 0; i < vSize; i++) {
        T q=v[i];
        if (q == K)
            return i;
    }
    return -1;
}

class Card {
public:
    bool isFaceUp;
    unsigned int value;
    unsigned int suit; //Not displayed, only used so that multiple cards with the same value can be equal
    explicit Card(int v = 0, int s = 0) {
        value = v;
        suit = s;
        isFaceUp = false;
    }

    [[maybe_unused]] void Flip() {isFaceUp = !isFaceUp;}

    [[nodiscard]] char print(bool no_card_is_space = false) const {
        if (!isFaceUp) return 'X';
        if (this->value != 0)
            switch (this->value) {
            case 10:
                return '$'; //Gems are represented by $ signs
            case 11:
                return 'J';
            case 12:
                return 'Q';
            case 13:
                return 'K';
            default:
                return (char)(this->value + 48); //This is C++ so casting to char uses ASCII
            }

        else return no_card_is_space ? ' ' : '\0';
    }

    [[nodiscard]] bool isNull() const {
        return this->value == 0;
    }

    bool operator==(const Card& rhs) const;
};
//Declare custom equals outside of class
bool Card::operator==(const Card& rhs) const {
    return value==rhs.value and suit==rhs.suit;
}

//Global variables!
Card cardsToFuse[3];
Card board[9];
queue<Card> Deck;

bool IsAlreadyFusing(Card c) {
    for (Card &i : cardsToFuse) {
        if (i == c) return true;
    }
    return false;
}

//Global variable related functions
Card* BoardPos(int x, int y) { //Starts at 1
    return &board[x-1 + 3 * (y-1)];
}

void InitialiseDeck() {
    Card cards[52]; //Build up an array of cards
    int index = 0;
    for (int s = 1; s <= 4; s++)
        for (int v = 1; v <= 13; v++) {
            cards[index] = Card(v, s);
            index++;
        }
    shuffle(&cards[0], &cards[51], random_device{}); //Shuffle the array
    for (Card &c : cards) Deck.push(c); //Add every card to the deck
}

void InitialiseBoard() {
    for (Card &i : board) {
        i = Deck.front();
        Deck.pop();
    }
}

//Check if a fusion is valid
bool isValidFuse() {
    int numCardsToFuse = sizeof cardsToFuse / sizeof cardsToFuse[0];

    for (Card &i: cardsToFuse) if (i.isNull()) numCardsToFuse--;  //Do not count null cards
    if (numCardsToFuse != 3) {
        const unsigned int sum = cardsToFuse[0].value + cardsToFuse[1].value + cardsToFuse[2].value;
        return sum == 9 || (sum == 10 && numCardsToFuse == 1); //If there are not 3 cards sum must be 9 or single 10
    } else {
        unsigned int values[] = {cardsToFuse[0].value, cardsToFuse[1].value, cardsToFuse[2].value};
        sort(values, values + 3);
        for (int i = 0; i < 3; i++)
            if (values[i] != i + 11) {
                return false;
            }
        return true; //Three cardsToFuse must be: 11, 12, 13
    }
}

//Player and player functions (DealHand)
class Player {
public:
    string name;
    int score;
    int tens;

    explicit Player(string s) {
        name = std::move(s);
        score = 0;
        tens = 0;
        lastCard = -1;
    }

    [[maybe_unused]] void addCard(Card c) {
        lastCard++;
        hand[lastCard] = c;
    }
    void removeCard(Card c) {
        int cardIndex = getIndex<Card>(hand, c, lastCard+1);
        for (int i = cardIndex; i <= lastCard; i++) {
            hand[i] = hand[i+1];
        }
        lastCard--;
    }

    [[maybe_unused]] void Fuse() {
        int numCardsToFuse = sizeof cardsToFuse / sizeof cardsToFuse[0];

        //Check for 10s
        for (int i = 0; i < numCardsToFuse; ++i)
            if (cardsToFuse[i].value == 10) { this->tens++; cout << "Gem collected!\n";}

        this->score += numCardsToFuse; //Give the player a point for each card they fuse
        for (Card &i: cardsToFuse) { if (i.isNull()) score--; } //Do not count null cards

        for (int i = 0; i < numCardsToFuse; i++) {
            Card c = cardsToFuse[i];
            if (c.isNull()) continue; //Ignore null cards.
            int CardIndex = getIndex<Card>(this->hand, c, lastCard+1);
            //Remove from hand nad remove/replenish from deck
            if (CardIndex != -1) {
                this->removeCard(c);
            } else {
                CardIndex = getIndex<Card>(board, c, 9);
                if (Deck.empty()) board[CardIndex].value = 0;
                else {
                    Card replenish = Deck.front();
                    if (!replenish.isNull()) {
                        board[CardIndex] = replenish;
                        Deck.pop();
                    } else board[CardIndex] = Card(); //If the deck is empty use a null card
                }
            }
        }
    }

    Card hand[52];
    int lastCard;
};

void DealHand(Player &p) {
    for (int i = 0; i <= 9; i++) {
        Card c = Deck.front();
        c.isFaceUp = true;
        p.addCard(c);
        Deck.pop();
    }
}

//Running the game
int getValidInt(int min, int max) {
    int out;
    string str;
    while (true) {
        cout << "Please enter a whole number between " << min << " and " << max << ": ";
        getline(cin, str, '\n');
        try {
            out = stoi(str);
            if (out >= min && out <= max) return out;
        } catch (...) {
            cout << "\nInvalid input please try again.\n";
        }
    }
}

void PlayerTurn(Player &p) {

    //Fusing loop
    while (true) {
        bool endTurn = false;

        cout << clrScreen;
        //Display Board
        cout << p.name << "'s turn!\n" << "Board:\n\n  123\n";
        for (int y = 1; y <= 3; y++) {
            cout << y << " ";
            for (int x = 1; x <= 3; x++)
                cout << BoardPos(x, y)->print(true);
            cout << "\n";
        }

        //Display Hand
        cout << "\nHand: ";
        for (int i = 0; i <= p.lastCard; i++) cout << p.hand[i].print() << " ";
        cout << "\nYou have " << p.score << " points and " << p.tens << " gems.\n";

        for (Card &i : cardsToFuse) i = Card();//Initialise cardsToFuse to be null cards
        for (Card &i : cardsToFuse) {

            cout << "Where would you like to fuse from: \n    "
                    "1: Your hand\n    "
                    "2: The board\n    "
                    "3: Finish fusing\n    "
                    "4: End turn\n";
            const int fuseLoc = getValidInt(1, 4);
            if (fuseLoc == 4) {
                endTurn = true; //Escape the for loop and the while loop
                break;
            } else if (fuseLoc == 3) {
                break; //Just escape the for loop
            } else if (fuseLoc == 2) {
                bool cancel = false;
                while (true) { //Input validation is annoying.
                    cout << "Choose a card from the board:\n[X]\n";
                    const int xChoice = getValidInt(0, 3);
                    cout << "[Y]\n";
                    const int yChoice = getValidInt(0, 3);
                    if (xChoice == 0 || yChoice == 0) {
                        cancel = true; //Escape if a 0 was entered
                        break;
                    }
                    Card *c = BoardPos(xChoice, yChoice);
                    if (c->isFaceUp && c->value != 0 && !IsAlreadyFusing(*c)) {
                        i = *c;
                        break;
                    } else  //Only face up non-null cards can be fused
                        cout << "(" << xChoice << ", " << yChoice << ") is not valid. Try again.";

                }
                if (cancel) break; //Actually escape
            } else {
                cout << "Choose a card from your hand:\n";
                while (true) {
                    Card c = p.hand[getValidInt(1, p.lastCard + 1) - 1]; //Index woo
                    if (!IsAlreadyFusing(c)) {
                        i = c;
                        break;
                    }
                    cout << "This card has already been chosen!\n";
                }
            }
        }
        if (endTurn) break; //Part of exiting turn

        if (!isValidFuse()) { //If a fusion is invalid try again.
            cout << "INVALID FUSION\nPress enter to try again...";
            string unused;
            getline(cin, unused, '\n');
            continue;
        }

        p.Fuse(); //FUSE!!!

        Card newCard = Deck.front();
        newCard.isFaceUp = true;
        if (!newCard.isNull()) p.addCard(newCard); //Draw a new card if the deck isn't empty

        cout << "You score is now: " << p.score << "!\n";
    }
}

bool CheckForGameEnd(Player &p1, Player &p2) {
    if (p1.lastCard < 0 || p2.lastCard < 0) return true;//End the game if someone empties their cards
    if (p1.tens >= 4 || p2.tens >= 4) return true; //or if they get all tens

    //Check for empty deck
    if (!Deck.empty()) return false; //Game can't end until the deck is empty
    //If the deck is empty and the board is all null cards end the game
    for (Card &c: board)
        if (c.value != 0) return false;

    return true;
}

Player* DecideWinner(Player &p1, Player &p2) {
    //If you have all 4 tens you win
    if (p1.tens >= 4) return &p1;
    if (p2.tens >= 4) return &p2;

    //Or if you have emptied your hand
    if (p1.lastCard < 0) return &p1;
    if (p2.lastCard < 0) return &p2;

    const unsigned int p1Score = p1.score + 3 * p1.tens; //Tens count as 3 points
    const unsigned int p2Score = p2.score + 3 * p2.tens;

    if (p1Score > p2Score) return &p1;
    if (p2Score > p1Score) return &p2;

    //In a draw the player with the most tens wins
    if (p1.tens > p2.tens) return &p1;
    if (p2.tens > p1.tens) return &p2;

    //True draw case:
    return nullptr;
}

void Game() {
    //Get player names and initialise players
    cout << "Welcome to FUSION!\n";
    string p1Name, p2Name;
    cout << "Please enter player 1's name: ";
    getline(cin, p1Name, '\n');
    cout << "\nPlease enter player 2's name: ";
    getline(cin, p2Name, '\n');
    Player P1 = Player(p1Name);
    Player P2 = Player(p2Name);

    // Initialise the deck and board and hands
    InitialiseDeck();
    InitialiseBoard();
    DealHand(P1);
    DealHand(P2);

    //Game loop
    while (true) {
        PlayerTurn(P1);
        PlayerTurn(P2);
        if (CheckForGameEnd(P1, P2)) break;
    }

    Player* winner = DecideWinner(P1, P2);
    if (winner)
        cout << "\n\n\nThe winner is: " << winner->name << "!\n";
    else
        cout << "\n\n\nThe result is a draw!";

}

int main() {
    Game();
    return 0;
}
#pragma clang diagnostic pop