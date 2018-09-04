/*
 * kdNode.cpp
 *
 *  Created on: 11 jul. 2017
 *      Author: mmoyano
 */


#include "kdNode.h"
#include <map>
#include <queue>
#include <math.h>


using namespace std;

  namespace clp{





    KdNode::KdNode(const long *t , const KdData *bl, long* min, long* max, int depth, int dim) :
    		tuple(t), data(bl), ltChild(NULL), gtChild(NULL), min(min), max(max), depth(depth), dim(dim)  { }

    const long* KdNode::getTuple() const
    {
	return this->tuple;
    }

    const KdData* KdNode::getKdData() const
    {
    return this->data;
    }
/**
 guarda los valores de el vector cordenadas al vectore referencia?? ---------------------------------------------------------------------------------------------------
**/
   void KdNode::initializeReference(vector<long*>& coordinates, vector<long*>& reference)
    {
    	for (long j = 0; j < coordinates.size(); j++) {
    	    reference.at(j) = coordinates.at(j);
    	}
    }

    long KdNode::superKeyCompare(const long *a, const long *b, const long p, const long dim)
    {
	long diff = 0;
	for (long i = 0; i < dim; i++) {
	    long r = i + p;
	    // A fast alternative to the modulus operator for (i + p) < 2 * dim.
	    r = (r < dim) ? r : r - dim;
	    diff = a[r] - b[r];
	    if (diff != 0) {
		break;
	    }
	}
	return diff;
    }


    /**
    Ordena los vectores
    reference:vector con los bloques
    temporary:vector que se va creando de forma auxiliar
    low:
    high:
    p:
    dim:
    **/
    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    void KdNode::mergeSort(vector<long*> &reference, vector<long*>& temporary, const long low, const long high,
			  const long p, const long dim)
    {
	long i, j, k;

	if (high > low) {

	    // Avoid overflow when calculating the median.
	    const long mid = low + ( (high - low) >> 1 );
      // ----------- Esta parte es iterativa por lo cual no se debe modificar----------------------------//
	    // Recursively subdivide the lower and upper halves of the array.
	    mergeSort(reference, temporary, low, mid, p, dim);
	    mergeSort(reference, temporary, mid+1, high, p, dim);

      //--------------------Aqui importa cambiar-------------------------------------------------//
	    // Merge the results for this level of subdivision.
	    for (i = mid+1; i > low; i--) {
		temporary.at(i-1) = reference.at(i-1);
	    }
	    for (j = mid; j < high; j++) {
		temporary.at(mid+(high-j)) = reference.at(j+1); // Avoid address overflow.
	    }
	    for (k = low; k <= high; k++) {
		reference.at(k) =
		    (superKeyCompare(temporary.at(i), temporary.at(j), p, dim) < 0) ? temporary.at(i++) : temporary.at(j--);
	    }
	}
}

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
    long KdNode::removeDuplicates(vector<long*>& reference, const long i, const long dim)
    {
    	long end = 0;
    	for (long j = 1; j < reference.size(); j++) {
    	   long compare = superKeyCompare(reference.at(j), reference.at(j-1), i, dim);
    	   if (compare < 0) {
    		 cout << "merge sort failure: superKeyCompare(ref[" << j << "], ref["
    		     << j-1 << "], (" << i << ") = " << compare  << endl;
    		 exit(1);
    	   } else if (compare > 0) {
    		     reference.at(++end) = reference.at(j);
    	   }
    	}
	    return end;
    }


    const KdData* KdNode::searchKdData(long *t, vector<const KdData*>& bloques){
      //deberiamos eliminar los bloques de la lista??
      // juntar toda en un if unico
      //--------------------------------------- cambiar el get
      for(int i=0;i<bloques.size();i++){
        if(bloques.at(i)->coordinates[0]==t[0]){
          if(bloques.at(i)->coordinates[1]==t[1]){
            if(bloques.at(i)->coordinates[2]==t[2]){

                return bloques.at(i);
            }
          }
        }
      }


      return NULL;

    }


    void KdNode::compute_minmax() const{

    	maxvol=1;
    	for(int i=0;i<dim; i++){
    		min[i]=tuple[i];
    		max[i]=tuple[i];

    	    maxvol*=tuple[i];

    	}

    	if(gtChild){
    		gtChild->compute_minmax();
    		for(int i=0;i<dim; i++){
    			min[i]=std::min(min[i],gtChild->min[i]);
    			max[i]=std::max(max[i],gtChild->max[i]);
    		}
    		maxvol = std::max(maxvol,gtChild->maxvol);


    	}

    	if(ltChild){
    		ltChild->compute_minmax();
    		for(int i=0;i<dim; i++){
    			min[i]=std::min(min[i],ltChild->min[i]);
    			max[i]=std::max(max[i],ltChild->max[i]);
    		}
    		maxvol = std::max(maxvol,ltChild->maxvol);
    	}
    }


    KdNode* KdNode::buildKdTree(vector< vector<long*> >& references, vector<long*>& temporary, const long start,
			       const long end, const long dim, const long depth, vector<const KdData*>& bloques, long* min, long* max)//-puede cambiar a un list
    {
    	KdNode *node;



    	// The axis permutes as x, y, z, w... and addresses the referenced data.
    	long axis = depth % dim;


    	if(min==NULL){
    		min=new long[dim];
    		max=new long[dim];

    		for(int i=0; i<dim; i++){
    			min[i]=0;
    			max[i]=600;
    		}
    	}

    	if (end == start) {

    	    // Only one reference was passed to this function, so add it to the tree.
    	    node = new KdNode( references.at(0).at(end),searchKdData(references.at(0).at(end), bloques), min, max, depth,dim);


    	} else if (end == start + 1) {

    	    // Two references were passed to this function in sorted order, so store the start
    	    // element at this level of the tree and store the end element as the > child.
    	    node = new KdNode( references.at(0).at(start),searchKdData(references.at(0).at(start), bloques), min, max, depth ,dim );

    	    long* min2=new long[dim];
    	    long* max2=new long[dim];
    		for(int i=0; i<dim; i++){
    			min2[i]=min[i];
    			max2[i]=max[i];
    		}

    		min2[axis] = node->getTuple()[axis];

    	    node->gtChild = new KdNode( references.at(0).at(end) ,searchKdData(references.at(0).at(end), bloques), min2, max2, depth+1,dim);

    	} else if (end == start + 2) {

    	    // Three references were passed to this function in sorted order, so
    	    // store the median element at this level of the tree, store the start
    	    // element as the < child and store the end element as the > child.
    	    node = new KdNode( references.at(0).at(start + 1) ,searchKdData(references.at(0).at(start + 1), bloques), min, max, depth, dim);

    	    long* min2=new long[dim];
    	    long* max2=new long[dim];
    		for(int i=0; i<dim; i++){
    			min2[i]=min[i];
    			max2[i]=max[i];
    		}

    		max2[axis] = node->getTuple()[axis];

    	    node->ltChild = new KdNode( references.at(0).at(start) ,searchKdData(references.at(0).at(start), bloques), min2, max2, depth+1, dim);

    	    min2=new long[dim];
    	    max2=new long[dim];
    		for(int i=0; i<dim; i++){
    			min2[i]=min[i];
    			max2[i]=max[i];
    		}

    		min2[axis] = node->getTuple()[axis];

    	    node->gtChild = new KdNode( references.at(0).at(end) ,searchKdData(references.at(0).at(end), bloques), min2, max2, depth+1, dim);

    	} else if (end > start + 2) {

    	    // More than three references were passed to this function, so
    	    // the median element of references[0] is chosen as the tuple about
    	    // which the other reference arrays will be partitioned.  Avoid
    	    // overflow when computing the median.
    	    const long median = start + ((end - start) / 2);

    	    // Store the median element of references[0] in a new kdNode.
    	    node = new KdNode( references.at(0).at(median) ,searchKdData(references.at(0).at(median), bloques), min, max, depth,dim );

    	    // Copy references[0] to the temporary array before partitioning.
    	    for (long i = start; i <= end; i++) {
    		temporary.at(i) = references.at(0).at(i);
    	    }

    	    // Process each of the other reference arrays in a priori sorted order
    	    // and partition it by comparing super keys.  Store the result from
    	    // references[i] in references[i-1], thus permuting the reference
    	    // arrays.  Skip the element of references[i] that that references
    	    // a point that equals the point that is stored in the new k-d node.
    	    long lower, upper, lowerSave, upperSave;
    	    for (long i = 1; i < dim; i++) {

    		// Process one reference array.  Compare once only.
    		lower = start - 1;
    		upper = median;
    		for (long j = start; j <= end; j++) {
    		    long compare = superKeyCompare(references.at(i).at(j), node->tuple, axis, dim);
    		    if (compare < 0) {
    			references.at(i-1).at(++lower) = references.at(i).at(j);
    		    } else if (compare > 0) {
    			references.at(i-1).at(++upper) = references.at(i).at(j);
    		    }
    		}

    		// Check the new indices for the reference array.
    		if (lower < start || lower >= median) {
    		    cout << "incorrect range for lower at depth = " << depth << " : start = "
    			 << start << "  lower = " << lower << "  median = " << median << endl;
    		    exit(1);
    		}

    		if (upper <= median || upper > end) {
    		    cout << "incorrect range for upper at depth = " << depth << " : median = "
    			 << median << "  upper = " << upper << "  end = " << end << endl;
    		    exit(1);
    		}

    		if (i > 1 && lower != lowerSave) {
    		    cout << " lower = " << lower << "  !=  lowerSave = " << lowerSave << endl;
    		    exit(1);
    		}

    		if (i > 1 && upper != upperSave) {
    		    cout << " upper = " << upper << "  !=  upperSave = " << upperSave << endl;
    		    exit(1);
    		}

    		lowerSave = lower;
    		upperSave = upper;
    	    }

    	    // Copy the temporary array to references[dim-1] to finish permutation.
    	    for (long i = start; i <= end; i++) {
    		references.at(dim - 1).at(i) = temporary.at(i);
    	    }


    	    long* min2=new long[dim];
    	    long* max2=new long[dim];
    		for(int i=0; i<dim; i++){
    			min2[i]=min[i];
    			max2[i]=max[i];
    		}

    		max2[axis] = node->getTuple()[axis];

    	    // Recursively build the < branch of the tree.
    	    node->ltChild = buildKdTree(references, temporary, start, lower, dim, depth+1, bloques, min2, max2);

    	    min2=new long[dim];
    	    max2=new long[dim];
    		for(int i=0; i<dim; i++){
    			min2[i]=min[i];
    			max2[i]=max[i];
    		}

    		min2[axis] = node->getTuple()[axis];

    	    // Recursively build the > branch of the tree.
    	    node->gtChild = buildKdTree(references, temporary, median+1, upper, dim, depth+1, bloques, min2, max2);

    	} else if (end < start) {

    	    // This is an illegal condition that should never occur, so test for it last.
    	    cout << "error has occurred at depth = " << depth << " : end = " << end
    		 << "  <  start = " << start << endl;
    	    exit(1);

    	}

    	if(node->depth==0) node->compute_minmax();

    	// Return the pointer to the root of the k-d tree.
    	return node;
    }


    long KdNode::verifyKdTree(const long dim, const long depth) const
    {
	long count = 1 ;
	if (this->tuple == NULL) {
	    cout << "point is null!" << endl;
	    exit(1);
	}

	// The partition cycles as x, y, z, w...
	long axis = depth % dim;

	if (this->ltChild != NULL) {
	    if (this->ltChild->tuple[axis] > this->tuple[axis]) {
		cout << "child is > node!" << endl;
		exit(1);
	    }
	    if (superKeyCompare(this->ltChild->tuple, this->tuple, axis, dim) >= 0) {
		cout << "child is >= node!" << endl;
		exit(1);
	    }
	    count += this->ltChild->verifyKdTree(dim, depth + 1);
	}
	if (this->gtChild != NULL) {
	    if (this->gtChild->tuple[axis] < this->tuple[axis]) {
		cout << "child is < node!" << endl;
		exit(1);
	    }
	    if (superKeyCompare(this->gtChild->tuple, this->tuple, axis, dim) <= 0) {
		cout << "child is <= node" << endl;
		exit(1);
	    }
	    count += this->gtChild->verifyKdTree(dim, depth + 1);
	}
	return count;
    }

//-------------------------------------Aqui tenemos que recuperar los datos--------------------//
    KdNode* KdNode::createKdTree(const list<const KdData*>& lista,vector<long*>& coordinates, const long numDimensions)
    {

	// Initialize and sort the reference arrays.
  //aqui dejaremos las pruebas con los bloques

    //creamos el vector que va a contener los bloques
      vector<const KdData*> bloques(lista.size());
    //pasamos los valores del set al vector
      copy(lista.begin(),lista.end(),bloques.begin());


      //crea un vector que contiene vectores,
    	vector< vector<long*> > references(numDimensions, vector<long*>( coordinates.size() ) );
    	vector<long*> temporary( coordinates.size() );;
    	for (long i = 0; i < references.size(); i++) {

          //asigna los valores de las cordenadas al vector references
    	    initializeReference(coordinates, references.at(i));
    	    mergeSort(references.at(i), temporary, 0, references.at(i).size()-1, i, numDimensions);
    	}

    //----------------------------------Eliminar esta Parte-------------------------------------
    	// Remove references to duplicate coordinates via one pass through each reference array.
      // no deberian existir bloques repetidos, podriamos eliminar este trozo de codigo.
    	vector<long> end( references.size() );
    	for (long i = 0; i < end.size(); i++) {
    	    end.at(i) = removeDuplicates(references.at(i), i, numDimensions);
    	}
    //esto tambien deberia ser innecesario considerando que no existen bloques repetidos.
    	// Check that the same number of references was removed from each reference array.
    	for (long i = 0; i < end.size()-1; i++) {
    	    for (long j = i + 1; j < end.size(); j++) {
    		if ( end.at(i) != end.at(j) ) {
    		    cout << "reference removal error" << endl;
    		    exit(1);
    		}
    	    }
    	}
    //---------------------------------------------------------------------------------------------

    	// Build the k-d tree.
      //dejar que haga todo el ordenamiento y que al dentro de la contruccion, busque el bloque que le pertenece esos valores ya que estos son unicos

    	KdNode *root = buildKdTree(references, temporary, 0, end.at(0), numDimensions, 0, bloques);

    	// Verify the k-d tree and report the number of KdNodes.
    	long numberOfNodes = root->verifyKdTree(numDimensions, 0);
      //cout << numberOfNodes << endl;
    	// Return the pointer to the root of the k-d tree.
    	return root;
    }


//-----------------------------------Funcion de Distancia Manhattan


    long KdNode::distanceManhattan(const long* query,const long* maxpoint, const long dim) const{//se deberia compara con el punto
        long dist=0;
        for (long i = 0; i < dim; i++)
            //dist=dist+abs(query[i]-maxpoint[i]);//Saca la distancia absoluta
            dist=dist+(query[i]-maxpoint[i])*(query[i]-maxpoint[i]);

        return sqrt(dist);
        //return dist;
    }

//--------------------------------------Funcion de comparacion manhattan

  //void ordenador(list<KdNode>* lista,const long *query,const long dim){}

//--------------------------------------------Busqueda Interna

/*
    void KdNode::searchKdTreeintern(const long* father,const long* query,
    		long* maxpoint, long& cut, const long dim, const long depth, int n,
			multimap<long, const KdData*>& result) const {

    KdNode::nn++;

    // query(contiene el vectr a buscar)
    //cut=distancia que puede tener - este se reemplazara por el punto maximo
    //dimension= dimension de la tupla
    //depht es profundidad , pero no se por

	// The partition cycles as x, y, z, w...
	//long cut=distanceManhattan(query , maxpoint,dim);

	long axis = depth % dim;

	bool inside = true;



	//cout<< this->tuple << endl <<endl;
	for (long i = 0; i < dim; i++) {


	    if(this->tuple[i] > maxpoint[i]){//revisa elemento a alementeo , si alguno de sus elementos en mayor que el maximo
            inside = false;
            break;
	    }

      if (abs(query[i] - this->tuple[i]) > cut ) {//revisa elemento a elemento , si la diferencia en mayor a esto
            inside = false;
            break;
	    }
	}


	if (inside && data->is_valid()){
	    result.insert(make_pair(-distanceManhattan(tuple,query,dim), data));
	    if(result.size()>=n){
	    	if(result.size()>n) result.erase(result.begin());
	    	//cut es igual a la distancia de manhattan de la peor solución del mapa
	    	//incluso podríamos restarle 1...
	    	cut=-result.begin()->first;
	    	//cout << cut  << endl;
	    }
	}

	// Search the > branch of the k-d tree if the partition coordinate of the query point plus
	// the cutoff distance is >= the partition coordinate of the k-d node.  The < branch must be
	// searched when the cutoff distance equals the partition coordinate because the super key
	// may assign a point to either branch of the tree if the sorting or partition coordinate,
	// which forms the most significant portion of the super key, shows equality.
	if ( this->gtChild != NULL && (query[axis] + cut) >= this->tuple[axis]  && maxpoint[axis] >= this->tuple[axis] )
	    this->gtChild->searchKdTreeintern(father,query, maxpoint, cut, dim, depth + 1, n, result);


	// Search the < branch of the k-d tree if the partition coordinate of the query point minus
	// the cutoff distance is <= the partition coordinate of the k-d node.  The < branch must be
	// searched when the cutoff distance equals the partition coordinate because the super key
	// may assign a point to either branch of the tree if the sorting or partition coordinate,
	// which forms the most significant portion of the super key, shows equality.

	//(La busqueda es recursiva , es decir ve si el actual sirve y despues manda la funcion a sus hijos)


	if ( this->ltChild != NULL && (query[axis] - cut) <= this->tuple[axis] )
	    this->ltChild->searchKdTreeintern(father,query, maxpoint, cut, dim, depth + 1, n, result);



}
*/

bool comparacionCut(KdNode& first,KdNode& second){}

//bool KdNode::comparator (const KdNode *aNode,const KdNode *bNode )const{
//  return distanceManhattan(aNode->getTuple(),aNode->que,3)>distanceManhattan(bNode->getTuple(),bNode->que,3);
//}

//bool KdNode::operator() (const KdNode *aNode,const KdNode *bNode ){

//  return distanceManhattan(aNode->getTuple(),aNode->que,3)>distanceManhattan(bNode->getTuple(),bNode->que,3);
//}


//--------------------------Esta es la funcion llamada para la busqueda
void KdNode::update_n_best_solution(double &LB,const KdNode* nodo,double eval,int n ,multimap<long, const KdData*>& best_solutions ) {

	  best_solutions.insert(make_pair(eval, nodo->getKdData()));//positivo, de peor a mejor

	  if(best_solutions.size()>n+1){ cout << "error!" << endl; exit(1); };

      if(best_solutions.size()==n+1)
    	  best_solutions.erase(best_solutions.begin()); //se elimina la peor

      if(best_solutions.size()==n)
          LB=best_solutions.begin()->first;

}

void KdNode::compute_ub(const KdNode* nodo, const long* minpoint, const long* maxpoint){
	long min[dim];
	long max[dim];


    //el eval deberia contener el dim
    for(int i=0;i<nodo->dim;i++){

    	if(nodo->max[i]<minpoint[i] || nodo->min[i]>maxpoint[i] ){ nodo->ub= 0.0; return; }// no satisface

    	min[i] = std::max(nodo->min[i],minpoint[i]);
    	max[i] = std::min(nodo->max[i],maxpoint[i]);

    }

    nodo->ub= nodo->getKdData()->compute_ub(min,max, nodo->maxvol);

}


bool KdNode::satisfy(const long *minpoint,const long *maxpoint) const{
  for (int i=0;i<dim;i++){
    if(tuple[i]<minpoint[i])return false;
    if(tuple[i]>maxpoint[i])return false;
  }
  return true;
}

int KdNode::nn=0;//que es esto , un contador???? si

void KdNode::search(const long* minpoint, const long* maxpoint,
		int dim, multimap<long, const KdData*>& best_solutions, int n)  {

	//multimap<double, const KdNode*> L;

	std::priority_queue<const KdNode*, vector<const KdNode*>, byub> L; //mypq;


	const KdNode* root = this;
	compute_ub(root, minpoint, maxpoint);

	L.push(root);

	double LB=0.0;//el valor del mejor nodo evaluado hasta el momento

	while(L.size()!=0){
		//KdNode::nn++;

		const KdNode* nodo = L.top();
		L.pop();

		/*for(int i=0;i<dim;i++)
			cout << "[" << nodo->min[i] << "," << nodo->max[i] << "]" << endl;

		cout << "maxpoint:(" << maxpoint[0] << "," << maxpoint[1] << "," << maxpoint[2] << ")" << endl;

		cout << "point:(" << nodo->tuple[0] << "," << nodo->tuple[1] << "," << nodo->tuple[2] << ")" << endl;

		cout << "node_ub:" << node_ub << endl;
		 */

		if(nodo->ub<=LB) return;


		if(nodo->satisfy(minpoint, maxpoint)){
			double eval=nodo->getKdData()->eval();
			if(eval>LB)
				update_n_best_solution(LB, nodo, eval, n, best_solutions);
		}

        if(nodo->gtChild){
        	compute_ub(nodo->gtChild, minpoint, maxpoint);
        	if(nodo->gtChild->ub > LB) L.push(nodo->gtChild);
        }
        if(nodo->ltChild){
        	compute_ub(nodo->ltChild, minpoint, maxpoint);
        	if(nodo->ltChild->ub > LB) L.push(nodo->ltChild);
        }

	}

}

/*
void KdNode::searchKdTreeroot(const long* query, long* maxpoint, const long dim,
				     const long depth,int n, list<const KdData*>& result_list) const {

	KdNode::nn++;

    //creo que esto es cuando n=1
    long minn[]={0,0,0};

    long cut = 10000;

	long axis = depth % dim;

	// If the distance from the query node to the k-d node is within the cutoff distance
	// in all k dimensions, add the k-d node to a list.
	multimap<long, const KdData*> result;

	bool inside = true;
	for (long i = 0; i < dim; i++) {

      if(this->tuple[i] > maxpoint[i]){//revisa elemento a elemento , si alguno de sus elementos es mayor que el maximo
           inside = false;
           break;
      }

      if (abs(query[i] - this->tuple[i]) > cut ) {//revisa elemento a elemento , si la diferencia en mayor a esto
            inside = false;
            break;
	  }

	}

	if (inside && data->is_valid()){
	    result.insert(make_pair(-distanceManhattan(tuple,query,dim), data));
	    if(result.size()==n){

	    	//cut es igual a la distancia de manhattan de la peor solución del mapa
	    	//incluso podríamos restarle 1...
	    	cut=-result.begin()->first;
	    	//cout << cut << endl;
	    }
	}


	// Search the > branch of the k-d tree if the partition coordinate of the query point plus
	// the cutoff distance is >= the partition coordinate of the k-d node.  The < branch must be
	// searched when the cutoff distance equals the partition coordinate because the super key
	// may assign a point to either branch of the tree if the sorting or partition coordinate,
	// which forms the most significant portion of the super key, shows equality.
	if ( this->gtChild != NULL && (query[axis] + cut) >= this->tuple[axis] && maxpoint[axis] >= this->tuple[axis])
	    this->gtChild->searchKdTreeintern(this->tuple,query, maxpoint, cut, dim, depth + 1, n, result);

	// Search the < branch of the k-d tree if the partition coordinate of the query point minus
	// the cutoff distance is <= the partition coordinate of the k-d node.  The < branch must be
	// searched when the cutoff distance equals the partition coordinate because the super key
	// may assign a point to either branch of the tree if the sorting or partition coordinate,
	// which forms the most significant portion of the super key, shows equality.

	//(La busqueda es recursiva , es decir ve si el actual sirve y despues manda la funcion a sus hijos)
	if ( this->ltChild != NULL && (query[axis] - cut) <= this->tuple[axis] )
	    this->ltChild->searchKdTreeintern(this->tuple,query, maxpoint, cut, dim, depth + 1, n, result);




  //result.sort(comparator);
  //vamos a ordenar la lista con la clase SortKdNode
 // escogemos los n**2 mejores resultados
  //result.erase(n,result.end())

	for(auto nodo:result)
		result_list.push_front(nodo.second);

    }
 */


    void KdNode::printTuple(const long* tuple, const long dim)
    {
	cout << "(" << tuple[0] << ",";
	for (long i=1; i<dim-1; i++) cout << tuple[i] << ",";
	cout << tuple[dim-1] << ")";
    }


      void KdNode::printKdTree(const long dim, const long depth) const
      {
  	if (this->gtChild != NULL) {
  	    this->gtChild->printKdTree(dim, depth+1);
  	}
  	for (long i=0; i<depth; i++) cout << "       ";
  	printTuple(this->tuple, dim);
  	cout << endl;
  	if (this->ltChild != NULL) {
  	    this->ltChild->printKdTree(dim, depth+1);
  	}
      }
      }
