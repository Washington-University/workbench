/*LICENSE_START*/
/*
 *  Copyright (C) 2026  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#include "AlgorithmSurfacePairsInteriorDistance.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiCreateDenseTimeseries.h"
#include "CaretHeap.h"
#include "CaretLogger.h"
#include "CaretMutex.h"
#include "CaretOMP.h"
//#include "CaretSparseFile.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "NiftiIO.h"
#include "SignedDistanceHelper.h"
#include "SurfaceFile.h"
#include "Vector3D.h"
#include "VolumeFile.h"

#include <fstream>
#include <sstream>

#include <array>
#include <map>

using namespace caret;
using namespace std;

AString AlgorithmSurfacePairsInteriorDistance::getCommandSwitch()
{
    return "-surface-pairs-interior-distance";
}

AString AlgorithmSurfacePairsInteriorDistance::getShortDescription()
{
    return "MEASURE PATHS INSIDE TISSUE";
}

OperationParameters* AlgorithmSurfacePairsInteriorDistance::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface that constrains the paths");

    ret->addStringParameter(2, "pairs-file", "a text file of vertex pairs to connect - zero-indexed, one pair per line, only whitespace between vertex indices");

    ret->addStringParameter(3, "reference-volume", "a volume file enclosing the neccesary tissue extent, smaller voxels allow more accurate paths");
    
    ret->addStringParameter(4, "distances-out-text", "output - a text file with one distance per line"); //HACK: fake the output formatting
    
    OptionalParameter* metricOutOpt = ret->createOptionalParameter(5, "-distance-metric-out", "output a metric file containing each pair as a separate map");
    metricOutOpt->addMetricOutputParameter(1, "distance-pairs-out", "the distances of the connections, one map per pair");
    
    OptionalParameter* failvalOpt = ret->createOptionalParameter(6, "-fail-value", "what value to use in <distances-out> when a path can't be found");
    failvalOpt->addDoubleParameter(1, "value", "value to use, default +infinity");

    OptionalParameter* neighOpt = ret->createOptionalParameter(7, "-neighborhood", "voxel neighborhood for candidate steps");
    neighOpt->addIntegerParameter(1, "num", "size of neighborhood cube measured from center to face, in voxels (default 2 = 5x5x5)");

    OptionalParameter* offsetOpt = ret->createOptionalParameter(8, "-offset", "set inwards step before path");
    offsetOpt->addDoubleParameter(1, "dist", "to avoid rejecting the first and last step as intersecting the surface, move the start and end points this distance along the negative normal of the vertices, in mm (default 0.001)");
    
    OptionalParameter* pathsOutOpt = ret->createOptionalParameter(9, "-paths-out", "output a 4D volume file tracing the optimal paths");
    pathsOutOpt->addStringParameter(1, "counting-space", "volume file with the desired voxel grid for counting");
    pathsOutOpt->addVolumeOutputParameter(2, "volume-out", "the path output file");
    
    ret->setHelpText(
        AString("Find the shortest path through the interior of the surface between each specified pair of vertices.  ")
    );
    return ret;
}

void AlgorithmSurfacePairsInteriorDistance::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    AString pairfileName = myParams->getString(2);
    ifstream pairfile(pairfileName.toStdString());
    if (!pairfile.good()) throw AlgorithmException("error opening file '" + pairfileName + "' for reading");
    string pairline;
    bool emptyline = false;
    int32_t numNodes = mySurf->getNumberOfNodes();
    vector<pair<int32_t, int32_t> > pairs;
    while (getline(pairfile, pairline))
    {
        istringstream linestream(pairline);
        int32_t pair1 = -1, pair2 = -1;
        if (linestream >> pair1 >> pair2)
        {
            if (emptyline) throw AlgorithmException("found empty or misformatted line in pairs text file '" + pairfileName + "'");
            if (pair1 < 0 || pair2 < 0 || pair1 >= numNodes || pair2 >= numNodes) throw AlgorithmException("found impossible vertex index in pair file: " + AString::number(pair1) + ", " + AString::number(pair2));
            pairs.push_back(make_pair(pair1, pair2));
        } else {
            emptyline = true; //allow empty lines at end of file
        }
    }
    NiftiIO myIO;
    myIO.openRead(myParams->getString(3));
    AString outTextName = myParams->getString(4);
    ofstream outText(outTextName.toStdString());
    if (!outText.good()) throw AlgorithmException("error opening output text file '" + outTextName + "' for writing");
    MetricFile* myMetricOut = NULL;
    OptionalParameter* metricOutOpt = myParams->getOptionalParameter(5);
    if (metricOutOpt->m_present)
    {
        myMetricOut = metricOutOpt->getOutputMetric(1);
    }
    OptionalParameter* failvalOpt = myParams->getOptionalParameter(6);
    float failVal = INFINITY;
    if (failvalOpt->m_present)
    {
        failVal = float(failvalOpt->getDouble(1));
    }
    OptionalParameter* neighOpt = myParams->getOptionalParameter(7);
    int numNeigh = 2;
    if (neighOpt->m_present)
    {
        numNeigh = neighOpt->getInteger(1);
    }
    OptionalParameter* offsetOpt = myParams->getOptionalParameter(8);
    float offset = 0.001f;
    if (offsetOpt->m_present)
    {
        offset = float(offsetOpt->getDouble(1));
    }
    OptionalParameter* pathsOutOpt = myParams->getOptionalParameter(9);
    //CaretPointer<CaretSparseFileWriter> sparseWriter;
    VolumeSpace countingSpace;
    VolumeFile* pathsOut = NULL;
    if (pathsOutOpt->m_present)
    {
        NiftiIO countingIO;
        countingIO.openRead(pathsOutOpt->getString(1));
        countingSpace = countingIO.getHeader().getVolumeSpace();
        pathsOut = pathsOutOpt->getOutputVolume(2);
        /*CiftiXML outXML;
        outXML.setNumberOfDimensions(2);
        VolumeFile* structLabelVol = pathsOutOpt->getVolume(1);
        CiftiBrainModelsMap volMap = AlgorithmCiftiCreateDenseTimeseries::makeDenseMapping(structLabelVol, structLabelVol);
//         MetricFile pairStarts;
//         pairStarts.setNumberOfNodesAndColumns(mySurf->getNumberOfNodes(), 1);
//         vector<float> scratchCol(mySurf->getNumberOfNodes(), 0.0f);
//         for (auto iter : pairs)
//         {
//             scratchCol[iter.first] = 1.0f;
//         }
//         pairStarts.setValuesForColumn(0, scratchCol.data());
//         map<StructureEnum::Enum, AlgorithmCiftiCreateDenseTimeseries::SurfParam> surfParams;
//         surfParams[mySurf->getStructure()] = AlgorithmCiftiCreateDenseTimeseries::SurfParam(&pairStarts, &pairStarts);
//         CiftiBrainModelsMap surfMap = AlgorithmCiftiCreateDenseTimeseries::makeDenseMapping(NULL, NULL, surfParams);
        CiftiScalarsMap colMap(pairs.size());
        for (int64_t i = 0; i < int64_t(pairs.size()); ++i)
        {
            colMap.setMapName(i, AString::number(pairs[i].first) + ", " + AString::number(pairs[i].second));
        }
        //outXML.setMap(CiftiXML::ALONG_COLUMN, surfMap);
        outXML.setMap(CiftiXML::ALONG_COLUMN, colMap);
        outXML.setMap(CiftiXML::ALONG_ROW, volMap);
        sparseWriter.grabNew(new CaretSparseFileWriter(pathsOutOpt->getString(2), outXML, CaretSparseFile::Int32));//*/
    }
    vector<float> distsOut;
    AlgorithmSurfacePairsInteriorDistance(myProgObj, mySurf, pairs, myIO.getHeader().getVolumeSpace(), distsOut, myMetricOut, failVal, numNeigh, offset, countingSpace, pathsOut);//executes the algorithm
    for (auto dist : distsOut)
    {
        outText << dist << endl;
    }
}

