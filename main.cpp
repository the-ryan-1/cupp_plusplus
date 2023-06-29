#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "inipp.h"

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/http/empty_body.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/write.hpp>

#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>

#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>
#include <cstdlib>


using namespace std::literals;
using namespace std::filesystem;
using namespace std;

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = boost::beast::http;       // from <boost/beast/http.hpp>
namespace asio = boost::asio;        // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;           // from <boost/asio/ip/tcp.hpp>
namespace ssl = asio::ssl;       // from <boost/asio/ssl.hpp>



// struct for config
struct {
    vector<string> years;
    vector<char> leet_mode_replacement;
    vector<char> leet_mode_to_replace;
    vector<string> spechars;
    vector<string> nums_to_add;
    int pwd_min_length;
    int pwd_max_length;
    int num_to;
    int num_from;
    int concat_threshold;
}config;


class iniConfig {
private:
    static inipp::Ini<char> read_config() {
        inipp::Ini<char> ini;
        std::ifstream is("/Users/ryan/CLionProjects/cupp++/config.ini");
        if (!is)
            throw std::runtime_error("Config File not found.\n"
                                     "Please make sure config.ini is in your working directory.");
        ini.parse(is);
        return ini;
    }
public:

    //logic for config
    static vector<string> get_years(){
        inipp::Ini<char> ini = read_config();
        vector<string> years{};
        string years_tmp{}, a{};
        inipp::get_value(ini.sections["DEFAULT"], "years", years_tmp);
        for (stringstream sst(years_tmp); getline(sst, a, ',');)
            years.push_back(a);
        return years;
    }
    static tuple<vector<char>, vector<char>> get_leet_mode() {
        inipp::Ini<char> ini = read_config();
        string leet_mode_replacement_tmp{}, leet_mode_to_replace_tmp{}, b{}, c{};
        vector<char> leet_mode_replacement{}, leet_mode_to_replace{};
        inipp::get_value(ini.sections["DEFAULT"], "leet_mode_to_replace", leet_mode_to_replace_tmp);
        inipp::get_value(ini.sections["DEFAULT"], "leet_mode_replacement", leet_mode_replacement_tmp);
        if (leet_mode_to_replace_tmp.size() != leet_mode_replacement_tmp.size())
            throw std::runtime_error("Leet Mode doesn't have a matching number of replacement chars.\n"
                                     "Please adjust leet_mode in config.ini and try again.");
        for (int i=0; i<leet_mode_to_replace_tmp.size(); i++)
            if(leet_mode_to_replace_tmp[i] != ','){
                char s = leet_mode_to_replace_tmp[i];
                leet_mode_to_replace.push_back(s);
            }
        for (int i=0; i<leet_mode_replacement_tmp.size(); i++)
            if(leet_mode_replacement_tmp[i] != ','){
                char s = leet_mode_replacement_tmp[i];
                leet_mode_replacement.push_back(s);
            }
        return std::make_tuple(leet_mode_to_replace, leet_mode_replacement);
    }
    static vector<string> get_spechars() {
        inipp::Ini<char> ini = read_config();
        string spechars_tmp{}, d{};
        vector<string> spechars{};
        inipp::get_value(ini.sections["DEFAULT"], "spechars", spechars_tmp);
        for (stringstream sst(spechars_tmp); getline(sst, d, ',');)
            spechars.push_back(d);
        return spechars;
    }
    static tuple<int, int> get_pwd_length(){
        inipp::Ini<char> ini = read_config();
        int pwd_min_length{};
        inipp::get_value(ini.sections["DEFAULT"], "pwd_min_length", pwd_min_length);
        int pwd_max_length{};
        inipp::get_value(ini.sections["DEFAULT"], "pwd_max_length", pwd_max_length);
        return std::make_tuple(pwd_min_length, pwd_max_length);
    }
    static int get_concat_threshold(){
        inipp::Ini<char> ini = read_config();
        int word_concat_threshold{};
        inipp::get_value(ini.sections["DEFAULT"], "threshold", word_concat_threshold);
        return word_concat_threshold;
    }
    static tuple<int,int> get_num_from_to(){
        inipp::Ini<char> ini = read_config();
        int num_from{};
        inipp::get_value(ini.sections["DEFAULT"], "num_from", num_from);
        //std::cout << "Num_from: " << num_from << std::endl;
        int num_to{};
        inipp::get_value(ini.sections["DEFAULT"], "num_to", num_to);
        return make_tuple(num_from, num_to);
    }
    static void setConfig(){
        config.years = iniConfig::get_years();
        //for (size_t i{0}; i < years.size(); i++)
        //    std::cout << years[i] << endl;
        tie(config.leet_mode_to_replace, config.leet_mode_replacement) = iniConfig::get_leet_mode();
        //for (size_t i{0}; i < leet_mode_to_replace.size(); i++)
        //    std::cout << leet_mode_to_replace[i] << " becomes: " << leet_mode_replacement[i] << std::endl;
        config.spechars = iniConfig::get_spechars();
        //for (size_t i{0}; i < spechars.size(); i++)
        //    std::cout << "Special Chars: " << spechars[i] << std::endl;
        tie(config.pwd_min_length, config.pwd_max_length) = iniConfig::get_pwd_length();
        //std::cout << "Password min: " << pwd_min_length << "\nPassword Max: " << pwd_max_length << std::endl;
        tie(config.num_from, config.num_to) = iniConfig::get_num_from_to();
        for (int i = config.num_from; i <= config.num_to; i++){
            config.nums_to_add.push_back(to_string(i));
        }
        //std::cout << "Num from: " << num_from << "\nNum to: " << num_to << std::endl;
        config.concat_threshold = iniConfig::get_concat_threshold();
        //std::cout << "Concat threshold: " << concat_threshold << std::endl;
    }



    //
    //saving this incase I need to reuse some of this logic for switching modes.
    //
    //std::cout << "raw ini file:" << std::endl;
    //ini.generate(std::cout);
    //ini.default_section(ini.sections["DEFAULT"]);
    //ini.interpolate();
    //std::cout << "ini file after default section and interpolation:" << std::endl;
    //ini.generate(std::cout);
};

//improve wordlist functions
string concat_list_on_right(string og_words, string concat);
vector<string> concat_list_on_right(vector<string> og_words, vector<string> concat);
void add_spechars(vector<string> og_words, char  location_spechars, unordered_set<string> *output_ptr);
void makeleet_long(unordered_set<string> *output_ptr);
void makeleet_short(unordered_set<string> *output_ptr);
void permute(string a, unordered_map<int,char> replacement_map, unordered_set<string> *output_ptr, unordered_map<int, char>:: iterator i, bool first_run);
bool pass_too_short(string word, int adding);
bool pass_too_long(string word, int adding);
string printNTimes(char c, int n);


//interactive mode functions
void permute_dates(unordered_set<string> *output, unordered_set<string>:: iterator i, bool first_run = true, string current_word = "");
unordered_set<string> concat_list_on_right(unordered_set<string> og_words, unordered_set<string> concat);
void parse_name(string name, unordered_set<string> *output);
void parse_spaces (string input, unordered_set<string> *output);
void combine_two_sets(unordered_set<string> *input, unordered_set<string> *input2,unordered_set<string> *output_of_func, unordered_set<string> *output_final);
void add_spechars(string location_spechars, unordered_set<string> *final_words_tmp, vector<string> spechars_vec);
vector<string> build_spechars();







int improve_dictionary(string file_path);
int interactive_mode_input();
void interactive_mode_generate(unordered_map<string,string> profile);
vector<string> download_http(string url, string targetfile);
int alectodb_download();
int download_wordlist();
int help_menu();
int version();


int main(int argc, char* argv[]) {
    cout << "   █████████  █████  █████ ███████████  ███████████                         \n"
            "  ███░░░░░███░░███  ░░███ ░░███░░░░░███░░███░░░░░███     ███         ███    \n"
            " ███     ░░░  ░███   ░███  ░███    ░███ ░███    ░███    ░███        ░███    \n"
            "░███          ░███   ░███  ░██████████  ░██████████  ███████████ ███████████\n"
            "░███          ░███   ░███  ░███░░░░░░   ░███░░░░░░  ░░░░░███░░░ ░░░░░███░░░ \n"
            "░░███     ███ ░███   ░███  ░███         ░███            ░███        ░███    \n"
            " ░░█████████  ░░████████   █████        █████           ░░░         ░░░     \n"
            "  ░░░░░░░░░    ░░░░░░░░   ░░░░░        ░░░░░                                \n"
            "                                                                            \n"
            "                                                                            \n"
            "                   Common User Password Profiler ++                         \n"
            << endl;
    auto start = std::chrono::system_clock::now();

    iniConfig::setConfig();

    //get run mode from cmd line arguments
    for(int i=0;i<argc;i++)
    {
        if(string(argv[i]) == "-h" || string(argv[i]) == "--help")
        {
            cout << "List of commands:\n"
                    "-w filename.txt    Improve Existing File\n"
                    "-i                 Interactive\n"
                    "-d                 Download Lists\n"
                    << endl;
        }
        if(string(argv[i]) == "-w" || string(argv[i]) == "-W"){
            string og_file {string(argv[++i])};
            improve_dictionary(og_file);
        }
        if(string(argv[i]) == "-i" || string(argv[i]) == "-I"){
            interactive_mode_input();
        }
        if (string(argv[i]) == "-d" || string(argv[i]) == "-D"){
            alectodb_download();
        }
    }
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    std::cout << elapsed.count() << '\n';

    return 0;
}
//For SSL  Requests
tcp::resolver::results_type resolve(asio::io_context& ctx, std::string const& hostname){
    tcp::resolver resolver{ctx};
    return resolver.resolve(hostname, "https");
}

