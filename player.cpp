#include "player.hpp"

//#include <time.h>
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
    throw player_exception{player_exception::index_out_of_bounds, "Wrong conversion"};
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
        case ' ':
            return Player::piece::e;
        default:
            break;
    }
    throw player_exception{player_exception::index_out_of_bounds, "Wrong conversion"};
}

struct Step{
    int begin[2];
    int take[2];
    int end[2];
    Player::piece move;
    Step(){
        for(int i = 0; i < 2; i++){
            begin[i] = 0;
            take[i] = 0;
            end[i] = 0;
        }
        move = Player::piece::e;
    }
    void step_pawn(int x, int y, int next_x, int next_y, int take_x, int take_y, Player::piece p){
        begin[0] = x;
        begin[1] = y;
        take[0] = take_x;
        take[1] = take_y;
        end[0] = next_x; 
        end[1] = next_y;
        move = p;
    }
};
Player::piece dame_p(Player::piece i) {
    return (i == Player::piece::x) ?  Player::piece::X :  Player::piece::O;
}

Player::piece enemy(Player::piece i) {
    return (i == Player::piece::x) ?  Player::piece::o :  Player::piece::x;
}

void check(int x, int y, int& next_x, int& next_y, int way,int& count,  Player::piece e_pawn, Player::piece pawn, Player::piece** board, Step*& moves){
    //std::cout<<"Check called"<<std::endl;
    int i = next_x - x;
    int j = next_y - y;
    //check board limits
    if((x >= 0) && (x < SIZE) && (y >= 0) && (y < SIZE)){
        
        //check spaces
        /*
        memory leak
        */
        //std::cout<<"X "<<next_x<<std::endl;
        //std::cout<<"Y "<<next_y<<std::endl;
        if(board[next_x][next_y] == Player::piece::e){
            //make move
            moves[count].step_pawn(x, y, next_x, next_y, 0, 0, pawn);
            count++;
        }
        //try to eat
        else if(board[next_x][next_y] == e_pawn){
            //check new board limits
            if((next_x + (way * i) >= 0) && (next_x + (way * i) < SIZE) && (next_y + (way * j)>= 0) && (next_y + (way * j) < SIZE)){
                //check new spaces
                /*std::cout<<"X "<<x<<std::endl;
                std::cout<<"Y "<<y<<std::endl;
                std::cout<<"Next X "<<next_x<<std::endl;
                std::cout<<"Next Y "<<next_y<<std::endl;
                std::cout<<"Board[ "<<(next_x + (way * i))<<"]["<<(next_y + (way * j))<<"]"<<std::endl;
                std::cout<<"Pedina "<<convert_char(board[next_x + (way * i)][next_y + (way * j)])<<std::endl;*/
                if(board[next_x + (way * i)][next_y + (way * j)] == Player::piece::e){
                    //std::cout<<"STOP"<<std::endl;
                    //make move
                    moves[count].step_pawn(x, y, (next_x + (way * i)), (next_y + (way * j)), next_x, next_y, pawn);
                    count++;
                }
            }
        }
    }
    //std::cout<<"Check over"<<std::endl;
}

void check_dame(int x, int y, int& next_x, int& next_y, int way,int& count, Player::piece dame,  Player::piece e_pawn, Player::piece e_dame, Player::piece** board, Step*& moves){
    int i = next_x - x;
    int j = next_y - y;
    //check board limits
    if((x >= 0) && (x < SIZE) && (y >= 0) && (y < SIZE)){
        //check spaces
        if(board[x][y] == Player::piece::e){
            //make move
            moves[count].step_pawn(x, y, next_x, next_y, 0, 0, dame);
            count++;
        }
        //try to eat
        else if(board[next_x][next_y] == e_pawn || board[next_x][next_y] == e_dame){
            //check new board limits
            if((next_x + (way * i) >= 0) && (next_x + (way * i) < SIZE) && (next_y + (way * j)>= 0) && (next_y + (way * j) < SIZE)){
                //check new spaces
                if(board[next_x + (way * i)][next_y + (way * j)] == Player::piece::e){
                    //make move
                    moves[count].step_pawn(x, y, (next_x + (way * i)), (next_y + (way * j)), next_x, next_y, dame);
                    count++;
                }
            }
        }
    }
}

