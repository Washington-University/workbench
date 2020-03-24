/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "OperationParameters.h"

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "ProgramParametersException.h"

#include "AnnotationFile.h"
#include "BorderFile.h"
#include "CiftiFile.h"
#include "FociFile.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "VolumeFile.h"

using namespace std;
using namespace caret;

ParameterComponent::ParameterComponent()
{
}

ParameterComponent::~ParameterComponent()
{
    for (size_t i = 0; i < m_paramList.size(); ++i)
    {
        delete m_paramList[i];
    }
    for (size_t i = 0; i < m_outputList.size(); ++i)
    {
        delete m_outputList[i];
    }
    for (size_t i = 0; i < m_optionList.size(); ++i)
    {
        delete m_optionList[i];
    }
    for (size_t i = 0; i < m_repeatableOptions.size(); ++i)
    {
        delete m_repeatableOptions[i];
    }
}

RepeatableOption::~RepeatableOption()
{
    for (size_t i = 0; i < m_instances.size(); ++i)
    {
        delete m_instances[i];
    }
}

OperationParameters::OperationParameters()
{
}

ParameterComponent::ParameterComponent(const ParameterComponent& rhs)
{
    m_paramList.resize(rhs.m_paramList.size());
    for (size_t i = 0; i < m_paramList.size(); ++i)
    {
        m_paramList[i] = rhs.m_paramList[i]->cloneAbstractParameter();
    }
    m_outputList.resize(rhs.m_outputList.size());
    for (size_t i = 0; i < m_outputList.size(); ++i)
    {
        m_outputList[i] = rhs.m_outputList[i]->cloneAbstractParameter();
    }
    m_optionList.resize(rhs.m_optionList.size());
    for (size_t i = 0; i < m_optionList.size(); ++i)
    {
        m_optionList[i] = new OptionalParameter(*(rhs.m_optionList[i]));
    }
    m_repeatableOptions.resize(rhs.m_repeatableOptions.size());
    for (size_t i = 0; i < m_repeatableOptions.size(); ++i)
    {
        m_repeatableOptions[i] = new RepeatableOption(*(rhs.m_repeatableOptions[i]));
    }
}

OptionalParameter* ParameterComponent::createOptionalParameter(const int32_t key, const AString& optionSwitch, const AString& description)
{
    if (!checkUniqueOption(key))
    {
        CaretAssert(false);
        throw ProgramParametersException("optional parameter created with previously used key");
    }
    if (optionSwitch.isEmpty() || optionSwitch[0] != '-') CaretLogWarning("developer warning: option '" + optionSwitch + "' created, but does not start with dash");
    OptionalParameter* ret = new OptionalParameter(key, optionSwitch, description);
    m_optionList.push_back(ret);
    return ret;
}

ParameterComponent* ParameterComponent::createRepeatableParameter(const int32_t key, const AString& optionSwitch, const AString& description)
{
    if (!checkUniqueRepeatable(key))
    {
        CaretAssert(false);
        throw ProgramParametersException("repeatable parameter created with previously used key");
    }
    if (optionSwitch.isEmpty() || optionSwitch[0] != '-') CaretLogWarning("developer warning: repeatable option '" + optionSwitch + "' created, but does not start with dash");
    RepeatableOption* newOpt = new RepeatableOption(key, optionSwitch, description);
    m_repeatableOptions.push_back(newOpt);
    return &(newOpt->m_template);
}

bool ParameterComponent::checkUniqueInput(const int32_t& key, const OperationParametersEnum::Enum& type)
{
    for (size_t i = 0; i < m_paramList.size(); ++i)
    {
        if (m_paramList[i]->m_key == key && type == m_paramList[i]->getType())
        {
            return false;
        }
    }
    return true;
}

bool ParameterComponent::checkUniqueOption(const int32_t& key)
{
    for (size_t i = 0; i < m_optionList.size(); ++i)
    {
        if (m_optionList[i]->m_key == key)
        {
            return false;
        }
    }
    return true;
}

