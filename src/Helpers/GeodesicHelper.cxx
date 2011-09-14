/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#include "SurfaceFile.h"
#include "GeodesicHelper.h"
#include "TopologyHelper.h"
#include <iostream>
#include "CaretMutex.h"

using namespace caret;

GeodesicHelperBase::GeodesicHelperBase(const SurfaceFile* surfaceIn)
{
   TopologyHelper topoHelpIn(surfaceIn, true, true, false);
   int i, j, k, m, myneigh, myneigh2, farnode, farbase, coordbase, neigh2base, neighbase;
   numNodes = surfaceIn->getNumberOfCoordinates();
   //allocate
   numNeighbors = new int[numNodes];
   nodeNeighbors = new int*[numNodes];
   distances = new float*[numNodes];
   const float* coords = surfaceIn->getCoordinate(0);//FIXME: make this not a brutal hack
   float d[3], g[3], ac[3], abhat[3], abmag, ad[3], efhat[3], efmag, ea[3], cdmag, eg[3], eh[3], ah[3], tempvec[3], tempf;
   for (i = 0; i < numNodes; ++i)
   {//get neighbors
      const int* neighbors = topoHelpIn.getNodeNeighbors(i, numNeighbors[i]);
      nodeNeighbors[i] = new int[numNeighbors[i]];
      distances[i] = new float[numNeighbors[i]];
      coordbase = i * 3;
      for (j = 0; j < numNeighbors[i]; ++j)
      {
         nodeNeighbors[i][j] = neighbors[j];
         coordDiff(coords + coordbase, coords + neighbors[j] * 3, tempvec);
         distances[i][j] = std::sqrt(tempvec[0] * tempvec[0] + tempvec[1] * tempvec[1] + tempvec[2] * tempvec[2]);//precompute for speed in calls
      }//so few floating point operations, this should turn out symmetric
   }
   //TODO: use edge info for this rather than manually finding tiles with shared edges
   std::vector<int> tempneigh2;
   std::vector<float> tempdist2;
   std::vector<int> maintiles, neightiles;
   nodeNeighbors2 = new int*[numNodes];
   numNeighbors2 = new int[numNodes];
   distances2 = new float*[numNodes];
   for (i = 0; i < numNodes; ++i)
   {//find shared neighbors, for smoothed dijkstra
      tempneigh2.clear();
      tempdist2.clear();
      coordbase = i * 3;//precompute the multiplicative part of the coordinate index, for efficiency
      topoHelpIn.getNodeTiles(i, maintiles);
      for (j = 0; j < (int)maintiles.size(); ++j)
      {
         const int32_t* tile1 = surfaceIn->getTriangle(maintiles[j]);
         myneigh = -1;
         myneigh2 = -1;
         for (k = 0; k < 3; ++k)
         {
            if (tile1[k] != i)
            {
               if (myneigh == -1)
               {
                  myneigh = tile1[k];
               } else {
                  myneigh2 = tile1[k];
               }
            }
         }
         if (myneigh2 == -1) continue;//a tile has less than 3 distinct node numbers, move to next tile
         neighbase = myneigh * 3;
         neigh2base = myneigh2 * 3;
         topoHelpIn.getNodeTiles(myneigh, neightiles);
         for (k = 0; k < (int)neightiles.size(); ++k)
         {
            const int* tile2 = surfaceIn->getTriangle(neightiles[k]);
            farnode = -1;
            for (m = 0; m < 3; ++m)
            {
               if (tile2[m] == i) break;//discard both tiles that have the root node
               if (tile2[m] != myneigh && tile2[m] != myneigh2)
               {//check for matching edge between myneigh and myneigh2 by looking for exactly 1 node that doesn't match either
                  if (farnode == -1)
                  {
                     farnode = tile2[m];
                  } else break;//if 2 nodes do not belong to the close triangle, this is not the right tile
               }
            }
            if (m == 3)//loop completed without break (correct triangle found), time to do some math
            {
               if (farnode < i)//we have already computed from anything less than i to all possibilities, so grab that value instead
               {
                  for (m = 0; m < numNeighbors2[farnode]; ++m)
                  {//so find it
                     if (nodeNeighbors2[farnode][m] == i)
                     {
                        tempneigh2.push_back(farnode);
                        tempdist2.push_back(distances2[farnode][m]);
                        break;
                     }
                  }//if it wasn't found, then it was invalid (obtuse tetralateral after unfolding)
                  break;//consider next root tile
               }
               farbase = farnode * 3;
               coordDiff(coords + neigh2base, coords + neighbase, abhat);//a is neigh, b is neigh2, b - a = (vector)ab
               abmag = normalize(abhat);
               coordDiff(coords + farbase, coords + neighbase, ac);//c is farnode, c - a = (vector)ac
               tempf = dotProd(abhat, ac);
               ad[0] = abhat[0] * tempf;//d is the point on the shared edge that farnode (c) is closest to
               ad[1] = abhat[1] * tempf;//this way we can "unfold" the triangles by projecting the unit vector of the root node to closest point on shared edge from d, the distance of cd
               ad[2] = abhat[2] * tempf;
               d[0] = coords[neighbase] + ad[0];//and now we have the point d
               d[1] = coords[neighbase + 1] + ad[1];
               d[2] = coords[neighbase + 2] + ad[2];
               coordDiff(coords + neighbase, coords + coordbase, ea);//e is node i, the root node, a - e = (vector)ea
               tempf = dotProd(abhat, ea);//find the component of ea perpendicular to the shared edge
               tempvec[0] = abhat[0] * tempf;//find vector fa, f being the point on shared edge closest to e, the root node
               tempvec[1] = abhat[1] * tempf;
               tempvec[2] = abhat[2] * tempf;
               efhat[0] = ea[0] - tempvec[0];//and subtract it to obtain only the perpendicular
               efhat[1] = ea[1] - tempvec[1];
               efhat[2] = ea[2] - tempvec[2];
               efmag = normalize(efhat);//normalize to get unit vector
               coordDiff(&d[0], coords + farbase, tempvec);//this is vector cd, perpendicular to shared edge, from shared edge to far point
               cdmag = normalize(tempvec);//get its magnitude
               tempvec[0] = efhat[0] * cdmag;//vector dg, from shared edge at closest point to c (far node), to the unfolded position of farnode (g)
               tempvec[1] = efhat[1] * cdmag;
               tempvec[2] = efhat[2] * cdmag;
               g[0] = d[0] + tempvec[0];//add vector dg to point d to get point g, the unfolded position of farnode
               g[1] = d[1] + tempvec[1];
               g[2] = d[2] + tempvec[2];
               coordDiff(&g[0], coords + coordbase, eg);//this is the vector from root (e) to far node after unfolding (g), this is our distance
               tempf = efmag / (efmag + cdmag);//now we need to check that the path stays inside the tetralateral (ie, that it is convex)
               eh[0] = eg[0] * tempf;//this is a vector from e (root node) to the point on the shared edge that the full path (eg) crosses
               eh[1] = eg[1] * tempf;//this is because the lengths of the two perpendiculars to the root and far nodes from the shared edge establishes proportionality
               eh[2] = eg[2] * tempf;
               ah[0] = eh[0] - ea[0];//eh - ea = eh + ae = ae + eh = ah, vector from neigh to the point on shared edge the path goes through
               ah[1] = eh[1] - ea[1];
               ah[2] = eh[2] - ea[2];
               tempf = dotProd(ah, abhat);//get the component along ab so we can test that it is positive and less than |ab|
               if (tempf <= 0.0f || tempf >= abmag) break;//tetralateral is obtuse or triangular, our path is invalid or not shorter, so consider next root tile
               tempf = normalize(eg);//this is our path length
               tempneigh2.push_back(farnode);
               tempdist2.push_back(tempf);
               break;//there should not be 3 tiles sharing one edge, move to next root tile
            }
         }
      }
      numNeighbors2[i] = tempneigh2.size();
      nodeNeighbors2[i] = new int[numNeighbors2[i]];
      distances2[i] = new float[numNeighbors2[i]];
      for (j = 0; j < numNeighbors2[i]; ++j)
      {
         nodeNeighbors2[i][j] = tempneigh2[j];
         distances2[i][j] = tempdist2[j];
      }
   }
}