Step* moves(int way, Player::piece pawn, Player::piece** const board){
    //std::cout<<"Moves called"<<std::endl;
    int x, y, next_x, next_y, count;
    x = y = next_x = next_y = count = 0;
    Step* moves = new Step[40];
    Player::piece dame = dame_p(pawn), e_pawn = enemy(pawn), e_dame = dame_p(enemy(pawn));

    for(int i = 0; i < SIZE - 1; i++){
        for(int j = 0; j < SIZE - 1; j++){
            
            x = i, y = j;
            if(pawn == board[x][y]){
                
                //try left
                next_x = x + way;
                next_y = y - way;
                check(x, y, next_x, next_y, way, count, e_pawn, pawn, board, moves);
                
                //try right
                next_x = x + way;
                next_y = y + way;
                check(x, y, next_x, next_y, way, count, e_pawn, pawn, board, moves);
                
                //std::cout<<"x2 "<<moves->end[0]<<std::endl;
                //std::cout<<"y2 "<<moves->end[1]<<std::endl;
            }
            else if(board[x][y] == dame){
                //std::cout<<"STOP"<<std::endl;
                //try top left
                next_x = x + way;
                next_y = y - way;
                check_dame(x, y, next_x, next_y, way, count, dame, e_pawn, e_dame, board, moves);
                
                //try top right
                next_x = x + way;
                next_y = y + way;
                check_dame(x, y, next_x, next_y, way, count, dame, e_pawn, e_dame, board, moves);
                
                //try bottom left
                next_x = x - way;
                next_y = y - way;
                check_dame(x, y, next_x, next_y, way, count, dame, e_pawn, e_dame, board, moves);
                
                //try bottom right
                next_x = x + way;
                next_y = y - way;
                check_dame(x, y, next_x, next_y, way, count, dame, e_pawn, e_dame, board, moves);
                

            }
        }
    }
    
    //std::cout<<"STOP"<<std::endl;
    //std::cout<<"Moves over"<<std::endl;
    return moves;
}

//sistemare codice
//fixed
/*
Returns the random coordinates of one of many possible moves
*/
Step rand_moves(Player::piece pawn, Player::piece** board){
    Step move;
    int way = 0;
    int count, num;
    count = num = 0;
    if(pawn == Player::piece::o) way = -1;
    else way = 1;

    Step* make_move = moves(way, pawn, board);

    for(int i = 0;(i < 40) && (make_move[i].move != Player::piece::e); i++)count ++;

    if(count > 0){
        //srand(time(NULL)*time(NULL)/rand());
        num = (std::rand() % count);
        move = make_move[num];
    }

    delete[] make_move;
    return move;
}

/*
Check if two boards are equals
*/
bool are_equal(Player::piece** board1, Player::piece** board2){
    for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE; j++) {
			if(board1[i][j] != board2[i][j]) return false;
		}
	}
    return true;
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

Player::piece** copy_board(Player::piece** temp,Player::piece** copy){
    for(int i = 0; i < SIZE; i ++){
        for(int j = 0; j < SIZE; j++){
            temp[i][j] = copy[i][j];
        }
    }
    return temp;
}

Player::Player(int player_nr){
    if(player_nr != 1 && player_nr != 2){
        throw player_exception{player_exception::index_out_of_bounds, "player_nr must be either 1 or 2"};
    }
    pimpl = new Impl{ nullptr, nullptr, player_nr, 0};
    pimpl->board = newboard();
}

Player::~Player(){
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
}

Player::Player(const Player& copy){
    this->pimpl = new Impl{newboard(), nullptr, copy.pimpl->player_nr, copy.pimpl->board_count};
    Impl* temp = this->pimpl;
    Impl* copytemp = copy.pimpl;

    while(copytemp != nullptr){
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
}

Player& Player::operator=(const Player& copy){
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
        }
        return *this;
    }
}

Player::piece Player::operator()(int r, int c, int history_offset) const{
    int count = -1;
    Impl* temp = this->pimpl;
    while(temp != nullptr){
        temp = temp->next;
        count++;
    }

    if(count < history_offset )
        throw player_exception{player_exception::index_out_of_bounds,"board not found"};
    temp = this->pimpl;
    
    while(count > history_offset){
        temp = temp->next;
        count--;
    }
    return temp->board[r][c];
}