bool ParameterComponent::checkUniqueRepeatable(const int32_t& key)
{
    for (size_t i = 0; i < m_repeatableOptions.size(); ++i)
    {
        if (m_repeatableOptions[i]->m_key == key)
        {
            return false;
        }
    }
    return true;
}

bool ParameterComponent::checkUniqueOutput(const int32_t& key, const OperationParametersEnum::Enum& type)
{
    for (size_t i = 0; i < m_outputList.size(); ++i)
    {
        if (m_outputList[i]->m_key == key && type == m_outputList[i]->getType())
        {
            return false;
        }
    }
    return true;
}

vector<AString> ParameterComponent::findUncheckedParams(const AString& contextString) const
{
    vector<AString> ret;
    for (size_t i = 0; i < m_paramList.size(); ++i)
    {
        if (!m_paramList[i]->m_operationUsed)
        {
            ret.push_back("parameter '" + m_paramList[i]->m_shortName + "' of " + contextString + " was not checked by the operation");
        }
    }
    for (size_t i = 0; i < m_outputList.size(); ++i)
    {
        if (!m_outputList[i]->m_operationUsed)
        {
            ret.push_back("parameter '" + m_outputList[i]->m_shortName + "' of " + contextString + " was not checked by the operation");
        }
    }
    for (size_t i = 0; i < m_optionList.size(); ++i)
    {
        if (!m_optionList[i]->m_operationUsed)
        {
            ret.push_back("option '" + m_optionList[i]->m_optionSwitch + "' of " + contextString + " was not checked by the operation");
        }
        if (m_optionList[i]->m_present)
        {
            vector<AString> temp = m_optionList[i]->findUncheckedParams("option '" + m_optionList[i]->m_optionSwitch + "'");
            ret.insert(ret.end(), temp.begin(), temp.end());
        }
    }
    for (size_t i = 0; i < m_repeatableOptions.size(); ++i)
    {
        if (!m_repeatableOptions[i]->m_operationUsed)
        {
            ret.push_back("option '" + m_repeatableOptions[i]->m_optionSwitch + "' of " + contextString + " was not checked by the operation");
        }
        for (size_t j = 0; j < m_repeatableOptions[i]->m_instances.size(); ++j)
        {
            vector<AString> temp = m_repeatableOptions[i]->m_instances[j]->findUncheckedParams("option '" + m_repeatableOptions[i]->m_optionSwitch + "'");
            ret.insert(ret.end(), temp.begin(), temp.end());
        }
    }
    return ret;
}

AbstractParameter* ParameterComponent::getInputParameter(const int32_t key, const OperationParametersEnum::Enum type)
{
    for (size_t i = 0; i < m_paramList.size(); ++i)
    {
        if (m_paramList[i]->m_key == key && type == m_paramList[i]->getType())
        {
            m_paramList[i]->m_operationUsed = true;
            return m_paramList[i];
        }
    }
    CaretAssert(false);
    throw ProgramParametersException("Algorithm asked for parameter it didn't specify, or of wrong type");
}

OptionalParameter* ParameterComponent::getOptionalParameter(const int32_t key)
{
    for (size_t i = 0; i < m_optionList.size(); ++i)
    {
        if (m_optionList[i]->m_key == key)
        {
            m_optionList[i]->m_operationUsed = true;
            return m_optionList[i];
        }
    }
    CaretAssert(false);
    throw ProgramParametersException("Algorithm asked for option it didn't specify");
}

const vector<ParameterComponent*>& ParameterComponent::getRepeatableParameterInstances(const int32_t key)
{
    for (size_t i = 0; i < m_repeatableOptions.size(); ++i)
    {
        if (m_repeatableOptions[i]->m_key == key)
        {
            m_repeatableOptions[i]->m_operationUsed = true;
            return m_repeatableOptions[i]->m_instances;
        }
    }
    CaretAssert(false);
    throw ProgramParametersException("Algorithm asked for repeatable option it didn't specify");
}

