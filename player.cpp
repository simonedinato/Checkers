#include "player.hpp"
#define SIZE 8
#define SIZEX 12
#define SIZEO 12

char convert_char(Player::piece i){
    switch(i){
        case Player::piece::x:
            return 'x';
        case Player::piece::o:
            return 'o';
        case Player::piece::X:
            return 'X';
        case Player::piece::O:
            return 'O';
        case Player::piece::e:
            return ' '; 
        default:
            break;
    }
}

Player::piece convert_piece(char i){
    switch(i){
        case 'x':
            return Player::piece::x;
        case 'o':
            return Player::piece::o;
        case 'X':
            return Player::piece::X;
        case 'O':
            return Player::piece::O;
        case 'e':
            return Player::piece::e;
        default:
            break;
    }
}

struct Player::Impl{
    Player::piece** board;
    Impl* next;
    int player_nr;
    int board_count;
};

Player::piece** newboard(){
    Player::piece** board = new Player::piece*[SIZE];
    for(int i = 0; i < SIZE; i++)
        board[i] = new Player::piece[SIZE];
    return board;
}

void clearboard(Player::piece** board){
    for(int y = 0; y < SIZE; y++){
        delete[] board[y];
    }
    delete[] board;
}

Player::Player(int player_nr){
    std::cout<< "constructor called"<<std::endl;
    if(player_nr != 1 && player_nr != 0){
        throw player_exception{player_exception::index_out_of_bounds, "player_nr must be either 0 or 1"};
    }
    pimpl = new Impl{ nullptr, nullptr, player_nr, 0};
    pimpl->board = newboard();
    std::cout<< "constructor over"<<std::endl;
}

Player::~Player(){
    std::cout<< "destructor called"<<std::endl;
    while(pimpl != nullptr){
        Impl* temp = pimpl;
        pimpl = pimpl->next;
        for(int i = 0; i < SIZE; i++){
            delete[] temp->board[i];
        }
        delete[] temp->board;
        delete temp;
    }
    delete pimpl;
    std::cout<< "destructor over"<<std::endl;
}

Player::Player(const Player& copy){
    std::cout<< "copy called"<<std::endl;
    this->pimpl = new Impl{newboard(), nullptr, copy.pimpl->player_nr, copy.pimpl->board_count};
    Impl* temp = this->pimpl;
    Impl* copytemp = copy.pimpl;
    /*
    while(copytemp != nullptr){
        temp->player_nr = copytemp->player_nr;;
        temp->board_count = copytemp->board_count;
        temp->board = newboard();
        
        for(int i = 0; i < SIZE; i++){
            for(int j = 0; j < SIZE; j++){
                temp->board[i][j] = copytemp->board[i][j];
            }
        }
        
        temp = temp->next;
        if(copytemp->next != nullptr){
            temp = new Impl{nullptr};
        }
        copytemp = copytemp->next;
    }*/
    while(copytemp != nullptr){
        //temp = new Impl{newboard(), nullptr, copytemp->player_nr, copytemp->board_count};
        for(int i = 0; i < SIZE; i++){
            for(int j = 0; j < SIZE; j++){
                temp->board[i][j] = copytemp->board[i][j];
            }
        }
        temp = temp->next;
        if(copytemp->next != nullptr){
            temp = new Impl{nullptr};
        }
        copytemp = copytemp->next;
    }
    std::cout<< "copy over"<<std::endl;
}

Player& Player::operator=(const Player& copy){
    std::cout<< "assignment called"<<std::endl;
    if(this->pimpl == copy.pimpl) return *this;
    else{
        Impl* temp = this->pimpl;
        this->pimpl = this->pimpl->next;
        for(int i = 0; i < SIZE; i++){
            delete[] temp->board[i];
        }
        delete[] temp->board;
        temp = new Impl{newboard(), nullptr, copy.pimpl->player_nr, copy.pimpl->board_count};
        Impl* copytemp = copy.pimpl;
        while(copytemp != nullptr){
            temp = new Impl{newboard(), nullptr, copytemp->player_nr, copytemp->board_count};
            for(int i = 0; i < SIZE; i++){
                for(int j = 0; j < SIZE; j++){
                    temp->board[i][j] = copytemp->board[i][j];
                }
            }
            temp = temp->next;
            if(copytemp->next != nullptr){
                temp = new Impl{nullptr};
            }
            copytemp = copytemp->next;
            std::cout<<"assignment over"<<std::endl;
        }
        return *this;
    }
}

