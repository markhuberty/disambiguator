
#ifndef PATENT_ENGINE_H
#define PATENT_ENGINE_H

#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>

// Can't use <cstdint> until c++11 is better supported
//#include <cstdint>
#include <stdint.h>

#include "typedefs.h"
#include "record.h"
#include "threading.h"

using std::string;
using std::list;
using std::vector;
using std::map;
using std::set;

// RecordPList is a list of const Record pointers.
// This definition will be used throughout the whole project.
//typedef std::list<const Record * > RecordPList;
// Ok, technically, it's crappy to use a naming pattern
// such as *List (or *Set, etc), but this will be clearer
// than RecordPList, which is something I *always* have
// to look up, I never remember it. Also, the records
// really ought to be in there own container class
// instead of stuffed into a bare list. Let the container
// class choose how to store the records, and make it
// invisible to the developer.
// Had to add a "P" because there is a another list<Record>
// which is not pointers. Ugh.
typedef std::list<const Record * > RecordPList;


/**
 * cSort_by_attrib:
 * A functor for comparison of attributes in associated containers such as map or set.
 */
/**
 * Example:
 *   cSort_by_attrib obj1("Patent"); this will create an instance obj1
 *   which compares two Record pointers based on the "Patent" information column.
 *
 *   map<const Record *, VALUE_TYPE, cSort_by_attrib> m1(obj1); will
 *   create a map (binary tree) whose key is const Record pointer, and
 *   the way of sorting is through comparison of "Patent" attribute.
 *   Binary tree is ideal for fast search, insertion and deletion.
 */
class cSort_by_attrib {

private:
    //attrib_index is the column index on which the Record's comparison depends.
    const uint32_t attrib_index;

public:
   /**
    * comparison between two records pointers based on their content.
    * The conparison content depends on the internal information of the
    * cSort_by_attrib object. i.e. attrib_index;
    */
    bool operator () (const Record * prec1, const Record *prec2 ) const {

        const Attribute * attr1 = prec1->vector_pdata.at(attrib_index);
        const Attribute * attr2 = prec2->vector_pdata.at(attrib_index);
        return attr1->get_data().at(0) < attr2->get_data().at(0);
    };

    cSort_by_attrib(const uint32_t i) : attrib_index(i) {};

    cSort_by_attrib(const string & attrib_name)
      : attrib_index(Record::get_index_by_name(attrib_name)) {};
};


// Same comment as above applies about *Tree as a name,
// but what can you do?
typedef map<const Record *, RecordPList, cSort_by_attrib> PatentTree;

// Point a unique record at a unique inventor.
typedef map < const Record *, const Record *> Uid2UinvTree;


#include "string_manipulator.h"
#include "blocking_operation.h"
#include "record_reconfigurator.h"


class cRatios; //forward declaration

/**
 * disambiguate_by_set:
 * This is a global function. It takes information from two clusters
 * (not cluster_info!!) and return whether the two are of the same inventor,
 * and if yes, what the cohesion should be.
 *
 * Arguments:
 *
 * TODO: This should be named "representative1" or "rep1".
 * key1: the pointer to the representative record of the first cluster
 *
 * match1: the list of the pointers that belongs to the first cluster,
 * including key1.
 *
 * cohesion1: the internal cohesive value of the first cluster,
 * interpreted as the probability for all the members of the first
 * cluster to be of the same inventor.
 *
 * TODO: This should be named "representative2" or "rep2".
 * key2: the pointer to the representative record of the second cluster
 *
 * match2: the list of the pointers that belongs to the second cluster,
 * including key2.
 *
 * cohesion2: the internal cohesive value of the second cluster,
 * interpreted as the probability for all the members of the
 * second cluster to be of the same inventor.
 *
 * prior: the priori probability. Check for the math equation of
 * the way to calculate the probability.
 *
 * ratio: the map (binary tree) that maps a similarity profile
 * (vector<uint32_t>) to a positive real number (double).
 *
 * threshold: threshold of the probability that the two clusters
 * should be the of the same inventor.
 *
 * Return Value:
 *
 * std::pair<const Record *, double>: This is a pair value,
 * which consists of two parts.
 *
 * 1. const Record *: NULL if the two clusters are identified as of
 * different inventors, and key1 if they are of the same inventors.
 *
 * TODO: Define "combination"
 * 2. double: the cohesion of the combination of the first and the
 * second cluster. This is only valid if the first returned pointer is not NULL.
 */
// TODO: This should be named "representative1" or "rep1".
std::pair<const Record *, double> disambiguate_by_set (const Record * key1,
                                                       const RecordPList & match1,
                                                       const double cohesion1,
                                                       const Record * key2,
                                                       const RecordPList & match2,
                                                       const double cohesion2,
                                                       const double prior,
                                                       const cRatios & ratio,
                                                       const double threshold);

/** @public
 * Copies a file, of course.
 * @param target output file
 * @param source input file
 */
void        copyfile                  (const char * target,
                                       const char * source);

Attribute * create_attribute_instance (const string & id );

void        build_patent_tree         (PatentTree & patent_tree,
                                       const list<Record> & all_records);

void        build_patent_tree         (PatentTree & patent_tree,
                                       const RecordPList & all_rec_pointers);
                                       //const list < const Record * > & all_rec_pointers);

string      check_file_existence      (const string & description);

std::vector<std::string> parse_column_names(std::string line);

vector<uint32_t> create_column_indices(std::vector<std::string> requested_columns,
    std::vector<std::string> total_col_names);

Attribute ** instantiate_attributes(std::vector<std::string> column_names, int num_cols);

vector <const Attribute *> parse_line (string line,
                                       vector<uint32_t> requested_column_indice,
                                       Attribute ** pointer_array,
                                       uint32_t num_cols,
                                       const char * delim,
                                       vector<string> & string_cache);

void check_interactive_consistency(Attribute ** pointer_array,
    uint32_t num_cols,
    std::vector<std::string> column_names);


#endif /* PATENT_ENGINE_H */