const vector<int32_t>& ParameterComponent::getRepeatableParameterPositions(const int32_t key)
{
    for (size_t i = 0; i < m_repeatableOptions.size(); ++i)
    {
        if (m_repeatableOptions[i]->m_key == key)
        {
            m_repeatableOptions[i]->m_operationUsed = true;
            return m_repeatableOptions[i]->m_positions;
        }
    }
    CaretAssert(false);
    throw ProgramParametersException("Algorithm asked for positions of repeatable options it didn't specify");
}

namespace
{
    int nextOpt(const vector<RepeatableOption*>& opts, const vector<size_t>& nextIndex)
    {
        int ret = -1;
        int32_t bestPos = -1;
        for (size_t i = 0; i < opts.size(); ++i)
        {
            if (nextIndex[i] < opts[i]->m_positions.size())
            {
                if (ret == -1 || opts[i]->m_positions[nextIndex[i]] < bestPos)
                {
                    ret = i;
                    bestPos = opts[i]->m_positions[nextIndex[i]];
                }
            }
        }
        return ret;
    }
}

vector<ParameterComponent::OrderInfo> ParameterComponent::getRepeatableOrder()
{
    vector<OrderInfo> ret;
    vector<size_t> indexList(m_repeatableOptions.size(), 0);
    while (true)//the for loop version of this is harder to read
    {
        int nextUse = nextOpt(m_repeatableOptions, indexList);
        if (nextUse == -1) break;
        ret.push_back(OrderInfo(m_repeatableOptions[nextUse]->m_key, indexList[nextUse]));
        ++indexList[nextUse];
    }
    return ret;
}

AbstractParameter* ParameterComponent::getOutputParameter(const int32_t key, const OperationParametersEnum::Enum type)
{
    for (size_t i = 0; i < m_outputList.size(); ++i)
    {
        if (m_outputList[i]->m_key == key && type == m_outputList[i]->getType())
        {
            m_outputList[i]->m_operationUsed = true;
            return m_outputList[i];
        }
    }
    CaretAssert(false);
    throw ProgramParametersException("Algorithm asked for output it didn't specify, or of wrong type");
}

//sadly, lots of boilerplate for convenience functions
//these only get called for all operations by -all-commands-help, so check and throw in release is fine
void ParameterComponent::addBooleanParameter(const int32_t key, const caret::AString& name, const caret::AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::BOOL))
    {
        CaretAssert(false);
        throw ProgramParametersException("input boolean parameter created with previously used key");
    }
    m_paramList.push_back(new BooleanParameter(key, name, description));
}

void ParameterComponent::addCiftiParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::CIFTI))
    {
        CaretAssert(false);
        throw ProgramParametersException("input cifti parameter created with previously used key");
    }
    m_paramList.push_back(new CiftiParameter(key, name, description));
}

void ParameterComponent::addFociParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::FOCI))
    {
        CaretAssert(false);
        throw ProgramParametersException("input foci parameter created with previously used key");
    }
    m_paramList.push_back(new FociParameter(key, name, description));
}

void ParameterComponent::addBorderParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::BORDER))
    {
        CaretAssert(false);
        throw ProgramParametersException("input border parameter created with previously used key");
    }
    m_paramList.push_back(new BorderParameter(key, name, description));
}

void ParameterComponent::addDoubleParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::DOUBLE))
    {
        CaretAssert(false);
        throw ProgramParametersException("input double parameter created with previously used key");
    }
    m_paramList.push_back(new DoubleParameter(key, name, description));
}

void ParameterComponent::addAnnotationParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::ANNOTATION))
    {
        CaretAssert(false);
        throw ProgramParametersException("input annotation parameter created with previously used key");
    }
    m_paramList.push_back(new AnnotationParameter(key, name, description));
}

void ParameterComponent::addMetricParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::METRIC))
    {
        CaretAssert(false);
        throw ProgramParametersException("input metric parameter created with previously used key");
    }
    m_paramList.push_back(new MetricParameter(key, name, description));
}