tcp::socket connect(asio::io_context& ctx, std::string const& hostname){
    tcp::socket socket{ctx};
    asio::connect(socket, resolve(ctx, hostname));
    return socket;
}

std::unique_ptr<ssl::stream<tcp::socket>> connect(asio::io_context& ctx, ssl::context& ssl_ctx, std::string const& hostname){
    auto stream = boost::make_unique<ssl::stream<tcp::socket>>(
            connect(ctx, hostname), ssl_ctx);
    // tag::stream_setup_source[]
    boost::certify::set_server_hostname(*stream, hostname);
    boost::certify::sni_hostname(*stream, hostname);
    // end::stream_setup_source[]

    stream->handshake(ssl::stream_base::handshake_type::client);
    return stream;
}

vector<string> get(ssl::stream<tcp::socket>& stream, boost::string_view hostname, boost::string_view uri){
    http::request<http::empty_body> request;
    request.method(http::verb::get);
    request.target(uri);
    request.keep_alive(false);
    request.set(http::field::host, hostname);
    http::write(stream, request);

    http::response<http::string_body> response;
    beast::flat_buffer buffer;
    http::read(stream, buffer, response);

    //We're only interested in the response body. So we get that and parse newlines into a vector.
    string body = response.body();
    vector<string> results{};
    string result{};
    //this works for txt files, for gz files I need to skip the for loop, return body, gunzip and write as txt.
    for (int i = 0; i < body.length(); i++){
        result = result + body[i];
        if (body[i] == '\n'){
            results.push_back(result);
            result = "";
        }
    }
    return results;
}

int improve_dictionary(string file_path){
/************************************
 *                                  *
 *     Improve Dictionary Mode      *
 *           User Options           *
 *                                  *
 ************************************/

//TODO: ADD INVALID INPUT HANDLING
//TODO: CLEAN UP OLD COMMENTS

    ifstream fin(file_path, ios::in);
    if (!fin.is_open()) {
        cout << "Error file not found" << endl;
        return -1;
    }


    string line;
    bool incl_concat {false};
    char concat_ans{};
    cout << "Do you want to concatenate words from the list? Y/[N] ";
    cin >> concat_ans;
    concat_ans = toupper(concat_ans);
    cout << endl;
    if(concat_ans == ' ')
        concat_ans = 'N';
    while (concat_ans != 'Y' && concat_ans != 'N'){
        cout << "Invalid input. Input Y for yes or N for no. \nDo you want to concatenate words from the list? Y/[N] ";
        cin >> concat_ans;
        cout << endl;
        concat_ans = toupper(concat_ans);
    }
    if (concat_ans == 'Y'){
        incl_concat = true;
        cout << "Maximum number of words that will be concatenated for each "
                "word is " << config.concat_threshold << "\nThis can be adjusted in config.ini" << endl;
    }

    bool incl_randnum{false};
    string randnum_ans;
    cout << "Would you like to include random numbers to end of words? Y/[N] ";
    cin >> randnum_ans;
    cout << endl;
    if (randnum_ans == "Y" || randnum_ans == "y"){
        incl_randnum = true;
    }

    bool incl_years{false};
    char years_ans;
    cout << "Would you like to include years from the config file to the end of words? Y/[N] ";
    cin >> years_ans;
    years_ans = toupper(years_ans);
    cout << endl;
    if (years_ans == 'Y'){
        incl_years = true;
    }

    bool incl_special_chars{false}, special_chars_prepend{false}, special_chars_append{false},
    special_chars_bookend{false}, special_chars_all{false};
    char loc_spechars_selection{};

    string spechars_answer;
    cout << "Do you want to add special characters? Y/[N] ";
    cin >> spechars_answer;
    cout << endl;

    if (spechars_answer == "Y" || spechars_answer == "y"){
        incl_special_chars = true;
        cout << "Would you like to 1. Prepend, 2. Append, 3. Bookend special characters or 4. All of the above? 1/[2]/3/4 ";
        cin >> loc_spechars_selection;
        cout << endl;
        if (loc_spechars_selection == ' ' || loc_spechars_selection == 'Y'){
            special_chars_append = true;
            loc_spechars_selection = '2';
        }
        while (loc_spechars_selection != '1' && loc_spechars_selection != '2' && loc_spechars_selection != '3' && loc_spechars_selection != '4' ){
                cout << "Invalid input. Input a number corresponding to the desired operation. "
                        "\nWould you like to 1. Prepend, 2. Append, 3. Bookend special characters or 4. All of the above? 1/2/3/4 ";
                cin >> loc_spechars_selection;
                cout << endl;
            }
        }



    bool incl_leet_basic{false}, incl_leet_comprehensive{false};
    string leet_ans;
    cout << "Convert characters to leet(1337)? Y/[N] ";
    cin >> leet_ans;
    cout << endl;
    if (leet_ans == "y" || leet_ans == "Y"){
        cout << "Comprehensive leet mode can change each character instead of it being all or nothing\n"
                "(ie leet becomes all possible permutations: 1eet, 13et, 133t, l3et, l33t... etc)\nComprehensive leet mode? Y/[N] ";
        string comprehensive_leet_ans{};
        cin >> comprehensive_leet_ans;
        cout << endl;
        if (comprehensive_leet_ans == "y" || comprehensive_leet_ans == "Y"){
            incl_leet_comprehensive = true;
        }else{
            incl_leet_basic = true;
        }
    }
    if (incl_concat && incl_leet_basic && incl_special_chars && incl_leet_comprehensive && incl_randnum){
        cout << "No improvements selected. Exiting Program." << endl;
        return 0;
    }

/************************************
 *                                  *
 *     Improve Dictionary Mode      *
 *        Build Improvements        *
 *                                  *
 ************************************/

    vector<string> og_words_tmp{};
    unordered_set <string> final_words_tmp{};
    while(getline(fin, line)) {
            og_words_tmp.push_back(line);
    }

    //first include original words in final list and make new const vector for sending list to other functions
    //must be done this way or else you risk losing words that would meet pwd length requirements after functions
    vector<string> og_words = og_words_tmp;
    for (int i = 0; i < og_words_tmp.size(); i++){
        if (pass_too_long(og_words_tmp[i],0))
            continue;
        if (!pass_too_short(og_words_tmp[i],0))
            final_words_tmp.insert(og_words_tmp[i]);
    }
    //final_words_tmp.push_back(og_words);
    fin.close();

    vector<string> concat_output_vec{};
    vector<string> randnum_output_vec{};
    vector<string> years_output_vec{};
    vector<string> spechars_output_vec{};


    cout << "Generating started." <<endl;
    if (incl_concat){
        cout << "Concatenating words" <<endl;
        string concat_output{};
        int threshold = config.concat_threshold < og_words.size() ? config.concat_threshold : og_words.size();
        for (string word:og_words){
            for (int j = 0; j <= threshold; j++){
                concat_output = concat_list_on_right(word, og_words[j]);
                final_words_tmp.insert(concat_output);
                concat_output_vec.push_back(concat_output);
            }
        }
    }

    if (incl_randnum){
        cout << "Including random numbers to original words" << endl;
        string word_with_randnum{};
        for (string word:og_words){
            for (int i = 0; i < config.nums_to_add.size(); i++){
                word_with_randnum = concat_list_on_right(word, config.nums_to_add[i]);
                final_words_tmp.insert(word_with_randnum);
                randnum_output_vec.push_back(word_with_randnum);
            }
        }
        if (incl_concat){
            cout << "Adding random numbers to words concatenated." << endl;
            for(auto word:concat_output_vec) {
                for (int i = 0; i < config.nums_to_add.size(); i++) {
                    word_with_randnum = concat_list_on_right(word, config.nums_to_add[i]);
                    final_words_tmp.insert(word_with_randnum);
                    randnum_output_vec.push_back(word_with_randnum);
                }
            }
        }
    }

    if (incl_years){
        cout << "Adding years to original words" << endl;
        string word_with_years{};
        for (string word:og_words){
            for (int i = 0; i < config.years.size(); i++){
                word_with_years = concat_list_on_right(word, config.years[i]);
                final_words_tmp.insert(word_with_years);
                years_output_vec.push_back(word_with_years);
            }
        }
        if (incl_concat){
            cout << "Adding Years to Words Concatenated." << endl;
            for(auto word:concat_output_vec) {
                for (int i = 0; i < config.years.size(); i++) {
                    word_with_years = concat_list_on_right(word, config.years[i]);
                    final_words_tmp.insert(word_with_years);
                    years_output_vec.push_back(word_with_years);
                }
            }
        }
    }

    if (incl_special_chars){
        add_spechars(og_words, loc_spechars_selection, &final_words_tmp);
        if (incl_concat){
            cout << "Adding special characters to concatenated words." << endl;
            add_spechars(concat_output_vec, loc_spechars_selection,&final_words_tmp);
        }
       if (incl_randnum){
           cout << "Adding special characters to words with random numbers." << endl;
           add_spechars(randnum_output_vec, loc_spechars_selection,&final_words_tmp);

       }
       if (incl_years){
           cout << "Adding special characters to words with years." << endl;
           add_spechars(years_output_vec, loc_spechars_selection,&final_words_tmp);
       }
    }

    if (incl_leet_comprehensive){
        cout << "Adding all possible permutations of leet mode.\nThis may take awhile." << endl;
        makeleet_long(&final_words_tmp);
    }

    if (incl_leet_basic){
        makeleet_short(&final_words_tmp);
    }
    cout << "Writing output file." << endl;



    ofstream outputfile;
    outputfile.open("open2.txt");

    cout << "Final list length : " << final_words_tmp.size() << endl;
    for (auto const &i:final_words_tmp){
        if (i.size() <= config.pwd_max_length && i.size() >= config.pwd_min_length)
        outputfile << i << endl;
    }
    return 0;
}

