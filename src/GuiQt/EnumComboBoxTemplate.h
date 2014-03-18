#ifndef __ENUM_COMBOBOX_TEMPLATE__H_
#define __ENUM_COMBOBOX_TEMPLATE__H_

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

#include <QComboBox>
#include "AString.h"
#include "WuQFactory.h"
#include "WuQWidget.h"

/**
 * \class caret::EnumComboBoxTemplate
 * \brief Create a combo box for a workbench enumerated type.
 * \ingroup GuiQt
 *
 * Create a combo box for a workbench enumerated type.  Typically,
 * the enumerated types are created with wb_command: 
 *   wb_command.app/Contents/MacOS/wb_command -class-create-enum LabelDrawingTypeEnum 2 true
 *
 * This class is not a template class because QObject subclasses may
 * not be templates.  See http://doc.trolltech.com/qq/qq15-academic.html
 * While the class cannot be a template, methods in the class be templates.
 *
 * <p>
 * How to Use This Class
 * <p>
 * Declare: <br>
 * EnumComboBoxTemplate* m_someTypeComboBox;
 *
 * <p>
 * Construct: <br>
 *  m_someTypeComboBox = new EnumComboBoxTemplate(this); <br>
 *  m_someTypeComboBox->setup<SomeTypeEnum, SomeTypeEnum::Enum>(); <br>
 *
 * <p>
 * Read: <br>
 * const SomeTypeEnum::Enum enumValue = m_someTypeComboBox->getSelectedItem<SomeTypeEnum, SomeTypeEnum::Enum>();
 *
 * <p>
 * Set: <br>
 * m_someTypeComboBox->setSelectedItem<SomeTypeEnum, SomeTypeEnum::Enum>(enumValue);
 *
 * <p>
 * Get notified when user makes selection:<br>
 * See itemActivated() and itemChanged().
 *
 */
namespace caret {

    class EnumComboBoxTemplate : public WuQWidget {
        
        Q_OBJECT

    public:
        /**
         * Constructor.
         * @param parent
         *     Parent object.
         */
        EnumComboBoxTemplate(QObject* parent)
        : WuQWidget(parent) {
            m_itemComboBox = WuQFactory::newComboBox();

            QObject::connect(m_itemComboBox, SIGNAL(currentIndexChanged(int)),
                             this, SLOT(itemComboBoxIndexChanged(int)));
            QObject::connect(m_itemComboBox, SIGNAL(activated(int)),
                             this, SLOT(itemComboBoxActivated(int)));
        }
        
        /**
         * Destructor.
         */
        virtual ~EnumComboBoxTemplate() {
            
        }
        
        /**
         * Setup the combo box.
         */
        template<class CT, typename ET>
        void setup() {
            std::vector<ET> allEnums;
            CT::getAllEnums(allEnums);
            
            m_itemComboBox->blockSignals(true);
            
            m_itemComboBox->clear();
            const int32_t numColors = static_cast<int32_t>(allEnums.size());
            for (int32_t i = 0; i < numColors; i++) {
                const ET enumValue = allEnums[i];
                const int32_t indx = m_itemComboBox->count();
                const AString name = CT::toGuiName(enumValue);
                m_itemComboBox->addItem(name);
                m_itemComboBox->setItemData(indx,
                                            CT::toIntegerCode(enumValue));
            }
            
            m_itemComboBox->blockSignals(false);
        }
        
        /**
         * Setup the combo box.
         */
        template<class CT, typename ET>
        void setupWithItems(const std::vector<ET>& comboBoxItems) {
            m_itemComboBox->blockSignals(true);
            
            m_itemComboBox->clear();
            const int32_t numColors = static_cast<int32_t>(comboBoxItems.size());
            for (int32_t i = 0; i < numColors; i++) {
                const ET enumValue = comboBoxItems[i];
                const int32_t indx = m_itemComboBox->count();
                const AString name = CT::toGuiName(enumValue);
                m_itemComboBox->addItem(name);
                m_itemComboBox->setItemData(indx,
                                            CT::toIntegerCode(enumValue));
            }
            m_itemComboBox->blockSignals(false);
        }
        
        /**
         * @return The selected item.
         */
        template<class CT, typename ET> ET getSelectedItem() const {
            const int32_t indx = m_itemComboBox->currentIndex();
            const int32_t integerCode = m_itemComboBox->itemData(indx).toInt();
            ET item = CT::fromIntegerCode(integerCode, NULL);
            return item;
            
        }
        
        /**
         * Set the selected item.
         * @param item
         *   New item for selection.
         */
        template<class CT, typename ET> 
        void setSelectedItem(const ET item) {
            const int32_t numItems = static_cast<int32_t>(m_itemComboBox->count());
            for (int32_t i = 0; i < numItems; i++) {
                const int32_t integerCode = m_itemComboBox->itemData(i).toInt();
                ET enumValue = CT::fromIntegerCode(integerCode, NULL);
                if (enumValue == item) {
                    m_itemComboBox->blockSignals(true);
                    m_itemComboBox->setCurrentIndex(i);
                    m_itemComboBox->blockSignals(false);
                    break;
                }
            }
            
        }
        
        /**
         * @return The actual widget.
         */
        virtual QWidget* getWidget() {
            return m_itemComboBox;
        }
        
        /**
         * @return The actual combo box.
         */
        QComboBox* getComboBox() {
            return m_itemComboBox;
        }
        
    signals:
        /**
         * This signal is sent when the user chooses an item in the combobox. 
         * The item's index is passed. Note that this signal is sent even 
         * when the choice is not changed. If you need to know when the 
         * choice actually changes, use signal itemChanged().
         */
        void itemActivated();
        
        /**
         * This signal is sent whenever the currentIndex in the 
         * combobox changes either through user interaction or
         * programmatically.
         */
        void itemChanged();
        
    private slots:
        /**
         * Called when the user selects an item 
         * @param indx
         *   Index of item selected.
         */
        void itemComboBoxIndexChanged(int) {
            emit itemChanged();
        }
        
        /**
         * Called when the user selects an item
         * @param indx
         *   Index of item selected.
         */
        void itemComboBoxActivated(int) {
            emit itemActivated();
        }
        
    private:
        EnumComboBoxTemplate(const EnumComboBoxTemplate&);

        EnumComboBoxTemplate& operator=(const EnumComboBoxTemplate&);
        
    private:
        QComboBox* m_itemComboBox;
    };
    
#ifdef __ENUM_COMBOBOX_TEMPLATE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ENUM_COMBOBOX_TEMPLATE_DECLARE__

} // namespace
#endif  //__ENUM_COMBOBOX_TEMPLATE__H_
