/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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
#include "CiftiXMLElements.h"
#include "CiftiFileException.h"

using namespace caret;

void CiftiBrainModelElement::setupLookup()
{
    if (m_modelType == CIFTI_MODEL_TYPE_SURFACE)
    {
        if (m_nodeIndices.size() == 0)
        {
            if (m_indexCount != m_surfaceNumberOfNodes)
            {
                throw CiftiFileException("empty index list found, but indexCount and surfaceNumberOfNodes don't match");
            }
            m_nodeToIndexLookup.resize(m_surfaceNumberOfNodes);
            for (int i = 0; i < m_surfaceNumberOfNodes; ++i)
            {
                m_nodeToIndexLookup[i] = i + m_indexOffset;
            }
        } else {
            if (m_indexCount != m_nodeIndices.size())
            {
                throw CiftiFileException("indexCount and size of nodeIndices don't match");
            }
            m_nodeToIndexLookup.resize(m_surfaceNumberOfNodes);
            for (int i = 0; i < m_surfaceNumberOfNodes; ++i)
            {
                m_nodeToIndexLookup[i] = -1;
            }
            for (int i = 0; i < m_indexCount; ++i)
            {
                m_nodeToIndexLookup[m_nodeIndices[i]] = i + m_indexOffset;
            }
        }
    }
}