int interactive_mode_input(){

/************************************
 *                                  *
 *          Interactive Mode        *
 *           User Options           *
 *                                  *
 ************************************/



    cout << "\n\n\nInsert information about the target" << endl;
    cout <<" [+] If you don't know all of the info, just hit enter when asked." << endl;
    cout <<" [+] Special Characters, Years, Numbers, Password Min/Max, and Concatination can be editted without recompiling using config.ini" << endl;


    unordered_map<string, string> profile{};
    string permute_level{};
    string osint_level{}, name{}, surname{}, nick{}, birthday{}, spouse{}, spouse_nick{}, spouse_bd{}, kid1{}, kid1_nick{}, kid1_bd{},
    kid2{}, kid2_nick{}, kid2_bd{},pet{}, company{}, address{}, address2{}, city{}, school{}, words{};

    bool keywords{false}, spechars{false}, randnum{false}, years{false},
    leet_mode{false}, leet_basic{false}, leet_comprehensive{false};

    cout << "\n\nDo you know a lot [1] or a little [2] about the target: 1/[2] ";
    getline(cin, osint_level);
    if (osint_level == "1")
        // I can edit this. It is much faster now.
        cout << "It is recommend you either fill in only the most important fields or do not use default settings."
                "\nUsing the default settings with only first name, last name, nickname, and birthday for 1 person plus "
                "as many special characters as possible will take a considerable amount of time and "
                "result in over 10 million combinations at permute level 5."
                "\n\nPlease be selective.\n\n" << endl;


    if (osint_level == "2")
        // I can edit this. It is much faster now.
        cout << "\nEven with only a little information CUPP can make enormous lists. Knowning only"
                " one person's first name, last name, nickname, birthday for \n"
                "and adding as many special characters as possible at permute level 5, CUPP "
                "\nwill result in over 10 million combinations and take a considerable amount of time."
                "\n\nPlease consider this when selecting permute level.\n\n" << endl;

    cout << "Permute level / number of transformations & combinations? (Higher is more combos & slower) [1]/2/3: ";
    getline(cin, permute_level);
    if (permute_level.empty())
        permute_level = "1";
    if(permute_level == "3"){
        cout << "Last chance to back out now. 1. I've got all day / I'm not going to add many details. Let's do this!"
                "2 I want to choose a different level. 1/2: ";
        string cant_say_I_didnt_warn_you{};
        getline(cin, cant_say_I_didnt_warn_you);

        if (cant_say_I_didnt_warn_you == "2"){
            permute_level = "";
            cout << "Permute level / number of transformations & combinations? (Higher is more combos & slower) [1]/2/3: ";
            getline(cin, permute_level);
            if (!permute_level.empty())
                permute_level = "1";
        }
    }
    profile.emplace("permute_level", permute_level);




    cout << "First Name: ";
    getline(cin, name);
    while (name.length() < 1){
        cout << "This is the only required field. If you don't know enter a company or other items here." << endl;
        cout << "First Name: ";
        getline(cin, name);
        cout << endl;
    }
    profile.emplace("name", name);

    cout << "Surname: ";
    getline(cin, surname);
    if (!surname.empty())
        profile.emplace("surname", surname);


    cout << "Nickname: ";
    getline(cin, nick);
    if (!nick.empty())
        profile.emplace("nick", nick);


    cout << "Birthday (DDMMYYYY): ";
    getline(cin, birthday);
    while (birthday.size() != 0 && birthday.size() != 8){
        cout << "You must enter 8 digits for birthday!" << endl;
        cout << "Birthday (DDMMYYYY): ";
        getline(cin, birthday);
    }
    if (!birthday.empty())
        profile.emplace("birthday", birthday);

    cout << "Partner's Name: ";
    getline(cin, spouse);
    if (!spouse.empty())
        profile.emplace("spouse", spouse);

    cout << "Partner's Nickname: ";
    getline(cin, spouse_nick);
    if (!spouse_nick.empty())
        profile.emplace("spouse_nick", spouse_nick);

    cout << "Partner's Birthday (DDMMYYYY): ";

    getline(cin, spouse_bd);
    while (spouse_bd.size() != 0 && spouse_bd.size() != 8){
        cout << "You must enter 8 digits for birthday!" << endl;
        cout << "Partner's Birthday (DDMMYYYY): ";
        getline(cin, spouse_bd);
    }
    if (!spouse_bd.empty())
        profile.emplace("spouse_bd", spouse_bd);

    cout << "Child 1 Name: ";
    getline(cin, kid1);
    if (!kid1.empty())
        profile.emplace("kid1", kid1);

    cout << "Child 1 Nickname: ";
    getline(cin, kid1_nick);
    if (!kid1_nick.empty())
        profile.emplace("kid1_nick", kid1_nick);

    cout << "Child 1 Birthday (DDMMYYYY): ";
    getline(cin, kid1_bd);
    while (kid1_bd.size() != 0 && kid1_bd.size() != 8){
        cout << "You must enter 8 digits for birthday!" << endl;
        cout << "Child 1 Birthday (DDMMYYYY): ";
        getline(cin, kid1_bd);
    }
    if (!kid1_bd.empty())
        profile.emplace("kid1_bd", kid1_bd);

    cout << "Child 2 Name: ";
    getline(cin, kid2);
    if (!kid2.empty())
        profile.emplace("kid2", kid2);

    cout << "Child 2 Nickname: ";
    getline(cin, kid2_nick);
    if (!kid2_nick.empty())
        profile.emplace("kid2_nick", kid2_nick);

    cout << "Child 2 Birthday (DDMMYYYY): ";
    getline(cin, kid2_bd);
    while (kid2_bd.size() != 0 && kid2_bd.size() != 8){
        cout << "You must enter 8 digits for birthday!" << endl;
        cout << "Child 2 Birthday (DDMMYYYY): ";
        getline(cin, kid2_bd);
    }
    if (!kid2_bd.empty())
        profile.emplace("kid2_bd", kid2_bd);

    cout << "Company: ";
    getline(cin, company, '\n');
    if (!company.empty())
        profile.emplace("company", company);

    cout << "Street Address: ";
    getline(cin, address, '\n');
    if (!address.empty())
        profile.emplace("address", address);

    cout << "Street Address 2: ";
    getline(cin, address2, '\n');
    if (!address2.empty())
        profile.emplace("address2", address2);

    cout << "City: ";
    getline(cin, city, '\n');
    if (!city.empty())
        profile.emplace("city", city);

    cout << "School: ";
    getline(cin, school, '\n');
    if (!school.empty())
        profile.emplace("school", school);

    cout << "Pet's Name: ";
    getline(cin, pet);
    if (!pet.empty())
        profile.emplace("pet", pet);

    cout << "Do you want to entered some keywords about the target? Y/[N]: ";
    string keywords_input{};
    getline(cin, keywords_input);
    if (keywords_input.empty())
        keywords = false;
    if (keywords_input == "Y" || keywords_input == "y"){
        keywords = true;
        cout << "Please enter the words seperated by a space: ";
        getline(cin, keywords_input, '\n');
        profile.emplace("keywords", keywords_input);
    }

    cout << "Do you want to add special characters? Y/[N]: ";
    string spechars_input{};
    getline(cin, spechars_input);
    if (spechars_input.empty())
        spechars = false;
    string spechars_loc{};
    if (spechars_input == "y" || spechars_input == "Y") {
        spechars = true;
        profile.emplace("spechars", "true");
        cout << "Do you want to add them to the 1. front, 2. back, 3. both sides, or 4. as many combinations as possible? 1/[2]/3/4 ";
        getline(cin, spechars_loc, '\n');
        if (spechars_loc.length() == 0)
            spechars_loc = '2';
        while (spechars_loc[0] != '1' && spechars_loc[0] != '2' && spechars_loc[0] != '3' && spechars_loc[0] != '4') {
            cout << "Invalid input. You must enter a number." << endl;
            cout << "Do you want to add them to the 1. front, 2. back, 3. both sides, or 4. as many combinations as possible? 1/[2]/3/4 ";
            getline(cin, spechars_loc, '\n');
            if (spechars_loc.length() == 0)
                spechars_loc = '2';
        }
        profile.emplace("spechars_loc", spechars_loc);

    }

    cout << "Do you want to add some random numbers to the end of words? Y/[N]: ";
    string randnum_input{};
    getline(cin, randnum_input);
    if (randnum_input.empty())
        randnum = false;
    if (randnum_input == "y" || randnum_input == "Y")
        profile.emplace("randnum","true");

    cout << "Do you want to add some years to the end of words? Y/[N]: ";
    string years_input{};
    getline(cin, years_input);
    if (years_input.empty())
        years = false;
    if (years_input == "y" || years_input == "Y")
        profile.emplace("years","true");

    cout << "Leet mode (i.e. leet = 1337)? Y/[N]: ";
    string leet_input{};
    getline(cin, leet_input);
    if (leet_input.empty())
        leet_mode = false;
    if (leet_input == "y" || leet_input == "Y")
        leet_mode = true;
    if (leet_mode) {
        cout << "Create as many permutations of leet as possible \n"
                "leet becomes 24 different combinations \n"
                "(i.e. lee7, le37,l337, 1337, 1ee7, l33t...)? Y/[N] ";
        string leet_type_input{};
    
        getline(cin, leet_type_input);
        if (leet_type_input.empty() || leet_type_input == "n" || leet_type_input == "N")
            profile.emplace("leet_basic","true");
        leet_basic = true;
        if (leet_type_input == "Y" || leet_type_input == "y")
            profile.emplace("leet_comprehensive","true");
        }

    interactive_mode_generate(profile);


    return 0;
    }

