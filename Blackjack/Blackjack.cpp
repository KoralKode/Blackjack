#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;


class Card {

public:
    enum rank {
        ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,JACK,QUEEN,KING
    };

    enum suit {
        Diamonds=0, Hearts, Spades, Clubs
    };

    Card(rank c_rank,suit c_suit, bool face)
    {
        this->c_rank = c_rank;
        this->c_suit = c_suit;
        is_face_up = face;
    }

    bool Is_face_up() {
        return is_face_up;
    }

    void Flip() {
        is_face_up = !is_face_up;
    }

    int get_value() {
        int val = 0;
        if(is_face_up==true){
            if (c_rank <= 10) {
                return c_rank;
            }
            val = 10;
        }
        return val;
    }

    friend ostream& operator<<(ostream& os, Card& c);


private:
    bool is_face_up;
    rank c_rank;
    suit c_suit;
};

ostream& operator<<(ostream& os, Card& c) {
    const char RANKS[] = { '0','A','2','3','4','5','6','7','8','9','10','J','Q','K'};
    const string SUITS[] = { "Diamonds", "Hearts", "Spades", "Clubs"};
    if (c.Is_face_up() == true) {
        os << RANKS[c.c_rank] << " " << SUITS[c.c_suit];
    }else {
        os << "Closed card";
    }
    return os;
}

class Hand {
protected:
    vector<Card*> cards;
public:
    

    void Add(Card* c) {
        cards.push_back(c);
    }

    void Clear() {
        auto it = cards.begin();
        for (it;it != cards.end();++it) {
            if (*it != nullptr) {
                delete *it;
                *it = nullptr;
            }
        }
        cards.clear();
    }

    int Total() {
        int tot = 0;
        bool isace = false;
        for (Card* var : cards) {
            if (var != nullptr) {
                if (var->get_value() == 1 && tot + 11 <= 21) {
                    isace = true;
                    tot += 11;
                }
                else {
                    tot += var->get_value();
                }
            }
        }
        if (tot > 21 && isace == true) {
            tot -= 10;
        }
        return tot;
    }

    virtual ~Hand() {
        Clear();
    }

};

class Player_gen:public Hand {
    string name;
public:
    Player_gen(string name)
    {
        this->name = name;
    }

    ~Player_gen() {}

    string get_name() {
        return name;
    }

    bool bust() {
        return Total() > 21;
    }

    void is_bust() {
        if (bust()) {
            cout << name << " has bust!" << endl;
        }
    }

    virtual bool is_hitting() = 0;

    friend ostream& operator<<(ostream& os, Player_gen& pg);

};

ostream& operator<<(ostream& os, Player_gen& pg) {
    for (auto var : pg.cards) {
        cout << *var<<" ";
    }
    os<<pg.get_name() << "'s total: " << pg.Total() << endl;
    return os;
}

class Player :public Player_gen {

public:

    Player(string name): Player_gen(name)
    {

    }

    bool is_hitting() override {
        cout << get_name() << ", do you want to take a card (y/n):";
        char b;
        cin >> b;
        return b == 'y';
    }

    void win() {
        cout << "The winner is " << get_name()<<endl;
    }

    void loose() {
        cout << get_name() << " is looser." << endl;
    }

    void draw() {
        cout << get_name() << " has draw." << endl;
    }
};

class House:public Player_gen {
public:
    House():Player_gen("dealer"){}

    void flip_first_card() {
        cards[0]->Flip();
    }

    bool is_hitting() override {
        return Total() <= 16;
    }

};

class Deck : public Hand {
public:
    Deck()
    {
        cards.reserve(52);
    }

    void populate() {
        for (int i = Card::ACE;i <= Card::KING;++i) {
            for (int j = Card::Diamonds;j <= Card::Clubs;++j) {
                cards.push_back(new Card(Card::rank(i), Card::suit(j), true));
            }
        }
    }

    void shuffle() {
        random_shuffle(cards.begin(),cards.end());
    }

    void deal(Player_gen& pg) {
        if (cards.empty()) {
            cout << "Out of cards!" << endl;
        }else{
        pg.Add(cards.back());
        cards.pop_back();
        }
    }

    void adding_cards(Player_gen& pg) {
        while (pg.is_hitting() == 1 && pg.bust() == 0) {
            deal(pg);
            cout << pg;
        }
        pg.is_bust();
    }

};

class Game {
    Deck deck;
    vector<Player> players;
    House dealer;
public:
    Game(vector<string>& names)
    {
        
        for (int i = 0;i < names.size();++i) {
            players.push_back(Player(names[i]));
            
        }
        srand(static_cast<unsigned int>(time(0)));
        deck.populate();
        deck.shuffle();
    }

    void Play() {
        auto it = players.begin();

        for (it;it != players.end();++it) {
            deck.deal(*it);
            deck.deal(*it);
        }
        
        deck.deal(dealer);
        deck.deal(dealer);
        dealer.flip_first_card();

        for (it = players.begin();it != players.end();++it) {
            cout << *it;
        }

        cout << dealer<<endl;

        for (it = players.begin();it != players.end();++it) {
            deck.adding_cards(*it);
        }

        dealer.flip_first_card();
        deck.adding_cards(dealer);
        if (dealer.bust()) {
            for (it = players.begin();it != players.end();++it) {
                if (it->bust() == false) {
                    it->win();
                }
                else {
                    it->loose();
                }
            }
        }
        else {
            for (it = players.begin();it != players.end();++it) {
                if (it->bust()) {
                    it->loose();
                }
                else if (it->Total() > dealer.Total()) {
                    it->win();
                }
                else if (it->Total() == dealer.Total()) {
                    it->draw();
                }
                else {
                    it->loose();
                }
            }
        }
        
        for (it = players.begin();it != players.end();++it) {
            it->Clear();
        }
        dealer.Clear();
        
    }
};

int main(){
    bool choice = true;
    cout << "_____Welcome to Blackjack!_____" << endl;
    while (choice) {
        cout << "How many players: ";
        int n;
        cin >> n;
        cout << "Enter names:" << endl;
        vector<string> vec;
        string s;

        while (n>0) {
            cin >> s;
            if (s == "" || s==" ") { break; }
            vec.push_back(s);
            --n;
        }

        Game g(vec);
        g.Play();
        cout << "Do you want to play again (y/n): ";
        cin >> s;
        if (!(s[0] == 'y' || s[0] == 'Y')) {
            choice = false;
            cout << "Bye-bye!" << endl;
        }
    }
}