namespace
{
    //NOTE: end convention is "inclusive"
    void memoizeRecurse(const vector<Vector3D>& path, const int start, const int end, SignedDistanceHelper* myHelp,
                        vector<vector<float> >& bestLength, vector<vector<bool> >& complete, vector<vector<int> >& nextPoint)
    {
        if (end - 1 <= start) return; //skip 1-step bits in addition to nonsense calls, they were precomputed (next conditional would catch them too, but why wait)
        if (complete[end][start]) return; //instead of conditionals before recursion, because lazy
        if (!myHelp->lineSegmentIntersectsSurface(path[start], path[end])) //can't beat a straight line
        {
            bestLength[end][start] = (path[end] - path[start]).length();
            complete[end][start] = true;
            nextPoint[end][start] = end;
            return;
        }
        //midpoint is required, ensure all possible midpoint choices are computed and choose the best
        float bestTotal = INFINITY;
        int bestMidpoint = -1;
        for (int i = start + 1; i < end; ++i)
        {
            //implement upper limit pruning?  seems fast enough as-is
            memoizeRecurse(path, start, i, myHelp, bestLength, complete, nextPoint);
            memoizeRecurse(path, i, end, myHelp, bestLength, complete, nextPoint);
            float thisLength = bestLength[i][start] + bestLength[end][i];
            if (thisLength < bestTotal)
            {
                bestTotal = thisLength;
                bestMidpoint = i;
            }
        }
        if (bestMidpoint != -1)
        {
            bestLength[end][start] = bestTotal;
            complete[end][start] = true;
            nextPoint[end][start] = nextPoint[bestMidpoint][start]; //our second point is the second point of the first segment (because we ruled out straight line, and avoided "point to itself" infinite loop nonsense)
        }
    }
    