void interactive_mode_generate(unordered_map<string, string> profile){
/************************************
 *                                  *
 *          Interactive Mode        *
 *           Build Lists            *
 *                                  *
 ************************************/
//I think what I want to do is create as many permutations as possible and see how it lines up with the original.
//probably should make a special permute function and pass profile too it.

cout << endl;
cout << "\n\nMaking a dictionary" << endl;
//make unordered set for sending to make permutations
unordered_set<string> name_parsed{}, profile_parsed{};
unordered_set<string> name_combos{};
unordered_set<string> bday_parsed{};
unordered_set<string> bday_combos{};
unordered_set<string> final_output{};
unordered_set<string> target_parsed{};
unordered_set<string> spouse_parsed{};
unordered_set<string> kid1_parsed{};
unordered_set<string> kid2_parsed{};
unordered_set<string> misc_parsed{};
unordered_set<string> profile_combo{};
unordered_set<string> name_bd_combos{};

int permute_level{};

permute_level = stoi(profile.at("permute_level"));


void parse_birthday(string bd_full, unordered_set<string> *output);


//maybe I should iterate through the unordered map and then set based on value found instead of calling find a dozen plus times.

//put all possible relevant dates from birthdays in a set
if (profile.find("birthday") != profile.end())
    parse_birthday(profile.at("birthday"), &bday_parsed);


if (profile.find("spouse_bd") != profile.end())
    parse_birthday(profile.at("spouse_bd"), &bday_parsed);

if (profile.find("kid1_bd") != profile.end())
     parse_birthday(profile.at("kid1_bd"), &bday_parsed);

if (profile.find("kid2_bd") != profile.end())
    parse_birthday(profile.at("kid2_bd"), &bday_parsed);

bool profile_change = false;

if (permute_level == 1)
    if (profile.find("name") != profile.end()){
        parse_name(profile.at("name"), &profile_combo);
        profile_change = true;
    }

    if (profile.find("surname") != profile.end())
        parse_name(profile.at("surname"), &profile_combo);

    if (profile.find("nick") != profile.end()) {
        parse_name(profile.at("nick"), &profile_combo);
        bool profile_change = true;
    }

    if (profile_change){
        combine_two_sets(&profile_combo,&profile_combo, &profile_parsed, &final_output);
        combine_two_sets(&profile_combo,&bday_parsed, &name_bd_combos, &final_output);
        profile_combo = {};
        profile_change = false;
    }



    if (profile.find("spouse") != profile.end()){
        if (profile.find("surname") != profile.end())
            parse_name(profile.at("surname"), &profile_combo);
        parse_name(profile.at("spouse"), &profile_combo);
        profile_change = true;
    }
    if (profile.find("spouse_nick") != profile.end()) {
        parse_name(profile.at("spouse_nick"), &profile_combo);
        profile_change = true;
    }
    if (profile_change){
        combine_two_sets(&profile_combo,&profile_combo, &profile_parsed, &final_output);
        combine_two_sets(&profile_combo,&bday_parsed, &name_bd_combos, &final_output);
        profile_combo = {};
        profile_change = false;
    }



    if (profile.find("kid1") != profile.end()){
        if (profile.find("surname") != profile.end())
            parse_name(profile.at("surname"), &profile_combo);
        parse_name(profile.at("kid1"), &profile_combo);
        profile_change = true;
    }
    if (profile.find("kid1_nick") != profile.end()) {
        parse_name(profile.at("kid1_nick"), &profile_combo);
        profile_change = true;
    }
    if (profile_change){
        combine_two_sets(&profile_combo,&profile_combo, &profile_parsed, &final_output);
        combine_two_sets(&profile_combo,&bday_parsed, &name_bd_combos, &final_output);
        profile_combo = {};
        profile_change = false;
    }

    if (profile.find("kid2") != profile.end()){
        if (profile.find("surname") != profile.end())
            parse_name(profile.at("surname"), &profile_combo);
        parse_name(profile.at("kid2"), &profile_combo);
        profile_change = true;
    }
    if (profile.find("kid2_nick") != profile.end()) {
        parse_name(profile.at("kid2_nick"), &profile_combo);
        profile_change = true;
    }
    if (profile_change){
        combine_two_sets(&profile_combo,&profile_combo, &profile_parsed, &final_output);
        combine_two_sets(&profile_combo,&bday_parsed, &name_bd_combos, &final_output);
        profile = {};
        profile_change = false;
    }

//put all possible names in a set
if (permute_level >= 3){
    if (profile.find("name") != profile.end())
        parse_name(profile.at("name"), &name_parsed);

    if (profile.find("surname") != profile.end())
        parse_name(profile.at("surname"), &name_parsed);

    if (profile.find("nick") != profile.end())
        parse_name(profile.at("nick"), &name_parsed);

    if (profile.find("spouse") != profile.end())
        parse_name(profile.at("spouse"), &name_parsed);

    if (profile.find("spouse_bd") != profile.end())
        parse_name(profile.at("spouse_bd"), &name_parsed);

    if (profile.find("spouse_nick") != profile.end())
        parse_name(profile.at("spouse_nick"), &name_parsed);

    if (profile.find("kid1") != profile.end())
        parse_name(profile.at("kid1"), &name_parsed);

    if (profile.find("kid1_nick") != profile.end())
        parse_name(profile.at("kid1_nick"), &name_parsed);

    if (profile.find("kid2") != profile.end())
        parse_name(profile.at("kid2"), &name_parsed);

    if (profile.find("kid2_nick") != profile.end())
        parse_name(profile.at("kid2_nick"), &name_parsed);
    if (profile.find("kid2_bd") != profile.end())
        parse_birthday(profile.at("kid2_bd"), &bday_parsed);
}

/* This works but is super slow. Might resuse for higher permutation levels but commenting out for testing for now.
combine_two_sets(&bday_parsed, &bday_parsed,&bday_combos, &final_output);
if (permute_level >= 2)
    combine_two_sets(&bday_parsed, &bday_combos,&bday_combos, &final_output);
combine_two_sets(&name_parsed,&name_parsed, &name_combos, &final_output);
//super slow if i add this
// combine_two_sets(&name_parsed,&name_combos, &name_combos, &final_output);

combine_two_sets(&bday_combos,&name_combos, &name_bd_combos, &final_output);
if (permute_level >= 3)
    combine_two_sets(&name_bd_combos,&bday_combos, &name_bd_combos, &final_output);
*/

//debug lines

cout << "final bday+names combos output size: "<< final_output.size() << endl;

if (profile.find("keywords") != profile.end())
    parse_spaces(profile.at("keywords"), &misc_parsed);

if (profile.find("pet") != profile.end())
    parse_name(profile.at("pet"), &misc_parsed);

if (profile.find("company") != profile.end())
    parse_spaces(profile.at("company"), &misc_parsed);

if (profile.find("address") != profile.end())
    parse_spaces(profile.at("address"), &misc_parsed);

if (profile.find("address2") != profile.end())
    parse_spaces(profile.at("address2"), &misc_parsed);

if (profile.find("city") != profile.end())
    parse_spaces(profile.at("city"), &misc_parsed);

if (profile.find("school") != profile.end())
    parse_spaces(profile.at("school"), &misc_parsed);

combine_two_sets(&misc_parsed, &misc_parsed, &misc_parsed, &final_output);

combine_two_sets(&name_bd_combos, &misc_parsed, &name_bd_combos, &final_output);

//This is a slow way to do it, but it will work
cout << "adding years";
if (profile.find("years") != profile.end()){
    for (auto i:name_bd_combos){
        for (int j = 0; j < config.years.size(); j++){
            string new_word = i + config.years[j];
            string new_word2 = config.years[j] + i;
            string new_word3 = config.years[j] + "_" + i;
            string new_word4 = i + "_" + config.years[j];
            if (!pass_too_long(new_word,0) && !pass_too_short(new_word, 0)) {
                final_output.insert(new_word);
                final_output.insert(new_word2);
            }
            if (!pass_too_long(new_word3,0) && !pass_too_short(new_word3, 0)) {
                final_output.insert(new_word3);
                final_output.insert(new_word4);
            }
            if (profile.find("randnum") != profile.end()){
                for (int j = 0; j < config.nums_to_add.size(); j++) {
                    string new_word = i + config.nums_to_add[j];
                    string new_word2 = config.nums_to_add[j] + i;
                    string new_word3 = config.nums_to_add[j] + "_" + i;
                    string new_word4 = i + "_" + config.nums_to_add[j];
                    if (!pass_too_long(new_word, 0) && !pass_too_short(new_word, 0)) {
                        final_output.insert(new_word);
                        final_output.insert(new_word2);
                    }
                    if (!pass_too_long(new_word3, 0) && !pass_too_short(new_word3, 0)) {
                        final_output.insert(new_word3);
                        final_output.insert(new_word4);
                    }
                }
            }
        }
    }
}


    cout << "adding randnums";

if (profile.find("randnum") != profile.end() && profile.find("years") == profile.end()) {
    for (auto i: name_bd_combos) {
        for (int j = 0; j < config.nums_to_add.size(); j++) {
            string new_word = i + config.nums_to_add[j];
            string new_word2 = config.nums_to_add[j] + i;
            string new_word3 = config.nums_to_add[j] + "_" + i;
            string new_word4 = i + "_" + config.nums_to_add[j];
            if (!pass_too_long(new_word, 0) && !pass_too_short(new_word, 0)) {
                final_output.insert(new_word);
                final_output.insert(new_word2);
            }
            if (!pass_too_long(new_word3, 0) && !pass_too_short(new_word3, 0)) {
                final_output.insert(new_word3);
                final_output.insert(new_word4);
            }
        }
    }
}


cout << "adding spechars to " << final_output.size() << "words.";
if (profile.find("spechars") != profile.end()) {
    if (profile.find("spechars_loc") != profile.end()){
        vector<string> spechars_built{};
        spechars_built = build_spechars();
        add_spechars(profile.at("spechars_loc"), &final_output, spechars_built);
    }
}

cout << "making leet " << final_output.size() << "words.";
if(profile.find("leet_basic") != profile.end()){
    makeleet_short(&final_output);
}

if(profile.find("leet_comprehensive") != profile.end()){
    makeleet_long(&final_output);
}

    ofstream outputfile;
    outputfile.open("open2.txt");

    cout << "Final list length : " << final_output.size() << endl;
    for (auto const &i:final_output){
        if (i.size() <= config.pwd_max_length && i.size() >= config.pwd_min_length)
            outputfile << i << endl;
    }
}