void ParameterComponent::addIntegerParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::INT))
    {
        CaretAssert(false);
        throw ProgramParametersException("input integer parameter created with previously used key");
    }
    m_paramList.push_back(new IntegerParameter(key, name, description));
}

void ParameterComponent::addLabelParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::LABEL))
    {
        CaretAssert(false);
        throw ProgramParametersException("input label parameter created with previously used key");
    }
    m_paramList.push_back(new LabelParameter(key, name, description));
}

void ParameterComponent::addStringParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::STRING))
    {
        CaretAssert(false);
        throw ProgramParametersException("input string parameter created with previously used key");
    }
    m_paramList.push_back(new StringParameter(key, name, description));
}

void ParameterComponent::addSurfaceParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::SURFACE))
    {
        CaretAssert(false);
        throw ProgramParametersException("input surface parameter created with previously used key");
    }
    m_paramList.push_back(new SurfaceParameter(key, name, description));
}

void ParameterComponent::addVolumeParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueInput(key, OperationParametersEnum::VOLUME))
    {
        CaretAssert(false);
        throw ProgramParametersException("input volume parameter created with previously used key");
    }
    m_paramList.push_back(new VolumeParameter(key, name, description));
}

void OperationParameters::setHelpText(const AString& textIn)
{
    m_helpText = textIn;
}

void ParameterComponent::addCiftiOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueOutput(key, OperationParametersEnum::CIFTI))
    {
        CaretAssert(false);
        throw ProgramParametersException("output cifti parameter created with previously used key");
    }
    m_outputList.push_back(new CiftiParameter(key, name, description));
}

void ParameterComponent::addFociOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueOutput(key, OperationParametersEnum::FOCI))
    {
        CaretAssert(false);
        throw ProgramParametersException("output foci parameter created with previously used key");
    }
    m_outputList.push_back(new FociParameter(key, name, description));
}

void ParameterComponent::addBorderOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueOutput(key, OperationParametersEnum::BORDER))
    {
        CaretAssert(false);
        throw ProgramParametersException("output foci parameter created with previously used key");
    }
    m_outputList.push_back(new BorderParameter(key, name, description));
}

void ParameterComponent::addAnnotationOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueOutput(key, OperationParametersEnum::ANNOTATION))
    {
        CaretAssert(false);
        throw ProgramParametersException("output annotation parameter created with previously used key");
    }
    m_outputList.push_back(new AnnotationParameter(key, name, description));
}

void ParameterComponent::addMetricOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueOutput(key, OperationParametersEnum::METRIC))
    {
        CaretAssert(false);
        throw ProgramParametersException("output metric parameter created with previously used key");
    }
    m_outputList.push_back(new MetricParameter(key, name, description));
}

void ParameterComponent::addLabelOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueOutput(key, OperationParametersEnum::LABEL))
    {
        CaretAssert(false);
        throw ProgramParametersException("output label parameter created with previously used key");
    }
    m_outputList.push_back(new LabelParameter(key, name, description));
}

void ParameterComponent::addSurfaceOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueOutput(key, OperationParametersEnum::SURFACE))
    {
        CaretAssert(false);
        throw ProgramParametersException("output surface parameter created with previously used key");
    }
    m_outputList.push_back(new SurfaceParameter(key, name, description));
}

void ParameterComponent::addVolumeOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    if (!checkUniqueOutput(key, OperationParametersEnum::VOLUME))
    {
        CaretAssert(false);
        throw ProgramParametersException("output volume parameter created with previously used key");
    }
    m_outputList.push_back(new VolumeParameter(key, name, description));
}

AString& OperationParameters::getHelpText()
{
    return m_helpText;
}

AbstractParameter::~AbstractParameter()
{
}

bool ParameterComponent::getBoolean(const int32_t key)
{
    return ((BooleanParameter*)getInputParameter(key, OperationParametersEnum::BOOL))->m_parameter;
}

