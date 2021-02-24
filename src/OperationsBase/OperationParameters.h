#ifndef __OPERATION_PARAMETERS_H__
#define __OPERATION_PARAMETERS_H__

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

#include "AString.h"
#include "CaretMutex.h"
#include "CaretPointer.h"
#include "OperationParametersEnum.h"
#include "DataFileException.h"

#include <QFile>

#include <cstdint>
#include <vector>

namespace caret {

    class AnnotationFile;
    class BorderFile;
    class CiftiFile;
    class FociFile;
    class LabelFile;
    class GiftiMetaData;
    class MetricFile;
    class SurfaceFile;
    class VolumeFile;
    
    struct OptionalParameter;
    struct RepeatableOption;
    
    struct ProvenanceHelper
    {
        const static AString PROVENANCE_NAME, PARENT_PROVENANCE_NAME, PROGRAM_PROVENANCE_NAME, CWD_PROVENANCE_NAME;
        AString m_parentProvenance, m_provenance, m_versionProvenance, m_workingDir;
        //track whether an input is lazy read after an on-disk output file is obtained
        bool m_outputProvDone, m_doProvenance;
        CaretMutex m_mutex;//synchronize by mutex rather than omp, to be safe
        ProvenanceHelper() { m_outputProvDone = false; m_doProvenance = true; }
        void addToProvenance(const GiftiMetaData* md, const AString& filename);
        void outputProvenance(GiftiMetaData* md);
    };
    
    struct AbstractParameter
    {
        int32_t m_key;//identifies this parameter uniquely for this algorithm
        AString m_shortName, m_description;
        bool m_operationUsed;//check if the operation called get...() for this parameter
        virtual OperationParametersEnum::Enum getType() = 0;
        virtual AbstractParameter* cloneAbstractParameter() = 0;
        AbstractParameter(int32_t key, const AString& shortName, const AString& description) :
        m_key(key),
        m_shortName(shortName),
        m_description(description),
        m_operationUsed(false)
        {
        };
        virtual ~AbstractParameter();
        virtual void checkExists() { }
    };
    
    struct ParameterComponent
    {//sadly, inheriting from a friend class doesn't give you access to private members, so these are entirely public so parsers can use them
        std::vector<AbstractParameter*> m_paramList;//mandatory arguments
        std::vector<AbstractParameter*> m_outputList;//should this be a different type? input and output parameters are very similar, just pointers to files
        std::vector<OptionalParameter*> m_optionList;//optional arguments
        std::vector<RepeatableOption*> m_repeatableOptions;//repeatable options
        
        //for lazy loading with on-disk output, provenance needs to be tracked (and added) during the operation code, so the parser can't do it
        //ParameterComponent does not own this instance
        ProvenanceHelper* m_provHelper;
        
        //recursively populate parameter tree with provenance helper pointer
        void prepareProvenance(ProvenanceHelper* helper);
        
        //helpers for lazy reading, throw rather than return on problem
        void checkInputFilesExist();
        void openAllInputFiles();
        
        ///constructor
        ParameterComponent();
        
        ///destructor
        virtual ~ParameterComponent();
        
        ///copy constructor so RepeatableOption can copy its template to a new instance
        ParameterComponent(const ParameterComponent& rhs);
        
        ///add a parameter to get next item as a string
        void addStringParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a string with a key
        const AString& getString(const int32_t key);
        
        ///add a parameter to get next item as a string
        void addBooleanParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a string with a key
        bool getBoolean(const int32_t key);
        
        ///add a parameter to get next item as an int32
        void addIntegerParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get an integer with a key
        int64_t getInteger(const int32_t key);
        
        ///add a parameter to get next item as a double
        void addDoubleParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a double with a key
        double getDouble(const int32_t key);
        
        ///add a parameter to get next item as a surface
        void addSurfaceParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a surface with a key
        SurfaceFile* getSurface(const int32_t key);
        
        ///add a parameter to get next item as a volume
        void addVolumeParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a volume with a key
        VolumeFile* getVolume(const int32_t key);
        
        ///add a parameter to get next item as an annotation file
        void addAnnotationParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get an annotation with a key
        AnnotationFile* getAnnotation(const int32_t key);
        
        ///add a parameter to get next item as a functional file (metric)
        void addMetricParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a metric with a key
        MetricFile* getMetric(const int32_t key);
        
        ///add a parameter to get next item as a label file
        void addLabelParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a label with a key
        LabelFile* getLabel(const int32_t key);
        
        ///add a parameter to get next item as a cifti file - TODO: make methods for different cifti types?
        void addCiftiParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a cifti with a key
        CiftiFile* getCifti(const int32_t key);
        
        ///add a parameter to get next item as a foci file
        void addFociParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a foci file with a key
        FociFile* getFoci(const int32_t key);
        
        ///add a parameter to get next item as a border file
        void addBorderParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a border file with a key
        BorderFile* getBorder(const int32_t key);
        
        ///add a parameter to get next item as a surface
        void addSurfaceOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a surface with a key
        SurfaceFile* getOutputSurface(const int32_t key);
        