int alectodb_download(){
    string type_download{};
    string download_src{};
    string host{};
    string path{};
    string file{}, file_selection{};
    int file_selection_int{};

    cout << "\n\nPlease select download source: " << endl;
    cout << "1. AlectoDB (Legacy CUPP/2010 list)"
            "\n2. Default Credentials  (danielmiessler/SecLists Repo)"
            "\n3. Leaked Databases (danielmiessler/SecLists Repo)"
            "\n[4]. Common-Credentials  (danielmiessler/SecLists Repo)"
            "\n5. Culture Wordlists (Legacy CUPP/1990s) "
            "\n\nFrom which source? 1/2/3/[4]/5: ";
    getline(cin, type_download);
    if (type_download == "1") {
        string path = "alectodb.csv.gz";
        string host = "github.com/yangbh/Hammer/raw/b0446396e8d67a7d4e53d6666026e078262e5bab/lib/cupp/alectodb.csv.gz";
    }
    if (type_download == "2") {
        cout << "1. Oracle EBS passwordlist.txt         10. oracle-betterdefaultpasslist.txt\n"
                "2. Oracle EBS userlist.txt             11. postgres-betterdefaultpasslist.txt\n"
                "3. avaya_defaultpasslist.txt           12. scada-pass.csv\n"
                "4. cryptominers.txt                    13. ssh-betterdefaultpasslist.txt\n"
                "5. db2-betterdefaultpasslist.txt       14. telnet-betterdefaultpasslist.txt\n"
                "6. default-passwords.txt               15. telnet-phenoelit.txt\n"
                "7. ftp-betterdefaultpasslist.txt       16. tomcat-betterdefaultpasslist.txt\n"
                "8. mssql-betterdefaultpasslist.txt     17. vnc-betterdefaultpasslist.txt\n"
                "9. mysql-betterdefaultpasslist.txt     18. windows-betterdefaultpasslist.txt\n\n"
                "Enter a number to download the list: ";
        string path = "/danielmiessler/SecLists/master/Passwords/Default-Credentials/";
        string host = "raw.githubusercontent.com";
        getline(cin, file_selection);
        file_selection_int = stoi(file_selection);
        switch (file_selection_int){
            case 1: file_selection = "Oracle%20EBS%20passwordlist.txt"; break;
            case 2: file_selection = "Oracle%20EBS%20userlist.txt"; break;
            case 3: file_selection = "avaya_defaultpasslist.txt"; break;
            case 4: file_selection = "cryptominers.txt"; break;
            case 5: file_selection = "db2-betterdefaultpasslist.txt"; break;
            case 6: file_selection = "default-passwords.txt"; break;
            case 7: file_selection = "ftp-betterdefaultpasslist.txt"; break;
            case 8: file_selection = "mssql-betterdefaultpasslist.txt"; break;
            case 9: file_selection = "mysql-betterdefaultpasslist.txt"; break;
            case 10: file_selection = "oracle-betterdefaultpasslist.txt"; break;
            case 11: file_selection = "postgres-betterdefaultpasslist.txt"; break;
            case 12: file_selection = "scada-pass.csv"; break;
            case 13: file_selection = "ssh-betterdefaultpasslist.txt"; break;
            case 14: file_selection = "telnet-betterdefaultpasslist.txt"; break;
            case 15: file_selection = "telnet-phenoelit.txt"; break;
            case 16: file_selection = "tomcat-betterdefaultpasslist.txt"; break;
            case 17: file_selection = "vnc-betterdefaultpasslist.txt"; break;
            case 18: file_selection = "windows-betterdefaultpasslist.txt"; break;
        }
        path = path + file_selection;
        vector<string> results{};
        results = download_http(host,path);

        ofstream outputfile;
        outputfile.open(file_selection);

        cout << "Final list length : " << results.size() << endl;
        for (auto i: results)
            outputfile << i;
    }
    if (type_download == "3"){
        cout << "1. 000webhost.txt              22. rockyou.txt.tar.gz\n"
                "2. Ashley-Madison.txt          23. rockyou-05.txt\n"
                "3. Lizard-Squad.txt            24. rockyou-10.txt\n"
                "4. NordVPN.txt                 25. rockyou-15.txt\n"
                "5. adobe100.txt                26. rockyou-20.txt\n"
                "6. alleged-gmail-passwords.txt 27. rockyou-25.txt\n"
                "7. bible.txt                   28. rockyou-30.txt\n"
                "8. carders.cc.txt              29. rockyou-35.txt\n"
                "9. elitehacker.txt             30. rockyou-40.txt\n"
                "10. faithwriters.txt           31. rockyou-45.txt\n"
                "11. fortinet-2021.txt          32. rockyou-50.txt\n"
                "12. hak5.txt                   33. rockyou-55.txt\n"
                "13. honeynet.txt               34. rockyou-60.txt\n"
                "14. honeynet2.txt              35. rockyou-65.txt\n"
                "15. hotmail.txt                36. rockyou-70.txt\n"
                "16. izmy.txt                   37. rockyou-75.txt\n"
                "17. md5decryptor-uk.txt        38. singles.org.txt\n"
                "18. muslimMatch.txt            39. tuscl.txt\n"
                "19. myspace.txt                40. youporn2012.txt\n"
                "20. phpbb.txt\n"
                "21. porn-unknown.txt\n\n"
                "Enter a number to download the list: ";
        string path = "/danielmiessler/SecLists/master/Passwords/Leaked-Databases/";
        string host = "raw.githubusercontent.com";
        getline(cin, file_selection);
        file_selection_int = stoi(file_selection);
        //TODO: rockyou.txt.tar.gz body limit is exceeded.
        //see https://stackoverflow.com/questions/50348516/boost-beast-message-with-body-limit for possible solution
        switch (file_selection_int){
            case 1: file_selection = "000webhost.txt"; break;
            case 2: file_selection = "Ashley-Madison.txt"; break;
            case 3: file_selection = "Lizard-Squad.txt"; break;
            case 4: file_selection = "NordVPN.txt"; break;
            case 5: file_selection = "adobe100.txt"; break;
            case 6: file_selection = "alleged-gmail-passwords.txt"; break;
            case 7: file_selection = "bible.txt"; break;
            case 8: file_selection = "carders.cc.txt"; break;
            case 9: file_selection = "elitehacker.txt"; break;
            case 10: file_selection = "faithwriters.txt"; break;
            case 11: file_selection = "fortinet-2021.txt"; break;
            case 12: file_selection = "hak5.txt"; break;
            case 13: file_selection = "honeynet.txt"; break;
            case 14: file_selection = "honeynet2.txt"; break;
            case 15: file_selection = "hotmail.txt"; break;
            case 16: file_selection = "izmy.txt"; break;
            case 17: file_selection = "md5decryptor-uk.txt"; break;
            case 18: file_selection = "muslimMatch.txt"; break;
            case 19: file_selection = "myspace.txt"; break;
            case 20: file_selection = "phpbb.txt"; break;
            case 21: file_selection = "porn-unknown.txt"; break;
            case 22: file_selection = "rockyou.txt.tar.gz"; break;
            case 23: file_selection = "rockyou-05.txt"; break;
            case 24: file_selection = "rockyou-10.txt"; break;
            case 25: file_selection = "rockyou-15.txt"; break;
            case 26: file_selection = "rockyou-20.txt"; break;
            case 27: file_selection = "rockyou-25.txt"; break;
            case 28: file_selection = "rockyou-30.txt"; break;
            case 29: file_selection = "rockyou-35.txt"; break;
            case 30: file_selection = "rockyou-40.txt"; break;
            case 31: file_selection = "rockyou-45.txt"; break;
            case 32: file_selection = "rockyou-50.txt"; break;
            case 33: file_selection = "rockyou-55.txt"; break;
            case 34: file_selection = "rockyou-60.txt"; break;
            case 35: file_selection = "rockyou-65.txt"; break;
            case 36: file_selection = "rockyou-70.txt"; break;
            case 37: file_selection = "rockyou-75.txt"; break;
            case 38: file_selection = "singles.org.txt"; break;
            case 39: file_selection = "tuscl.txt"; break;
            case 40: file_selection = "youporn2012.txt"; break;
            default: file_selection = "rockyou-75.txt";
        }
        path = path + file_selection;
        vector<string> results{};
        results = download_http(host,path);

        ofstream outputfile;
        outputfile.open(file_selection);

        cout << "Final list length : " << results.size() << endl;
        for (auto i: results)
            outputfile << i;
    }
    if (type_download == "4") {
        cout << "Which file would you like to download: " << endl;
        cout << "1. 10-million-password-list-top-100.txt        12. SplashData-2015-1.txt\n"
                "[2]. 10-million-password-list-top-1000.txt       13. SplashData-2015-2.txt\n"
                "3. 10-million-password-list-top-10000.txt      14. best1050.txt\n"
                "4. 10-million-password-list-top-100000.txt     15. best110.txt\n"
                "5. 10-million-password-list-top-1000000.txt    16. best15.txt\n"
                "6. 10-million-password-list-top-500.txt        17. common-passwords-win.txt\n"
                "7. 100k-most-used-password-NCSC.txt            18. four-digit-pin-codes-sorted-by-frequency-withcount.csv\n"
                "8. 10k-most-common.txt                         19. medical-devices.txt\n"
                "9. 1900-2020.txt                               20. top-20-common-SSH-password.txt\n"
                "10. 500-worst-passwords.txt                    21. top-passwords-shortlist.txt\n"
                "11. SplashData-2014.txt                        22. worst-passwords-2017-top100-slashdata.txt\n\n"
                "Enter a number to download the list: ";
        string path = "/danielmiessler/SecLists/master/Passwords/Common-Credentials/";
        string host = "raw.githubusercontent.com";
        getline(cin, file_selection);
        file_selection_int = stoi(file_selection);
        switch (file_selection_int){
            case 1: file_selection = "10-million-password-list-top-100.txt"; break;
            case 2: file_selection = "10-million-password-list-top-1000.txt"; break;
            case 3: file_selection = "10-million-password-list-top-10000.txt"; break;
            case 4: file_selection = "10-million-password-list-top-100000.txt"; break;
            case 5: file_selection = "10-million-password-list-top-1000000.txt"; break;
            case 6: file_selection = "10-million-password-list-top-500.txt"; break;
            case 7: file_selection = "100k-most-used-password-NCSC.txt"; break;
            case 8: file_selection = "10k-most-common.txt"; break;
            case 9: file_selection = "1900-2020.txt"; break;
            case 10: file_selection = "500-worst-passwords.txt"; break;
            case 11: file_selection = "SplashData-2014.txt"; break;
            case 12: file_selection = "SplashData-2015-1.txt"; break;
            case 13: file_selection = "SplashData-2015-2.txt"; break;
            case 14: file_selection = "best1050.txt"; break;
            case 15: file_selection = "best110.txt"; break;
            case 16: file_selection = "best15.txt"; break;
            case 17: file_selection = "common-passwords-win.txt"; break;
            case 18: file_selection = "four-digit-pin-codes-sorted-by-frequency-withcount.csv"; break;
            case 19: file_selection = "medical-devices.txt"; break;
            case 20: file_selection = "top-20-common-SSH-password.txt"; break;
            case 21: file_selection = "top-passwords-shortlist.txt"; break;
            case 22: file_selection = "worst-passwords-2017-top100-slashdata.txt"; break;
            default: file_selection = "10-million-password-list-top-1000.txt";
           }
            path = path + file_selection;
            vector<string> results{};
            results = download_http(host,path);

            ofstream outputfile;
            outputfile.open(file_selection);

            cout << "Final list length: " << results.size() << endl;
            for (auto i: results)
                outputfile << i;

    }
    if (type_download == "5"){
        cout << "1. Moby            14. french          27. places\n"
                "2. afrikaans       15. german          28. polish\n"
                "3. american        16. hindi           29. random\n"
                "4. aussie          17. hungarian       30. religion\n"
                "5. chinese         18. italian         31. russian\n"
                "6. computer        19. japanese        32. science\n"
                "7. croation        20. latin           33. spanish\n"
                "8. czech           21. literature      34. swahili\n"
                "9. danish          22. movieTV         35. swedish\n"
                "10. databases      23. music           36. turkish\n"
                "11. dictionaries   24. names           37. yiddish\n"
                "12. dutch          25. net             38. exit program\n"
                "13. finnish        26. norwegian       \n"
                "Choose the section you want to download: ";

        host = "ftp.funet.fi";
        path = "/pub/unix/security/passwd/crack/dictionaries/";
        bool multilists = false;
        getline(cin, file_selection);
        file_selection_int = stoi(file_selection);
        string url_directory{};
        switch (file_selection_int){
            case 1: file_selection = "mhyph.tar.gz,mlang.tar.gz,moby.tar.gz,mpos.tar.gz,mpron.tar.gz,mthes.tar.gz,mwords.tar.gz";
                    url_directory="Moby"; multilists=true; break;
            case 2: file_selection = "afr_dbf.zip";url_directory="afrikaans"; break;
            case 3: file_selection = "dic-0294.tar.gz";url_directory="american"; break;
            case 4: file_selection = "oz.gz";url_directory="aussie"; break;
            case 5: file_selection = "chinese.gz";url_directory="chinese"; break;
            case 6: file_selection = "Domains.gz,Dosref.gz,Ftpsites.gz,Jargon.gz,common-passwords.txt.gz,etc-hosts.gz,"
                                     "foldoc.gz,language-list.gz,unix.gz";url_directory="computer";multilists=true; break;
            case 7: file_selection = "croatian.gz";url_directory="croatian"; break;
            case 8: file_selection = "czech-wordlist-ascii-cstug-novak.gz";url_directory="czech"; break;
            case 9: file_selection = "danish.words.gz,dansk.zip";url_directory="danish";multilists=true; break;
            case 10: file_selection = "acronyms.gz,att800.gz,computer-companies.gz,world_heritage.gz";
                url_directory="databases";multilists=true; break;
            case 11: file_selection = "Antworth.gz,CRL.words.gz,Roget.words.gz,Unabr.dict.gz,Unix.dict.gz,englex-dict.gz"
                                      ",knuth_britsh.gz,knuth_words.gz,pocket-dic.gz,shakesp-glossary.gz,special.eng.gz,"
                                      "words-english.gz";url_directory="dictionaries";multilists=true; break;
            case 12: file_selection = "words.dutch.gz";url_directory="dutch"; break;
            case 13: file_selection = "finnish.gz,firstnames.finnish.gz,words.finnish.FAQ.gz";url_directory="finnish";multilists=true; break;
            case 14: file_selection = "dico.gz";url_directory="french"; break;
            case 15: file_selection = "deutsch.dic.gz,germanl.gz,words.german.gz";url_directory="german";multilists=true; break;
            case 16: file_selection = "hindu-names.gz";url_directory="hindi"; break;
            case 17: file_selection = "hungarian.gz";url_directory="hungarian"; break;
            case 18: file_selection = "words.italian.gz";url_directory="italian"; break;
            case 19: file_selection = "words.japanese.gz";url_directory="japanese"; break;
            case 20: file_selection = "wordlist.aug.gz";url_directory="latin"; break;
            case 21: file_selection = "LCarrol.gz,Paradise.Lost.gz,aeneid.gz,arthur.gz,cartoon.gz,cartoons-olivier.gz"
                        "charlemagne.gz,fable.gz,iliad.gz,myths-legends.gz,odyssey.gz,sf.gz,shakespeare.gz,"
                        "tolkien.words.gz";url_directory="literature"; multilists=true; break;
            case 22: file_selection = "Movies.gz,Python.gz,Trek.gz";url_directory="movieTV";multilists=true; break;
            case 23: file_selection = "music-classical.gz,music-country.gz,music-jazz.gz,music-other.gz,music-rock.gz,"
                        "music-shows.gz,rock-groups.gz";url_directory="music"; multilists=true; break;
            case 24: file_selection = "ASSurnames.gz,Congress.gz,Family-Names.gz,Given-Names.gz,actor-givenname.gz,"
                        "actor-surname.gz,cis-givenname.gz,cis-surname.gz,crl-names.gz,famous.gz,fast-names.gz,"
                        "female-names-kantr.gz,female-names.gz,givennames-ol.gz,male-names-kantr.gz,male-names.gz,"
                        "movie-characters.gz,names.french.gz,names.hp.gz,other-names.gz,shakesp-names.gz,"
                        "surnames-ol.gz,surnames.finnish.gz,usenet-names.gz";url_directory="names"; multilists=true; break;
            case 25: file_selection = "hosts-txt.gz,inet-machines.gz,usenet-loginids.gz,"
                        "usenet-machines.gz,uunet-sites.gz";url_directory="net"; multilists=true; break;
            case 26: file_selection = "words.norwegian.gz";url_directory="norwegian"; break;
            case 27: file_selection = "Colleges.gz,US-counties.gz,World.factbook.gz,Zipcodes.gz,places.gz";
                url_directory="places";multilists=true; break;
            case 28: file_selection = "words.polish.gz";url_directory="polish"; break;
            case 29: file_selection = "Ethnologue.gz,abbr.gz,chars.gz,dogs.gz,drugs.gz,junk.gz,numbers.gz"
                        "phrases.gz,sports.gz,statistics.gz"; url_directory="random";multilists=true; break;
            case 30: file_selection = "Koran.gz,kjbible.gz,norse.gz"; url_directory="religion";multilists=true; break;
            case 31: file_selection = "russian.lst.gz,russian_words.koi8.gz"; url_directory="russian";multilists=true; break;
            case 32: file_selection = "Acr-diagnosis.gz,Algae.gz,Bacteria.gz,Fungi.gz,Microalgae.gz,Viruses.gz,"
                        "asteroids.gz,biology.gz,tech.gz"; url_directory="science";multilists=true; break;
            case 33: file_selection = "words.spanish.gz"; url_directory="spanish";break;
            case 34: file_selection = "swahili.gz"; url_directory="swahili";break;
            case 35: file_selection = "words.swedish.gz"; url_directory="swedish";break;
            case 36: file_selection = "turkish.dict.gz"; url_directory="turkish";break;
            case 37: file_selection = "yiddish.gz"; url_directory="yiddish";break;

        }
        //TODO: add new get function to work with gz files. see comment on get above.
        if (multilists == false) {
            path = path + url_directory + '/' + file_selection;
            vector<string> results{};
            results = download_http(host, path);

            ofstream outputfile;
            outputfile.open(file_selection);

            cout << "Final list length : " << results.size() << endl;
            for (auto i: results)
                outputfile << i;
        }
        if (multilists == true) {
            vector<string> files{};
            const string path_with_dir = path + url_directory + '/';

            // first parse the individual files we need
            int last_comma = 0;
            for (int i =0; i < file_selection.size(); i++) {
                if (file_selection[i] == ',') {
                    files.push_back(file_selection.substr(last_comma, i-last_comma));
                    last_comma = i+1;
                }
            }
            create_directory(url_directory);
            for (auto i: files) {
                ofstream outputfile(url_directory);
                outputfile.open(i);
                path = path_with_dir + i;
                vector<string> results{};
                results = download_http(host, path);
                for (auto i: results)
                    outputfile << i;
                outputfile.close();
            }
        }
    }
return 1;
}