//sistemare codice
//fixed
/*
Load the board from "filename"
*/
void Player::load_board(const std::string& filename){
    Player::piece** board = newboard();
    char c;
    int x_count = 0;
    int o_count = 0;
    int count = 0, i = SIZE - 1, j = 0;

    if(this->pimpl->next == nullptr) this->pimpl->board = newboard();
    Impl* temp = this->pimpl;

    int pc = this->pimpl->board_count;
    while(temp->next) {
        pc++;
        temp = temp->next;
    }

    std::fstream file(filename, std::fstream::in);
    while(file.get(c)) {
        if (i<0){
            clearboard(board);
            throw player_exception{player_exception::invalid_board, "ERROR: invalid board"};
        }
        if(c == 'x' || c == 'X') x_count ++;
        else if(c == 'o' || c == 'O') o_count++;
        if ((i + j) % 2 == 0 && c != ' ' && c != '\n' ){
            clearboard(board);
            throw player_exception{player_exception::invalid_board, "ERROR: invalid board"};
        }
        if(c != '\n'){
            board[i][j] = convert_piece(c);
            j++;
            count++;
            if(j == SIZE){
                j = 0;
                i--;
            }
        }
        if (file.get(c) && c != ' ' && c != '\n' ){
            clearboard(board);
            throw player_exception{player_exception::invalid_board, "ERROR: invalid board"};
        }
        if(count > (SIZE * SIZE)){
            clearboard(board);
            throw player_exception{player_exception::invalid_board, "ERROR: invalid board"};
        }
        if(x_count>12 || o_count>12){
            clearboard(board);
            throw player_exception{player_exception::invalid_board, "ERROR: invalid board"};
        }
    }
    file.close();
    temp = this->pimpl;
    int count2 = this->pimpl->board_count;
    while(temp->next) {
        count2++;
        temp = temp->next;
    }

    temp->next = new Impl{
            newboard(),
            nullptr,
            this->pimpl->player_nr,
            pc + 1
    };

    temp = temp->next;
    temp->board = copy_board(temp->board, board);
    clearboard(board);
}

//sistemare codice
//fixed
/*
store the board in "filename" and in "hystory_offset" hystory postion
*/
void Player::store_board(const std::string& filename, int history_offset) const{
    Impl* temp = this->pimpl;
    int count = -1;
    while(temp != nullptr){
        count = temp->board_count;
        temp = temp->next;
    }
    if(count < history_offset || history_offset < 0 || count + 1 == 0)
        throw player_exception{player_exception::index_out_of_bounds,"board not found"};
    
    temp = this->pimpl;
    int pc = count - history_offset;

    while(temp->board_count != pc){
        temp = temp->next;
    }

    std::fstream file;
    file.open(filename, std::fstream::out);

    for(int i = (SIZE - 1); i >= 0; i--) {
        for(int j = 0; j < SIZE; j++) {
            file << convert_char(temp->board[i][j]);
            if(j != (SIZE - 1))
                file << ' ';
        }
        if(i != 0)
            file << "\n";
    }

    file.close();
}

/*
Initialize a board and store it into "filename"
*/
void Player::init_board(const std::string& filename) const{
    Player::piece** board = newboard();
    for(int i = 0; i < SIZE; i++){
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
            file <<"\n";
    }

    file.close();
    clearboard(board);
}

//sistemare codice
//fixed
/*
insert moves in a board
*/
Player::piece** insert_board(Step moves, Player::piece** const board){
    Player::piece** temp = board;
    temp[moves.begin[0]][moves.begin[1]] = Player::piece::e;
    Player::piece pawn;
    if(moves.move == Player::piece::x)
        if(moves.end[0] == (SIZE - 1)) pawn = Player::piece::X;
        else pawn = Player::piece::x;

    else if(moves.move == Player::piece::o) 
        if(moves.end[0] == 0) pawn = Player::piece::O;
        else pawn = Player::piece::o;

    else pawn = moves.move;
    temp[moves.end[0]][moves.end[1]] == pawn;
    temp[moves.take[0]][moves.take[1]] = Player::piece::e;
    return temp;
}

//sistemare codice
void Player::move(){
    Player::piece** board = newboard();
    Player::piece** result;
    Step move;
    Player::piece pawn;
    Impl* temp = this->pimpl;
    int count = -1;
    while(temp->next){
        temp = temp->next;
    }
    board = copy_board(board, temp->board);
    if(temp->player_nr == 2) pawn = o;
    else pawn = x;
    move = rand_moves(pawn, board);
    
    board = copy_board(board, temp->board);
    result = insert_board(move, board);
    temp = this->pimpl;
    count = this->pimpl->board_count;

    while(temp->next){
        count++;
        temp = temp->next;
    }

    temp->next = new Impl{newboard(), nullptr, temp->player_nr, count+1};
    temp = temp->next;
    temp->board = copy_board(temp->board, result);
    clearboard(board);
}

