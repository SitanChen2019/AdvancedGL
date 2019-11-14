#ifndef PBD_CONSTRAINT_H
#define PBD_CONSTRAINT_H
#include <defs.h>


namespace PBD {
    class Simualtor;

    class EXPORT_SYMBOL DistanceConstraint
    {
    public:
        DistanceConstraint(int vid0, int vid1, float distance)
        {
            mId0 = vid0;
            mId1 = vid1;
            mDistance = distance;
        }

        int mId0;
        int mId1;
        float mDistance;
    };

    class EXPORT_SYMBOL Constraint
    {
    public:
        enum class ConstraintType
        {
            CT_DISTANCE_CONSTRAINT
        };

        ConstraintType mType;
        union {
            DistanceConstraint mDistanceCons;
        };


        Constraint(DistanceConstraint& dc)
        {
            mType = ConstraintType::CT_DISTANCE_CONSTRAINT;
            mDistanceCons = dc;
        }

        void solve( Simualtor* pSimulator);
    };

}



#endif