GeodesicHelper::GeodesicHelper(GeodesicHelperBase& baseIn)
{
   //get references and info from base
   numNodes = baseIn.numNodes;
   distances = baseIn.distances;
   distances2 = baseIn.distances2;
   numNeighbors = baseIn.numNeighbors;
   numNeighbors2 = baseIn.numNeighbors2;
   nodeNeighbors = baseIn.nodeNeighbors;
   nodeNeighbors2 = baseIn.nodeNeighbors2;
   //allocate private scratch space
   output = new float[numNodes];
   marked = new int[numNodes];
   changed = new int[numNodes];
   parent = new int[numNodes];
   for (int i = 0; i < numNodes; ++i)
   {
      marked[i] = 0;//initialize
   }
}

void GeodesicHelper::getNodesToGeoDist(const int node, const float maxdist, std::vector<int>& nodesOut, std::vector<float>& distsOut, const bool smoothflag)
{//public methods sanity check, private methods process
   nodesOut.clear();
   distsOut.clear();
   if (node >= numNodes || maxdist < 0.0f || node < 0) return;
   CaretMutexLocker locked(&inUse);//let sanity checks go multithreaded, as if it mattered
   dijkstra(node, maxdist, nodesOut, distsOut, smoothflag);
}

void GeodesicHelper::getNodesToGeoDist(const int node, const float maxdist, std::vector<int>& nodesOut, std::vector<float>& distsOut, std::vector<int>& parentsOut, const bool smoothflag)
{//public methods sanity check, private methods process
   nodesOut.clear();
   distsOut.clear();
   if (node >= numNodes || maxdist < 0.0f || node < 0) return;
   CaretMutexLocker locked(&inUse);//we need the parents array to stay put, so don't scope this
   dijkstra(node, maxdist, nodesOut, distsOut, smoothflag);
   int mysize = (int)nodesOut.size();
   parentsOut.resize(mysize);
   for (int i = 0; i < mysize; ++i)
   {
      parentsOut[i] = parent[nodesOut[i]];
   }
}

