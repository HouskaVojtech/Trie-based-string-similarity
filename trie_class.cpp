#include <Python.h>
#include <string>
#include <codecvt>
#include <locale>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm> 
#include <limits>
#include <numeric>
#include <utility>
#include <fstream>
#include <iterator>

typedef int Py_ssize_t;

using namespace std;

struct node {
  u32string word;
  unordered_map<char32_t,node* > children;

  node () {
  }

  ~node () {
    for ( pair<char32_t,node*> element : children ) {
      delete element.second;
    }
  }

  // tady asi destruktor
};

struct result {
  u32string word;
  double ratio;
  unsigned int edit_dist;
};



class Trie {

    node * root;
    double max_ratio;
    int ins;
    int del;
    int rep;
  public:
    vector<result> results;

    Trie( int insert = 1, int delet = 1, int replace = 1)
        : ins(insert),del(delet),rep(replace)
    {
      root = new node;
    }

    void insert ( u32string str ) {
      node * cur_node = root;
      for ( unsigned int i = 0; i < str . size(); ++i ) {
        char32_t c = str[i];

        // check if character is in children ( unordered map ) 
        if ( cur_node -> children . find ( c ) != cur_node -> children . end () ) {
          cur_node = cur_node -> children [ c ];

        } else {

          node * new_node = new node;
          // kazdej dalsi charakter uz tam neni proto se bude muset vytvorit dalsi character  
          cur_node -> children . insert ( { c , new_node } ); 
          cur_node = new_node;
        }

        if ( i == str . size() - 1 ) {
          cur_node -> word = str;
        }
      }
    }

    void search_recur ( node *              cur_node, 
                        char32_t            letter, 
                        u32string           word,
                        vector<uint16_t>    prev_row,
                        uint16_t            max_cost ) 
    {
      uint16_t          columns     =  word.size() + 1;

      vector<uint16_t>  cur_row; 
      cur_row . push_back ( prev_row[0] + 1);

      //#include <cstdint>
      uint16_t row_min = UINT16_MAX;

      for ( uint16_t i = 1; i < columns; ++i ) {
        uint16_t insert_cost = cur_row [i - 1] + ins;
        uint16_t delete_cost = prev_row [i] + del;

        uint16_t replace_cost;

        if (word [i - 1] != letter )
            replace_cost = prev_row [i - 1] + rep;
        else                
            replace_cost = prev_row [i - 1];
	
			// min() is not C function
        uint16_t edit_min = min( { insert_cost, delete_cost, replace_cost } );

        if ( edit_min < row_min )
          row_min = edit_min;

				cur_row . push_back ( edit_min );
      }

      double cur_ratio = (double)cur_row.back() / max( (uint16_t)word . length (), (uint16_t) cur_row [0] );

      if ( cur_row.back() <= max_cost && 
         ! cur_node -> word . empty() && 
           cur_ratio <= max_ratio ) {
        result res;
        res . word = cur_node -> word; 
        res . edit_dist = cur_row.back();
        res . ratio = cur_ratio;
        results . push_back ( res );
      }

      double min_ratio = (double)row_min / max( (uint16_t)word . length (), (uint16_t) cur_row [0] );

			if ( row_min <= max_cost && min_ratio <= max_ratio ) {
					for ( pair<char32_t,node*> child : cur_node -> children ) {
							this->search_recur ( child . second, 
                                   child . first,
                                   word, 
                                   cur_row, 
                                   max_cost );
					}
			}


    }

    void search ( u32string word, unsigned int max_cost, double max_ratio) {
      this -> max_ratio = max_ratio;
      results.clear();
      // jak k tomu pristupovat
      
      vector<uint16_t> cur_row( word.size() + 1 ); 

      iota ( cur_row.begin(), cur_row.end(), 0 );

      for ( pair<char32_t,node*> child : root -> children ) {
        search_recur ( child.second, child.first , word, cur_row, max_cost );
      }
    }
    
    void traverse () {

      wstring_convert<codecvt_utf8<char32_t>,char32_t> converter;
      queue<pair<char32_t,node*>> q;

      q.push ( make_pair ( '\t',root) ) ;
      q . push ( make_pair ( '\n', (node*)NULL ) );

      cout << "root" ;
      pair<char32_t,node*> s;

      while ( ! q.empty() ) {
        s = q. front ();
        cout << converter.to_bytes(s . first) ;
        q . pop ();

        if ( s . second == NULL  && s . first != ' ') {
          q . push ( make_pair ( '\n', (node*)NULL ) );
          auto peek = q . front ();
          if ( peek . first == '\n' &&  peek . second == NULL )
            break;
        } else { 
          if ( s . first != ' ' ) {
            for ( pair<char32_t,node*> child : s . second -> children ) {
              q . push( child );
            }
            q . push ( { ' ', NULL } );
          } 

        }
      }

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


static PyObject * build ( PyObject * self, PyObject * args ) 
{

  Trie t(1,1,1);

  PyObject * list;
  if ( ! PyArg_ParseTuple( args, "O", &list ) )
    return NULL;

  PyObject * iter = PyObject_GetIter(list);

  if ( ! iter )
    return NULL;

  PyObject * next_item = NULL;
  PyObject * s = NULL;

  vector<u32string> words;
  wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;

  while ( 1 ) {
    next_item = PyIter_Next(iter);

    if ( ! next_item )
      break;

    if( PyUnicode_Check( next_item ) ) 
      s = PyUnicode_AsUTF8String( next_item );

    u32string str;
    if(s) {
      str = converter.from_bytes(PyBytes_AsString(s));
      Py_XDECREF(s);
    }

    // instead of this put the string into trie
    
    t     . insert    ( str );
    words . push_back ( str );
  }


  PyObject * dick = PyDict_New();
  if ( ! dick ) {
    // you are a woman ( probabbly )
    return NULL;
  }



  for ( const auto & word : words ) {
    t. search ( word, 2 , 0.2 );
    t . results; // to an PyList
    PyObject * list = PyList_New( t.results.length() ) ;

    PyObject * tup = PyTuple_New( 3 );
    for ( const auto & r : t . results ) {
      // TOUPLE instead
      tup . 
      cout << converter . to_bytes( r . word )
           << " " 
           << r . edit_dist 
           << " " 
           << r . ratio
           << endl; 
    }

    PyList_SetItem(PyObject *list, Py_ssize_t index, PyObject *item) 
    // TODO: maybe  word has to be converted back to byte char representation
    // because the function wantsch const char * not const char32 *
    PyDict_SetItemString( dick, word, )
  
  }
}

static PyObject * version ( PyObject * self )
{
  return Py_BuildValue("s", "Version 1.0");
}



/*
int main () {

  vector<u32string> words;

  wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;

  ifstream input_file ( "words.txt" );

  string str;

  while (getline(input_file, str)) {
        words.push_back(converter.from_bytes(str));
  }

  Trie t(1,1,1);

  for ( const auto & w: words ) {
    t . insert ( w );
  }

  //t.traverse();
  
  for ( const auto & word : words ) {
    t. search ( word, 2 , 0.2 );
  
    cout << converter . to_bytes ( word ) 
         << ":" 
         << endl;

    for ( const auto & r : t . results ) {
      cout << converter . to_bytes(r . word )
           << " " 
           << r . edit_dist 
           << " " 
           << r . ratio
           << endl; 
    }
    cout << endl;
  }
}
*/
