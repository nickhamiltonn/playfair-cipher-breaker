#include <stdlib.h>
#include <stdio.h>
#include <limits>
#include <tuple>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <iomanip>
#include <signal.h>
#include <math.h>


#define MAX_RESETS 1000000
#define MAX__CLIMB_ATTEMPTS 1000000
#define STUCK 500
#define LOG_PROBABILITY

double DEFAULT_FREQ = log(1.0/4224127912.0);


// Set that supports getting statistically random elements in the set in O(1). Retrieved from https://leetcode.com/problems/insert-delete-getrandom-o1/discuss/85422/AC-C%2B%2B-Solution.-Unordered_map-%2B-Vector
class RandomizedSet {
public:
    /** Initialize your data structure here. */
    RandomizedSet() {
        
    }
    
    /** Inserts a value to the set. Returns true if the set did not already contain the specified element. */
    bool insert(int val) {
        if (m.find(val) != m.end()) return false;
        nums.emplace_back(val);
        m[val] = nums.size() - 1;
        return true;
    }
    
    /** Removes a value from the set. Returns true if the set contained the specified element. */
    bool remove(int val) {
        if (m.find(val) == m.end()) return false;
        int last = nums.back();
        m[last] = m[val];
        nums[m[val]] = last;
        nums.pop_back();
        m.erase(val);
        return true;
    }
    
    /** Get a random element from the set. */
    int getRandom() {
        return nums[rand() % nums.size()];
    }
private:
    std::vector<int> nums;
    std::unordered_map<int, int> m;
};


struct key_struct_t
{
    char key[5][5];
    std::unordered_map<char, std::tuple<int,int>> key_map;
};

std::string decrypt(key_struct_t* key, const std::string &ciphertext){
    std::string plaintext = "";
    for (int i = 0; i < ciphertext.length(); i += 2){
        auto tuple_0 = key->key_map[ciphertext[i]];
        auto tuple_1 = key->key_map[ciphertext[i+1]];
        
        int row_0 = std::get<0>(tuple_0);
        int col_0 = std::get<1>(tuple_0);

        int row_1 = std::get<0>(tuple_1);
        int col_1 = std::get<1>(tuple_1);

        if (col_0 == col_1){
            int row_0_idx = (5 + ( (row_0-1) % 5)) % 5;
            int row_1_idx = (5 + ( (row_1-1) % 5)) % 5;
            plaintext.append(1,key->key[row_0_idx][col_0]);
            plaintext.append(1,key->key[row_1_idx][col_0]);

        } else if (row_0 == row_1){
            int col_0_idx = (5 + ( (col_0-1) % 5)) % 5;
            int col_1_idx = (5 + ( (col_1-1) % 5)) % 5;
            plaintext.append(1,key->key[row_0][col_0_idx]);
            plaintext.append(1,key->key[row_0][col_1_idx]);
        } else {
            plaintext.append(1,key->key[row_0][col_1]);
            plaintext.append(1,key->key[row_1][col_0]);
        }
    }
    return plaintext;
}

double get_score(const std::string &text,const std::unordered_map<std::string, double> &expected_frequency) {
    double sum = 0;
    #ifdef LOG_PROBABILITY
        for (int i = 0; i < text.length()-4; i++){
            std::string ngram = text.substr(i,4);
            auto iter = expected_frequency.find(ngram);
            if (iter != expected_frequency.end())
                sum += iter->second;
            else {
                sum += DEFAULT_FREQ;
            }
        }
    #else
        std::unordered_map<std::string, double> observed_frequency;
        for (int i = 0; i < text.length()-3; i++){
            observed_frequency[text.substr(i,3)] += 1;
        }
        for (auto& kv: expected_frequency){
            double observed_occurrences = observed_frequency[kv.first] / text.length() / 3.0;
            double diff = observed_occurrences - kv.second;
            double diff_sq = diff*diff;
            sum += diff_sq / kv.second;
        }
    #endif
    return sum;
}


key_struct_t best_key;
std::string ciphertext_g;

key_struct_t gen_key(){
    key_struct_t key_struct;
    RandomizedSet rset;
    for (int c = 'A'; c < 'Z'+1; c++){
        if (c == 'J')
            continue;
        rset.insert(c);
    }

    for (int row = 0; row < 5; row++){
        for (int column = 0; column < 5; column++){
            int rand_char = rset.getRandom();
            key_struct.key[row][column] = rand_char;
            key_struct.key_map[rand_char] = std::make_tuple(row,column);
            rset.remove(rand_char);
        }
    }
    return key_struct;
}

void swap_char(key_struct_t* key, int coords[4]){
    int row_0 = coords[0];
    int col_0 = coords[1];
    int row_1 = coords[2];
    int col_1 = coords[3];

    char c0 = key->key[row_0][col_0];
    char c1 = key->key[row_1][col_1];

    key->key[row_0][col_0] = c1;
    key->key[row_1][col_1] = c0;

    std::get<0>(key->key_map[c0]) = row_1;
    std::get<1>(key->key_map[c0]) = col_1;
    std::get<0>(key->key_map[c1]) = row_0;
    std::get<1>(key->key_map[c1]) = col_0;
}

void swap_row(key_struct_t* key, int row0, int row1){
    int coords[4];

    if (row0 == row1)
        row1 = (row1 + 2) % 5;

    coords[0] = row0;
    coords[2] = row1;

    for (int i = 0; i < 5; i++){
        coords[1] = i;
        coords[3] = i;
        swap_char(key,coords);
    }
}

