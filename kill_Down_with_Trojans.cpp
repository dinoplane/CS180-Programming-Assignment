#include <iostream>
#include <fstream>
#include <vector>
#include <cstring> // For strlen

constexpr int MAX_N = 100;

constexpr int DAMAGE = 0;
constexpr int HEALING = 1;
constexpr int PROTECTION = 2;
constexpr int MULTIPLIER = 3;


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
        std::cout << '\n' << '\n';
    }
}

int top_down_pathfind(int n, const std::vector<std::vector<Tile>>& tiles, 
                        int**** memo,
                        int i, int j, int has_multiplier);

int eval_val(int n, const std::vector<std::vector<Tile>>& tiles, 
                        int**** memo,
                        int new_i, int new_j, int has_multiplier, 
                        int tile_type, int tile_val){
    // int tile_type = tiles[i][j].t;
    // int tile_val = tiles[i][j].v;
    if (tile_type == HEALING) 
        tile_val *= -1;

    int ret = 0;
    if (has_multiplier && tile_type == HEALING){
        // Either you go right or down
        int used_m = 2 * tile_val + top_down_pathfind(n, tiles, memo, new_i, new_j, 0);
        int kept_m =     tile_val + top_down_pathfind(n, tiles, memo, new_i, new_j, 1);
        ret = std::min(used_m, kept_m);
    } else {
        ret = tile_val + top_down_pathfind(n, tiles, memo, new_i, new_j, 0);
    }
    
    if (ret < 0) 
        ret = 0;

    return ret;
}

int top_down_pathfind(int n, const std::vector<std::vector<Tile>>& tiles, 
                        int**** memo,
                        int i, int j, int has_multiplier){
    // Check your tile
    int tile_type = tiles[i][j].t;
    int tile_val = tiles[i][j].v;

    if (i == n - 1 && j == n - 1)
        return (tile_type == DAMAGE) ? tile_val : 0;

    // We found the value in the memo
    if (memo[i][j][0][0] != -1)
        return memo[i][j][0][has_multiplier];

    // You are at the bottom.
    if (i == n - 1)
        memo[i][j][0][has_multiplier] = eval_val(n, tiles, memo, 
                                                        i, j + 1, has_multiplier, 
                                                        tile_type, tile_val);

    // You are on the rightmost side
    // -> go down
    else if (j == n - 1)
        memo[i][j][0][has_multiplier] = eval_val(n, tiles, memo, 
                                                        i + 1, j, has_multiplier,
                                                        tile_type, tile_val);

    else {
        // Either you go right or down
        int go_righ = eval_val(n, tiles, memo, 
                                    i, j + 1, has_multiplier,
                                    tile_type, tile_val);

        int go_down = eval_val(n, tiles, memo, 
                                    i + 1, j, has_multiplier, 
                                    tile_type, tile_val);

        memo[i][j][0][has_multiplier] = std::min(go_righ, go_down);
    }

    return memo[i][j][0][has_multiplier];
}

// int top_down_pathfind(int n, const std::vector<std::vector<Tile>>& tiles, 
//                         int**** memo,
//                         int H, int i, int j, 
//                         int has_protection, int has_multiplier){

//     // Base Case:

//     // the memo is filled and is false. 
//     // Note: I spent a significant amount of time thinking about whether 
//     // or not the order of the memo check matters (it does, but it NEEDS
//     // to take place after the damage check), but even so, our runtime 
//     // change is negligible 

//     // Special actions to take

//     // Check your tile
//     int tile_type = tiles[i][j].t;
//     int tile_val = tiles[i][j].v;

//     switch (tile_type){

//         // If damage, use the protection if any else, take damage. 
//         case DAMAGE: {
//             if (has_protection){
//                 has_protection = false;
//             } else {
//                 H -= tile_val;
            
//                 // Check if health < 0 and return false if so.
//                 if (H < 0)
//                     return false;
//             }
//         } break;
        
//         // If heal, use multiplier if have, and heal
//         case HEALING: {
//             if (has_multiplier){
//                 has_multiplier = false;
//                 H += tile_val * 2;
//             } else {
//                 H += tile_val;
//             }
//         } break;

//         case PROTECTION: 
//             has_protection = true;
//             break;
        
//         case MULTIPLIER: 
//             has_multiplier = true;
//             break;
    
//     }

//     // You at the bottom right. 
//     // -> return true cuz you're alive!!!
//     if (i == n - 1 && j == n - 1)
//         return true;

//     // We found the value in the memo
//     if (memo[i][j][has_protection][has_multiplier] != -1)
//         return memo[i][j][has_protection][has_multiplier];
    
//     // You are at the bottom.
//     // -> go right
//     if (i == n - 1)
//         memo[i][j][has_protection][has_multiplier] = top_down_pathfind(n, tiles, memo, H, i    , j + 1, has_protection, has_multiplier);

//     // You are on the rightmost side
//     // -> go down
//     else if (j == n - 1)
//         memo[i][j][has_protection][has_multiplier] = top_down_pathfind(n, tiles, memo, H, i + 1, j    , has_protection, has_multiplier);


//     // Tautology: Either you go right or down
//     else {
//         bool go_righ = top_down_pathfind(n, tiles, memo, H, i    , j + 1, has_protection, has_multiplier);
//         bool go_down = top_down_pathfind(n, tiles, memo, H, i + 1, j    , has_protection, has_multiplier);
//         memo[i][j][has_protection][has_multiplier] = go_righ || go_down;
//     }

//     return memo[i][j][has_protection][has_multiplier];
// }




bool DP(int n, int H, const std::vector<std::vector<Tile>>& tiles) {
    // TODO
    // Placeholder function - implement your logic here
    // Your code to check whether it is possible to reach the bottom-right
    // corner without running out of HP should go here.
    // You should use dynamic programming to solve the problem.
    // Return true if possible, false otherwise.
    
    // int**** memo = new int***[n];
    // for (int i = 0; i < n; ++i) {
    //     memo[i] = new int**[n];
    //     for (int j = 0; j < n; ++j) {
    //         memo[i][j] = new int*[2];
    //         for (int k = 0; k < 2; ++k) {
    //             memo[i][j][k] = new int[2];
    //             memo[i][j][k][0] = -1;
    //             memo[i][j][k][1] = -1;
    //         }
    //     }
    // }

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

    int needed_hp = top_down_pathfind(n, tiles, memo, 0, 0, 0);

    show_memo(n, memo);

    // By default, we return false
    // TODO you should change this
    // bool res = top_down_pathfind(n, tiles, memo, H, 0, 0, 0, 0);


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