    //set up like dynamic, then recurse top-down so we can skip solving subpaths we don't actually need
    float optimizePathMemoize(const vector<Vector3D>& path, SignedDistanceHelper* myHelp, vector<Vector3D>* pathOut = NULL)
    {
        int numPoints = int(path.size());
        vector<vector<float> > bestLength(numPoints);
        vector<vector<bool> > complete(numPoints);
        vector<vector<int> > nextPoint(numPoints); //second point in the optimal subchain, to reconstruct the full path
        for (int i = 0; i < numPoints; ++i)
        {
            bestLength[i].resize(i + 1);
            complete[i].resize(i + 1);
            nextPoint[i].resize(i + 1);

            bestLength[i][i] = 0.0f; //honestly, these shouldn't get used unless path.size() = 1
            complete[i][i] = true;
            nextPoint[i][i] = -1;
            if (i > 0)
            {
                bestLength[i][i - 1] = (path[i - 1] - path[i]).length(); //assume consecutive points don't intersect the surface
                complete[i][i - 1] = true;
                nextPoint[i][i - 1] = i;
            }
        }
        memoizeRecurse(path, 0, numPoints - 1, myHelp, bestLength, complete, nextPoint);
        if (pathOut != NULL)
        {
            vector<Vector3D>& retPath = *pathOut;
            int curPoint = 0;
            while (curPoint != numPoints - 1)
            {
                retPath.push_back(path[curPoint]);
                curPoint = nextPoint[numPoints - 1][curPoint];
            }
            retPath.push_back(path[curPoint]);
        }
        return bestLength[numPoints - 1][0];
    }
}