void GeodesicHelper::dijkstra(const int root, const float maxdist, std::vector<int>& nodes, std::vector<float>& dists, bool smooth)
{
   int i, j, whichnode, whichneigh, numNeigh, numChanged = 0;
   int* neighbors;
   float tempf;
   output[root] = 0.0f;
   marked[root] |= 4;
   parent[root] = root;//idiom for end of path
   changed[numChanged++] = root;
   myheap active;
   active.push(root, 0.0f);
   //we keep values greater than maxdist off the stack, so anything pulled from the stack which is unmarked belongs in the list
   while (!active.isEmpty())
   {
      whichnode = active.pop();
      if (!(marked[whichnode] & 1))
      {
         nodes.push_back(whichnode);
         dists.push_back(output[whichnode]);
         marked[whichnode] |= 1;//anything pulled from stack will already be marked as having a valid value (flag 4)
         neighbors = nodeNeighbors[whichnode];
         numNeigh = numNeighbors[whichnode];
         for (j = 0; j < numNeigh; ++j)
         {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if marked
               tempf = output[whichnode] + distances[whichnode][j];//isn't precomputation wonderful
               if (tempf <= maxdist)
               {//keep it off the heap if it is too far
                  if (!(marked[whichneigh] & 4))
                  {
                     parent[whichneigh] = whichnode;
                     marked[whichneigh] |= 4;
                     changed[numChanged++] = whichneigh;
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  } else if (tempf < output[whichneigh]) {
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  }
               }
            }
         }
         if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
         {
            neighbors = nodeNeighbors2[whichnode];
            numNeigh = numNeighbors2[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
               whichneigh = neighbors[j];
               if (!(marked[whichneigh] & 1))
               {//skip floating point math if marked
                  tempf = output[whichnode] + distances2[whichnode][j];//isn't precomputation wonderful
                  if (tempf <= maxdist)
                  {//keep it off the heap if it is too far
                     if (!(marked[whichneigh] & 4))
                     {
                        parent[whichneigh] = whichnode;
                        marked[whichneigh] |= 4;
                        changed[numChanged++] = whichneigh;
                        output[whichneigh] = tempf;
                        active.push(whichneigh, tempf);
                     } else if (tempf < output[whichneigh]) {
                        output[whichneigh] = tempf;
                        active.push(whichneigh, tempf);
                     }
                  }
               }
            }
         }
      }
   }
   for (i = 0; i < numChanged; ++i)
   {
      marked[changed[i]] = 0;//minimize reinitialization of arrays
   }
}

