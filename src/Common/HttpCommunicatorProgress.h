#ifndef __HTTP_COMMUNICATOR_PROGRESS_H__
#define __HTTP_COMMUNICATOR_PROGRESS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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


#include "CaretObject.h"



namespace caret {

    class HttpCommunicator;
    
    class HttpCommunicatorProgress : public CaretObject {
        
    public:
        enum Status {
            STATUS_FINISHED_OK,
            STATUS_FINISHED_WITH_ERROR,
            STATUS_IN_PROGRESS,
            STATUS_NOT_STARTED
        };
        
        HttpCommunicatorProgress(HttpCommunicator* httpCommunicator,
                                 const Status status,
                                 const AString& progressMessage,
                                 const int32_t& progressMinimum,
                                 const int32_t& progressMaximum,
                                 const int32_t& progressValue);
        
        virtual ~HttpCommunicatorProgress();
        
        HttpCommunicator* getHttpCommunicator();
        
        Status getStatus() const;
        
        AString getProgressMessage() const;
        
        int32_t getProgressMinimum() const;
        
        int32_t getProgressMaximum() const;
        
        int32_t getProgressValue() const;

        bool isCancelled() const;
        
        void setCancelled(const bool cancelled);

        // ADD_NEW_METHODS_HERE

    private:
        HttpCommunicatorProgress(const HttpCommunicatorProgress&);

        HttpCommunicatorProgress& operator=(const HttpCommunicatorProgress&);
        
        HttpCommunicator* m_httpCommunicator;
        
        const Status m_status;
        
        const AString m_progressMessage;
        
        const int32_t  m_progressMinimum;
        
        const int32_t  m_progressMaximum;
        
        const int32_t  m_progressValue;
        
        bool m_cancelled;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HTTP_COMMUNICATOR_PROGRESS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HTTP_COMMUNICATOR_PROGRESS_DECLARE__

} // namespace
#endif  //__HTTP_COMMUNICATOR_PROGRESS_H__