AlgorithmSurfacePairsInteriorDistance::AlgorithmSurfacePairsInteriorDistance(ProgressObject* myProgObj, const SurfaceFile* mySurf,
                                                                             const vector<pair<int32_t, int32_t> > pairs,
                                                                             const VolumeSpace refSpace,
                                                                             vector<float>& distsOut, MetricFile* myMetricOut,
                                                                             const float failVal, const int32_t numNeigh, const float offset,
                                                                             VolumeSpace countingSpace, VolumeFile* pathsOut) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (numNeigh < 1) throw AlgorithmException("-neighborhood value must be positive");
    if (!(offset > 0.0f)) throw AlgorithmException("-offset value must be positive");
    /*if (pathsOut != NULL)
    {
        const CiftiXML& outXML = pathsOut->getCiftiXML();
        if (outXML.getNumberOfDimensions() != 2) throw AlgorithmException("sparse path output writer must be initialized with 2 dimensions with appropriate BRAIN_MODELS mappings");
        if (outXML.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::BRAIN_MODELS ||
            outXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::SCALARS)
            throw AlgorithmException("sparse path output writer must be initialized with dscalar-like mappings");
        const CiftiBrainModelsMap& rowMapping = outXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
        if (size_t(outXML.getDimensionLength(CiftiXML::ALONG_COLUMN)) != pairs.size())
            throw AlgorithmException("sparse path output writer needs a matching surface mapping along columns");
        if (!rowMapping.hasVolumeData())
            throw AlgorithmException("sparse path output writer needs a mapping along rows that contains voxels");
    }//*/
    int32_t numNodes = mySurf->getNumberOfNodes();
    const int64_t numPairs = int64_t(pairs.size());
    distsOut.resize(numPairs);
    if (myMetricOut != NULL)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numPairs);
        myMetricOut->setStructure(mySurf->getStructure());
    }
    vector<int32_t> neighmoves;
    vector<float> neighdists;
    Vector3D ivec, jvec, kvec, origin;
    refSpace.getSpacingVectors(ivec, jvec, kvec, origin);
    for (int k = -numNeigh; k <= numNeigh; ++k) //TODO: another helper for this stuff?
    {
        for (int j = -numNeigh; j <= numNeigh; ++j)
        {
            for (int i = -numNeigh; i <= numNeigh; ++i)
            {//NOTE: uses gcd(0, x) = x, including gcd(0, 0) = 0
                if (MathFunctions::gcd(abs(i), MathFunctions::gcd(abs(j), abs(k))) == 1)
                {
                    neighmoves.push_back(i);
                    neighmoves.push_back(j);
                    neighmoves.push_back(k);
                    neighdists.push_back((i * ivec + j * jvec + k * kvec).length());
                }
            }
        }
    }
    const float* normals = mySurf->getNormalData();
    const float* coords = mySurf->getCoordinateData();
    const int64_t* volDims = refSpace.getDims();
    const int64_t framesize = volDims[0] * volDims[1] * volDims[2];
    vector<vector<Vector3D> > allPaths(numPairs);
    CaretMutex outputLock;
    bool fail = false;
    exception_ptr exPtr;