void GeodesicHelper::dijkstra(const int root, bool smooth)
{//straightforward dijkstra, no cutoffs, full surface
   int i, j, whichnode, whichneigh, numNeigh;
   int* neighbors;
   float tempf;
   output[root] = 0.0f;
   parent[root] = root;//idiom for end of path
   myheap active;
   active.push(root, 0.0f);
   while (!active.isEmpty())
   {
      whichnode = active.pop();
      if (!(marked[whichnode] & 1))
      {
         marked[whichnode] |= 1;
         neighbors = nodeNeighbors[whichnode];
         numNeigh = numNeighbors[whichnode];
         for (j = 0; j < numNeigh; ++j)
         {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if marked
               tempf = output[whichnode] + distances[whichnode][j];
               if (!(marked[whichneigh] & 4))
               {
                  parent[whichneigh] = whichnode;
                  marked[whichneigh] |= 4;
                  output[whichneigh] = tempf;
                  active.push(whichneigh, tempf);
               } else if (tempf < output[whichneigh]) {
                  output[whichneigh] = tempf;
                  active.push(whichneigh, tempf);
               }
            }
         }
         if (smooth)
         {
            neighbors = nodeNeighbors2[whichnode];
            numNeigh = numNeighbors2[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
               whichneigh = neighbors[j];
               if (!(marked[whichneigh] & 1))
               {//skip floating point math if marked
                  tempf = output[whichnode] + distances2[whichnode][j];
                  if (!(marked[whichneigh] & 4))
                  {
                     parent[whichneigh] = whichnode;
                     marked[whichneigh] |= 4;
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  } else if (tempf < output[whichneigh]) {
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  }
               }
            }
         }
      }
   }
   for (i = 0; i < numNodes; ++i)
   {
      marked[i] = 0;
   }
}

float** GeodesicHelper::getGeoAllToAll(const bool smooth)
{
   long long bytes = (((long long)numNodes) * numNodes * (sizeof(float) + sizeof(int)) + numNodes * (sizeof(float*) + sizeof(int*))) * 100;//fixed point
   short index = 0;
   static const char *labels[9] = {" Bytes", " Kilobytes", " Megabytes", " Gigabytes", " Terabytes", " Petabytes", " Exabytes", " Zettabytes", " Yottabytes"};
   while (index < 8 && bytes > 80000)
   {//add 2 zeroes, thats 800.00
      ++index;
      bytes = bytes >> 10;
   }
   CaretMutexLocker locked(&inUse);//don't sit there with memory allocated but locked out of computation, lock early - also before status messages
   std::cout << "attempting to allocate " << bytes / 100 << "." << bytes % 100 << labels[index] << "...";
   std::cout.flush();
   int i = -1, j;
   bool fail = false;
   float** ret = NULL;
   int** parents = NULL;
   try
   {
      ret = new float*[numNodes];
      if (ret != NULL)
      {
         for (i = 0; i < numNodes; ++i)
         {
            ret[i] = new float[numNodes];
            if (ret[i] == NULL)
            {
               fail = true;
               break;//have to break so it doesn't increment i
            }
         }
      }
   } catch (std::bad_alloc e) {//should catch if new likes to throw exceptions instead of returning null
      fail = true;
   }
   if (ret == NULL)
   {
      std::cout << "failed" << std::endl;
      return NULL;
   }
   if (fail)
   {
      std::cout << "failed" << std::endl;
      for (j = 0; j < i; ++j) delete[] ret[j];
      if (i > -1) delete[] ret;
      return NULL;
   }
   i = -1;
   try
   {
      parents = new int*[numNodes];
      if (parents != NULL)
      {
         for (i = 0; i < numNodes; ++i)
         {
            parents[i] = new int[numNodes];
            if (parents[i] == NULL)
            {
               fail = true;
               break;//have to break so it doesn't increment i
            }
         }
      }
   } catch (std::bad_alloc e) {//should catch if new likes to throw exceptions instead of returning null
      fail = true;
   }
   if (parents == NULL)
   {
      std::cout << "failed" << std::endl;
      for (i = 0; i < numNodes; ++i) delete[] ret[i];
      delete[] ret;
      return NULL;
   }
   if (fail)
   {
      std::cout << "failed" << std::endl;
      for (j = 0; j < i; ++j) delete[] parents[j];
      if (i > -1) delete[] parents;
      for (i = 0; i < numNodes; ++i) delete[] ret[i];
      delete[] ret;
      return NULL;
   }
   std::cout << "success" << std::endl;
   alltoall(ret, parents, smooth);
   for (i = 0; i < numNodes; ++i) delete[] parents[i];
   delete[] parents;
   return ret;
}

