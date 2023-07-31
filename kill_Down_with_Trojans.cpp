#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // For strlen

#include <limits>
#include <iomanip>

constexpr int MAX_N = 100;

constexpr int DAMAGE = 0;
constexpr int HEALING = 1;
constexpr int PROTECTION = 2;
constexpr int MULTIPLIER = 3;
constexpr int INFINITY = std::numeric_limits<int>::max();

struct Tile {
    int t;
    int v;
};

void load_input_file(const std::string& file_name, int& n, int& H, std::vector<std::vector<Tile>>& tiles) {
    std::ifstream file(file_name);
    file >> n >> H;
    
    // Initialize the grid
    tiles.resize(n, std::vector<Tile>(n));

    for (int i = 0; i < n * n; i++) {
        if (i == 0) {
            continue; // The initial tile is zero type with zero value
        }
        Tile tile;
        int x,y;
        file >> x >> y >> tile.t >> tile.v;
        tiles[x][y] = tile;
    }
    
    file.close();
}

void print_tile_data(int n, const std::vector<std::vector<Tile>>& tiles) {
    std::cout << "Tile Types:\n";
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            std::cout << "[" << tiles[x][y].t << "] ";
        }
        std::cout << '\n';
    }

    std::cout << "\nTile Values:\n";
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            std::cout << "[" << tiles[x][y].v << "] ";
        }
        std::cout << '\n';
    }
}

void pretty_print_tiles(int n, const std::vector<std::vector<Tile>>& tiles, 
                            std::vector<std::pair<int, int>> path) {
    std::cout << "\nTiles in Question:\n";

    int i = 0;
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            std::string lb = "[";
            std::string rb = "]";
            if (x == path[i].first && y == path[i].second){
                i++;
                lb = "*";
                rb = "*";
            }

            if (tiles[x][y].t == DAMAGE)
                std::cout << lb << std::setw(3) << -tiles[x][y].v << rb << " ";
            if (tiles[x][y].t == HEALING)
                std::cout << lb << std::setw(3) << tiles[x][y].v << rb << " ";
            if (tiles[x][y].t == PROTECTION)
                std::cout << lb << " P " << rb << " ";
            if (tiles[x][y].t == MULTIPLIER)
                std::cout << lb << " M " << rb << " ";
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}
void show_memo(int n, int**** memo){
    std::cout << "\nMemo:\n";
    for (int x = 0; x < n; x++) {

        for (int y = 0; y < n; y++) {
            std::cout << "[" << memo[x][y][0][0] << "|" << memo[x][y][0][1] << "]  ";
        }
        std::cout << '\n';
        for (int y = 0; y < n; y++) {
            std::cout << "[" << memo[x][y][1][0] << "|" << memo[x][y][1][1] << "]  ";
        }
        std::cout << "\n\n";
    }
}

int top_down_pathfind(int n, const std::vector<std::vector<Tile>>& tiles, 
                        int**** memo,
                        int i, int j, int has_multiplier, int has_protection);

int eval_val(int n, const std::vector<std::vector<Tile>>& tiles, 
                        int**** memo,
                        int new_i, int new_j, int has_multiplier, int has_protection,
                        int tile_type, int tile_val){
    // int tile_type = tiles[i][j].t;
    // int tile_val = tiles[i][j].v;
    if (tile_type == HEALING)
        tile_val *= -1;

    int ret = 0;
    if (has_multiplier && tile_type == HEALING){
        // Either you use or keep
        int used_m = 2 * tile_val + top_down_pathfind(n, tiles, memo, new_i, new_j, 0, has_protection);
        int kept_m =     tile_val + top_down_pathfind(n, tiles, memo, new_i, new_j, 1, has_protection);
        // std::cout << "Used"  << used_m << "\n";
        // std::cout << "Kept"  << kept_m << "\n";
        
        ret = std::min(used_m, kept_m);
    } else if (has_protection && tile_type == DAMAGE){
        // Either you use or keep
        int used_p =             top_down_pathfind(n, tiles, memo, new_i, new_j, has_multiplier, 0);
        int kept_p =  tile_val + top_down_pathfind(n, tiles, memo, new_i, new_j, has_multiplier, 1);
        // std::cout << "Used"  << used_p << "\n";
        // std::cout << "Kept"  << kept_p << "\n";
        
        ret = std::min(used_p, kept_p);
    } else {
        if (tile_type == MULTIPLIER){
            ret = top_down_pathfind(n, tiles, memo, new_i, new_j, 1, has_protection);
        } else if (tile_type == PROTECTION){
            ret = top_down_pathfind(n, tiles, memo, new_i, new_j, has_multiplier, 1);
        } else {
            ret = tile_val + top_down_pathfind(n, tiles, memo, new_i, new_j, has_multiplier, has_protection);
        }
    }
    
    if (ret < 0) 
        ret = 0;

    return ret;
}

