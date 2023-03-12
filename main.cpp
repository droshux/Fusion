#include <iostream>
#include <array>
#include <utility>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

//A bunch of warning ignoring here because apparently clang hates sizeof
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wsizeof-pointer-div"
#pragma clang diagnostic ignored "-Wsizeof-array-argument"
template<typename T>
int arraySize([[maybe_unused]] T arr[]) {
    //return *(&arr + 1) - arr;
    return sizeof arr / sizeof arr[0];
}
#pragma clang diagnostic pop

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
int getIndex(T (&v)[], T &K)
{
    for (int i = 0; i < arraySize<T>(v); i++) {
        T q=v[i];
        if (q == K)
            return i;
    }
    return -1;
}

class Card {
public:
    bool isFaceUp;
    int value;
    int suit; //Not displayed, only used so that multiple cards with the same value can be equal
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

bool isValidFuse(Card cards[]) {
    int numCards = arraySize(cards);

    for (int i = 0; i < numCards; i++) if (cards[i].isNull()) return false; //Any fusions using null cards are invalid

    switch (numCards) {
        case 1: {
            ::printf("%s", "There is only one card\n");
            return cards[0].value == 9; //The only card that fuses with itself is 9
        }
        case 2: {
            return cards[0].value + cards[1].value == 9; //Two cards must sum to 9
        }
        case 3: {
            int values[] = {cards[0].value, cards[1].value, cards[2].value};
            sort(values, values + 3);
            for (int i = 0; i < 3; i++)
                if (values[i] != i + 11) {
                    return false;
                }
            return true; //Three cards must be: 11, 12, 13
        }
        default:
            ::printf("%s", "There are not 1 2 3 cards");
            return false; //Only 1, 2 or 3 cards are allowed.
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
        lastCard = 0;
        hand[0] = Card(9);
    }

    [[maybe_unused]] void addCard(Card c) {
        lastCard++;
        hand[lastCard] = c;
    }
    void removeCard(Card c) {
        int cardIndex = getIndex<Card>(hand, c);
        for (int i = cardIndex; i < lastCard; i++) {
            hand[i] = hand[i+1];
        }
        lastCard--;
    }

    [[maybe_unused]] void Fuse(Card cards[], queue<Card>& deck, Card (&board)[]) {
        int numCards = arraySize<Card>(cards);

        //Check for 10s
        for (int i = 0; i < numCards; ++i)
            if (cards[i].value == 10) this->tens++;


        if (!isValidFuse(cards)) return; //If the cards are invalid do not fuse
        ::printf("%s", "Fusion is valid!\n");
        this->score += numCards; //Give the player a point for each card they fuse

        for (int i = 0; i < numCards; i++) {
            Card c = cards[i];
            int CardIndex = getIndex<Card>(this->hand, c);
            if (CardIndex != -1) {
                this->removeCard(c);
            } else {
                CardIndex = getIndex<Card>(board, c);
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
    Card c[] = {p1.hand[0]};
    p1.Fuse(c, d, b);
    ::printf("%u", p1.lastCard);
    ::printf("%u", p1.hand[0].print());
    return 0;
}