void GeodesicHelper::alltoall(float** out, int** parents, bool smooth)
{//propagates info about shortest paths not containing root to other roots, hopefully making the problem tractable
   int root, i, j, whichnode, whichneigh, numNeigh, remain, myparent, myparent2, myparent3, prevdots = 0, dots;
   int* neighbors;
   float tempf, tempf2;
   myheap active;
   for (i = 0; i < numNodes; ++i)
   {
      for (j = 0; j < numNodes; ++j)
      {
         out[i][j] = -1.0f;
      }
   }
   remain = numNodes;
   std::cout << "|0%      calculating geodesic distances      100%|" << std::endl;
   //            ..................................................
   for (root = 0; root < numNodes; ++root)
   {
      //std::cout << root << std::endl;
      dots = (50 * root) / numNodes;//simple progress indicator
      while (prevdots < dots)
      {
         std::cout << '.';
         std::cout.flush();
         ++prevdots;
      }
      if (root != 0)
      {
         remain = 0;
         for (i = 0; i < numNodes; ++i)//find known values
         {
            if (out[root][i] > 0.0f)
            {
               marked[i] = 2;//mark that we already have a value, skip calculation, but not yet added to active list
            } else {
               marked[i] = 0;
               ++remain;//count how many more we need to compute so we can stop early
            }
         }
         active.clear();
      }//marking done, dijkstra time
      out[root][root] = 0.0f;
      parents[root][root] = root;//idiom for end of path
      active.push(root, 0.0f);
      while (remain && !active.isEmpty())
      {
         whichnode = active.pop();
         if (!(marked[whichnode] & 1))
         {
            if (!(marked[whichnode] & 2)) --remain;
            marked[whichnode] |= 1;
            neighbors = nodeNeighbors[whichnode];
            numNeigh = numNeighbors[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
               whichneigh = neighbors[j];
               if (marked[whichneigh] & 2)
               {//already has a value and parent for this root
                  if (!(marked[whichneigh] & 8))
                  {//not yet in active list
                     active.push(whichneigh, out[root][whichneigh]);
                     marked[whichneigh] |= 8;
                  }
               } else {
                  if (!(marked[whichneigh] & 1))
                  {//skip floating point math if marked
                     tempf = out[root][whichnode] + distances[whichnode][j];
                     if (!(marked[whichneigh] & 4))
                     {
                        out[root][whichneigh] = tempf;
                        parents[root][whichneigh] = whichnode;
                        marked[whichneigh] |= 4;
                        active.push(whichneigh, tempf);
                     } else if (tempf < out[root][whichneigh]) {
                        out[root][whichneigh] = tempf;
                        parents[root][whichneigh] = whichnode;
                        active.push(whichneigh, tempf);
                     }
                  }
               }
            }
            if (smooth)
            {
               neighbors = nodeNeighbors2[whichnode];
               numNeigh = numNeighbors2[whichnode];
               for (j = 0; j < numNeigh; ++j)
               {
                  whichneigh = neighbors[j];
                  if (marked[whichneigh] & 2)
                  {//already has a value and parent for this root
                     if (!(marked[whichneigh] & 8))
                     {//not yet in active list
                        active.push(whichneigh, out[root][whichneigh]);
                        marked[whichneigh] |= 8;
                     }
                  } else {
                     if (!(marked[whichneigh] & 1))
                     {//skip floating point math if marked
                        tempf = out[root][whichnode] + distances2[whichnode][j];
                        if (!(marked[whichneigh] & 4))
                        {
                           out[root][whichneigh] = tempf;
                           parents[root][whichneigh] = whichnode;
                           marked[whichneigh] |= 4;
                           active.push(whichneigh, tempf);
                        } else if (tempf < out[root][whichneigh]) {
                           out[root][whichneigh] = tempf;
                           parents[root][whichneigh] = whichnode;
                           active.push(whichneigh, tempf);
                        }
                     }
                  }
               }
            }
         }
      }//dijkstra done...lotsa brackets...now to propagate the information gained to other roots
      for (i = root + 1; i < numNodes; ++i)
      {//any node smaller than root already has all distances calculated for entire surface
         if (!(marked[i] & 2))
         {//if endpoint already had distance to root precomputed, all available info from this node has been propagated previously
            myparent2 = i;
            myparent = parents[root][i];
            myparent3 = myparent;
            tempf = out[root][i];
            while (myparent != root)
            {
               tempf2 = tempf - out[root][myparent];
               if (myparent > root)
               {//try to be swap-friendly by not setting values in finished columns
                  out[myparent][i] = tempf2;//use midpoint as root, parent of endpoint is myparent3
                  parents[myparent][i] = myparent3;
               }
               out[i][myparent] = tempf2;//use endpoint as root, parent of midpoint is myparent2
               parents[i][myparent] = myparent2;
               myparent2 = myparent;//step along path
               myparent = parents[root][myparent];
            }
            out[i][root] = out[root][i];//finally, fill the transpose
            parents[i][root] = myparent2;
         }
      }//propagation of best paths to other roots complete, dijkstra again
   }
   while (prevdots < 50)
   {
      std::cout << '.';
      ++prevdots;
   }
   std::cout << std::endl;
   for (i = 0; i < numNodes; ++i)
   {
      marked[i] = 0;
   }
}