        ///add a parameter to get next item as a volume
        void addVolumeOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a volume with a key
        VolumeFile* getOutputVolume(const int32_t key);
        
        ///add a parameter to get next item as an annotation file
        void addAnnotationOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get an annotation with a key
        AnnotationFile* getOutputAnnotation(const int32_t key);
        
        ///add a parameter to get next item as a functional file (metric)
        void addMetricOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a metric with a key
        MetricFile* getOutputMetric(const int32_t key);
        
        ///add a parameter to get next item as a label file
        void addLabelOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a label with a key
        LabelFile* getOutputLabel(const int32_t key);
        
        ///add a parameter to get next item as a cifti file - TODO: make methods for different cifti types?
        void addCiftiOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a cifti with a key
        CiftiFile* getOutputCifti(const int32_t key);
        
        ///add a parameter to get next item as a foci file
        void addFociOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a foci file with a key
        FociFile* getOutputFoci(const int32_t key);
        
        ///add a parameter to get next item as a border file
        void addBorderOutputParameter(const int32_t key, const AString& name, const AString& description);
        
        ///get a border file with a key
        BorderFile* getOutputBorder(const int32_t key);
        
        ///convenience method to create, add, and return an optional parameter
        OptionalParameter* createOptionalParameter(const int32_t key, const AString& optionSwitch, const AString& description);
        
        ///convenience method to create, add, and return an optional parameter
        ParameterComponent* createRepeatableParameter(const int32_t key, const AString& optionSwitch, const AString& description);
        
        ///return pointer to an option
        OptionalParameter* getOptionalParameter(const int32_t key);
        
        ///return instances of a repeatable option
        const std::vector<ParameterComponent*>& getRepeatableParameterInstances(const int32_t key);
        
        ///return positions of the instances, only needed for chaining different transform types in volume resample so far
        const std::vector<int32_t>& getRepeatableParameterPositions(const int32_t key);
        
        struct OrderInfo
        {
            int32_t key;
            size_t index;
            OrderInfo(int32_t keyIn, size_t indexIn) { key = keyIn; index = indexIn; }
        };
        ///return a vector in the order of all repeatable options, with key and index into instances
        std::vector<OrderInfo> getRepeatableOrder();

        ///functions to check for key/type uniqueness - used only in asserts
        bool checkUniqueInput(const int32_t& key, const OperationParametersEnum::Enum& type);
        bool checkUniqueOutput(const int32_t& key, const OperationParametersEnum::Enum& type);
        bool checkUniqueOption(const int32_t& key);
        bool checkUniqueRepeatable(const int32_t& key);
        
        ///helper for checking that all parameters have been checked by the operation, returns warning strings
        std::vector<AString> findUncheckedParams(const AString& contextString) const;
        
    private:
        //these should be private to help ensure parent provenance gets populated, as that code is in the type-specific functions
        AbstractParameter* getInputParameter(const int32_t key, const OperationParametersEnum::Enum type);
        AbstractParameter* getOutputParameter(const int32_t key, const OperationParametersEnum::Enum type);
    };
    
    struct OptionalParameter : public ParameterComponent
    {
        int32_t m_key;//uniquely identifies this option
        AString m_optionSwitch, m_description;
        bool m_present;//to be filled by parser
        bool m_operationUsed;//check if the operation called get...() for this parameter
        OptionalParameter(const OptionalParameter& rhs) ://copy constructor is used by cloning in RepeatableParameter
        ParameterComponent(rhs),
        m_key(rhs.m_key),
        m_optionSwitch(rhs.m_optionSwitch),
        m_description(rhs.m_description),
        m_present(false),
        m_operationUsed(false)
        {
        }
        OptionalParameter(int32_t key, const AString& optionSwitch, const AString& description) :
        m_key(key),
        m_optionSwitch(optionSwitch),
        m_description(description),
        m_present(false),
        m_operationUsed(false)
        {
        }
    private:
        OptionalParameter();//no default construction
    };
    
    struct RepeatableOption
    {
        int32_t m_key;//uniquely identifies this option
        AString m_optionSwitch, m_description;
        ParameterComponent m_template;
        bool m_operationUsed;//check if the operation called get...() for this parameter
        std::vector<ParameterComponent*> m_instances;//to be filled by parser
        std::vector<int32_t> m_positions;//to resolve ambiguities in ordering between different repeatable options
        RepeatableOption(const RepeatableOption& rhs) :
        m_key(rhs.m_key),
        m_optionSwitch(rhs.m_optionSwitch),
        m_description(rhs.m_description),
        m_template(rhs.m_template),
        m_operationUsed(false)
        {
        }
        RepeatableOption(int32_t key, const AString& optionSwitch, const AString& description) :
        m_key(key),
        m_optionSwitch(optionSwitch),
        m_description(description),
        m_operationUsed(false)
        {
        }
        ~RepeatableOption();
        void prepareProvenance(ProvenanceHelper* helper);
        void checkInputFilesExist();
        void openAllInputFiles();
    };
    
