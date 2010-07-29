/*
 * opencog/server/DimensionalEmbedding.h
 *
 * Copyright (C) 2010 by Singularity Institute for Artificial Intelligence
 * All Rights Reserved
 *
 * Written by David Crane <dncrane@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef _OPENCOG_DIMENSIONAL_EMBEDDING_H
#define _OPENCOG_DIMENSIONAL_EMBEDDING_H

#include <vector>
#include <map>
#include <string>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/server/Agent.h>
#include <opencog/server/CogServer.h>

namespace opencog
{
    /**
     * The DimensionalEmbedding class  implements the dimensional embedding
     * technique as described on
     * http://www.opencog.org/wiki/OpenCogPrime:DimensionalEmbedding
     *
     * @todo Add support for non-symmetric links
     */
    class DimensionalEmbedding : public opencog::Agent {
    private:
        typedef std::map<Handle, std::vector<double> > AtomEmbedding;
        typedef std::vector<Handle> PivotSeq;
        typedef std::map<Type, PivotSeq> PivotMap;
        typedef std::map<Type, AtomEmbedding> AtomEmbedMap;

        AtomSpace* as;
        AtomEmbedMap atomMaps;
        PivotMap pivotsMap;//Pivot atoms which act as the basis
        unsigned int numDimensions;//Number of pivot atoms
        
        /**
         * Adds h as a pivot and adds the distances from each node to
         * the pivot to the appropriate atomEmbedding.
         *
         * @param h Handle to be added as a pivot
         * @param linkType Type of link for which h should be added as a pivot
         */
        void addPivot(const Handle& h, const Type& linkType);

        /**
         * Clears the AtomEmbedMap and PivotMap for linkType
         *
         * @param linkType Type of link for which the embedding should be
         * cleared.
         */
        void clearEmbedding(const Type& linkType);

        /**
         * Returns the highest weight path between the handles following
         * links of type linkType, where path weight is the product of
         * (tv.strength*tv.confidence) of the links in the path. ie if
         * (l1, l2,...ln) are the links in the path, then the path weight is
         * defined as
         * (l1.tv.strength*l1.tv.confidence)*(l2.tv.strength*l2.tv.confidence)*
         * ...(ln.tv.strength*ln.tv.confidence).
         *
         * Path weight will always be between 0 and 1. Returns 0 if no path
         * exists and 1 if startHandle == targetHandle.
         *
         * Uses a modified version of Dijkstra's algorithm.
         *
         * @param startHandle The starting handle for pathfinding
         * @param targetHandle The target handle for pathfinding
         * @param linkType The type of link to follow in pathfinding
         *
         * @return The highest weight path from startHandle to targetHandle
         * following only links of type linkType, where path weight is defined
         * above.
         */
        double findHighestWeightPath(const Handle& startHandle,
                                     const Handle& targetHandle,
                                     const Type& linkType);

        /**
         * Returns a vector of doubles corresponding to the handle h's
         * embedding of link type l
         *
         * @param h The handle whose embedding vector is returned
         * @param l The link type for which h's embedding vector is wanted
         *
         * @return A vector of doubles corresponding to handle h's distance
         * from each of the pivots.
         */
        std::vector<double> getEmbedVector(const Handle& h, const Type& l);

        /**
         * Returns the distance between handles h1 and h2 for the embedding of
         * link type l
         */
        double euclidDist(const Handle& h1, const Handle& h2, const Type& l);
    public:
        virtual const ClassInfo& classinfo() const { return info(); }
        static const ClassInfo& info() {
            static const ClassInfo _ci("DimensionalEmbedding");
            return _ci;
        }

        void run (CogServer* server);

        DimensionalEmbedding();
        ~DimensionalEmbedding() { };

        /**
         * Embeds and prints similarity links.
         * Just used for scheme for testing
         */
        void embedSimLinks();
        void logSimEmbedding();
        //double euclidDistSim(const Handle& h1, const Handle& h2);
        
        /**
         * Creates an AtomEmbedding of the atomspace using linkType
         * and registers it with the AtomEmbedMap. If an AtomEmbedding
         * already exists for the supplied link type it will replace
         * it.
         *
         * @param linkType The type of link for which a dimensional embedding
         * is wanted.
         */
        void embedAtomSpace(const Type& linkType);
        
        /**
         * Logs a string representation of of the (Handle,vector<Double>)
         * pairs for linkType. This will have as many entries as there are nodes
         * in the atomspace. Just used for testing/debugging.
         */
        void logAtomEmbedding(const Type& linkType);

        /**
         * Adds node to the appropriate AtomEmbedding in the AtomEmbedMap.
         *
         * @param h Handle of node to be embedded.
         * @param linkType Type of link to use to embed h
         */
        void addNode(const Handle& h, const Type& linkType);

        /** updates the given atom (recalculates its distance from pivots) */
        //void updateAtom(const Handle& h, const Type& linkType);
    };
}
#endif // _OPENCOG_DIMENSIONAL_EMBEDDING_H