#pragma omp CARET_PAR
    {
        CaretPointer<SignedDistanceHelper> myHelp = mySurf->getSignedDistanceHelper();
        vector<int64_t> heapIDs(framesize);
        vector<float> dists(framesize);
        vector<array<int64_t, 3> > parents(framesize);
        vector<char> marked(framesize, 0);
        vector<int64_t> resetList;
        vector<float> outDistsScratch(mySurf->getNumberOfNodes(), 0.0f);
#pragma omp CARET_FOR schedule(dynamic)
        for (int whichPair = 0; whichPair < numPairs; ++whichPair)
        {
            if (fail) continue; //break isn't allowed
            try
            {
                int32_t startNode = pairs[whichPair].first, endNode = pairs[whichPair].second;
                if (startNode < 0 || endNode < 0 || startNode >= numNodes || endNode >= numNodes) throw AlgorithmException("invalid node pair: " + AString::number(startNode) + ", " + AString::number(endNode));
                Vector3D startCoord(&(coords[startNode * 3])), endCoord(&(coords[endNode * 3]));
                int64_t startVox[3], endVox[3];
                refSpace.enclosingVoxel(startCoord, startVox);
                refSpace.enclosingVoxel(endCoord, endVox);
                if (!refSpace.indexValid(startVox) || !refSpace.indexValid(endVox))
                {
                    CaretMutexLocker locked(&outputLock);
                    CaretLogWarning("node pair exceeds the bounds of the reference space: " + AString::number(startNode) + ", " + AString::number(endNode));
                }
                startCoord -= offset * Vector3D(&(normals[startNode * 3])).normal(); //double check unit length
                endCoord -= offset * Vector3D(&(normals[endNode * 3])).normal();
                float bestTotal = -1.0f;
                vector<Vector3D> path(1, startCoord);
                if (myHelp->lineSegmentIntersectsSurface(startCoord, endCoord)) //if there is a straight path, use it
                {
                    array<int64_t, 3> bestVox = {-1, -1, -1};
                    CaretMinHeap<array<int64_t, 3>, float> myHeap;
                    //these diagonals aren't exactly overlapping, so don't reduce them to using neighmoves
                    for (int64_t k = startVox[2] - numNeigh; k <= startVox[2] + numNeigh; ++k) //reuse the same neighborhood
                    {
                        for (int64_t j = startVox[1] - numNeigh; j <= startVox[1] + numNeigh; ++j)
                        {
                            for (int64_t i = startVox[0] - numNeigh; i <= startVox[0] + numNeigh; ++i)
                            {
                                if (refSpace.indexValid(i, j, k))
                                {
                                    Vector3D thisCoord = refSpace.indexToSpace(i, j, k);
                                    if (myHelp->pointInsideSurface(thisCoord) && !myHelp->lineSegmentIntersectsSurface(startCoord, thisCoord))
                                    {
                                        int64_t voxFlat = refSpace.getIndex(i, j, k);
                                        float thisDist = (thisCoord - startCoord).length(), remaining = (thisCoord - endCoord).length();
                                        //we are starting fresh, no need to check for prior answer
                                        dists[voxFlat] = thisDist;
                                        parents[voxFlat] = {-1, -1, -1}; //sentinel for starting point
                                        resetList.push_back(voxFlat); //remember it for zeroing later
                                        marked[voxFlat] = 1; //has value
                                        heapIDs[voxFlat] = myHeap.push({i, j, k}, thisDist + remaining); //A*, since we have a target
                                    }
                                }
                            }
                        }
                    }
                    while (!myHeap.isEmpty())
                    {
                        auto thisVox = myHeap.pop();
                        Vector3D thisCoord = refSpace.indexToSpace(thisVox.data());
                        int64_t thisFlat = refSpace.getIndex(thisVox.data());
                        CaretAssert((marked[thisFlat] & 2) == 0); //since our heap is fancy, we can run without duplicates, so this would indicate a bug
                        marked[thisFlat] = 3; //has value and expanded (complete)
                        //using a "remaining squared distance" threshold is actually slightly slower than this intersection test (presumably because it evaluates more total voxels due to a slightly worse path)
                        if (myHelp->lineSegmentIntersectsSurface(thisCoord, endCoord))
                        {
                            for (size_t i = 0; i < neighmoves.size(); i += 3)
                            {
                                array<int64_t, 3> nextVox = {thisVox[0] + neighmoves[i],
                                                            thisVox[1] + neighmoves[i + 1],
                                                            thisVox[2] + neighmoves[i + 2]};
                                if (refSpace.indexValid(nextVox.data()))
                                {
                                    Vector3D nextCoord = refSpace.indexToSpace(nextVox.data());
                                    int64_t nextFlat = refSpace.getIndex(nextVox.data());
                                    //if we already expanded the neighbor, skip it
                                    //if already marked, then it was at least inside the surface, so we can skip that test
                                    if ((marked[nextFlat] & 2) == 0 &&
                                        (marked[nextFlat] != 0 || myHelp->pointInsideSurface(nextCoord)) &&
                                        !myHelp->lineSegmentIntersectsSurface(thisCoord, nextCoord))
                                    {
                                        float nextDist = (nextCoord - thisCoord).length() + dists[thisFlat], remaining = (nextCoord - endCoord).length();
                                        //walk back through parents and test for surface intersection?  expensive for small gains
                                        //even a single parent test takes twice as long
                                        //makes more sense to get a path fast, then optimize it afterward
                                        //compare to best known path to set the end of the search
                                        if (bestTotal < 0.0f || nextDist + remaining < bestTotal)
                                        {
                                            if (marked[nextFlat] & 1)
                                            {
                                                if (nextDist < dists[nextFlat])
                                                {
                                                    dists[nextFlat] = nextDist;
                                                    parents[nextFlat] = thisVox;
                                                    myHeap.changekey(heapIDs[nextFlat], nextDist + remaining);
                                                }
                                            } else {
                                                dists[nextFlat] = nextDist;
                                                parents[nextFlat] = thisVox;
                                                resetList.push_back(nextFlat);
                                                marked[nextFlat] = 1;
                                                heapIDs[nextFlat] = myHeap.push(nextVox, nextDist + remaining);
                                            }
                                        }
                                    }
                                }
                            }
                        } else {
                            float totalDist = dists[thisFlat] + (endCoord - thisCoord).length();
                            if (bestTotal < 0.0f || totalDist < bestTotal)
                            {
                                bestTotal = totalDist;
                                bestVox = thisVox;
                            }
                        }
                    }
                    if (bestTotal > 0.0f)
                    {
                        vector<Vector3D> traceback;
                        array<int64_t, 3> iterVox = bestVox;
                        while (iterVox != array<int64_t, 3>({-1, -1, -1}))
                        {
                            traceback.push_back(refSpace.indexToSpace(iterVox.data()));
                            iterVox = parents[refSpace.getIndex(iterVox.data())];
                        }
                        for (auto iter = traceback.rbegin(); iter != traceback.rend(); ++iter)
                        {
                            path.push_back(*iter);
                        }
                        path.push_back(endCoord);
                    } else {
                        path.clear();
                    }
                    for (auto iter = resetList.begin(); iter != resetList.end(); ++iter)
                    {
                        marked[*iter] = 0;
                    }
                } else {
                    path.push_back(endCoord);
                    bestTotal = (endCoord - startCoord).length();
                }
                if (path.empty())
                {
                    CaretMutexLocker locked(&outputLock);
                    CaretLogWarning("failed to find path for pair: " + AString::number(startNode) + ", " + AString::number(endNode));
                    bestTotal = failVal; //NOTE: don't trust bestTotal to decide if the pathing failed
                } else {
                    //optimize for subpaths that can be replaced with straight lines
                    vector<Vector3D> newPath;
                    bestTotal = optimizePathMemoize(path, myHelp, &newPath);
                    path = newPath;
                }
                distsOut[whichPair] = bestTotal;
                if (myMetricOut != NULL)
                {
                    CaretMutexLocker locked(&outputLock); //may be unneccesary, but for peace of mind
                    outDistsScratch[endNode] = outDistsScratch[startNode] = bestTotal;
                    myMetricOut->setValuesForColumn(whichPair, outDistsScratch.data());
                    myMetricOut->setMapName(whichPair, AString::number(startNode) + ", " + AString::number(endNode));
                    outDistsScratch[endNode] = outDistsScratch[startNode] = 0.0f;
                }
                if (pathsOut != NULL)
                {
                    allPaths[whichPair] = path;
                }
            } catch (...) {
#pragma omp critical
                {
                    exPtr = current_exception();
                    fail = true;
                }
            }
        }
    }
    if (fail) rethrow_exception(exPtr);
    //NOTE: actually supports different volume space for output than for A*
    //QByteArray apparently limits CiftiXML to about 1GB, so this is important
    if (pathsOut != NULL)
    {
        pathsOut->reinitialize(countingSpace, numPairs);
        Vector3D iout, jout, kout, originout;
        countingSpace.getSpacingVectors(iout, jout, kout, originout);
        float maxstep = min(min(iout.length(), jout.length()), kout.length()); //assume orthogonal, this is just for visual purposes
        auto countDims = countingSpace.getDims();
        const int64_t frameSize = countDims[0] * countDims[1] * countDims[2];
        for (int64_t i = 0; i < int64_t(allPaths.size()); ++i)
        {
            vector<float> scratchFrame(frameSize, 0.0f); //because lazy - they asked for a big file, so...
            const auto& thisPath = allPaths[i];
            if (thisPath.empty()) continue; //failed
            int64_t tempVox[3];
            for (size_t i = 0; i < thisPath.size() - 1; ++i)
            { //mark start and points along line, but not end (aka next start)
                const Vector3D& start = thisPath[i];
                const Vector3D& end = thisPath[i + 1];
                Vector3D segmentVec = end - start;
                int numSteps = ceil(segmentVec.length() / maxstep);
                for (int i = 0; i < numSteps; ++i)
                {
                    countingSpace.enclosingVoxel(start + segmentVec * (float(i) / numSteps), tempVox);
                    if (countingSpace.indexValid(tempVox))
                    {
                        int64_t tempIndex = countingSpace.getIndex(tempVox);
                        scratchFrame[tempIndex] = 1;
                    }
                }
            }
            countingSpace.enclosingVoxel(thisPath.back(), tempVox); //mark last end
            if (countingSpace.indexValid(tempVox))
            {
                int64_t tempIndex = countingSpace.getIndex(tempVox);
                scratchFrame[tempIndex] = 1;
            }
            pathsOut->setFrame(scratchFrame.data(), i);
        }
        /*const CiftiXML& outXML = pathsOut->getCiftiXML();
        const CiftiBrainModelsMap& volMap = outXML.getBrainModelsMap(CiftiXML::ALONG_ROW);
        VolumeSpace outSpace = volMap.getVolumeSpace();
        Vector3D iout, jout, kout, originout;
        outSpace.getSpacingVectors(iout, jout, kout, originout);
        float maxstep = min(min(iout.length(), jout.length()), kout.length()); //assume orthogonal, this is just for visual purposes
        for (int64_t i = 0; i < int64_t(allPaths.size()); ++i)
        {
            const auto& thisPath = allPaths[i];
            if (thisPath.empty()) continue; //failed
            map<int64_t, int32_t> sparseValues;
            int64_t tempVox[3];
            for (size_t i = 0; i < thisPath.size() - 1; ++i)
            { //mark start and points along line, but not end (aka next start)
                const Vector3D& start = thisPath[i];
                const Vector3D& end = thisPath[i + 1];
                Vector3D segmentVec = end - start;
                int numSteps = ceil(segmentVec.length() / maxstep);
                for (int i = 0; i < numSteps; ++i)
                {
                    outSpace.enclosingVoxel(start + segmentVec * (float(i) / numSteps), tempVox);
                    int64_t tempIndex = volMap.getIndexForVoxel(tempVox);
                    if (tempIndex >= 0)
                    {
                        sparseValues[tempIndex] = 1;
                    }
                }
            }
            outSpace.enclosingVoxel(thisPath.back(), tempVox); //mark last end
            int64_t tempIndex = volMap.getIndexForVoxel(tempVox);
            if (tempIndex >= 0)
            {
                sparseValues[tempIndex] = 1;
            }
            if (!sparseValues.empty())
            {
                pathsOut->writeRowSparse(i, sparseValues);
            }
        }//*/
    }
}

float AlgorithmSurfacePairsInteriorDistance::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmSurfacePairsInteriorDistance::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}
