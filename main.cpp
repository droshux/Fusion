#include <iostream>
#include <array>
#include <utility>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

template <typename T>
[[maybe_unused]] bool contains([[maybe_unused]] vector<T> vec, [[maybe_unused]] const T & elem)
{
    bool result = false;
    for (auto & x : vec)
    {
        if (&x == &elem)
        {
            result = true;
            break;
        }
    }
    return result;
}
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
        if (isFaceUp) return 'X';
        if (this->value != 0) return (char)this->value;
        else if (no_card_is_space) return '\0';
        else return ' ';
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

Card cardsToFuse[3];

bool isValidFuse() {
    int numCardsToFuse = sizeof cardsToFuse / sizeof cardsToFuse[0];

    for (int i = 0; i < numCardsToFuse; i++) if (cardsToFuse[i].isNull()) numCardsToFuse -= 1; //Do not count null cards
    if (numCardsToFuse != 3) {
        const unsigned int sum = cardsToFuse[0].value + cardsToFuse[1].value + cardsToFuse[2].value;
        return sum == 9; //If there are not 3 cards then they must sum to 9
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
        hand[0] = Card(9);
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

    [[maybe_unused]] void Fuse(queue<Card>& deck, Card (&board)[]) {
        int numCardsToFuse = sizeof cardsToFuse / sizeof cardsToFuse[0];

        //Check for 10s
        for (int i = 0; i < numCardsToFuse; ++i)
            if (cardsToFuse[i].value == 10) this->tens++;


        if (!isValidFuse()) return; //If the cardsToFuse are invalid do not fuse
        ::printf("%s", "Fusion is valid!\n");
        this->score += numCardsToFuse; //Give the player a point for each card they fuse

        for (int i = 0; i < numCardsToFuse; i++) {
            Card c = cardsToFuse[i];
            if (c.isNull()) continue; //Ignore null cards.
            int CardIndex = getIndex<Card>(this->hand, c, lastCard+1);
            if (CardIndex != -1) {
                this->removeCard(c);
            } else {
                CardIndex = getIndex<Card>(board, c, 9);
                if (deck.empty()) board[CardIndex].value = 0;
                else {
                    board[CardIndex] = deck.front();
                    deck.pop();
                }
            }
        }
    }

    Card hand[52];
    int lastCard;
private:
};


int main() {
    queue<Card> d;
    Card b[] = {};
    Player p1 = Player("P1");
    cardsToFuse[0] = Card(0);
    cardsToFuse[1] = Card(1);
    cardsToFuse[2] = Card(8);
    p1.Fuse(d, b);
    return 0;
}