void GeodesicHelper::getGeoFromNode(const int node, float* valuesOut, const bool smoothflag)
{
   if (node < 0 || node >= numNodes || !valuesOut)
   {
      return;
   }
   CaretMutexLocker locked(&inUse);//don't screw with member variables while in use
   float* temp = output;//swap out the output pointer to avoid allocation
   output = valuesOut;
   dijkstra(node, smoothflag);
   output = temp;//restore the pointer to the original memory
}

void GeodesicHelper::getGeoFromNode(const int node, float* valuesOut, int* parentsOut, const bool smoothflag)
{
   if (node < 0 || node >= numNodes || !valuesOut || !parentsOut)
   {
      return;
   }
   CaretMutexLocker locked(&inUse);//don't screw with member variables while in use
   float* temp = output;//swap out the output pointer to avoid allocation
   int* tempi = parent;
   output = valuesOut;
   parent = parentsOut;
   dijkstra(node, smoothflag);
   output = temp;//restore the pointers to the original memory
   parent = tempi;
}

void GeodesicHelper::getGeoFromNode(const int node, std::vector<float>& valuesOut, const bool smoothflag)
{
   if (node < 0 || node >= numNodes)
   {
      return;
   }
   CaretMutexLocker locked(&inUse);
   dijkstra(node, smoothflag);
   valuesOut.resize(numNodes);
   for (int i = 0; i < numNodes; ++i)
   {
      valuesOut[i] = output[i];
   }
}

void GeodesicHelper::getGeoFromNode(const int node, std::vector<float>& valuesOut, std::vector<int>& parentsOut, const bool smoothflag)
{
   if (node < 0 || node >= numNodes)
   {
      return;
   }
   CaretMutexLocker locked(&inUse);
   dijkstra(node, smoothflag);
   valuesOut.resize(numNodes);
   parentsOut.resize(numNodes);
   for (int i = 0; i < numNodes; ++i)
   {
      valuesOut[i] = output[i];
      parentsOut[i] = parent[i];
   }
}