/************************************
 *                                  *
 *      General Use Functions       *
 *                                  *
 ************************************/
bool pass_too_short(string word, int adding){
    if ((word.size()) + adding < config.pwd_min_length){
        return true;
    }else{
        return false;
    }
}
bool pass_too_long(string word, int adding){
    if ((word.size()) + adding > config.pwd_max_length) {
        return true;
    }else {
        return false;
    }
}

string concat_list_on_right(string og_words,string concat){
    string concat_output{};
    if (!pass_too_long(og_words,concat.size()) || !pass_too_short((og_words), concat.size()))
        concat_output = (og_words + concat);

    return concat_output;
    }

//TODO: Test if I need this for http request or if my https request works without it
    /*
int download_http(string url, string targetfile){
    cout << "Downloading " << targetfile << "from " << urgit add .l << " ... ";

    try
    {
        // Check command line arguments.

        auto const host = url;
        auto const port = 443;
        auto const target = targetfile;
        int version = 1.1;

        // The io_context is required for all I/O
        net::io_context ioc;

        // The SSL context is required, and holds certificates
        ssl::context ctx(ssl::context::tlsv12_client);

        // This holds the root certificate used for verification
        load_root_certificates(ctx);

        // Verify the remote server's certificate
        ctx.set_verify_mode(ssl::verify_peer);

        // These objects perform our I/O
        tcp::resolver resolver(ioc);
        beast::ssl_stream<beast::tcp_stream> stream(ioc, ctx);

        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(stream.native_handle(), host))
        {
            beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            throw beast::system_error{ec};
        }

        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        // Make the connection on the IP address we get from a lookup
        beast::get_lowest_layer(stream).connect(results);

        // Perform the SSL handshake
        stream.handshake(ssl::stream_base::client);

        // Set up an HTTP GET request message
        http::request<http::string_body> req{http::verb::get, target, version};
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request to the remote host
        http::write(stream, req);

        // This buffer is used for reading and must be persisted
        beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(stream, buffer, res);

        // Write the message to standard out
        std::cout << res << std::endl;

        // Gracefully close the stream
        beast::error_code ec;
        stream.shutdown(ec);
        if(ec == net::error::eof)
        {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec = {};
        }
        if(ec)
            throw beast::system_error{ec};

        // If we get here then the connection is closed gracefully
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
*/
    vector<string> download_http(string url, string targetfile){
        vector<string> results{};
        asio::io_context ctx;
        ssl::context ssl_ctx{ssl::context::tls_client};
        auto const hostname = std::string{url};
        ssl_ctx.set_verify_mode(ssl::context::verify_peer |
                                ssl::context::verify_fail_if_no_peer_cert);
        ssl_ctx.set_default_verify_paths();
        // tag::ctx_setup_source[]
        boost::certify::enable_native_https_server_verification(ssl_ctx);
        // end::ctx_setup_source[]
        auto stream_ptr = connect(ctx, ssl_ctx, hostname);
        auto response = get(*stream_ptr, hostname, targetfile);
        results = response;

        boost::system::error_code ec;
        stream_ptr->shutdown(ec);
        stream_ptr->next_layer().close(ec);

        return results;
    }