Player::piece Player::operator()(int r, int c, int history_offset) const{
    std::cout<< "operator() called"<<std::endl;
    int count = 0;
    Impl* temp = this->pimpl;
    while(temp != nullptr){
        temp = temp->next;
        count++;
    }
    if(count > history_offset)
        throw player_exception{player_exception::index_out_of_bounds,"board not found"};
    temp = this->pimpl;
    while(count > history_offset){
        temp = temp->next;
        count--;
    }
    std::cout<< "operator() over"<<std::endl;
    return temp->board[r][c];
}

void Player::load_board(const std::string& filename){
    std::cout<<"Load called"<<std::endl;
    Impl* temp;
    int tail;
    if(this->pimpl->board == nullptr){
        this->pimpl->board = newboard();
        temp = this->pimpl;
        tail = this->pimpl->board_count;
    }
    else{
        temp = this->pimpl;
        while(temp->next != nullptr){
            temp = temp->next;
        }
        tail = temp->board_count;
        temp->next = new Impl{newboard(), nullptr, this->pimpl->player_nr, tail+1};
        temp = temp->next;
    }
    std::ifstream f(filename.c_str());
    bool test = false;
    test = f.good();
	
    if(!test) throw player_exception{player_exception::index_out_of_bounds, "file not found"};
    std::fstream file(filename, std::fstream::in);
    Player::piece** board = newboard();
    char cell;
    int i = SIZE - 1, j = 0, xcount = 0, ocount = 0;
    
    while(file.get(cell)){
        if(cell != '\n'){
            if(i < 0){
                clearboard(board);
                throw player_exception{player_exception::index_out_of_bounds, "too much pieces"};
            }
            if((i+j) % 2 == 0 && cell != ' '){
                clearboard(board);
                std::cout<<"Block"<<std::endl;
                throw player_exception{player_exception::index_out_of_bounds, "piece in a white space"};
            }
            if(cell == 'o' || cell == 'O') ocount++;
            if(cell == 'x' || cell == 'X') xcount++;
            board[i][j] = convert_piece(cell);
            j++;
            if(j == SIZE){
                i--;
                j = 0;
            }
        }
        file.get(cell);
    }
    file.close();
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            std::cout<<convert_char(board[i][j])<<" ";
        }
        std::cout<<std::endl;   
    }
    if(xcount > SIZEX){
		clearboard(board);
		throw player_exception{player_exception::invalid_board, "too many x pieces"};
	}
	if(ocount > SIZEO){
		clearboard(board);
		throw player_exception{player_exception::invalid_board, "too many o pieces"};
	}
    clearboard(board);
    std::cout<<"Load over"<<std::endl;
}

void Player::store_board(const std::string& filename, int history_offset) const{
    std::cout<< "Store called"<<std::endl;
    int count = 0;
    Impl* temp = this->pimpl;
    while(temp != nullptr){
        temp = temp->next;
        count++;
    }
    if(count > history_offset)
        throw player_exception{player_exception::index_out_of_bounds,"board not found"};
    temp = this->pimpl;
    while(count > history_offset){
        temp = temp->next;
        count--;
    }
    std::fstream file;
    file.open(filename, std::fstream::out);
    for(int i = SIZE - 1; i >= 0; i--){
        for(int j = 0; j <= SIZE - 1; j++){
            file <<convert_char(temp->board[i][j]);
            if(j != SIZE - 1)
                file <<" ";
        }
        if(i != 0)
            file <<"/n";
    }
    file.close();
    std::cout<< "Store over"<<std::endl;
}

void Player::init_board(const std::string& filename) const{
    std::cout<< "Init called"<<std::endl;
    Player::piece** board = newboard();
    for(int i = 0; i < SIZE; i--){
        for(int j = 0; j <= SIZE - 1; j++){
            if(i >= 0 && i <= 2){
                if((i + j) % 2 == 0) board[i][j] = Player::piece::e;
                else board[i][j] = Player::piece::x;
            }
            else if(i >= 5 && i <= 7){
                if((i + j) % 2 == 0) board[i][j] = Player::piece::e;
                else board[i][j] = Player::piece::o;
            }
            else{
                board[i][j] = Player::piece::e;
            }
        }
    }
    std::fstream file;
    file.open(filename, std::fstream::out);
    for(int i = SIZE - 1; i >= 0; i--){
        for(int j = 0; j <= SIZE - 1; j++){
            file <<convert_char(board[i][j]);
            if(j != SIZE - 1)
                file <<" ";
        }
        if(i != 0)
            file <<"/n";
    }
    file.close();
    clearboard(board);
    std::cout<< "Init over"<<std::endl;
}

int main(){
    try{
        Player p1(0);
        Player p2(p1);
        p1.load_board("board.txt");
    }
    catch(player_exception& e){
        std::cout<<e.msg<<std::endl;
    }
    
    return 0;
}