    struct OperationParameters : public ParameterComponent
    {
        AString m_helpText;//to be formatted by the parser object for display in terminal or modal window

        ///constructor
        OperationParameters();
        
        ///set the help text of the algorithm - you DO NOT need to add newlines within paragraphs or list the parameters, or give a description of each parameter! describe ONLY what it does, plus any quirks
        void setHelpText(const AString& textIn);
        
        ///get the unformatted help text, without command or arguments descriptions, to be formatted by the argument parser
        AString& getHelpText();
    };

    //templates for the common cases
    template<typename T, OperationParametersEnum::Enum TYPE>
    struct PointerTemplateParameter : public AbstractParameter
    {
        virtual OperationParametersEnum::Enum getType() { return TYPE; }
        virtual AbstractParameter* cloneAbstractParameter()
        {
            AbstractParameter* ret = new PointerTemplateParameter<T, TYPE>(m_key, m_shortName, m_description);
            return ret;
        }
        CaretPointer<T> m_parameter;//so the GUI parser and the commandline parser don't need to do different things to delete the parameter info
        PointerTemplateParameter(const int32_t key, const AString& shortName, const AString& description) : AbstractParameter(key, shortName, description)
        {//CaretPointer self-initializes to NULL, so don't need to do anything
        }
    };
    
    template<typename T, OperationParametersEnum::Enum TYPE>
    struct PrimitiveTemplateParameter : public AbstractParameter
    {
        virtual OperationParametersEnum::Enum getType() { return TYPE; }
        T m_parameter;
        virtual AbstractParameter* cloneAbstractParameter()
        {
            PrimitiveTemplateParameter<T, TYPE>* ret = new PrimitiveTemplateParameter<T, TYPE>(m_key, m_shortName, m_description);
            ret->m_parameter = 0;
            return ret;
        }
        PrimitiveTemplateParameter(const int32_t key, const AString& shortName, const AString& description) : AbstractParameter(key, shortName, description)
        {
            m_parameter = T(0);
        }
    };
    
    struct StringParameter : public AbstractParameter
    {
        virtual OperationParametersEnum::Enum getType() { return OperationParametersEnum::STRING; }
        virtual AbstractParameter* cloneAbstractParameter()
        {
            AbstractParameter* ret = new StringParameter(m_key, m_shortName, m_description);
            return ret;
        }
        AString m_parameter;
        StringParameter(int32_t key, const AString& shortName, const AString& description) : AbstractParameter(key, shortName, description)
        {//AString self-initializes to "", so don't need to do anything
        }
    };
    
    template<typename T, OperationParametersEnum::Enum TYPE>
    struct LazyFileParameter : public AbstractParameter
    {
        virtual OperationParametersEnum::Enum getType() { return TYPE; }
        virtual AbstractParameter* cloneAbstractParameter()
        {//clone is currently implemented to NOT copy pointer or value in other things of this type, so...
            AbstractParameter* ret = new LazyFileParameter<T, TYPE>(m_key, m_shortName, m_description);
            return ret;
        }
        CaretPointer<T> m_parameter;//so the GUI parser and the commandline parser don't need to do different things to delete the parameter info
        AString m_filename;
        bool m_doOnDiskWrite;
        const LazyFileParameter<T, TYPE>* m_collidingParam;
        LazyFileParameter(const int32_t key, const AString& shortName, const AString& description) : AbstractParameter(key, shortName, description)
        {
            m_doOnDiskWrite = false;//on-disk writing, like cifti, needs special checks for overwriting inputs, so default to false
            m_collidingParam = NULL;
        }
        void checkExists() { if (!QFile::exists(m_filename)) throw DataFileException(m_filename, "file does not exist"); }
        T* lazyGet() { if (m_parameter == NULL) { m_parameter.grabNew(new T()); } return m_parameter; }
    };
    
    //some friendlier names
    typedef LazyFileParameter<AnnotationFile, OperationParametersEnum::ANNOTATION> AnnotationParameter;
    typedef LazyFileParameter<BorderFile, OperationParametersEnum::BORDER> BorderParameter;
    typedef LazyFileParameter<CiftiFile, OperationParametersEnum::CIFTI> CiftiParameter;
    typedef LazyFileParameter<FociFile, OperationParametersEnum::FOCI> FociParameter;
    typedef LazyFileParameter<LabelFile, OperationParametersEnum::LABEL> LabelParameter;
    typedef LazyFileParameter<MetricFile, OperationParametersEnum::METRIC> MetricParameter;
    typedef LazyFileParameter<SurfaceFile, OperationParametersEnum::SURFACE> SurfaceParameter;
    typedef LazyFileParameter<VolumeFile, OperationParametersEnum::VOLUME> VolumeParameter;
    
    typedef PrimitiveTemplateParameter<double, OperationParametersEnum::DOUBLE> DoubleParameter;
    typedef PrimitiveTemplateParameter<int64_t, OperationParametersEnum::INT> IntegerParameter;
    typedef PrimitiveTemplateParameter<bool, OperationParametersEnum::BOOL> BooleanParameter;
    
}

#endif //__OPERATION_PARAMETERS_H__