/************************************
 *                                  *
 *        Improve Wordlist          *
 *           Functions              *
 *                                  *
 ************************************/
void add_spechars(vector<string> og_words, char location_spechars, unordered_set<string> *final_words_tmp){
    vector<string> spechars_output{};
    bool special_chars_prepend{false},special_chars_append{false},special_chars_bookend{false},special_chars_all{false};

    switch (location_spechars){
        case '1': special_chars_prepend = true; break;
        case '2': special_chars_append = true; break;
        case '3': special_chars_bookend = true; break;
        case '4': special_chars_all = true; break;
        default: special_chars_append = true;
    }

        //I'm sorry for the mess that follows. This can definitely be improved.
    if (special_chars_append || special_chars_all){
        for (int i = 0; i < og_words.size(); i++){
            for (int spechar1 = 0; spechar1 < config.spechars.size(); spechar1++){
                if (pass_too_long(og_words[i],1))
                    continue;
                if (!pass_too_short(og_words[i],1))
                    final_words_tmp->insert((og_words[i]) + (config.spechars[spechar1]));
                for (int spechar2 = 0;spechar2 < config.spechars.size(); spechar2++){
                    if (pass_too_long(og_words[i],2))
                        continue;
                    if (!pass_too_short(og_words[i],2))
                        final_words_tmp->insert((og_words[i]) + (config.spechars[spechar1]) + (config.spechars[spechar2]) );
                    for(int spechar3 = 0;spechar3 < config.spechars.size(); spechar3++){
                        if (pass_too_long(og_words[i],3))
                            continue;
                        if (!pass_too_short(og_words[i],3))
                            final_words_tmp->insert((og_words[i]) + (config.spechars[spechar1])
                                                  + (config.spechars[spechar2]) + (config.spechars[spechar3]) );
                    }
                }
            }
        }
    }
    if (special_chars_prepend || special_chars_all){
        for (int i = 0; i < og_words.size(); i++){
            for (int spechar1 = 0; spechar1 < config.spechars.size(); spechar1++){
                if (pass_too_long(og_words[i],1))
                    continue;
                if (!pass_too_short(og_words[i],1))
                    final_words_tmp->insert((config.spechars[spechar1]) + (og_words[i]));
                for (int spechar2 = 0;spechar2 < config.spechars.size(); spechar2++){
                    if (pass_too_long(og_words[i],2))
                        continue;
                    if (!pass_too_short(og_words[i],2))
                        final_words_tmp->insert((config.spechars[spechar1]) + (config.spechars[spechar2]) + (og_words[i]));
                    for(int spechar3 = 0;spechar3 < config.spechars.size(); spechar3++){
                        if (pass_too_long(og_words[i],3))
                            continue;
                        if (!pass_too_short(og_words[i],3))
                            final_words_tmp->insert((config.spechars[spechar1]) + (config.spechars[spechar2])
                                                  + (config.spechars[spechar3]) + (og_words[i]) );
                    }
                }
            }
        }
    }
    if (special_chars_bookend || special_chars_all){
        for (int i = 0; i < og_words.size(); i++){
            for (int spechar1 = 0; spechar1 < config.spechars.size(); spechar1++){
                if (pass_too_long(og_words[i],1))
                    continue;
                if (!pass_too_short(og_words[i],1))
                    final_words_tmp->insert((config.spechars[spechar1]) + (og_words[i]) + (config.spechars[spechar1]));
                for (int spechar2 = 0;spechar2 < config.spechars.size(); spechar2++){
                    if (pass_too_long(og_words[i],2))
                        continue;
                    if (!pass_too_short(og_words[i],2)) {
                        final_words_tmp->insert((config.spechars[spechar1]) + (config.spechars[spechar2]) +
                                                  (og_words[i]) + (config.spechars[spechar1]) +
                                                  (config.spechars[spechar2]));
                        //reverse order of last 2 pairs
                        if (config.spechars[spechar1] != config.spechars[2])
                            final_words_tmp->insert((config.spechars[spechar1]) + (config.spechars[spechar2]) +
                                                      (og_words[i]) + (config.spechars[spechar2]) +
                                                      (config.spechars[spechar1]));
                    }
                    for(int spechar3 = 0;spechar3 < config.spechars.size(); spechar3++){
                        if (pass_too_long(og_words[i],3))
                            continue;
                        if (!pass_too_short(og_words[i],3)) {
                            final_words_tmp->insert((config.spechars[spechar1]) + (config.spechars[spechar2])
                                                      + (config.spechars[spechar3]) + (og_words[i])
                                                      + (config.spechars[spechar1]) + (config.spechars[spechar2])
                                                      + (config.spechars[spechar3]));
                            //reverse order of last 3 chars
                            if (config.spechars[spechar1] != config.spechars[spechar2] &&
                                config.spechars[spechar1] != config.spechars[spechar3])
                                final_words_tmp->insert((config.spechars[spechar1]) + (config.spechars[spechar2])
                                                          + (config.spechars[spechar3]) + (og_words[i])
                                                          + (config.spechars[spechar3]) + (config.spechars[spechar2])
                                                          + (config.spechars[spechar1]));
                        }
                    }
                }
            }
        }
    }
}