bool Player::valid_move() const {
    Step* x_move;
	Step* o_move;
    bool check = false;
    Impl* temp1 = this->pimpl;

    while(temp1->next->next){
        temp1 = temp1->next;
    }
    Impl* temp2 = temp1;
    temp1 = temp1->next;

	if(are_equal(temp1->board, temp2->board)){
        return check;
    }
    else{
        Player::piece** start =newboard();
	    Player::piece** end = newboard();
	    Player::piece** copy = newboard();
        start = copy_board(start, temp1->board);
        end = copy_board(end, temp2->board);
        
        x_move = moves(1, Player::piece::x, start);
        for(int i = 0; ((i < 40) && (x_move[i].move != Player::piece::e) && (!check)); i++){
            copy = copy_board(copy, start);
            if(are_equal(end, insert_board(x_move[i], copy))) check = true;
        }
        o_move = moves(-1, Player::piece::o, start);
        for(int i = 0; ((i < 40) && (o_move[i].move != Player::piece::e) && (!check)); i++){
            copy = copy_board(copy, start);
            if(are_equal(end, insert_board(o_move[i], copy))) check = true;
        }
        clearboard(start);
        clearboard(end);
        clearboard(copy);
        delete[] x_move;
        delete[] o_move;
    }
    return check;
}

void Player::pop(){
    Impl* temp = this->pimpl;

    if(temp->next != nullptr){
        while(temp->next->next){
            temp = temp->next;
        }
    }
    clearboard(temp->next->board);
    delete temp->next;
    temp->next = nullptr;
}

int pieces(Player::piece** board, int player_nr){
    int count = 0;
    if(player_nr == 2){
        for(int i = 0; i < SIZE; i++) {
		    for(int j = 0; j < SIZE; j++) {
                if(board[i][j] == Player::piece::x || board[i][j] == Player::piece::X) count ++;
            }
        }
    }
    else{
        for(int i = 0; i < SIZE; i++) {
		    for(int j = 0; j < SIZE; j++) {
                if(board[i][j] == Player::piece::o || board[i][j] == Player::piece::O) count ++;
            }
        }
    }
    return count;
}

bool Player::wins(int player_nr) const{
    int left = pieces(this->pimpl->board, player_nr);
    if(left != 0) return false;
    else return true;
}

bool Player::wins() const{
    return wins(this->pimpl->player_nr);
}

bool Player::loses(int player_nr) const{
    int i;
    if(player_nr == 1) i = 2;
    else i = 1;
    int left = pieces(this->pimpl->board, i);
    if(left != 0) return false;
    else return true;
}

bool Player::loses() const{
    return loses(this->pimpl->player_nr);
}

int Player::recurrence() const{
    Impl* temp = this->pimpl;
    Impl* pc = this->pimpl;
    Player::piece** board;
    int rec = 0;
    while(temp->next){
        temp = temp->next;
    }
    board = temp->board;
    while(pc->next){
        if(are_equal(board, pc->board)) rec++;
        pc = pc->next;
    }
    return rec + 1;
}


int main(){
    int count = 0;
    try{
        
        Player p1(1);
        p1.init_board("board_0.txt");
        Player p2(2);
        p1.load_board("board_0.txt");
        p1.move();
        p1.store_board("board_1.txt");
        p2.load_board("board_1.txt");
        p2.move();
        p2.store_board("board_2.txt");
        int round = 2;
        while(/*p1.valid_move() && p2.valid_move() &&*/ count != 30){
            //std::cout<<"STOP"<<std::endl;
            count++;
            p1.load_board("board_" + std::to_string(round) + ".txt");
            p1.move();
            p1.store_board("board_" + std::to_string(++round) + ".txt");
            p2.load_board("board_" + std::to_string(round) + ".txt");
            p2.move();
            p2.store_board("board_" + std::to_string(++round) + ".txt");
        }

        (p1.wins())? std::cout << "Player 1 wins!" << std::endl : std::cout << "Player 2 wins!" << std::endl;
        p2.store_board("board_exit.txt",0);



    }
    catch(player_exception& e){
        std::cout<<e.msg<<std::endl;
    }
    
    return 0;
}


