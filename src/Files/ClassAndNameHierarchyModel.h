#ifndef __CLASS_AND_NAME_HIERARCHY_MODEL_H_
#define __CLASS_AND_NAME_HIERARCHY_MODEL_H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <deque>
#include <map>

#include "BrainConstants.h"
#include "CaretObject.h"
#include "DisplayGroupEnum.h"

namespace caret {

    class BorderFile;
    class FociFile;
    class LabelFile;
    
    class ClassAndNameHierarchyModel : public CaretObject {
    public:
        /* ==================== Member classes ========================================
         *
         * Need to declare these member classes here.  Otherwise, the compiler will
         * have problems when these classes are used in templates as members of
         * the encapsulating class.
         */
        
        class NameDisplayGroupSelector  : public CaretObject{
        public:
            NameDisplayGroupSelector(const AString& name,
                                     const int32_t key);
            
            ~NameDisplayGroupSelector();
            
            void copySelections(const int32_t sourceTabIndex,
                                const int32_t targetTabIndex);
            
            AString getName() const;
            
            int32_t getKey() const;
            
            bool isSelected(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex) const;
            
            void setSelected(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const bool status);
            
            void clearCounter();
            
            void incrementCounter();
            
            int32_t getCounter() const;
            
        private:
            /** Name of an item (border, focus, etc) */
            AString name;
            
            /** Key for quickly locating item */
            int32_t key;
            
            /** Selection for each display group */
            bool selectedInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
            
            /** Selection for each tab */
            bool selectedInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
            
            /** Counter for tracking usage of item */
            int32_t counter;
        };
        
        class ClassDisplayGroupSelector : public NameDisplayGroupSelector {
        protected:
            ClassDisplayGroupSelector(const AString& name,
                                      const int32_t key);
            
        public:
            virtual ~ClassDisplayGroupSelector();
            
            virtual void clear();
            
            void copySelections(const int32_t sourceTabIndex,
                                const int32_t targetTabIndex);
            
            void setAllSelected(const bool status);
            
            void setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                                const int32_t tabIndex,
                                const bool status);
            
            std::vector<int32_t> getAllNameKeysSortedByName() const;
            
            virtual NameDisplayGroupSelector* getNameSelectorWithKey(const int32_t nameKey) = 0;
            
            virtual const NameDisplayGroupSelector* getNameSelectorWithKey(const int32_t nameKey) const = 0;
            
            NameDisplayGroupSelector* getNameSelectorWithName(const AString& name);
            
            bool isExpanded(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex) const;
            
            void setExpanded(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const bool expanded);

            virtual int32_t getNumberOfNamesWithCountersGreaterThanZero() const = 0;
            
            virtual void clearAllNameCounters() = 0;
            
            virtual void removeNamesWithCountersEqualZero() = 0;
            
        protected:
            void addToNameSelectorMap(const AString& name,
                                      NameDisplayGroupSelector* nameSelector);
            
            void removeNameSelector(NameDisplayGroupSelector* nameSelector);
            
        private:
            void clearPrivate();
            
            /** Maps a name to its name information.  Map is fastest way to search by name.   */
            std::map<AString, NameDisplayGroupSelector*> nameToNameSelectorMap;
            
            /** Expanded (collapsed) status in display group */
            bool expandedStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
            
            /** Expanded (collapsed) status in tab */
            bool expandedStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        };
        
        class ClassDisplayGroupSelectorGeneratedKey : public ClassDisplayGroupSelector {
        public:
            ClassDisplayGroupSelectorGeneratedKey(const AString& name,
                                      const int32_t key);
            
            ~ClassDisplayGroupSelectorGeneratedKey();
            
            void clear();
            
            int32_t addName(const AString& name);

            NameDisplayGroupSelector* getNameSelectorWithKey(const int32_t nameKey);
            
            const NameDisplayGroupSelector* getNameSelectorWithKey(const int32_t nameKey) const;
            
            int32_t getNumberOfNamesWithCountersGreaterThanZero() const;
            
            void clearAllNameCounters();
            
            void removeNamesWithCountersEqualZero();
            
        private:
            /** If keys are removed, they are stored here for future reuse. */
            std::deque<int32_t> availableNameKeys;
            
            /** Indexes name information by name key.  Vector provides fast access by key. */
            std::vector<NameDisplayGroupSelector*> keyToNameSelectorVector;
        };
        
    public:
        ClassAndNameHierarchyModel();
        
        virtual ~ClassAndNameHierarchyModel();
        
        void clear();
        
        void copyClassNameAndHierarchy(const int32_t sourceTabIndex,
                                       const int32_t targetTabIndex);
        
        void removeUnusedNamesAndClasses(BorderFile* borderFile);
        
        void removeUnusedNamesAndClasses(FociFile* fociFile);
        
        void removeUnusedNamesAndClasses(LabelFile* labelFile);
        
        bool isClassValid(const int32_t classKey) const;
        
        void setAllSelected(const bool status);
        
        void setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const bool status);
        
        void update(BorderFile* borderFile,
                    const bool forceUpdate);
        
        void update(FociFile* fociFile,
                    const bool forceUpdate);
        
        void update(LabelFile* labelFile,
                    const bool forceUpdate);
        
        AString toString() const;
        
        AString getName() const;
        
        bool isSelected(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex) const;
        
        void setSelected(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const bool selectionStatus);
        
        std::vector<int32_t> getAllClassKeysSortedByName() const;
        
        ClassDisplayGroupSelector* getClassSelectorForClassName(const AString& className);
        
        const ClassDisplayGroupSelector* getClassSelectorForClassName(const AString& className) const;
        
        ClassDisplayGroupSelector* getClassSelectorForClassKey(const int32_t classKey);
        
        const ClassDisplayGroupSelector* getClassSelectorForClassKey(const int32_t classKey) const;
        
        void addName(const AString& parentClassName,
                     const AString& name,
                     int32_t& parentClassKeyOut,
                     int32_t& nameKeyOut);
        
        bool isClassSelected(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const int32_t classKey) const;
        
        void setClassSelected(const DisplayGroupEnum::Enum displayGroup,
                              const int32_t tabIndex,
                              const int32_t classKey,
                              const bool selected);
        
        bool isNameSelected(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const int32_t parentClassKey,
                            const int32_t nameKey) const;

        void setNameSelected(const DisplayGroupEnum::Enum displayGroup,
                             const int32_t tabIndex,
                             const int32_t parentClassKey,
                             const int32_t nameKey,
                             const bool selected);
        
        bool isExpanded(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex) const;
        
        void setExpanded(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const bool expanded);
        
    private:
        ClassAndNameHierarchyModel(const ClassAndNameHierarchyModel&);

        ClassAndNameHierarchyModel& operator=(const ClassAndNameHierarchyModel&);
        
        /** Name of model, does NOT get cleared. */
        AString name;
        
        /* overlay selection status in display group */
        bool selectionStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        /* overlay selection status in tab */
        bool selectionStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Expanded (collapsed) status in display group */        
        bool expandedStatusInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];

        /** Expanded (collapsed) status in tab */        
        bool expandedStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** If keys are removed, they are stored here for future reuse. */
        std::deque<int32_t> availableClassKeys;
        
        /** Holds class and its names, indexed by class key.  Vector provides fast access by key. */
        std::vector<ClassDisplayGroupSelector*> keyToClassNameSelectorVector;
        
        /** Maps a class name to its class selector.  Map is fastest way to search by name.  */
        std::map<AString, ClassDisplayGroupSelector*> classNameToClassSelectorMap;
        
        
    };
    
#ifdef __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_MODEL_H_