int top_down_pathfind(int n, const std::vector<std::vector<Tile>>& tiles, 
                        int**** memo,
                        int i, int j, int has_multiplier, int has_protection){
    // Check your tile
    int tile_type = tiles[i][j].t;
    int tile_val = tiles[i][j].v;

    // We found the value in the memo
    if (memo[i][j][has_protection][has_multiplier] != -1)
        return memo[i][j][has_protection][has_multiplier];

    // We at the bottom right
    if (i == n - 1 && j == n - 1){
        // If I have a protection token, and we at damage, just use it, no reason not to
        if (has_protection && tile_type == DAMAGE){
            memo[i][j][has_protection][has_multiplier] = 0;
        } else {
            memo[i][j][has_protection][has_multiplier] = (tile_type == DAMAGE) ? tile_val : 0;
        }
        return memo[i][j][has_protection][has_multiplier];
    }
        
    // You are at the bottom.
    if (i == n - 1)
        memo[i][j][has_protection][has_multiplier] = eval_val(n, tiles, memo, 
                                                        i, j + 1, has_multiplier, has_protection,
                                                        tile_type, tile_val);

    // You are on the rightmost side
    // -> go down
    else if (j == n - 1)
        memo[i][j][has_protection][has_multiplier] = eval_val(n, tiles, memo, 
                                                        i + 1, j, has_multiplier, has_protection,
                                                        tile_type, tile_val);

    else {
        // Either you go right or down
        int go_righ = eval_val(n, tiles, memo, 
                                    i, j + 1, has_multiplier, has_protection,
                                    tile_type, tile_val);

        int go_down = eval_val(n, tiles, memo, 
                                    i + 1, j, has_multiplier, has_protection,
                                    tile_type, tile_val);

        memo[i][j][has_protection][has_multiplier] = std::min(go_righ, go_down);
    }

    if (memo[i][j][has_protection][has_multiplier] < 0) 
        memo[i][j][has_protection][has_multiplier] = 0;

    return memo[i][j][has_protection][has_multiplier];
}


std::vector<std::pair<int, int>>  checker(int n, const std::vector<std::vector<Tile>>& tiles, 
                        int**** memo){
    int i = 0;
    int j = 0;
    int P = 0;
    int M = 0;

    std::vector<std::pair<int, int>> path;
    while (i < n && j < n){
        path.push_back({i, j});
        // Check your tile
        int tile_type = tiles[i][j].t;
        
        if (tile_type == PROTECTION)
            P = 1;

        if (tile_type == MULTIPLIER)
            M = 1;
        
        if (P == 1 && tile_type == DAMAGE){
            int u_right = (j < n - 1) ? memo[i][j + 1][0][M] : INFINITY;
            int u_down = (i < n - 1) ? memo[i + 1][j][0][M] : INFINITY;
            int use = std::min(u_right, u_down);


            int k_right = (j < n - 1) ? memo[i][j + 1][1][M] : INFINITY;
            int k_down = (i < n - 1) ? memo[i + 1][j][1][M] : INFINITY;
            int keep = std::min(k_right, k_down);

            if (use <= keep){
                P = 0;
            } else {
                P = 1;
            }

        } else if (M == 1 && tile_type == HEALING){
            int u_right = (j < n - 1) ? memo[i][j + 1][P][0] : INFINITY;
            int u_down = (i < n - 1) ? memo[i + 1][j][P][0] : INFINITY;
            int use = std::min(u_right, u_down);


            int k_right = (j < n - 1) ? memo[i][j + 1][P][1] : INFINITY;
            int k_down = (i < n - 1) ? memo[i + 1][j][P][1] : INFINITY;
            int keep = std::min(k_right, k_down);

            if (use <= keep){
                P = 0;
            } else {
                P = 1;
            }
        } 

        int right = (j < n - 1) ? memo[i][j + 1][P][M] : INFINITY;
        int down = (i < n - 1) ? memo[i + 1][j][P][M] : INFINITY;

        if (right <= down){
            j += 1;
        } else {
            i += 1;
        }   
    }
    
    std::cout << "Path: ";
    for (auto p : path){
        std::cout << "(" << p.first  << ", " << p.second << ") ";
    }
    std::cout << "\n";

    return path;
}

bool DP(int n, int H, const std::vector<std::vector<Tile>>& tiles) {
    // Create our n by n by 2 by 2 memo
    int**** memo = new int***[n];
    for (int i = 0; i < n; ++i) {
        memo[i] = new int**[n];
        for (int j = 0; j < n; ++j) {
            memo[i][j] = new int*[2];
            for (int k = 0; k < 2; ++k) {
                memo[i][j][k] = new int[2];
                memo[i][j][k][0] = -1;
                memo[i][j][k][1] = -1;
            }
        }
    }

    // Basically, given the grid, we can calculate the amount of hp
    // needed at every tile to progress to the bottom right. This is
    // fully independent of what HP is at the current state. 

    // Essentially, I abstracted HP out of the problem kek
    int needed_hp = top_down_pathfind(n, tiles, memo, 0, 0, 0, 0);

    // show_memo(n, memo);

    std::cout << "Current HP: " << H << "\n";
    
    std::cout << "Needed HP: " << needed_hp << "\n";
    // pretty_print_tiles(n, tiles, checker(n, tiles, memo));

    // Don't forget to deallocate the memory to avoid memory leaks
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < 2; ++k) {
                delete[] memo[i][j][k];
            }
            delete[] memo[i][j];
        }
        delete[] memo[i];
    }
    delete[] memo;

    return H >= needed_hp;
}


void write_output_file(const std::string& output_file_name, bool result) {
    std::ofstream file(output_file_name);
    file << result;
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " a_file_name.txt\n";
        return 1;
    }

    int n, H;
    std::vector<std::vector<Tile>> tiles;
    load_input_file(argv[1], n, H, tiles);
    print_tile_data(n, tiles);
    bool result = DP(n, H, tiles);
    std::cout << "Result: " << result << '\n';
    
    // Construct the output file name
    std::string output_file_name(argv[1]);
    output_file_name.insert(output_file_name.find_last_of('.'), "_out");
    std::ofstream outfile(output_file_name);
    outfile << result;
    outfile.close();

    return 0;
}
