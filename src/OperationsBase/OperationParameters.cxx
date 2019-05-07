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
    CaretAssertMessage(checkUniqueOption(key), "optional parameter created with previously used key");
    if (optionSwitch.isEmpty() || optionSwitch[0] != '-') CaretLogWarning("developer warning: option '" + optionSwitch + "' created, but does not start with dash");
    OptionalParameter* ret = new OptionalParameter(key, optionSwitch, description);
    m_optionList.push_back(ret);
    return ret;
}

ParameterComponent* ParameterComponent::createRepeatableParameter(const int32_t key, const AString& optionSwitch, const AString& description)
{
    CaretAssertMessage(checkUniqueRepeatable(key), "repeatable parameter created with previously used key");
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
    CaretAssertMessage(false, "Algorithm asked for parameter it didn't specify, or of wrong type");
    return NULL;
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
    CaretAssertMessage(false, "Algorithm asked for option it didn't specify");
    return NULL;
}

const vector<ParameterComponent*>* ParameterComponent::getRepeatableParameterInstances(const int32_t key)
{
    for (size_t i = 0; i < m_repeatableOptions.size(); ++i)
    {
        if (m_repeatableOptions[i]->m_key == key)
        {
            m_repeatableOptions[i]->m_operationUsed = true;
            return &(m_repeatableOptions[i]->m_instances);
        }
    }
    CaretAssertMessage(false, "Algorithm asked for option it didn't specify");
    return NULL;
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
    CaretAssertMessage(false, "Algorithm asked for output it didn't specify, or of wrong type");
    return NULL;
}

//sadly, lots of boilerplate for convenience functions
void ParameterComponent::addBooleanParameter(const int32_t key, const caret::AString& name, const caret::AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::BOOL), "input boolean parameter created with previously used key");
    m_paramList.push_back(new BooleanParameter(key, name, description));
}

void ParameterComponent::addCiftiParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::CIFTI), "input cifti parameter created with previously used key");
    m_paramList.push_back(new CiftiParameter(key, name, description));
}

void ParameterComponent::addFociParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::FOCI), "input foci parameter created with previously used key");
    m_paramList.push_back(new FociParameter(key, name, description));
}

void ParameterComponent::addBorderParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::BORDER), "input border parameter created with previously used key");
    m_paramList.push_back(new BorderParameter(key, name, description));
}

void ParameterComponent::addDoubleParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::DOUBLE), "input double parameter created with previously used key");
    m_paramList.push_back(new DoubleParameter(key, name, description));
}

void ParameterComponent::addAnnotationParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::ANNOTATION), "input annotation parameter created with previously used key");
    m_paramList.push_back(new AnnotationParameter(key, name, description));
}

void ParameterComponent::addMetricParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::METRIC), "input metric parameter created with previously used key");
    m_paramList.push_back(new MetricParameter(key, name, description));
}

void ParameterComponent::addIntegerParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::INT), "input integer parameter created with previously used key");
    m_paramList.push_back(new IntegerParameter(key, name, description));
}

void ParameterComponent::addLabelParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::LABEL), "input label parameter created with previously used key");
    m_paramList.push_back(new LabelParameter(key, name, description));
}

void ParameterComponent::addStringParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::STRING), "input string parameter created with previously used key");
    m_paramList.push_back(new StringParameter(key, name, description));
}

void ParameterComponent::addSurfaceParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::SURFACE), "input surface parameter created with previously used key");
    m_paramList.push_back(new SurfaceParameter(key, name, description));
}

void ParameterComponent::addVolumeParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueInput(key, OperationParametersEnum::VOLUME), "input volume parameter created with previously used key");
    m_paramList.push_back(new VolumeParameter(key, name, description));
}

void OperationParameters::setHelpText(const AString& textIn)
{
    m_helpText = textIn;
}

void ParameterComponent::addCiftiOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueOutput(key, OperationParametersEnum::CIFTI), "output cifti parameter created with previously used key");
    m_outputList.push_back(new CiftiParameter(key, name, description));
}

void ParameterComponent::addFociOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueOutput(key, OperationParametersEnum::FOCI), "output foci parameter created with previously used key");
    m_outputList.push_back(new FociParameter(key, name, description));
}

void ParameterComponent::addBorderOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueOutput(key, OperationParametersEnum::BORDER), "output foci parameter created with previously used key");
    m_outputList.push_back(new BorderParameter(key, name, description));
}

void ParameterComponent::addAnnotationOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueOutput(key, OperationParametersEnum::ANNOTATION), "output annotation parameter created with previously used key");
    m_outputList.push_back(new AnnotationParameter(key, name, description));
}

void ParameterComponent::addMetricOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueOutput(key, OperationParametersEnum::METRIC), "output metric parameter created with previously used key");
    m_outputList.push_back(new MetricParameter(key, name, description));
}

void ParameterComponent::addLabelOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueOutput(key, OperationParametersEnum::LABEL), "output label parameter created with previously used key");
    m_outputList.push_back(new LabelParameter(key, name, description));
}

void ParameterComponent::addSurfaceOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueOutput(key, OperationParametersEnum::SURFACE), "output surface parameter created with previously used key");
    m_outputList.push_back(new SurfaceParameter(key, name, description));
}

void ParameterComponent::addVolumeOutputParameter(const int32_t key, const AString& name, const AString& description)
{
    CaretAssertMessage(checkUniqueOutput(key, OperationParametersEnum::VOLUME), "output volume parameter created with previously used key");
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
