#!/usr/bin/env python
# -*- coding: utf-8
#
# Gibran Fuentes Pineda <gibranfp@unam.mx>
# IIMAS, UNAM
# 2016
#
# -------------------------------------------------------------------------
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
# -------------------------------------------------------------------------
"""
Functions perform Sampled LSH
"""

import itertools
import numpy as np
from scipy.sparse import csr_matrix
import lsh_api as la

def lsh_init(seed):
    la.mh_rng_init(seed)

def lsh_load(filename):
    """
    Loads a ListDB array from a given file
    """
    ldb = la.listdb_load_from_file(filename)
    return LSH(ldb=ldb)

def csr_to_listdb(csr):
    """
    Converts a Compressed Sparse Row (CSR) matrix to a ListDB structure
    """
    if issubclass(csr.dtype.type,np.integer):
        factor=1
    else:
        factor= 100000000
 
    ldb = la.listdb_create(csr.shape[0], csr.shape[1])
    coo = csr.tocoo()    
    for i,j,v in itertools.izip(coo.row, coo.col, coo.data):
        ldb.push(int(i), int(j), int(round(v * factor)))

    return LSH(ldb=ldb)

def ndarray_to_listdb(arr):
    """
    Converts a numpy multidimensional array to a ListDB structure
    """
    if issubclass(arr.dtype.type,np.integer):
        factor=1
    else:
        factor= 100000000
    ldb = la.listdb_create(arr.shape[0], arr.shape[1])
    for i,row in enumerate(arr):
        for j,item in enumerate(row):
            if item != 0:
                ldb.push(int(i), int(j), int(round(item * factor)))
    return LSH(ldb=ldb)

def centers_from_labels(data, labels):
    """
    Computes the center points of a set of clusters from a list of labels
    """
    number_of_classes = np.max(labels) + 1
    centers = np.zeros((number_of_classes, data.shape[1]))
    for i in range(number_of_classes):
        centers[i] = data[np.where(labels==i)].mean(axis=0)

    return ndarray_to_listdb(centers)

class LSH:
    """
    Interface class to Sampled LSH
    """
    def __init__(self,size=0,dim=0,ldb=None):
        """
        Initializes class with ListDB structure
        """
        self._inverted=False
        self._original=None
        if ldb:
            self.ldb=ldb
        else:
            self.ldb = la.listdb_create(size,dim)
   
    def push(self,topic):
        """
        Appends list to a ListDB structure
        """
        la.listdb_push(self.ldb,topic)

    def save(self,filename):
        """
        Saves ListDB structure to file
        """
        la.listdb_save_to_file(filename,self.ldb)

    def show(self):
        """
        Prints ListDB structure
        """
        la.listdb_print(self.ldb)

    def mine(self,tuple_size,num_tuples,table_size=2**19):
        """
        Mines co-occurring items from a database of lists using Sampled LSH
        """

        ldb=la.sampledmh_l1mine(self.ldb,tuple_size,num_tuples,max_value,table_size)
            
        return LSH(ldb=ldb)

    def cluster_mhlink(self, num_tuples=255, tuple_size=3, table_size=2**20, thres=0.7,
                       min_cluster_size=3):
        """
        Clusters a database of mined lists using agglomerative clustering based on LSH
        """
        models=la.mhlink_cluster(self.ldb, tuple_size, num_tuples, table_size,
                                 la.list_overlap, thres, min_cluster_size)

        la.listdb_apply_to_all(models, la.list_sort_by_frequency_back)
                
        return LSH(ldb=models)

    def cluster_sklearn(self, algorithm):
        """
        Clusters a database of mined lists using the clustering methods available in scikit-learn
        """
        csr = self.tocsr()
        algorithm.fit(csr.toarray())
        if hasattr(algorithm, 'cluster_centers_'):
            ldb = ndarray_to_listdb(algorithm.cluster_centers_)
        else:
            ldb = centers_from_labels(csr, algorithm.labels_)

        la.listdb_apply_to_all(ldb.ldb, la.list_sort_by_frequency_back)
        
        return ldb

    def cutoff(self,min=5,max=None):
        """
        Removes the largest and smallest lists in the ListDB
        """
        if min:
            la.listdb_delete_smallest(self.ldb,min)
        if max:
            la.listdb_delete_largest(self.ldb,max)

    def size(self):
        """
        Returns the size of the ListDB structure
        """
        return self.ldb.size

    def dim(self):
        """
        Returns the dimensionality of the ListDB structure
        """
        return self.ldb.dim

    def destroy(self):
        """
        Destroys the ListDB structure
        """
        la.listdb_destroy(self.ldb)

    def tocsr(self):
        """
        Returns the ListDB structure as a Compressed Sparse Row (CSR) matrix
        """
        indptr = [0]
        indices = []
        data = []
        for l in self.ldb:
            for i in l:
                indices.append(i.item)
                data.append(i.freq)
                indptr.append(len(indices))

        return csr_matrix((data, indices, indptr), dtype=int)

    def toarray(self):
        """
        Converts a listdb structure to a numpy multidimensional array
        """
        arr = np.zeros((self.size(), self.dim()))
        for i, l in enumerate(self.ldb):
            for j in l:
                arr[i, j.item] = j.freq

        return arr

if __name__ == "__main__":
    import argparse
    p = argparse.ArgumentParser("Mines")
    p.add_argument("-r","--tuple_size",default=4,type=int,
        action="store", dest='r',help="Size of the tupple")
    p.add_argument("-l","--number_tuples",default=10,type=int,
        action="store", dest='l',help="Number of the tupple")
    p.add_argument("--output",default=None,type=str,
        action="store", dest='output',help="Filename to save mined model")

    opts = p.parse_args()

    s=lsh_load(opts.file)
    print "Size of loaded file:",s.size()
    m=s.mine(opts.r,opts.l)
    print "Size of original mined topics:",m.size()
    m.cutoff(min=opts.min)
    print "Size of cutted off mined topics:",m.size()
    if opts.output:
        print "Saving resulting model to",opts.output
        m.save(opts.output)