CiftiFile* ParameterComponent::getCifti(const int32_t key)
{
    return ((CiftiParameter*)getInputParameter(key, OperationParametersEnum::CIFTI))->m_parameter.getPointer();
}

FociFile* ParameterComponent::getFoci(const int32_t key)
{
    return ((FociParameter*)getInputParameter(key, OperationParametersEnum::FOCI))->m_parameter.getPointer();
}

BorderFile* ParameterComponent::getBorder(const int32_t key)
{
    return ((BorderParameter*)getInputParameter(key, OperationParametersEnum::BORDER))->m_parameter.getPointer();
}

double ParameterComponent::getDouble(const int32_t key)
{
    return ((DoubleParameter*)getInputParameter(key, OperationParametersEnum::DOUBLE))->m_parameter;
}

int64_t ParameterComponent::getInteger(const int32_t key)
{
    return ((IntegerParameter*)getInputParameter(key, OperationParametersEnum::INT))->m_parameter;
}

LabelFile* ParameterComponent::getLabel(const int32_t key)
{
    return ((LabelParameter*)getInputParameter(key, OperationParametersEnum::LABEL))->m_parameter.getPointer();
}

AnnotationFile* ParameterComponent::getAnnotation(const int32_t key)
{
    return ((AnnotationParameter*)getInputParameter(key, OperationParametersEnum::ANNOTATION))->m_parameter.getPointer();
}

MetricFile* ParameterComponent::getMetric(const int32_t key)
{
    return ((MetricParameter*)getInputParameter(key, OperationParametersEnum::METRIC))->m_parameter.getPointer();
}

const AString& ParameterComponent::getString(const int32_t key)
{
    return ((StringParameter*)getInputParameter(key, OperationParametersEnum::STRING))->m_parameter;
}

SurfaceFile* ParameterComponent::getSurface(const int32_t key)
{
    return ((SurfaceParameter*)getInputParameter(key, OperationParametersEnum::SURFACE))->m_parameter.getPointer();
}

VolumeFile* ParameterComponent::getVolume(const int32_t key)
{
    return ((VolumeParameter*)getInputParameter(key, OperationParametersEnum::VOLUME))->m_parameter.getPointer();
}

CiftiFile* ParameterComponent::getOutputCifti(const int32_t key)
{
    return ((CiftiParameter*)getOutputParameter(key, OperationParametersEnum::CIFTI))->m_parameter.getPointer();
}

FociFile* ParameterComponent::getOutputFoci(const int32_t key)
{
    return ((FociParameter*)getOutputParameter(key, OperationParametersEnum::FOCI))->m_parameter.getPointer();
}

BorderFile* ParameterComponent::getOutputBorder(const int32_t key)
{
    return ((BorderParameter*)getOutputParameter(key, OperationParametersEnum::BORDER))->m_parameter.getPointer();
}

LabelFile* ParameterComponent::getOutputLabel(const int32_t key)
{
    return ((LabelParameter*)getOutputParameter(key, OperationParametersEnum::LABEL))->m_parameter.getPointer();
}

SurfaceFile* ParameterComponent::getOutputSurface(const int32_t key)
{
    return ((SurfaceParameter*)getOutputParameter(key, OperationParametersEnum::SURFACE))->m_parameter.getPointer();
}

VolumeFile* ParameterComponent::getOutputVolume(const int32_t key)
{
    return ((VolumeParameter*)getOutputParameter(key, OperationParametersEnum::VOLUME))->m_parameter.getPointer();
}

AnnotationFile* ParameterComponent::getOutputAnnotation(const int32_t key)
{
    return ((AnnotationParameter*)getOutputParameter(key, OperationParametersEnum::ANNOTATION))->m_parameter.getPointer();
}

MetricFile* ParameterComponent::getOutputMetric(const int32_t key)
{
    return ((MetricParameter*)getOutputParameter(key, OperationParametersEnum::METRIC))->m_parameter.getPointer();
}
