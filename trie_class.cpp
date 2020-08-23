#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm> 
#include <limits>

using namespace std;

struct node {
  bool word;
  unordered_map<char,node* > children;

  node () {
    word = false;
  }

  ~node () {
    for ( pair<char,node*> element : children ) {
      delete element.second;
    }
  }

  // tady asi destruktor
};

struct result {
  string word;
  double ratio;
  unsigned int edit_dist;
};


class Trie {

  // we need to know how many characters there are then sort them and so on so it is going to be easier if we use has table
    node * root;
  public:
    vector<result> results;

    Trie() {
      root = new node;
    }

    void insert ( string str ) {
      node * cur_node = root;
      for ( int i = 0; i < str . size(); ++i ) {
        // check if character is in the ( unordered map ) children
        char c = str[i];
        if ( cur_node -> children . find ( c ) != cur_node -> children . end () ) {
          cur_node = cur_node -> children [ c ];
        } else {
          node * new_node = new node;
          // kazdej dalsi charakter uz tam neni proto se bude muset vytvorit dalsi character  
          cur_node -> children . insert ( { c , new_node } ); 
          cur_node = new_node;
        }

        if ( i = str . size() - 1 ) {
          cur_node -> word = true;
        }
      }
    }

    void search_recur ( node *              cur_node, 
                        char                letter, 
                        string              word,
                        vector<uint16_t>    prev_row,
                        uint16_t            max_cost ) 
    {
      uint16_t          columns     =  word.size() + 1;

      vector<uint16_t>  cur_row; 
      cur_row . push_back ( prev_row[0] + 1);

      //#include <cstdint>
      uint16_t row_min = UINT16_MAX;

      for ( uint16_t i = 0; i < columns; ++i ) {
        uint16_t insert_cost = cur_row [i - 1] + 1;
        uint16_t delete_cost = prev_row [i] + 1;

        uint16_t replace_cost = ( word [i - 1] != letter ? prev_row [i-1] + 1 : prev_row [i-1] );

        if (word [i - 1] != letter )
            replace_cost = prev_row [i - 1] + 1;
        else                
            replace_cost = prev_row [i - 1];
	
			// min() is not C function
        uint16_t edit_min = min( { insert_cost, delete_cost, replace_cost } );

        if ( edit_min < row_min )
          row_min = edit_min;

				cur_row . push_back ( edit_min );
      }

      if ( cur_row[-1] <= max_cost && cur_node -> word  ) {
        result res;
        res . word = word;
        res . edit_dist = cur_row[-1];
        results. push_back ( res );
      }

			if ( row_min <= max_cost ) {
					for ( pair<char,node*> child : cur_node -> children ) {
							this->search_recur ( child . second, 
                                   child . first,
                                   word, 
                                   cur_row, 
                                   max_cost );
					}
			}


    }

    void search ( string word, unsigned int max_cost ) {
      // jak k tomu pristupovat
    }

    /* Be respectful to your stack
    struct cell{
      node n;
      vector<uint16_t> row; 
    };

    void bfs () {
      queue<node> q;

      q . push ( 

    }
    */

    ~Trie () {
      delete root;
    }
};

int main () {

  vector<string> input {
    "witty",
    "sponge",
    "dark",
    "scissors",
    "gold",
    "guttural",
    "succeed",
    "best",
    "memory",
    "sedate",
    "carpenter",
    "join"
  };

  Trie t;

  for ( const auto & a: input ) {
    t.insert ( a );
  }
}