void swap_col(key_struct_t* key, int col0, int col1){
    int coords[4];

    if (col0 == col1)
        col1 = (col1 + 2) % 5;

    coords[1] = col0;
    coords[3] = col1;
    for (int i = 0; i < 5; i++){
        coords[0] = i;
        coords[2] = i;
        swap_char(key,coords);
    }
}

void flip_y(key_struct_t* key){
    swap_row(key,0,4);
    swap_row(key,1,3);
}

void flip_x(key_struct_t* key){
    swap_col(key,0,4);
    swap_col(key,1,3);
}

void random_mutate(key_struct_t* key, int choice, int coords[4]){
    switch(choice){
        case 0:
            swap_row(key,coords[0],coords[1]);
            break;
        case 1:
            swap_col(key,coords[0],coords[1]);
            break;
        case 2:
            flip_y(key);
            break;
        case 3: 
            flip_x(key);
            break;
        default:
            swap_char(key,coords);
    }
}

double hill_climb(key_struct_t* key, const std::string &ciphertext, const std::unordered_map<std::string, double> &freq){
    double max = -std::numeric_limits<double>::infinity();
    double old_max = -std::numeric_limits<double>::infinity();
    double score;
    int stuck;
    std::cout << "New State" << std::endl;
    for (long attempt = 0; attempt < MAX__CLIMB_ATTEMPTS; attempt++){
        int coords[4];
        for (int i = 0; i < 4; i++){
            coords[i] = rand() % 5;
        }

        int choice = rand() % 45;
        random_mutate(key,choice,coords);
        std::string plaintext = decrypt(key,ciphertext);
        score = get_score(plaintext,freq);
        if (score < max){
            random_mutate(key,choice,coords);
        } else {
            max = score;
        }

        if (max-old_max < 2){
            stuck++;
        } else {
            stuck = 0;
            old_max = max;
        }

        if (stuck > STUCK){
            break;
        }
    }
    return max;
}

std::unordered_map<std::string,double> parse(){
    std::string txt_line;
    long total_occurrences = 0;
    std::unordered_map<std::string, double> trigram_percents;

    // Read from the text file
    std::ifstream MyReadFile("english_quadgrams.txt");

    // Use a while loop together with the getline() function to read the file line by line
    while (std::getline(MyReadFile, txt_line)) {
        int pos = txt_line.find(" ");
        std::string trigram = txt_line.substr(0, pos);
        std::string num_occurrences_str = txt_line.substr(pos + 1);
        int occurrences = std::stoi(num_occurrences_str);
        total_occurrences += occurrences;
        trigram_percents[trigram] = (double) occurrences;
    }
    std::cout << total_occurrences << std::endl;

    for (std::pair<std::string, double> x: trigram_percents) {
        trigram_percents[x.first] = log(x.second / (double) total_occurrences);
    }

    MyReadFile.close();
    return trigram_percents;
}

void signal_handler(int signum){
    std::cout << decrypt(&best_key, ciphertext_g) << std::endl;
    std::cout << "Key" << std::endl;
    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            std::cout << best_key.key[i][j];
        }
    }
    exit(signum);
}

int main(){
    FILE* file = fopen("ct.txt","r");
    int pos = fseek(file,0,SEEK_END);
    long num_char = ftell(file);
    fseek(file,0,SEEK_SET);

    srand(time(NULL));
    
    char ciphertext_s[num_char+1];
    if(size_t read = fread(ciphertext_s,1,num_char,file) != num_char){
        printf("Expected to read %ld characters. Actually read %ld characters",num_char,read);
        return 0;
    }

    ciphertext_s[num_char] = 0;
    ciphertext_g = std::string(ciphertext_s);

    std::string t = "THEREWASNOTHINGSOVERYREMARKABLEINTHATNORDIDALICETHINKITSOVERYMUCHOUTOFTHEWAYTOHEARTHERABBITSAYTOITSELFCOMMAOHDEAROHDEARISHALLBELATEWHENSHETHOUGHTITOVERAFTERWARDSCOMMAITOCCURREDTOHERTHATSHEOUGHTTOHAVEWONDEREDATTHISCOMMABUTATTHETIMEITALLSEEMEDQUITENATURALBUTWHENTHERABBITACTUALLYTOOKAWATCHOUTOFITSWAISTCOATPOCKETCOMMAANDLOOKEDATITCOMMAANDTHENHURRIEDONCOMMAALICESTARTEDTOHERFEETCOMMAFORITFLASHEDACROSSHERMINDTHATSHEHADNEVERBEFORESEENARABBITWITHEITHERAWAISTCOATPOCKETCOMMAORAWATCHTOTAKEOUTOFITCOMMAANDBURNINGWITHCURIOSITYCOMMASHERANACROSSTHEFIELDAFTERITCOMMAANDFORTUNATELYWASJUSTINTIMETOSEEITPOPDOWNALARGERABBITHOLEUNDERTHEHEDGE";


    std::unordered_map<std::string,double> trigram_freq = parse();
    std::cout << get_score(t,trigram_freq) << std::endl;

    double max = -std::numeric_limits<double>::infinity();

    signal(SIGINT,signal_handler);
    
    for (int resets = 0; resets = MAX_RESETS; resets++){
        key_struct_t key = gen_key();
        double score = hill_climb(&key,ciphertext_g,trigram_freq);
        std::cout << max << std::endl;
        if (score > max){
            best_key = key;
            max = score;
        }
    }
    std::cout << decrypt(&best_key, ciphertext_g);
}
