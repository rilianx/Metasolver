/*
 * kdNode.h
 *
 *  Created on: 11 jul. 2016
 *      Author: mmoyano
 */


#ifndef _KDNODE
#define _KDNODE

#include <stdbool.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <iostream>
#include <set>
#include <algorithm>
#include <map>

#define DIMENSIONS (3)

using namespace std;

namespace clp {


class KdData{

public:
	virtual double eval() const=0;

	virtual double compute_ub(const long* minpoint, const long* maxpoint, long max_vol) const=0;

	public:
		long *coordinates;


};



/* One node of a k-d tree */
class KdNode
{

public:
    const long *tuple;
    mutable long *min;
    mutable long *max;
    mutable long maxvol;
	const int dim;
    const int depth;

    const KdData *data;

    mutable double ub;


public:
    const KdNode *ltChild, *gtChild;



    static int nn;

    KdNode(const long *t , const KdData *bl, long* min, long* max, int depth,int dim);
    const long *getTuple() const;
    const KdData *getKdData() const ;

private:
    /*
     * inicializar a reference array by creating references into the coordinates array.
     *
     * calling parameters:
     *
     * coordinates - a vector<long*> of pointers to each of the (x, y, z, w...) tuples
     * reference - a vector<long*> that represents one of the reference arrays
     */
    static void initializeReference(vector<long*>& coordinates, vector<long*>& reference);

    /*
     * The superKeyCompare method compares two long arrays in all k dimensions,
     * and uses the sorting or partition coordinate as the most significant dimension.
     *
     * calling parameters:
     *
     * a - a long*
     * b - a long*
     * p - the most significant dimension
     * dim - the number of dimensions
     *
     * returns: a long result of comparing two long arrays
     */

    static long superKeyCompare(const long *a, const long *b, const long p, const long dim);

    /*
     * The mergeSort function recursively subdivides the array to be sorted
     * then merges the elements. Adapted from Robert Sedgewick's "Algorithms
     * in C++" p. 166. Addison-Wesley, Reading, MA, 1992.
     *
     * calling parameters:
     *
     * reference - a vector<long*> that represents the reference array to sort
     * temporary - a temporary array into which to copy intermediate results;
     *             this array must be as large as the reference array
     * low - the start index of the region of the reference array to sort
     * high - the high index of the region of the reference array to sort
     * p - the sorting partition (x, y, z, w...)
     * dim - the number of dimensions
     * depth - the depth of subdivision
     */

    static void mergeSort(vector<long*> &reference, vector<long*>& temporary, const long low, const long high,
			  const long p, const long dim);

    /*
     * Check the validity of the merge sort and remove duplicates from a reference array.
     *
     * calling parameters:
     *
     * reference - a vector<long*> that represents one of the reference arrays
     * i - the leading dimension for the super key
     * dim - the number of dimensions
     *
     * returns: the end index of the reference array following removal of duplicate elements
     */

    static long removeDuplicates(vector<long*>& reference, const long i, const long dim);

    /*
     * This function builds a k-d tree by recursively partitioning the
     * reference arrays and adding kdNodes to the tree.  These arrays
     * are permuted cyclically for successive levels of the tree in
     * order that sorting occur on x, y, z, w...
     *
     * calling parameters:
     *
     * references - a vector< vector<long*> > of pointers to each of the (x, y, z, w...) tuples
     * temporary - a vector<long*> that is used as a temporary array
     * start - start element of the reference arrays
     * end - end element of the reference arrays
     * dim - the number of dimensions
     * depth - the depth in the tree
     *
     * returns: a KdNode pointer to the root of the k-d tree
     */

    static KdNode *buildKdTree(vector< vector<long*> >& references, vector<long*>& temporary, const long start,
			       const long end, const long dim, const long depth, vector<const KdData*>& bloques, long* min=NULL, long* max=NULL);

    /*
     * Walk the k-d tree and check that the children of a node are in the correct branch of that node.
     *
     * calling parameters:
     *
     * dim - the number of dimensions
     * depth - the depth in the k-d tree
     *
     * returns: a count of the number of kdNodes in the k-d tree
     */

    long verifyKdTree(const long dim, const long depth) const;

    /*
     * The createKdTree function performs the necessary initialization then calls the buildKdTree function.
     *
     * calling parameters:
     *
     * coordinates - a vector<long*> of references to each of the (x, y, z, w...) tuples
     * numDimensions - the number of dimensions
     *
     * returns: a KdNode pointer to the root of the k-d tree
     */
public:
    void compute_ub(const KdNode* nodo, const long* minpoint, const long* maxpoint);

    bool satisfy(const long *min,const long *max) const;

    void update_n_best_solution(double &LB,const KdNode* nodo,double eval,int n ,multimap<long, const KdData*>& best_solutions );


    /**
     * Compute the vectors min/max using the information of the tuples
     */
    void compute_minmax() const;



    static KdNode *createKdTree(const list<const KdData*>& lista, vector<long*>& coordinates, const long numDimensions);

    /*
     * Search the k-d tree and find the KdNodes that lie within a cutoff distance
     * from a query node in all k dimensions.
     *
     * calling parameters:
     *
     * query - the query point
     * cut - the cutoff distance
     * dim - the number of dimensions
     * depth - the depth in the k-d tree
     *
     * returns: a list that contains the kdNodes that lie within the cutoff distance of the query node
     */

     /*
     Esta funcion obtiene la distancia manhattan entre 2 puntos entre mayor sean los puntos, mas grande sera

     */

    long distanceManhattan(const long* query,const long* maxpoint, const long dim) const;

    static const KdData* searchKdData( long* t, vector<const KdData*>& bloques);


    void search(const long* minpoint, const long* maxpoint,
    		int dim, multimap<long, const KdData*>& best_solutions, int n) ;

//    void searchKdTreeintern(const long* father,const long* query, long* maxpoint, long& cut, const long dim,
	//			     const long depth, int n, multimap<long, const KdData*>& ) const;


//    void searchKdTreeroot(const long* , long* maxpoint, const long dim,
	//			     const long depth, int n, list<const KdData*>& ) const;
/*
    void search(const long* query, long* maxpoint, const long dim, int n, list<const KdData*>& data) const{
    	list<KdNode> nodes=searchKdTreeroot(query, maxpoint, dim, 0, n) ;
    	list<KdNode>::iterator it=nodes.begin();
    	for(;it!=nodes.end();it++)
    		data.push_back((*it).getKdData());
    }
*/
    /*
     * Print one tuple.
     *
     * calling parameters:
     *
     * tuple - the tuple to print
     * dim - the number of dimensions
     */

    static void printTuple(const long* tuple, const long dim);

    /*
     * Print the k-d tree "sideways" with the root at the ltChild.
     *
     * calling parameters:
     *
     * dim - the number of dimensions
     * depth - the depth in the k-d tree
     */

    void printKdTree(const long dim, const long depth) const;


//------------------Ordenamiento, aun quiero probar si puedo hacer el sort
		//bool comparator (const KdNode *aNode,const KdNode *bNode )const;
		//void ordenador(list<KdNode>* lista,const long query);
		//void quickSort();
		//int partition();
		//bool operator()(const KdNode *aNode,const KdNode *bNode );
		//struct docompare
		//{
		//	bool operator()(const KdNode *aNode,const KdNode *bNode );
		//}

};

class byub
{

public:

  bool operator() (const KdNode* a, const KdNode* b) const
  {
    return (a->ub > b->ub);
  }
};


}

#endif