void makeleet_long(unordered_set<string> *output) {
    //make all possible permutations
    for (auto const &word: *output) {

        int word_len = word.size();
        string map{};
        int num_replacements{0};
        map = printNTimes(0, word_len);
        unordered_map<int,char> replacement_map;
        //go through each letter of the word
        for (int j = 0; j < word_len; j++) {
            //compare for match for every letter in chars to replace
            for (int k = 0; k < config.leet_mode_to_replace.size(); k++) {
                if (word[j] == config.leet_mode_to_replace[k]) {
                    num_replacements += 1;
                    replacement_map.emplace(j,config.leet_mode_replacement[k]);
                }
            }
        }
        unordered_map<int, char>:: iterator i = replacement_map.begin();
        if (num_replacements > 0) {
            bool first_run = true;
            permute(word, replacement_map, output, i, first_run);
        }
    }
}

void makeleet_short(unordered_set<string> *output){
    for (auto const &word: *output) {
        for (int j = 0; j < word.size(); j++) {
            bool matches{false};
            string leet_word = word;
            for (int k=0; k < config.leet_mode_replacement.size(); k++)
                if (word[j] == config.leet_mode_to_replace[k]) {
                    matches = true;
                    leet_word[j] = config.leet_mode_replacement[k];
                }
            if(matches)
                output->insert(leet_word);
        }
    }
}

// useful for leet_mode. could be expanded for other cool things (like all possible capital letters etc). also would be
// wise to test speed diff of adding all posible permutations of spechars at start of program, outputting to vector
// and concatting to word instead of making them each time.
void permute(string a, unordered_map<int, char> replacement_map, unordered_set<string> *output, unordered_map<int, char>:: iterator i, bool first_run = true) {
    if (first_run){
        i = replacement_map.begin();
        string full_word_leet = a;
        while(i != replacement_map.end()){
            full_word_leet[i->first] = i->second;
            i++;
        }
        output->insert(full_word_leet);
        i = replacement_map.begin();
    }

    // move through the map
    while (i != replacement_map.end()){
        //move through matches vector
        swap(a[i->first], i->second);
        output->insert(a);
        if (i == replacement_map.end())
            return;
        first_run = false;
        swap(a[i->first], i->second);
        i++;
        permute(a, replacement_map, output, i,first_run);
    }
}

string printNTimes(char c, int n) {
        return string(n, c);
    }


/************************************
 *                                  *
 *         Generate Wordlist        *
 *            -i Functions          *
 *                                  *
 ************************************/
void parse_birthday(string bd_full, unordered_set<string> *output) {
    string bd_yyyy{}, bd_yyy{}, bd_mm{}, bd_dd{}, bd_m{}, bd_d{};

    if (bd_full.length() > 7) {
        //BDay Format: DDMMYYYY
        bd_yyyy = bd_full.substr(4, 4);
        bd_yyy = bd_full.substr(5, 3);
        bd_mm = bd_full.substr(2, 2);
        bd_dd = bd_full.substr(0, 2);
        output->insert(bd_full);
        output->insert(bd_yyyy);
        output->insert(bd_yyy);
        output->insert(bd_mm);
        output->insert(bd_dd);
        if (bd_mm[0] == '0'){
            bd_m = bd_full.substr(3, 1);
            output->insert(bd_m);
        }
        if (bd_dd[0] == '0'){
            bd_d = bd_full.substr(1, 1);
            output->insert(bd_d);
        }
    }
}

unordered_set<string> concat_list_on_right(unordered_set<string> og_words, unordered_set<string> concat){
    unordered_set<string> :: iterator o;
    unordered_set<string> :: iterator c;
    unordered_set<string> output;
    string word{};

    for (o = og_words.begin(); o != og_words.end(); o++) {
        for (c = concat.begin(); c != concat.end(); c++) {
            string concat_output{};
            int concat_length = (*c).size();
            if ((*o) != (*c))
                //TODO: pass_too_long may not be working like I think below. needs double checked.
                if (!pass_too_long(*o, concat_length)){
                    output.insert(*o + *c);
                    output.insert(*c + *o);
                }
        }
    }
    return output;
}

void parse_name(string name, unordered_set<string> *output) {
    //adds reversed name and name with opposite case (upper/lower) initial letter to output set.
    output->insert(name);
    string opposite_case{};
    char first_letter[1]= "";
    if (isupper(name[0])){
        char first_letter = tolower(name[0]);
        opposite_case = first_letter + name.substr(1,name.size());
    }
    else{
        char first_letter = toupper(name[0]);
        opposite_case = first_letter + name.substr(1,name.size());
    }
    output->insert(opposite_case);
    reverse(name.begin(), name.end());
    reverse(opposite_case.begin(), opposite_case.end());
    output->insert(name);
    output->insert(opposite_case);
}

void parse_spaces (string input, unordered_set<string> *output){
    string word = "";
    for (int i = 0; i < input.length(); i++){
        if (input[i] == ' ' || i == input.length() - 1) {
            if (i == input.length() -1)
                word = word + input[i];
            parse_name(word, output);
            word = "";
        }else
            word = word + input[i];
    }
}

void combine_two_sets(unordered_set<string> *input, unordered_set<string> *input2,unordered_set<string> *output_of_func, unordered_set<string> *output_final) {
    unordered_set<string>::iterator itr{};
    unordered_set<string>::iterator itr2{};
    for (itr = input->begin(); itr != input->end(); itr++) {
        if (!pass_too_long((*itr), 0))
            output_of_func->insert(*itr);
        if (!pass_too_short((*itr), 0) && !pass_too_long((*itr), 0))
            output_final->insert(*itr);
        for (itr2 = input2->begin(); itr2 != input2->end(); itr2++) {
            string current_word{};
            output_of_func->insert(*itr2);
            current_word = (*itr + (*itr2));
            if (!pass_too_long(current_word, 0))
                output_of_func->insert(current_word);
            if (!pass_too_short((current_word), 0) && !pass_too_long((current_word), 0)) {
                output_final->insert(current_word);
            }
        }
    }
}

void add_spechars(string spechars_loc, unordered_set<string> *final_words, vector<string> spechars_vec) {
    vector<string> spechars_output{};
    unordered_set<string> final_words_tmp{};
    bool special_chars_prepend{false}, special_chars_append{false}, special_chars_bookend{false}, special_chars_all{
            false};
    const char spechars_location = spechars_loc[0];
    switch (spechars_location) {
        case '1':
            special_chars_prepend = true;
            break;
        case '2':
            special_chars_append = true;
            break;
        case '3':
            special_chars_bookend = true;
            break;
        case '4':
            special_chars_all = true;
            break;
        default:
            special_chars_append = true;
    }
    unordered_set<string>::iterator itr;
    int progress = 0;
    //I'm sorry for the mess that follows. This can definitely be improved.
    if (special_chars_append || special_chars_all) {
        for (itr = final_words->begin(); itr != final_words->end(); itr++) {
            if (progress % 100 == 0){
                cout << "Currently on word "<< progress;
                cout << "\r";
            }
            progress++;
            for (const string &spechar: spechars_vec) {
                if (!pass_too_long((*itr), spechar.size()) && !pass_too_short((*itr), spechar.size()))
                    final_words_tmp.insert((*itr) + spechar);
            }
        }
    }
    if (special_chars_prepend || special_chars_all) {
        for (itr = final_words->begin(); itr != final_words->end(); itr++) {
            if (progress % 100 == 0){
                cout << "Currently on word "<< progress;
                cout << "\r";
            }
            progress++;
            for (const string &spechar: spechars_vec) {
                if (!pass_too_long((*itr), spechar.size()) && !pass_too_short((*itr), spechar.size()))
                    final_words_tmp.insert(spechar + (*itr));
            }
        }
    }
    if (special_chars_bookend || special_chars_all) {
        for (itr = final_words->begin(); itr != final_words->end(); itr++) {
            if (progress % 100 == 0){
                cout << "Currently on word "<< progress;
                cout << "\r";
            }
            progress++;
            for (const string &spechar: spechars_vec) {
                if (!pass_too_long((*itr), spechar.size()) && !pass_too_short((*itr), spechar.size())) {
                    final_words_tmp.insert(spechar + (*itr) + spechar);
                    string spechar_copy = spechar;
                    reverse(spechar_copy.begin(), spechar_copy.end());
                    if (spechar_copy != spechar)
                        final_words_tmp.insert(spechar_copy + (*itr) + spechar);
                }
            }
        }
    }
    cout << "added " << final_words_tmp.size() << "unique combos of words with special characters";
    for (auto i:final_words_tmp)
        final_words->insert(i);
}

vector<string> build_spechars(){
    vector<string> output;
    for (int spechar1 = 0; spechar1 < config.spechars.size(); spechar1++){
            output.push_back((config.spechars[spechar1]));
        for (int spechar2 = 0;spechar2 < config.spechars.size(); spechar2++){
                output.push_back((config.spechars[spechar1]) + (config.spechars[spechar2]));
            for(int spechar3 = 0;spechar3 < config.spechars.size(); spechar3++){
                    output.push_back((config.spechars[spechar1]) + (config.spechars[spechar2]) + (config.spechars[spechar3]) );
                }
            }
        }
    return output;
}



