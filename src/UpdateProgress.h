#ifndef UPDATEPROGRESS_H_
#define UPDATEPROGRESS_H_

class UpdateProgress
{
    public:
        UpdateProgress();
        ~UpdateProgress();
        void getProgress();

    private:
        int m_SWprogressFd; // File descriptor for SWupdate progress
};


#endif /* UPDATEPROGRESS_H_*/