void GeodesicHelper::dijkstra(const int root, const std::vector<int>& interested, bool smooth)
{
   int i, j, whichnode, whichneigh, numNeigh, numChanged = 0, remain = 0;
   int* neighbors;
   float tempf;
   j = interested.size();
   for (i = 0; i < j; ++i)
   {
      whichnode = interested[i];
      if (!marked[whichnode])
      {
         ++remain;
         marked[whichnode] = 2;//interested, not expanded, no valid value
         changed[numChanged++] = whichnode;
      }
   }
   output[root] = 0.0f;
   if (!marked[root])
   {
      changed[numChanged++] = root;
   }
   marked[root] |= 4;
   parent[root] = root;//idiom for end of path
   myheap active;
   active.push(root, 0.0f);
   while (remain && !active.isEmpty())
   {
      whichnode = active.pop();
      if (!(marked[whichnode] & 1))
      {
         if (marked[whichnode] & 2)
         {
            --remain;
         }
         marked[whichnode] |= 1;//anything pulled from stack will already be marked as having a valid value (flag 4), so already in changed list
         neighbors = nodeNeighbors[whichnode];
         numNeigh = numNeighbors[whichnode];
         for (j = 0; j < numNeigh; ++j)
         {
            whichneigh = neighbors[j];
            if (!(marked[whichneigh] & 1))
            {//skip floating point math if marked
               tempf = output[whichnode] + distances[whichnode][j];//isn't precomputation wonderful
               if (!(marked[whichneigh] & 4))
               {
                  parent[whichneigh] = whichnode;
                  if (!marked[whichneigh])
                  {
                     changed[numChanged++] = whichneigh;
                  }
                  marked[whichneigh] |= 4;
                  output[whichneigh] = tempf;
                  active.push(whichneigh, tempf);
               } else if (tempf < output[whichneigh]) {
                  output[whichneigh] = tempf;
                  active.push(whichneigh, tempf);
               }
            }
         }
         if (smooth)//repeat with numNeighbors2, nodeNeighbors2, distance2
         {
            neighbors = nodeNeighbors2[whichnode];
            numNeigh = numNeighbors2[whichnode];
            for (j = 0; j < numNeigh; ++j)
            {
               whichneigh = neighbors[j];
               if (!(marked[whichneigh] & 1))
               {//skip floating point math if marked
                  tempf = output[whichnode] + distances2[whichnode][j];//isn't precomputation wonderful
                  if (!(marked[whichneigh] & 4))
                  {
                     parent[whichneigh] = whichnode;
                     if (!marked[whichneigh])
                     {
                        changed[numChanged++] = whichneigh;
                     }
                     marked[whichneigh] |= 4;
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  } else if (tempf < output[whichneigh]) {
                     output[whichneigh] = tempf;
                     active.push(whichneigh, tempf);
                  }
               }
            }
         }
      }
   }
   for (i = 0; i < numChanged; ++i)
   {
      marked[changed[i]] = 0;//minimize reinitialization of arrays
   }
}

void GeodesicHelper::getGeoToTheseNodes(const int root, const std::vector<int>& ofInterest, std::vector<float>& distsOut, bool smoothflag)
{
   if (root < 0 || root >= numNodes)
   {
      distsOut.clear();//empty array is error condition
      return;
   }
   int i, mysize = ofInterest.size(), node;
   for (i = 0; i < mysize; ++i)
   {//needs to do a linear scan of this array later anyway, so lets sanity check it
      node = ofInterest[i];
      if (node < 0 || node >= numNodes)
      {
         distsOut.clear();//empty array is error condition
         return;
      }
   }
   CaretMutexLocker locked(&inUse);//let sanity checks fail without locking
   dijkstra(root, ofInterest, smoothflag);
   distsOut.resize(mysize);
   for (i = 0; i < mysize; ++i)
   {
      distsOut[i] = output[ofInterest[i]];
   }
}
